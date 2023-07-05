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

/*
 * Functions for operators in Cypher expressions.
 */

#include "postgres.h"

#include <math.h>

#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/numeric.h"

#include "utils/agtype.h"

static void ereport_op_str(const char *op, agtype *lhs, agtype *rhs);
static agtype *agtype_concat(agtype *agt1, agtype *agt2);
static agtype_value *iterator_concat(agtype_iterator **it1,
                                     agtype_iterator **it2,
                                     agtype_parse_state **state);
static void concat_to_agtype_string(agtype_value *result, char *lhs, int llen,
                                    char *rhs, int rlen);
static char *get_string_from_agtype_value(agtype_value *agtv, int *length);

static void concat_to_agtype_string(agtype_value *result, char *lhs, int llen,
                                    char *rhs, int rlen)
{
    int length = llen + rlen;
    char *buffer = result->val.string.val;

    Assert(llen >= 0 && rlen >= 0);
    check_string_length(length);
    buffer = palloc(length);

    strncpy(buffer, lhs, llen);
    strncpy(buffer + llen, rhs, rlen);

    result->type = AGTV_STRING;
    result->val.string.len = length;
    result->val.string.val = buffer;
}

static char *get_string_from_agtype_value(agtype_value *agtv, int *length)
{
    Datum number;
    char *string;

    switch (agtv->type)
    {
    case AGTV_INTEGER:
        number = DirectFunctionCall1(int8out,
                                     Int8GetDatum(agtv->val.int_value));
        string = DatumGetCString(number);
        *length = strlen(string);
        return string;
    case AGTV_FLOAT:
        number = DirectFunctionCall1(float8out,
                                     Float8GetDatum(agtv->val.float_value));
        string = DatumGetCString(number);
        *length = strlen(string);

        if (is_decimal_needed(string))
        {
            char *str = palloc(*length + 2);
            strncpy(str, string, *length);
            strncpy(str + *length, ".0", 2);
            *length += 2;
            string = str;
        }
        return string;
    case AGTV_STRING:
        *length = agtv->val.string.len;
        return agtv->val.string.val;

    case AGTV_NUMERIC:
        string = DatumGetCString(DirectFunctionCall1(numeric_out,
                     PointerGetDatum(agtv->val.numeric)));
        *length = strlen(string);
        return string;

    case AGTV_NULL:
    case AGTV_BOOL:
    case AGTV_ARRAY:
    case AGTV_OBJECT:
    case AGTV_BINARY:
    default:
        *length = 0;
        return NULL;
    }
    return NULL;
}

Datum get_numeric_datum_from_agtype_value(agtype_value *agtv)
{
    switch (agtv->type)
    {
    case AGTV_INTEGER:
        return DirectFunctionCall1(int8_numeric,
                                   Int8GetDatum(agtv->val.int_value));
    case AGTV_FLOAT:
        return DirectFunctionCall1(float8_numeric,
                                   Float8GetDatum(agtv->val.float_value));
    case AGTV_NUMERIC:
        return NumericGetDatum(agtv->val.numeric);

    default:
        break;
    }

    return 0;
}

bool is_numeric_result(agtype_value *lhs, agtype_value *rhs)
{
    if (((lhs->type == AGTV_NUMERIC || rhs->type == AGTV_NUMERIC) &&
         (lhs->type == AGTV_INTEGER || lhs->type == AGTV_FLOAT ||
          rhs->type == AGTV_INTEGER || rhs->type == AGTV_FLOAT )) ||
        (lhs->type == AGTV_NUMERIC && rhs->type == AGTV_NUMERIC))
        return true;
    return false;
}

PG_FUNCTION_INFO_V1(agtype_add);

