#include "postgres.h"
#include "knl/knl_variable.h"

#include "nodes/parsenodes.h"
#include "access/tuptoaster.h"
#include "utils/varbit.h"

#include "db_b_parser_header.h"


PG_FUNCTION_INFO_V1( mp_bit_length_bit );
PG_FUNCTION_INFO_V1( mp_bit_length_text );
PG_FUNCTION_INFO_V1( mp_bit_length_bytea );

extern "C" Datum mp_bit_length_bit(PG_FUNCTION_ARGS);
extern "C" Datum mp_bit_length_text(PG_FUNCTION_ARGS);
extern "C" Datum mp_bit_length_bytea(PG_FUNCTION_ARGS);


Datum mp_bit_length_bit(PG_FUNCTION_ARGS)
{
    VarBit* arg = PG_GETARG_VARBIT_P(0);

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


