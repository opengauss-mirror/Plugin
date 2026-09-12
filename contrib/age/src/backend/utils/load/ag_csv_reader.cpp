/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "postgres.h"

#include "miscadmin.h"
#include "storage/smgr/fd.h"

#include "utils/load/ag_csv_reader.h"

#define AG_CSV_QUOTE '"'
#define AG_CSV_INITIAL_FIELD_CAPACITY 16
#define AG_CSV_FIELD_GROWTH_FACTOR 2

/*
 * Parser states.  A record is a sequence of fields; a field is either quoted
 * or not.  FIELD_MIGHT_HAVE_ENDED is entered when a quote is seen inside a
 * quoted field: the next character decides whether it closed the field ("x"),
 * escaped a quote ("") or (leniently) was just data.
 */
typedef enum ag_csv_state {
    RECORD_NOT_BEGUN,       /* nothing seen yet on this record */
    FIELD_NOT_BEGUN,        /* after a delimiter, before the field content */
    FIELD_BEGUN,            /* inside a field */
    FIELD_MIGHT_HAVE_ENDED  /* inside a quoted field, right after a quote */
} ag_csv_state;

static inline bool ag_csv_is_space(int c)
{
    return c == ' ' || c == '\t';
}

static inline bool ag_csv_is_terminator(int c)
{
    return c == '\n' || c == '\r';
}

/* A CR followed by LF is one terminator. */
static void ag_csv_consume_crlf(ag_csv_reader *reader, int c)
{
    if (c == '\r') {
        int next = getc(reader->file);
        if (next != '\n' && next != EOF) {
            ungetc(next, reader->file);
        }
    }
}

static void ag_csv_begin_field(ag_csv_reader *reader)
{
    if (reader->nfields == reader->field_capacity) {
        reader->field_capacity *= AG_CSV_FIELD_GROWTH_FACTOR;
        reader->field_offsets = (int *)repalloc(
            reader->field_offsets, sizeof(int) * reader->field_capacity);
        reader->fields = (char **)repalloc(
            reader->fields, sizeof(char *) * reader->field_capacity);
    }

    reader->field_offsets[reader->nfields] = reader->data.len;
}

static void ag_csv_end_field(ag_csv_reader *reader)
{
    appendStringInfoChar(&reader->data, '\0');
    reader->nfields++;
    ag_csv_begin_field(reader);
}

/* Turn the collected offsets into field pointers; the buffer is final now. */
static bool ag_csv_end_record(ag_csv_reader *reader, char ***fields,
                              int *nfields)
{
    int i;

    for (i = 0; i < reader->nfields; i++) {
        reader->fields[i] = reader->data.data + reader->field_offsets[i];
    }

    *fields = reader->fields;
    *nfields = reader->nfields;
    return true;
}

ag_csv_reader *ag_csv_open(const char *path, char delimiter)
{
    ag_csv_reader *reader;

    if (delimiter == AG_CSV_QUOTE || ag_csv_is_terminator(delimiter) ||
        delimiter == '\0') {
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                 errmsg("CSV delimiter cannot be a quote, a newline or NUL")));
    }

    reader = (ag_csv_reader *)palloc0(sizeof(ag_csv_reader));
    reader->path = pstrdup(path);
    reader->delimiter = delimiter;
    initStringInfo(&reader->data);
    reader->field_capacity = AG_CSV_INITIAL_FIELD_CAPACITY;
    reader->field_offsets = (int *)palloc(sizeof(int) * reader->field_capacity);
    reader->fields = (char **)palloc(sizeof(char *) * reader->field_capacity);

    reader->file = AllocateFile(path, PG_BINARY_R);
    if (reader->file == NULL) {
        ereport(ERROR,
                (errcode_for_file_access(),
                 errmsg("could not open file \"%s\" for reading: %m", path)));
    }

    return reader;
}

void ag_csv_close(ag_csv_reader *reader)
{
    if (reader->file != NULL) {
        (void)FreeFile(reader->file);
        reader->file = NULL;
    }
}

typedef enum ag_csv_action {
    AG_CSV_ACT_CONTINUE,
    AG_CSV_ACT_RECORD_READY
} ag_csv_action;

typedef struct ag_csv_parse {
    ag_csv_state state;
    bool quoted;
    int spaces;
    int delimiter;
} ag_csv_parse;

static bool ag_csv_flush_eof_record(ag_csv_reader *reader, ag_csv_parse *parse,
                                    char ***fields, int *nfields)
{
    switch (parse->state) {
        case RECORD_NOT_BEGUN:
            return false;
        case FIELD_MIGHT_HAVE_ENDED:
            /* drop the closing quote and what followed it */
            reader->data.len -= parse->spaces + 1;
            break;
        case FIELD_BEGUN:
        case FIELD_NOT_BEGUN:
            if (!parse->quoted) {
                reader->data.len -= parse->spaces;
            }
            break;
    }
    ag_csv_end_field(reader);
    return ag_csv_end_record(reader, fields, nfields);
}