/* agtype addition and concat function for + operator */
Datum agtype_add(PG_FUNCTION_ARGS)
{
    agtype *lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *rhs = AG_GET_ARG_AGTYPE_P(1);
    agtype_value *agtv_lhs;
    agtype_value *agtv_rhs;
    agtype_value agtv_result;

    /* If both are not scalars */
    if (!(AGT_ROOT_IS_SCALAR(lhs) && AGT_ROOT_IS_SCALAR(rhs)))
    {
        Datum agt;

        /* It can't be a scalar and an object */
        if ((AGT_ROOT_IS_SCALAR(lhs) && AGT_ROOT_IS_OBJECT(rhs)) ||
            (AGT_ROOT_IS_OBJECT(lhs) && AGT_ROOT_IS_SCALAR(rhs)) ||
            /* It can't be two objects */
            (AGT_ROOT_IS_OBJECT(lhs) && AGT_ROOT_IS_OBJECT(rhs)))
            ereport_op_str("+", lhs, rhs);

        agt = AGTYPE_P_GET_DATUM(agtype_concat(lhs, rhs));

        PG_RETURN_DATUM(agt);
    }

    /* Both are scalar */
    agtv_lhs = get_ith_agtype_value_from_container(&lhs->root, 0);
    agtv_rhs = get_ith_agtype_value_from_container(&rhs->root, 0);

    /*
     * One or both values is a string OR one is a string and the other is
     * either an integer, float, or numeric. If so, concatenate them.
     */
    if ((agtv_lhs->type == AGTV_STRING || agtv_rhs->type == AGTV_STRING) &&
        (agtv_lhs->type == AGTV_INTEGER || agtv_lhs->type == AGTV_FLOAT ||
         agtv_lhs->type == AGTV_NUMERIC || agtv_lhs->type == AGTV_STRING ||
         agtv_rhs->type == AGTV_INTEGER || agtv_rhs->type == AGTV_FLOAT ||
         agtv_rhs->type == AGTV_NUMERIC || agtv_rhs->type == AGTV_STRING))
    {
        int llen = 0;
        char *lhs = get_string_from_agtype_value(agtv_lhs, &llen);
        int rlen = 0;
        char *rhs = get_string_from_agtype_value(agtv_rhs, &rlen);

        concat_to_agtype_string(&agtv_result, lhs, llen, rhs, rlen);
    }
    /* Both are integers - regular addition */
    else if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_INTEGER;
        agtv_result.val.int_value = agtv_lhs->val.int_value +
                                    agtv_rhs->val.int_value;
    }
    /* Both are floats - regular addition */
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.float_value +
                                      agtv_rhs->val.float_value;
    }
    /* The left is a float, the right is an integer - regular addition */
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.float_value +
                                      agtv_rhs->val.int_value;
    }
    /* The right is a float, the left is an integer - regular addition */
    else if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.int_value +
                                      agtv_rhs->val.float_value;
    }
    /* Is this a numeric result */
    else if (is_numeric_result(agtv_lhs, agtv_rhs))
    {
        Datum numd, lhsd, rhsd;

        lhsd = get_numeric_datum_from_agtype_value(agtv_lhs);
        rhsd = get_numeric_datum_from_agtype_value(agtv_rhs);
        numd = DirectFunctionCall2(numeric_add, lhsd, rhsd);

        agtv_result.type = AGTV_NUMERIC;
        agtv_result.val.numeric = DatumGetNumeric(numd);
    }
    else
        /* Not a covered case, error out */
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid input parameter types for agtype_add")));

    AG_RETURN_AGTYPE_P(agtype_value_to_agtype(&agtv_result));
}

PG_FUNCTION_INFO_V1(agtype_any_add);

/* agtype addition between bigint and agtype */
Datum agtype_any_add(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_add, AGTYPE_P_GET_DATUM(lhs),
                                             AGTYPE_P_GET_DATUM(rhs));

    AG_RETURN_AGTYPE_P(DATUM_GET_AGTYPE_P(result));
}

PG_FUNCTION_INFO_V1(agtype_sub);

/*
 * agtype subtraction function for - operator
 */
