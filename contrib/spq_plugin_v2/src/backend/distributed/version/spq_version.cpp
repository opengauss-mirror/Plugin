/*-------------------------------------------------------------------------
 *
 * spq_version.cc
 *
 * This file contains functions for displaying the Citus version string
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "utils/builtins.h"

#include "spq_version.h"
#include "utils/uuid.h"

PG_FUNCTION_INFO_V1(spq_version);
PG_FUNCTION_INFO_V1(spq_server_id);

extern "C" Datum spq_version(PG_FUNCTION_ARGS);
extern "C" Datum spq_server_id(PG_FUNCTION_ARGS);

/* GIT_VERSION is passed in as a compiler flag during builds that have git installed */
#ifdef GIT_VERSION
#define GIT_REF " gitref: " GIT_VERSION
#else
#define GIT_REF
#endif

Datum spq_version(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(cstring_to_text(SPQ_VERSION_STR GIT_REF));
}

/*
 * citus_server_id returns a random UUID value as server identifier. This is
 * modeled after PostgreSQL's pg_random_uuid().
 */
Datum spq_server_id(PG_FUNCTION_ARGS)
{
    uint8* buf = (uint8*)palloc(UUID_LEN);

    for (int bufIdx = 0; bufIdx < UUID_LEN; bufIdx++) {
        buf[bufIdx] = (uint8)(random() & 0xFF);
    }

    /*
     * Set magic numbers for a "version 4" (pseudorandom) UUID, see
     * http://tools.ietf.org/html/rfc4122#section-4.4
     */
    buf[6] = (buf[6] & 0x0f) | 0x40; /* "version" field */
    buf[8] = (buf[8] & 0x3f) | 0x80; /* "variant" field */

    PG_RETURN_UUID_P((pg_uuid_t*)buf);
}
