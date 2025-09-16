/*-------------------------------------------------------------------------
 *
 * errormessage.h
 *	  Error handling related support functionality.
 *
 * Copyright (c) Citus Data, Inc.
 *-------------------------------------------------------------------------
 */

#ifndef ERRORMESSAGE_H
#define ERRORMESSAGE_H

#include "c.h"
#include "pg_version_compat.h"
#include "distributed/citus_nodes.h"

class DeferredErrorMessage {
public:
    DeferredErrorMessage() = delete;

    void Raise(int elevel);

    static DeferredErrorMessage* Create(int code, const char* message, const char* detail,
                                        const char* hint, const char* filename,
                                        int linenumber, const char* functionname);

    CitusNode tag;
    int code;
    const char* message;
    const char* detail;
    const char* hint;
    const char* filename;
    int linenumber;
    const char* functionname;
};

/*
 * DeferredError allocates a deferred error message, that can later be emitted
 * using RaiseDeferredError().  These error messages can be
 * serialized/copied/deserialized, i.e. can be embedded in plans and such.
 */
#define DeferredError(code, message, detail, hint)                                \
    DeferredErrorMessage::Create(code, message, detail, hint, __FILE__, __LINE__, \
                                 PG_FUNCNAME_MACRO)

/*
 * RaiseDeferredError emits a previously allocated error using the specified
 * severity.
 *
 * The trickery with __builtin_constant_p/pg_unreachable aims to have the
 * compiler understand that the function will not return if elevel >= ERROR.
 */
#ifdef HAVE__BUILTIN_CONSTANT_P
#define RaiseDeferredError(error, elevel)                        \
    do {                                                         \
        error->Raise(elevel);                                    \
        if (__builtin_constant_p(elevel) && (elevel) >= ERROR) { \
            pg_unreachable();                                    \
        }                                                        \
    } while (0)
#else /* !HAVE_BUILTIN_CONSTANT_P */
#define RaiseDeferredError(error, elevel) \
    do {                                  \
        const int elevel_ = (elevel);     \
        error->Raise(elevel_);            \
        if (elevel_ >= ERROR) {           \
            pg_unreachable();             \
        }                                 \
    } while (0)
#endif /* HAVE_BUILTIN_CONSTANT_P */

/*
 * ThrowErrorData --- report an error described by an ErrorData structure
 *
 * This is somewhat like ReThrowError, but it allows elevels besides ERROR,
 * and the boolean flags such as output_to_server are computed via the
 * default rules rather than being copied from the given ErrorData.
 * This is primarily used to re-report errors originally reported by
 * background worker processes and then propagated (with or without
 * modification) to the backend responsible for them.
 */
void ThrowErrorData(ErrorData* edata);
#endif