Datum agtype_sub(PG_FUNCTION_ARGS)
{
    agtype *lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *rhs = AG_GET_ARG_AGTYPE_P(1);
    agtype_value *agtv_lhs;
    agtype_value *agtv_rhs;
    agtype_value agtv_result;

    if (!(AGT_ROOT_IS_SCALAR(lhs)) || !(AGT_ROOT_IS_SCALAR(rhs)))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("must be scalar value, not array or object")));

        PG_RETURN_NULL();
    }

    agtv_lhs = get_ith_agtype_value_from_container(&lhs->root, 0);
    agtv_rhs = get_ith_agtype_value_from_container(&rhs->root, 0);

    if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_INTEGER;
        agtv_result.val.int_value = agtv_lhs->val.int_value -
                                    agtv_rhs->val.int_value;
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.float_value -
                                      agtv_rhs->val.float_value;
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.float_value -
                                      agtv_rhs->val.int_value;
    }
    else if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.int_value -
                                      agtv_rhs->val.float_value;
    }
    /* Is this a numeric result */
    else if (is_numeric_result(agtv_lhs, agtv_rhs))
    {
        Datum numd, lhsd, rhsd;

        lhsd = get_numeric_datum_from_agtype_value(agtv_lhs);
        rhsd = get_numeric_datum_from_agtype_value(agtv_rhs);
        numd = DirectFunctionCall2(numeric_sub, lhsd, rhsd);

        agtv_result.type = AGTV_NUMERIC;
        agtv_result.val.numeric = DatumGetNumeric(numd);
    }
    else
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid input parameter types for agtype_sub")));

    AG_RETURN_AGTYPE_P(agtype_value_to_agtype(&agtv_result));
}

PG_FUNCTION_INFO_V1(agtype_any_sub);

/* agtype subtraction between bigint and agtype */
Datum agtype_any_sub(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_sub, AGTYPE_P_GET_DATUM(lhs),
                                             AGTYPE_P_GET_DATUM(rhs));

    AG_RETURN_AGTYPE_P(DATUM_GET_AGTYPE_P(result));
}

PG_FUNCTION_INFO_V1(agtype_neg);

/*
 * agtype negation function for unary - operator
 */
Datum agtype_neg(PG_FUNCTION_ARGS)
{
    agtype *v = AG_GET_ARG_AGTYPE_P(0);
    agtype_value *agtv_value;
    agtype_value agtv_result;

    if (!(AGT_ROOT_IS_SCALAR(v)))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("must be scalar value, not array or object")));

        PG_RETURN_NULL();
    }

    agtv_value = get_ith_agtype_value_from_container(&v->root, 0);

    if (agtv_value->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_INTEGER;
        agtv_result.val.int_value = -agtv_value->val.int_value;
    }
    else if (agtv_value->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = -agtv_value->val.float_value;
    }
    else if (agtv_value->type == AGTV_NUMERIC)
    {
        Datum numd, vald;

        vald = NumericGetDatum(agtv_value->val.numeric);
        numd = DirectFunctionCall1(numeric_uminus, vald);

        agtv_result.type = AGTV_NUMERIC;
        agtv_result.val.numeric = DatumGetNumeric(numd);
    }
    else
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid input parameter type for agtype_neg")));

    AG_RETURN_AGTYPE_P(agtype_value_to_agtype(&agtv_result));
}

PG_FUNCTION_INFO_V1(agtype_mul);

/*
 * agtype multiplication function for * operator
 */
