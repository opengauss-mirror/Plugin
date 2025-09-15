/*-------------------------------------------------------------------------
 *
 * string_utils.c
 *
 * This file contains functions to perform useful operations on strings.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "distributed/relay_utility.h"
#include "distributed/string_utils.h"
#include "parser/scansup.h"

/*
 * ConvertIntToString returns the string version of given integer.
 */
char* ConvertIntToString(int val)
{
    StringInfo str = makeStringInfo();

    appendStringInfo(str, "%d", val);

    return str->data;
}

/*
 * pg_clean_ascii -- Replace any non-ASCII chars with a "\xXX" string
 *
 * Makes a newly allocated copy of the string passed in, which must be
 * '\0'-terminated. In the backend, additional alloc_flags may be provided and
 * will be passed as-is to palloc_extended(); in the frontend, alloc_flags is
 * ignored and the copy is malloc'd.
 *
 * This function exists specifically to deal with filtering out
 * non-ASCII characters in a few places where the client can provide an almost
 * arbitrary string (and it isn't checked to ensure it's a valid username or
 * database name or similar) and we don't want to have control characters or other
 * things ending up in the log file where server admins might end up with a
 * messed up terminal when looking at them.
 *
 * In general, this function should NOT be used- instead, consider how to handle
 * the string without needing to filter out the non-ASCII characters.
 *
 * Ultimately, we'd like to improve the situation to not require replacing all
 * non-ASCII but perform more intelligent filtering which would allow UTF or
 * similar, but it's unclear exactly what we should allow, so stick to ASCII only
 * for now.
 */
char* pg_clean_ascii(const char* str, int alloc_flags)
{
    size_t dstlen;
    char* dst;
    const char* p;
    size_t i = 0;

    /* Worst case, each byte can become four bytes, plus a null terminator. */
    dstlen = strlen(str) * 4 + 1;

#ifdef FRONTEND
    dst = static_cast<char*>(malloc(dstlen));
#else
    dst = static_cast<char*>(palloc_extended(dstlen, alloc_flags));
#endif

    if (!dst)
        return NULL;

    for (p = str; *p != '\0'; p++) {

        /* Only allow clean ASCII chars in the string */
        if (*p < 32 || *p > 126) {
            Assert(i < (dstlen - 3));
            snprintf(&dst[i], dstlen - i, "\\x%02x", (unsigned char)*p);
            i += 4;
        } else {
            Assert(i < dstlen);
            dst[i] = *p;
            i++;
        }
    }

    Assert(i < dstlen);
    dst[i] = '\0';
    return dst;
}

/*
 * SplitGUCList --- parse a string containing identifiers or file names
 *
 * This is used to split the value of a GUC_LIST_QUOTE GUC variable, without
 * presuming whether the elements will be taken as identifiers or file names.
 * We assume the input has already been through flatten_set_variable_args(),
 * so that we need never downcase (if appropriate, that was done already).
 * Nor do we ever truncate, since we don't know the correct max length.
 * We disallow embedded whitespace for simplicity (it shouldn't matter,
 * because any embedded whitespace should have led to double-quoting).
 * Otherwise the API is identical to SplitIdentifierString.
 *
 * XXX it's annoying to have so many copies of this string-splitting logic.
 * However, it's not clear that having one function with a bunch of option
 * flags would be much better.
 *
 * XXX there is a version of this function in src/bin/pg_dump/dumputils.c.
 * Be sure to update that if you have to change this.
 *
 * Inputs:
 *	rawstring: the input string; must be overwritable!	On return, it's
 *			   been modified to contain the separated identifiers.
 *	separator: the separator punctuation expected between identifiers
 *			   (typically '.' or ',').  Whitespace may also appear around
 *			   identifiers.
 * Outputs:
 *	namelist: filled with a palloc'd list of pointers to identifiers within
 *			  rawstring.  Caller should list_free() this even on error return.
 *
 * Returns true if okay, false if there is a syntax error in the string.
 */
bool SplitGUCList(char* rawstring, char separator, List** namelist)
{
    char* nextp = rawstring;
    bool done = false;

    *namelist = NIL;

    while (scanner_isspace(*nextp))
        nextp++; /* skip leading whitespace */

    if (*nextp == '\0')
        return true; /* allow empty string */

    /* At the top of the loop, we are at start of a new identifier. */
    do {
        char* curname;
        char* endp;

        if (*nextp == '"') {
            /* Quoted name --- collapse quote-quote pairs */
            curname = nextp + 1;
            for (;;) {
                endp = strchr(nextp + 1, '"');
                if (endp == NULL)
                    return false; /* mismatched quotes */
                if (endp[1] != '"')
                    break; /* found end of quoted name */
                /* Collapse adjacent quotes into one quote, and look again */
                memmove(endp, endp + 1, strlen(endp));
                nextp = endp;
            }
            /* endp now points at the terminating quote */
            nextp = endp + 1;
        } else {
            /* Unquoted name --- extends to separator or whitespace */
            curname = nextp;
            while (*nextp && *nextp != separator && !scanner_isspace(*nextp))
                nextp++;
            endp = nextp;
            if (curname == nextp)
                return false; /* empty unquoted name not allowed */
        }

        while (scanner_isspace(*nextp))
            nextp++; /* skip trailing whitespace */

        if (*nextp == separator) {
            nextp++;
            while (scanner_isspace(*nextp))
                nextp++; /* skip leading whitespace for next */
                         /* we expect another name, so done remains false */
        } else if (*nextp == '\0')
            done = true;
        else
            return false; /* invalid syntax */

        /* Now safe to overwrite separator with a null */
        *endp = '\0';

        /*
         * Finished isolating current name --- add it to list
         */
        *namelist = lappend(*namelist, curname);

        /* Loop back if we didn't reach end of string */
    } while (!done);

    return true;
}
