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

#ifndef AG_CSV_READER_H
#define AG_CSV_READER_H

#include "postgres.h"

#include <stdio.h>

#include "lib/stringinfo.h"

/*
 * Record reader for the CSV files consumed by the AGE loaders.
 *
 * openGauss' COPY parser cannot be reused here: its raw field array is sized
 * to the target relation's column count and silently drops any further
 * fields, while label CSV files carry one column per property.  This reader
 * is self-contained and implements RFC 4180 with the leniencies of the
 * non-strict libcsv mode the loaders were originally written against:
 *
 *   - one single-byte delimiter, '"' as the quote character;
 *   - records end at LF, CR or CRLF, a terminator inside quotes is data;
 *   - "" inside a quoted field is a literal quote;
 *   - unquoted fields lose surrounding spaces and tabs, quoted fields are
 *     returned verbatim (so "  x  " keeps its padding);
 *   - a quote inside an unquoted field, or text following a closing quote,
 *     is taken literally instead of raising an error;
 *   - empty and whitespace-only lines produce no record; a final record
 *     without trailing newline is returned.
 *
 * The reader and everything it returns are palloc'd in the memory context
 * that is current when ag_csv_open() is called.  Field strings are owned by
 * the reader and stay valid until the next ag_csv_next_record() call; an
 * empty field is "" (never NULL).
 */
typedef struct ag_csv_reader {
    FILE *file;
    char *path;
    char delimiter;
    bool eof;

    /* bytes of the current record's fields, each NUL terminated */
    StringInfoData data;
    /* start offset of every field in data, and the pointers built from it */
    int *field_offsets;
    char **fields;
    int nfields;
    int field_capacity;
} ag_csv_reader;

ag_csv_reader *ag_csv_open(const char *path, char delimiter);
bool ag_csv_next_record(ag_csv_reader *reader, char ***fields, int *nfields);
void ag_csv_close(ag_csv_reader *reader);

#endif /* AG_CSV_READER_H */