Datum agtype_mul(PG_FUNCTION_ARGS)
{
    agtype *lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *rhs = AG_GET_ARG_AGTYPE_P(1);
    agtype_value *agtv_lhs;
    agtype_value *agtv_rhs;
    agtype_value agtv_result;

    if (!(AGT_ROOT_IS_SCALAR(lhs)) || !(AGT_ROOT_IS_SCALAR(rhs)))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("must be scalar value, not array or object")));

        PG_RETURN_NULL();
    }

    agtv_lhs = get_ith_agtype_value_from_container(&lhs->root, 0);
    agtv_rhs = get_ith_agtype_value_from_container(&rhs->root, 0);

    if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_INTEGER;
        agtv_result.val.int_value = agtv_lhs->val.int_value *
                                    agtv_rhs->val.int_value;
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.float_value *
                                      agtv_rhs->val.float_value;
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.float_value *
                                      agtv_rhs->val.int_value;
    }
    else if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.int_value *
                                      agtv_rhs->val.float_value;
    }
    /* Is this a numeric result */
    else if (is_numeric_result(agtv_lhs, agtv_rhs))
    {
        Datum numd, lhsd, rhsd;

        lhsd = get_numeric_datum_from_agtype_value(agtv_lhs);
        rhsd = get_numeric_datum_from_agtype_value(agtv_rhs);
        numd = DirectFunctionCall2(numeric_mul, lhsd, rhsd);

        agtv_result.type = AGTV_NUMERIC;
        agtv_result.val.numeric = DatumGetNumeric(numd);
    }
    else
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid input parameter types for agtype_mul")));

    AG_RETURN_AGTYPE_P(agtype_value_to_agtype(&agtv_result));
}

PG_FUNCTION_INFO_V1(agtype_any_mul);

/* agtype multiplication between bigint and agtype */
Datum agtype_any_mul(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_mul, AGTYPE_P_GET_DATUM(lhs),
                                             AGTYPE_P_GET_DATUM(rhs));

    AG_RETURN_AGTYPE_P(DATUM_GET_AGTYPE_P(result));
}

PG_FUNCTION_INFO_V1(agtype_div);

/*
 * agtype division function for / operator
 */
Datum agtype_div(PG_FUNCTION_ARGS)
{
    agtype *lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *rhs = AG_GET_ARG_AGTYPE_P(1);
    agtype_value *agtv_lhs;
    agtype_value *agtv_rhs;
    agtype_value agtv_result;

    if (!(AGT_ROOT_IS_SCALAR(lhs)) || !(AGT_ROOT_IS_SCALAR(rhs)))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("must be scalar value, not array or object")));

        PG_RETURN_NULL();
    }

    agtv_lhs = get_ith_agtype_value_from_container(&lhs->root, 0);
    agtv_rhs = get_ith_agtype_value_from_container(&rhs->root, 0);

    if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_INTEGER)
    {
        if (agtv_rhs->val.int_value == 0)
        {
            ereport(ERROR, (errcode(ERRCODE_DIVISION_BY_ZERO),
                            errmsg("division by zero")));
            PG_RETURN_NULL();
        }

        agtv_result.type = AGTV_INTEGER;
        agtv_result.val.int_value = agtv_lhs->val.int_value /
                                    agtv_rhs->val.int_value;
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_FLOAT)
    {
        if (agtv_rhs->val.float_value == 0)
        {
            ereport(ERROR, (errcode(ERRCODE_DIVISION_BY_ZERO),
                            errmsg("division by zero")));
            PG_RETURN_NULL();
        }

        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.float_value /
                                      agtv_rhs->val.float_value;
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_INTEGER)
    {
        if (agtv_rhs->val.int_value == 0)
        {
            ereport(ERROR, (errcode(ERRCODE_DIVISION_BY_ZERO),
                            errmsg("division by zero")));
            PG_RETURN_NULL();
        }

        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.float_value /
                                      agtv_rhs->val.int_value;
    }
    else if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_FLOAT)
    {
        if (agtv_rhs->val.float_value == 0)
        {
            ereport(ERROR, (errcode(ERRCODE_DIVISION_BY_ZERO),
                            errmsg("division by zero")));
            PG_RETURN_NULL();
        }

        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = agtv_lhs->val.int_value /
                                      agtv_rhs->val.float_value;
    }
    /* Is this a numeric result */
    else if (is_numeric_result(agtv_lhs, agtv_rhs))
    {
        Datum numd, lhsd, rhsd;

        lhsd = get_numeric_datum_from_agtype_value(agtv_lhs);
        rhsd = get_numeric_datum_from_agtype_value(agtv_rhs);
        numd = DirectFunctionCall2(numeric_div, lhsd, rhsd);

        agtv_result.type = AGTV_NUMERIC;
        agtv_result.val.numeric = DatumGetNumeric(numd);
    }
    else
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid input parameter types for agtype_div")));

     AG_RETURN_AGTYPE_P(agtype_value_to_agtype(&agtv_result));
}

