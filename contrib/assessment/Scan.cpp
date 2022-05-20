/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * -------------------------------------------------------------------------
 *
 * Scan.cpp
 *    File Scanner
 *
 * IDENTIFICATION
 *    contrib/assessment/Scan.cpp
 *
 * -------------------------------------------------------------------------
 */

#include "Scan.h"
#include "malloc.h"
#include "psqlscan.inc"

/**
 * mock function
 * mock psql_error. print log
 * @param fmt
 * @param ...
 */
void psql_error(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, _(fmt), ap);
    va_end(ap);
}


/**
 * mock function
 * gsql runtime value, only for file analyse.
 * @param space space input
 * @param name name input
 * @return nullptr
 */
const char* GetVariable(VariableSpace space, const char* name)
{
    return nullptr;
}

/**
 * mock function
 * return true if `standard_conforming_strings` is on
 * Compatible with common scenarios
 * @return true
 */
bool standard_strings(void)
{
    return true;
}

/**
 * psqlscan.inc
 * set db = nullptr
 * set vars = nullptr
 */
PsqlSettings pset;

char* gets_fromFile(FILE* source)
{
    static PQExpBuffer buffer = NULL;
    /* first time through? */
    if (buffer == NULL) {
        buffer = createPQExpBuffer();
    } else {
        (void) resetPQExpBuffer(buffer);
    }

    for (;;) {
        char* result = NULL;


        /* Get some data */
        char currLine[MAX_DECRYPT_BUFF_LEN];
        result = fgets(currLine, sizeof(currLine), source);

        /* EOF or error? */
        if (result == NULL) {
            if (ferror(source)) {
                psql_error("could not read from input file: %s\n", strerror(errno));
                return NULL;
            }
            break;
        }

        /*
         * Data importing/dumping support AES128 through VPP SSL.
         * when decrypt text is longer than MAX_DECRYPT_BUFF_LEN
         * We use decryptBuff here.
         */
        appendPQExpBufferStr(buffer, currLine);

        if (PQExpBufferBroken(buffer)) {
            psql_error("out of memory\n");
            return NULL;
        }

        /* EOL? */
        if (buffer->len > 0 && buffer->data[buffer->len - 1] == '\n') {
            char* line = NULL;
            buffer->data[buffer->len - 1] = '\0';
            line = pg_strdup(buffer->data);
            return line;
        }
    }

    if (buffer->len > 0) { /* EOF after reading some bufferload(s) */
        char* line = NULL;
        line = pg_strdup(buffer->data);
        return line;
    }

    /* EOF, so return null */
    return NULL;
}

void* pg_malloc_zero(size_t size)
{
    void* tmp = NULL;
    errno_t rc = 0;

    tmp = malloc(size);
    if (tmp == nullptr) {
        fprintf(stderr, "%s: out of memeory", pset.progname);
        exit(EXIT_FAILURE);
    }
    rc = memset_s(tmp, size, 0, size);
    check_memset_s(rc);
    return tmp;
}

void PSqlPostInit(void)
{
    pset.cur_cmd_interactive = false;
    pset.db = nullptr;
    pset.progname = "assessment_database";
    pset.encoding = 0;
}

vector <std::string> Scan::GetNextSql()
{
    volatile PQExpBuffer query_buf = createPQExpBuffer();
    vector <std::string> result{};
    while (true) {
        auto line = gets_fromFile(this->fd);
        /* incomplete sql at the end of file will not be processed */
        if (line == nullptr) {
            destroyPQExpBuffer(query_buf);
            return result;
        }
        if (query_buf->len > 0) {
            appendPQExpBufferChar(query_buf, '\n');
        }
        psql_scan_setup(this->scanState, line, (int) strlen(line));
        while (true) {
            promptStatus_t prompt_tmp = PROMPT_READY;
            PsqlScanResult scan_result = psql_scan(this->scanState, query_buf, &prompt_tmp);

            if (PQExpBufferBroken(query_buf)) {
                psql_error("out of memory\n");
                exit(EXIT_FAILURE);
            }

            if (scan_result == PSCAN_SEMICOLON) {
                result.emplace_back(query_buf->data);
                resetPQExpBuffer(query_buf);
                break;
            } else if (scan_result == PSCAN_BACKSLASH) {
                /* skip backslash command */
                resetPQExpBuffer(query_buf);
                break;
            }

            if (scan_result == PSCAN_INCOMPLETE && prompt_tmp == PROMPT_READY) {
                /* comment, do nothing */
            }

            /* fall out of loop if lexer reached EOL */
            if (scan_result == PSCAN_INCOMPLETE || scan_result == PSCAN_EOL) {
                break;
            }
        }
        psql_scan_finish(this->scanState);
        free(line);
    }
    destroyPQExpBuffer(query_buf);
    return result;
}

void Scan::FreeScan()
{
    psql_scan_destroy(this->scanState);
}

Scan::Scan(FILE* fd)
{
    this->fd = fd;
    this->scanState = (PsqlScanStateData*) pg_malloc_zero(sizeof(PsqlScanStateData));
    auto rc = memset_s(this->scanState, sizeof(PsqlScanStateData), 0, sizeof(PsqlScanStateData));
    securec_check_c(rc, "", "");
    psql_scan_reset(this->scanState);
}