static ag_csv_action ag_csv_on_field_not_begun(ag_csv_reader *reader, int c,
                                               ag_csv_parse *parse)
{
    if (ag_csv_is_space(c) && c != parse->delimiter) {
        /* leading whitespace of an unquoted field */
        return AG_CSV_ACT_CONTINUE;
    }
    if (ag_csv_is_terminator(c)) {
        ag_csv_consume_crlf(reader, c);
        if (parse->state == FIELD_NOT_BEGUN) {
            /* record ends with an empty field */
            ag_csv_end_field(reader);
            return AG_CSV_ACT_RECORD_READY;
        }
        /* blank line: no record */
        return AG_CSV_ACT_CONTINUE;
    }
    if (c == parse->delimiter) {
        ag_csv_end_field(reader);
        parse->state = FIELD_NOT_BEGUN;
        return AG_CSV_ACT_CONTINUE;
    }
    if (c == AG_CSV_QUOTE) {
        parse->state = FIELD_BEGUN;
        parse->quoted = true;
        return AG_CSV_ACT_CONTINUE;
    }
    parse->state = FIELD_BEGUN;
    parse->quoted = false;
    parse->spaces = 0;
    appendStringInfoChar(&reader->data, (char)c);
    return AG_CSV_ACT_CONTINUE;
}

static ag_csv_action ag_csv_on_field_begun(ag_csv_reader *reader, int c,
                                           ag_csv_parse *parse)
{
    if (c == AG_CSV_QUOTE) {
        /*
         * Keep the quote for now: it is removed again if it turns
         * out to close the field, and kept if it was an escaped
         * ("") or, leniently, a stray quote.
         */
        appendStringInfoChar(&reader->data, (char)c);
        if (parse->quoted) {
            parse->state = FIELD_MIGHT_HAVE_ENDED;
        }
        parse->spaces = 0;
        return AG_CSV_ACT_CONTINUE;
    }
    if (c == parse->delimiter && !parse->quoted) {
        reader->data.len -= parse->spaces;
        ag_csv_end_field(reader);
        parse->quoted = false;
        parse->spaces = 0;
        parse->state = FIELD_NOT_BEGUN;
        return AG_CSV_ACT_CONTINUE;
    }
    if (ag_csv_is_terminator(c) && !parse->quoted) {
        ag_csv_consume_crlf(reader, c);
        reader->data.len -= parse->spaces;
        ag_csv_end_field(reader);
        return AG_CSV_ACT_RECORD_READY;
    }
    /* data, including delimiters and newlines inside quotes */
    appendStringInfoChar(&reader->data, (char)c);
    if (!parse->quoted && ag_csv_is_space(c)) {
        parse->spaces++;
    } else {
        parse->spaces = 0;
    }
    return AG_CSV_ACT_CONTINUE;
}

static ag_csv_action ag_csv_on_might_have_ended(ag_csv_reader *reader, int c,
                                                ag_csv_parse *parse)
{
    if (c == parse->delimiter) {
        /* the quote closed the field: drop it and any padding */
        reader->data.len -= parse->spaces + 1;
        ag_csv_end_field(reader);
        parse->quoted = false;
        parse->spaces = 0;
        parse->state = FIELD_NOT_BEGUN;
        return AG_CSV_ACT_CONTINUE;
    }
    if (ag_csv_is_terminator(c)) {
        ag_csv_consume_crlf(reader, c);
        reader->data.len -= parse->spaces + 1;
        ag_csv_end_field(reader);
        return AG_CSV_ACT_RECORD_READY;
    }
    if (ag_csv_is_space(c)) {
        appendStringInfoChar(&reader->data, (char)c);
        parse->spaces++;
        return AG_CSV_ACT_CONTINUE;
    }
    if (c == AG_CSV_QUOTE) {
        if (parse->spaces == 0) {
            /* "" inside a quoted field: the kept quote is data */
            parse->state = FIELD_BEGUN;
        } else {
            /* lenient: `" "` sequence, take both as data */
            appendStringInfoChar(&reader->data, (char)c);
            parse->spaces = 0;
        }
        return AG_CSV_ACT_CONTINUE;
    }
    /* lenient: text after a closing quote is data */
    appendStringInfoChar(&reader->data, (char)c);
    parse->spaces = 0;
    parse->state = FIELD_BEGUN;
    return AG_CSV_ACT_CONTINUE;
}

/*
 * Read the next record.  Returns false when the file is exhausted; fields and
 * nfields are only valid after a true return.
 */
bool ag_csv_next_record(ag_csv_reader *reader, char ***fields, int *nfields)
{
    ag_csv_parse parse;
    int c;
    ag_csv_action act;

    parse.state = RECORD_NOT_BEGUN;
    parse.quoted = false;
    parse.spaces = 0;
    parse.delimiter = (unsigned char)reader->delimiter;

    if (reader->eof) {
        return false;
    }

    resetStringInfo(&reader->data);
    reader->nfields = 0;
    ag_csv_begin_field(reader);

    while ((c = getc(reader->file)) != EOF) {
        CHECK_FOR_INTERRUPTS();
        act = AG_CSV_ACT_CONTINUE;
        switch (parse.state) {
            case RECORD_NOT_BEGUN:
            case FIELD_NOT_BEGUN:
                act = ag_csv_on_field_not_begun(reader, c, &parse);
                break;
            case FIELD_BEGUN:
                act = ag_csv_on_field_begun(reader, c, &parse);
                break;
            case FIELD_MIGHT_HAVE_ENDED:
                act = ag_csv_on_might_have_ended(reader, c, &parse);
                break;
        }
        if (act == AG_CSV_ACT_RECORD_READY) {
            return ag_csv_end_record(reader, fields, nfields);
        }
    }

    if (ferror(reader->file)) {
        ereport(ERROR,
                (errcode_for_file_access(),
                 errmsg("could not read file \"%s\": %m", reader->path)));
    }
    reader->eof = true;
    return ag_csv_flush_eof_record(reader, &parse, fields, nfields);
}