PG_FUNCTION_INFO_V1(agtype_any_div);

/* agtype division between bigint and agtype */
Datum agtype_any_div(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_div, AGTYPE_P_GET_DATUM(lhs),
                                             AGTYPE_P_GET_DATUM(rhs));

    AG_RETURN_AGTYPE_P(DATUM_GET_AGTYPE_P(result));
}

PG_FUNCTION_INFO_V1(agtype_mod);

/*
 * agtype modulo function for % operator
 */
Datum agtype_mod(PG_FUNCTION_ARGS)
{
    agtype *lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *rhs = AG_GET_ARG_AGTYPE_P(1);
    agtype_value *agtv_lhs;
    agtype_value *agtv_rhs;
    agtype_value agtv_result;

    if (!(AGT_ROOT_IS_SCALAR(lhs)) || !(AGT_ROOT_IS_SCALAR(rhs)))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("must be scalar value, not array or object")));

        PG_RETURN_NULL();
    }

    agtv_lhs = get_ith_agtype_value_from_container(&lhs->root, 0);
    agtv_rhs = get_ith_agtype_value_from_container(&rhs->root, 0);

    if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_INTEGER;
        agtv_result.val.int_value = agtv_lhs->val.int_value %
                                    agtv_rhs->val.int_value;
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = fmod(agtv_lhs->val.float_value,
                                           agtv_rhs->val.float_value);
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = fmod(agtv_lhs->val.float_value,
                                           agtv_rhs->val.int_value);
    }
    else if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = fmod(agtv_lhs->val.int_value,
                                           agtv_rhs->val.float_value);
    }
    /* Is this a numeric result */
    else if (is_numeric_result(agtv_lhs, agtv_rhs))
    {
        Datum numd, lhsd, rhsd;

        lhsd = get_numeric_datum_from_agtype_value(agtv_lhs);
        rhsd = get_numeric_datum_from_agtype_value(agtv_rhs);
        numd = DirectFunctionCall2(numeric_mod, lhsd, rhsd);

        agtv_result.type = AGTV_NUMERIC;
        agtv_result.val.numeric = DatumGetNumeric(numd);
    }
    else
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid input parameter types for agtype_mod")));

    AG_RETURN_AGTYPE_P(agtype_value_to_agtype(&agtv_result));
}

PG_FUNCTION_INFO_V1(agtype_any_mod);

/* agtype modulo between bigint and agtype */
Datum agtype_any_mod(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_mod, AGTYPE_P_GET_DATUM(lhs),
                                             AGTYPE_P_GET_DATUM(rhs));

    AG_RETURN_AGTYPE_P(DATUM_GET_AGTYPE_P(result));
}

PG_FUNCTION_INFO_V1(agtype_pow);

/*
 * agtype power function for ^ operator
 */
