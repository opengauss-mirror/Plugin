#include "postgres.h"
#include "knl/knl_variable.h"
#include "nodes/parsenodes.h"
#include "access/tuptoaster.h"
#include "utils/varbit.h"
#include "db_b_parser_header.h"
#include "parser/scansup.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/datetime.h"
#include "utils/formatting.h"
PG_FUNCTION_INFO_V1(mp_bit_length_bit);
PG_FUNCTION_INFO_V1(mp_bit_length_text);
PG_FUNCTION_INFO_V1(mp_bit_length_bytea);
PG_FUNCTION_INFO_V1(time_sec);

extern "C" Datum mp_bit_length_bit(PG_FUNCTION_ARGS);
extern "C" Datum mp_bit_length_text(PG_FUNCTION_ARGS);
extern "C" Datum mp_bit_length_bytea(PG_FUNCTION_ARGS);
extern "C" Datum time_sec(PG_FUNCTION_ARGS);

Datum mp_bit_length_bit(PG_FUNCTION_ARGS)
{
    VarBit *arg = PG_GETARG_VARBIT_P(0);

    PG_RETURN_INT32(VARBITBYTES(arg) * 8);
}

Datum mp_bit_length_text(PG_FUNCTION_ARGS)
{
    Datum str = PG_GETARG_DATUM(0);

    /* We need not detoast the input at all */
    PG_RETURN_INT32((toast_raw_datum_size(str) - VARHDRSZ) * 8);
}

Datum mp_bit_length_bytea(PG_FUNCTION_ARGS)
{
    Datum str = PG_GETARG_DATUM(0);

    /* We need not detoast the input at all */
    PG_RETURN_INT32((toast_raw_datum_size(str) - VARHDRSZ) * 8);
}

Datum time_sec(PG_FUNCTION_ARGS)
{
    text *units = PG_GETARG_TEXT_PP(0);
    Timestamp timestamp = PG_GETARG_TIMESTAMP(1);
    float8 result;
    int type, val;
    char *lowunits = NULL;
    fsec_t fsec;
    struct pg_tm tt, *tm = &tt;

    if (TIMESTAMP_NOT_FINITE(timestamp)) {
        result = 0;
        PG_RETURN_FLOAT8(result);
    }

    lowunits = downcase_truncate_identifier(VARDATA_ANY(units), VARSIZE_ANY_EXHDR(units), false);

    type = DecodeUnits(0, lowunits, &val);
    if (type == UNKNOWN_FIELD)
        type = DecodeSpecial(0, lowunits, &val);

    if (type == UNITS) {
        if (timestamp2tm(timestamp, NULL, tm, &fsec, NULL, NULL) != 0)
            ereport(ERROR, (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE), errmsg("timestamp out of range")));
        switch (val) {
            case DTK_MICROSEC:
#ifdef HAVE_INT64_TIMESTAMP
                result = fsec;
#else
                result = fsec * 1000000;
#endif
                break;
            case DTK_SECOND:
#ifdef HAVE_INT64_TIMESTAMP
                result = tm->tm_sec;
#else
                result = tm->tm_sec;
#endif
                break;
            default:
                ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                                errmsg("timestamp units \"%s\" not supported", lowunits)));
                result = 0;
        }
    }
    PG_RETURN_FLOAT8(result);
}