Datum agtype_pow(PG_FUNCTION_ARGS)
{
    agtype *lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *rhs = AG_GET_ARG_AGTYPE_P(1);
    agtype_value *agtv_lhs;
    agtype_value *agtv_rhs;
    agtype_value agtv_result;

    if (!(AGT_ROOT_IS_SCALAR(lhs)) || !(AGT_ROOT_IS_SCALAR(rhs)))
    {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("must be scalar value, not array or object")));

        PG_RETURN_NULL();
    }

    agtv_lhs = get_ith_agtype_value_from_container(&lhs->root, 0);
    agtv_rhs = get_ith_agtype_value_from_container(&rhs->root, 0);

    if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = pow(agtv_lhs->val.int_value,
                                          agtv_rhs->val.int_value);
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = pow(agtv_lhs->val.float_value,
                                          agtv_rhs->val.float_value);
    }
    else if (agtv_lhs->type == AGTV_FLOAT && agtv_rhs->type == AGTV_INTEGER)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = pow(agtv_lhs->val.float_value,
                                          agtv_rhs->val.int_value);
    }
    else if (agtv_lhs->type == AGTV_INTEGER && agtv_rhs->type == AGTV_FLOAT)
    {
        agtv_result.type = AGTV_FLOAT;
        agtv_result.val.float_value = pow(agtv_lhs->val.int_value,
                                          agtv_rhs->val.float_value);
    }
    /* Is this a numeric result */
    else if (is_numeric_result(agtv_lhs, agtv_rhs))
    {
        Datum numd, lhsd, rhsd;

        lhsd = get_numeric_datum_from_agtype_value(agtv_lhs);
        rhsd = get_numeric_datum_from_agtype_value(agtv_rhs);
        numd = DirectFunctionCall2(numeric_power, lhsd, rhsd);

        agtv_result.type = AGTV_NUMERIC;
        agtv_result.val.numeric = DatumGetNumeric(numd);
    }
    else
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid input parameter types for agtype_pow")));

    AG_RETURN_AGTYPE_P(agtype_value_to_agtype(&agtv_result));
}

PG_FUNCTION_INFO_V1(agtype_eq);

Datum agtype_eq(PG_FUNCTION_ARGS)
{
    agtype *agtype_lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *agtype_rhs = AG_GET_ARG_AGTYPE_P(1);
    bool result;

    result = (compare_agtype_containers_orderability(&agtype_lhs->root,
                                                     &agtype_rhs->root) == 0);

    PG_FREE_IF_COPY(agtype_lhs, 0);
    PG_FREE_IF_COPY(agtype_rhs, 1);

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_any_eq);

Datum agtype_any_eq(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_eq, AGTYPE_P_GET_DATUM(lhs),
                                            AGTYPE_P_GET_DATUM(rhs));

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_ne);

Datum agtype_ne(PG_FUNCTION_ARGS)
{
    agtype *agtype_lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *agtype_rhs = AG_GET_ARG_AGTYPE_P(1);
    bool result = true;

    result = (compare_agtype_containers_orderability(&agtype_lhs->root,
                                                     &agtype_rhs->root) != 0);

    PG_FREE_IF_COPY(agtype_lhs, 0);
    PG_FREE_IF_COPY(agtype_rhs, 1);

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_any_ne);

Datum agtype_any_ne(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_ne, AGTYPE_P_GET_DATUM(lhs),
                                            AGTYPE_P_GET_DATUM(rhs));

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_lt);

Datum agtype_lt(PG_FUNCTION_ARGS)
{
    agtype *agtype_lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *agtype_rhs = AG_GET_ARG_AGTYPE_P(1);
    bool result;

    result = (compare_agtype_containers_orderability(&agtype_lhs->root,
                                                     &agtype_rhs->root) < 0);

    PG_FREE_IF_COPY(agtype_lhs, 0);
    PG_FREE_IF_COPY(agtype_rhs, 1);

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_any_lt);

Datum agtype_any_lt(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_lt, AGTYPE_P_GET_DATUM(lhs),
                                            AGTYPE_P_GET_DATUM(rhs));

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_gt);

Datum agtype_gt(PG_FUNCTION_ARGS)
{
    agtype *agtype_lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *agtype_rhs = AG_GET_ARG_AGTYPE_P(1);
    bool result;

    result = (compare_agtype_containers_orderability(&agtype_lhs->root,
                                                     &agtype_rhs->root) > 0);

    PG_FREE_IF_COPY(agtype_lhs, 0);
    PG_FREE_IF_COPY(agtype_rhs, 1);

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_any_gt);

Datum agtype_any_gt(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_gt, AGTYPE_P_GET_DATUM(lhs),
                                            AGTYPE_P_GET_DATUM(rhs));

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_le);

Datum agtype_le(PG_FUNCTION_ARGS)
{
    agtype *agtype_lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *agtype_rhs = AG_GET_ARG_AGTYPE_P(1);
    bool result;

    result = (compare_agtype_containers_orderability(&agtype_lhs->root,
                                                     &agtype_rhs->root) <= 0);

    PG_FREE_IF_COPY(agtype_lhs, 0);
    PG_FREE_IF_COPY(agtype_rhs, 1);

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_any_le);

Datum agtype_any_le(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_le, AGTYPE_P_GET_DATUM(lhs),
                                            AGTYPE_P_GET_DATUM(rhs));

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_ge);

Datum agtype_ge(PG_FUNCTION_ARGS)
{
    agtype *agtype_lhs = AG_GET_ARG_AGTYPE_P(0);
    agtype *agtype_rhs = AG_GET_ARG_AGTYPE_P(1);
    bool result;

    result = (compare_agtype_containers_orderability(&agtype_lhs->root,
                                                     &agtype_rhs->root) >= 0);

    PG_FREE_IF_COPY(agtype_lhs, 0);
    PG_FREE_IF_COPY(agtype_rhs, 1);

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(agtype_any_ge);

Datum agtype_any_ge(PG_FUNCTION_ARGS)
{
    agtype *lhs;
    agtype *rhs;
    Datum result;

    lhs = get_one_agtype_from_variadic_args(fcinfo, 0, 2);
    rhs = get_one_agtype_from_variadic_args(fcinfo, 1, 1);

    if (lhs == NULL || rhs == NULL)
    {
        PG_RETURN_NULL();
    }

    result = DirectFunctionCall2(agtype_ge, AGTYPE_P_GET_DATUM(lhs),
                                            AGTYPE_P_GET_DATUM(rhs));

    PG_RETURN_BOOL(result);
}


static agtype *agtype_concat(agtype *agt1, agtype *agt2)
{
    agtype_parse_state *state = NULL;
    agtype_value *res;
    agtype_iterator *it1;
    agtype_iterator *it2;

    /*
     * If one of the agtype is empty, just return the other if it's not scalar
     * and both are of the same kind.  If it's a scalar or they are of
     * different kinds we need to perform the concatenation even if one is
     * empty.
     */
    if (AGT_ROOT_IS_OBJECT(agt1) == AGT_ROOT_IS_OBJECT(agt2))
    {
        if (AGT_ROOT_COUNT(agt1) == 0 && !AGT_ROOT_IS_SCALAR(agt2))
            return agt2;
        else if (AGT_ROOT_COUNT(agt2) == 0 && !AGT_ROOT_IS_SCALAR(agt1))
            return agt1;
    }

    it1 = agtype_iterator_init(&agt1->root);
    it2 = agtype_iterator_init(&agt2->root);

    res = iterator_concat(&it1, &it2, &state);

    Assert(res != NULL);

    return (agtype_value_to_agtype(res));
}

/*
 * Iterate over all agtype objects and merge them into one.
 * The logic of this function copied from the same hstore function,
 * except the case, when it1 & it2 represents jbvObject.
 * In that case we just append the content of it2 to it1 without any
 * verifications.
 */
static agtype_value *iterator_concat(agtype_iterator **it1,
                                     agtype_iterator **it2,
                                     agtype_parse_state **state)
{
    agtype_value v1, v2, *res = NULL;
    agtype_iterator_token r1, r2, rk1, rk2;

    r1 = rk1 = agtype_iterator_next(it1, &v1, false);
    r2 = rk2 = agtype_iterator_next(it2, &v2, false);

    /*
     * Both elements are objects.
     */
    if (rk1 == WAGT_BEGIN_OBJECT && rk2 == WAGT_BEGIN_OBJECT)
    {
        /*
         * Append the all tokens from v1 to res, except last WAGT_END_OBJECT
         * (because res will not be finished yet).
         */
        push_agtype_value(state, r1, NULL);
        while ((r1 = agtype_iterator_next(it1, &v1, true)) != WAGT_END_OBJECT)
            push_agtype_value(state, r1, &v1);

        /*
         * Append the all tokens from v2 to res, include last WAGT_END_OBJECT
         * (the concatenation will be completed).
         */
        while ((r2 = agtype_iterator_next(it2, &v2, true)) != 0)
            res = push_agtype_value(state, r2,
                                    r2 != WAGT_END_OBJECT ? &v2 : NULL);
    }

    /*
     * Both elements are arrays (either can be scalar).
     */
    else if (rk1 == WAGT_BEGIN_ARRAY && rk2 == WAGT_BEGIN_ARRAY)
    {
        push_agtype_value(state, r1, NULL);

        while ((r1 = agtype_iterator_next(it1, &v1, true)) != WAGT_END_ARRAY)
        {
            Assert(r1 == WAGT_ELEM);
            push_agtype_value(state, r1, &v1);
        }

        while ((r2 = agtype_iterator_next(it2, &v2, true)) != WAGT_END_ARRAY)
        {
            Assert(r2 == WAGT_ELEM);
            push_agtype_value(state, WAGT_ELEM, &v2);
        }

        res = push_agtype_value(state, WAGT_END_ARRAY,
                                NULL /* signal to sort */);
    }
    /* have we got array || object or object || array? */
    else if (((rk1 == WAGT_BEGIN_ARRAY && !(*it1)->is_scalar) &&
              rk2 == WAGT_BEGIN_OBJECT) ||
             (rk1 == WAGT_BEGIN_OBJECT &&
              (rk2 == WAGT_BEGIN_ARRAY && !(*it2)->is_scalar)))
    {
        agtype_iterator **it_array = rk1 == WAGT_BEGIN_ARRAY ? it1 : it2;
        agtype_iterator **it_object = rk1 == WAGT_BEGIN_OBJECT ? it1 : it2;

        bool prepend = (rk1 == WAGT_BEGIN_OBJECT);

        push_agtype_value(state, WAGT_BEGIN_ARRAY, NULL);

        if (prepend)
        {
            push_agtype_value(state, WAGT_BEGIN_OBJECT, NULL);
            while ((r1 = agtype_iterator_next(it_object, &v1, true)) != 0)
                push_agtype_value(state, r1,
                                  r1 != WAGT_END_OBJECT ? &v1 : NULL);

            while ((r2 = agtype_iterator_next(it_array, &v2, true)) != 0)
                res = push_agtype_value(state, r2,
                                        r2 != WAGT_END_ARRAY ? &v2 : NULL);
        }
        else
        {
            while ((r1 = agtype_iterator_next(it_array, &v1, true)) !=
                   WAGT_END_ARRAY)
                push_agtype_value(state, r1, &v1);

            push_agtype_value(state, WAGT_BEGIN_OBJECT, NULL);
            while ((r2 = agtype_iterator_next(it_object, &v2, true)) != 0)
                push_agtype_value(state, r2,
                                  r2 != WAGT_END_OBJECT ? &v2 : NULL);

            res = push_agtype_value(state, WAGT_END_ARRAY, NULL);
        }
    }
    else
    {
        /*
         * This must be scalar || object or object || scalar, as that's all
         * that's left. Both of these make no sense, so error out.
         */
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("invalid concatenation of agtype objects")));
    }

    return res;
}

static void ereport_op_str(const char *op, agtype *lhs, agtype *rhs)
{
    const char *msgfmt;
    const char *lstr;
    const char *rstr;

    AssertArg(rhs != NULL);

    if (lhs == NULL)
    {
        msgfmt = "invalid expression: %s%s%s";
        lstr = "";
    }
    else
    {
        msgfmt = "invalid expression: %s %s %s";
        lstr = agtype_to_cstring(NULL, &lhs->root, VARSIZE(lhs));
    }
    rstr = agtype_to_cstring(NULL, &rhs->root, VARSIZE(rhs));

    ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg(msgfmt, lstr, op, rstr)));
}
