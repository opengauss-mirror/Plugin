CREATE SCHEMA dolphin_catalog;

/* int4 */
create function dolphin_catalog.dolphin_int4pl (
int4,
int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4pl';
create operator dolphin_catalog.+(leftarg = int4, rightarg = int4, procedure = dolphin_catalog.dolphin_int4pl);

create function dolphin_catalog.dolphin_int4mi (
int4,
int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4mi';
create operator dolphin_catalog.-(leftarg = int4, rightarg = int4, procedure = dolphin_catalog.dolphin_int4mi);

create function dolphin_catalog.dolphin_int4mul (
int4,
int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4mul';
create operator dolphin_catalog.*(leftarg = int4, rightarg = int4, procedure = dolphin_catalog.dolphin_int4mul);

create function dolphin_catalog.dolphin_int4div (
int4,
int4
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4div';
create operator dolphin_catalog./(leftarg = int4, rightarg = int4, procedure = dolphin_catalog.dolphin_int4div);

/* int2 */
create function dolphin_catalog.dolphin_int2pl (
int2,
int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2pl';
create operator dolphin_catalog.+(leftarg = int2, rightarg = int2, procedure = dolphin_catalog.dolphin_int2pl);

create function dolphin_catalog.dolphin_int2mi (
int2,
int2
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2mi';
create operator dolphin_catalog.-(leftarg = int2, rightarg = int2, procedure = dolphin_catalog.dolphin_int2mi);

create function dolphin_catalog.dolphin_int2mul (
int2,
int2
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2mul';
create operator dolphin_catalog.*(leftarg = int2, rightarg = int2, procedure = dolphin_catalog.dolphin_int2mul);

create function dolphin_catalog.dolphin_int2div (
int2,
int2
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2div';
create operator dolphin_catalog./(leftarg = int2, rightarg = int2, procedure = dolphin_catalog.dolphin_int2div);

/* int1 */
create function dolphin_catalog.dolphin_int1pl (
int1,
int1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1pl';
create operator dolphin_catalog.+(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1pl);

create function dolphin_catalog.dolphin_int1mi (
int1,
int1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1mi';
create operator dolphin_catalog.-(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1mi);

create function dolphin_catalog.dolphin_int1mul (
int1,
int1
) RETURNS int4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1mul';
create operator dolphin_catalog.*(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1mul);

create function dolphin_catalog.dolphin_int1div (
int1,
int1
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1div';
create operator dolphin_catalog./(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1div);

CREATE OPERATOR pg_catalog./(leftarg = int1, rightarg = int1, procedure = dolphin_catalog.dolphin_int1div);
COMMENT ON OPERATOR pg_catalog./(int1, int1) IS 'dolphin_int1div';

/* int8 */
create function dolphin_catalog.dolphin_int8pl (
int8,
int8
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int8pl';
create operator dolphin_catalog.+(leftarg = int8, rightarg = int8, procedure = dolphin_catalog.dolphin_int8pl);

create function dolphin_catalog.dolphin_int8mi (
int8,
int8
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int8mi';
create operator dolphin_catalog.-(leftarg = int8, rightarg = int8, procedure = dolphin_catalog.dolphin_int8mi);

create function dolphin_catalog.dolphin_int8mul (
int8,
int8
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int8mul';
create operator dolphin_catalog.*(leftarg = int8, rightarg = int8, procedure = dolphin_catalog.dolphin_int8mul);

create function dolphin_catalog.dolphin_int8div (
int8,
int8
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int8div';
create operator dolphin_catalog./(leftarg = int8, rightarg = int8, procedure = dolphin_catalog.dolphin_int8div);

/* uint1 */
create function dolphin_catalog.dolphin_uint1pl (
uint1,
uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1pl';
create operator dolphin_catalog.+(leftarg = uint1, rightarg = uint1, procedure = dolphin_catalog.dolphin_uint1pl);

create function dolphin_catalog.dolphin_uint1mi (
uint1,
uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1mi';
create operator dolphin_catalog.-(leftarg = uint1, rightarg = uint1, procedure = dolphin_catalog.dolphin_uint1mi);

create function dolphin_catalog.dolphin_uint1mul (
uint1,
uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1mul';
create operator dolphin_catalog.*(leftarg = uint1, rightarg = uint1, procedure = dolphin_catalog.dolphin_uint1mul);

create function dolphin_catalog.dolphin_uint1div (
uint1,
uint1
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1div';
create operator dolphin_catalog./(leftarg = uint1, rightarg = uint1, procedure = dolphin_catalog.dolphin_uint1div);

/* int1_uint1 */
create function dolphin_catalog.dolphin_int1_pl_uint1 (
int1,
uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1_pl_uint1';
create operator dolphin_catalog.+(leftarg = int1, rightarg = uint1, procedure = dolphin_catalog.dolphin_int1_pl_uint1);

create function dolphin_catalog.dolphin_int1_mi_uint1 (
int1,
uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1_mi_uint1';
create operator dolphin_catalog.-(leftarg = int1, rightarg = uint1, procedure = dolphin_catalog.dolphin_int1_mi_uint1);

create function dolphin_catalog.dolphin_int1_mul_uint1 (
int1,
uint1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1_mul_uint1';
create operator dolphin_catalog.*(leftarg = int1, rightarg = uint1, procedure = dolphin_catalog.dolphin_int1_mul_uint1);

create function dolphin_catalog.dolphin_int1_div_uint1 (
int1,
uint1
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int1_div_uint1';
create operator dolphin_catalog./(leftarg = int1, rightarg = uint1, procedure = dolphin_catalog.dolphin_int1_div_uint1);

/* uint1_int1 */
create function dolphin_catalog.dolphin_uint1_pl_int1 (
uint1,
int1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1_pl_int1';
create operator dolphin_catalog.+(leftarg = uint1, rightarg = int1, procedure = dolphin_catalog.dolphin_uint1_pl_int1);

create function dolphin_catalog.dolphin_uint1_mi_int1 (
uint1,
int1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1_mi_int1';
create operator dolphin_catalog.-(leftarg = uint1, rightarg = int1, procedure = dolphin_catalog.dolphin_uint1_mi_int1);

create function dolphin_catalog.dolphin_uint1_mul_int1 (
uint1,
int1
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1_mul_int1';
create operator dolphin_catalog.*(leftarg = uint1, rightarg = int1, procedure = dolphin_catalog.dolphin_uint1_mul_int1);

create function dolphin_catalog.dolphin_uint1_div_int1 (
uint1,
int1
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint1_div_int1';
create operator dolphin_catalog./(leftarg = uint1, rightarg = int1, procedure = dolphin_catalog.dolphin_uint1_div_int1);

/* int2_int4 */
create function dolphin_catalog.dolphin_int24pl (
int2,
int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int24pl';
create operator dolphin_catalog.+(leftarg = int2, rightarg = int4, procedure = dolphin_catalog.dolphin_int24pl);

create function dolphin_catalog.dolphin_int24mi (
int2,
int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int24mi';
create operator dolphin_catalog.-(leftarg = int2, rightarg = int4, procedure = dolphin_catalog.dolphin_int24mi);

create function dolphin_catalog.dolphin_int24mul (
int2,
int4
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int24mul';
create operator dolphin_catalog.*(leftarg = int2, rightarg = int4, procedure = dolphin_catalog.dolphin_int24mul);

create function dolphin_catalog.dolphin_int24div (
int2,
int4
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int24div';
create operator dolphin_catalog./(leftarg = int2, rightarg = int4, procedure = dolphin_catalog.dolphin_int24div);

/* int2_int8 */
create function dolphin_catalog.dolphin_int28pl (
int2,
int8
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int28pl';
create operator dolphin_catalog.+(leftarg = int2, rightarg = int8, procedure = dolphin_catalog.dolphin_int28pl);

create function dolphin_catalog.dolphin_int28mi (
int2,
int8
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int28mi';
create operator dolphin_catalog.-(leftarg = int2, rightarg = int8, procedure = dolphin_catalog.dolphin_int28mi);

create function dolphin_catalog.dolphin_int28mul (
int2,
int8
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int28mul';
create operator dolphin_catalog.*(leftarg = int2, rightarg = int8, procedure = dolphin_catalog.dolphin_int28mul);

create function dolphin_catalog.dolphin_int28div (
int2,
int8
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int28div';
create operator dolphin_catalog./(leftarg = int2, rightarg = int8, procedure = dolphin_catalog.dolphin_int28div);

/* int2_uint2 */
create function dolphin_catalog.dolphin_int2_pl_uint2 (
int2,
uint2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2_pl_uint2';
create operator dolphin_catalog.+(leftarg = int2, rightarg = uint2, procedure = dolphin_catalog.dolphin_int2_pl_uint2);

create function dolphin_catalog.dolphin_int2_mi_uint2 (
int2,
uint2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2_mi_uint2';
create operator dolphin_catalog.-(leftarg = int2, rightarg = uint2, procedure = dolphin_catalog.dolphin_int2_mi_uint2);

create function dolphin_catalog.dolphin_int2_mul_uint2 (
int2,
uint2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2_mul_uint2';
create operator dolphin_catalog.*(leftarg = int2, rightarg = uint2, procedure = dolphin_catalog.dolphin_int2_mul_uint2);

create function dolphin_catalog.dolphin_int2_div_uint2 (
int2,
uint2
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int2_div_uint2';
create operator dolphin_catalog./(leftarg = int2, rightarg = uint2, procedure = dolphin_catalog.dolphin_int2_div_uint2);

/* int4_int2 */
create function dolphin_catalog.dolphin_int42pl (
int4,
int2
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int42pl';
create operator dolphin_catalog.+(leftarg = int4, rightarg = int2, procedure = dolphin_catalog.dolphin_int42pl);

create function dolphin_catalog.dolphin_int42mi (
int4,
int2
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int42mi';
create operator dolphin_catalog.-(leftarg = int4, rightarg = int2, procedure = dolphin_catalog.dolphin_int42mi);

create function dolphin_catalog.dolphin_int42mul (
int4,
int2
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int42mul';
create operator dolphin_catalog.*(leftarg = int4, rightarg = int2, procedure = dolphin_catalog.dolphin_int42mul);

create function dolphin_catalog.dolphin_int42div (
int4,
int2
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int42div';
create operator dolphin_catalog./(leftarg = int4, rightarg = int2, procedure = dolphin_catalog.dolphin_int42div);

/* int4_int8 */
create function dolphin_catalog.dolphin_int48pl (
int4,
int8
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int48pl';
create operator dolphin_catalog.+(leftarg = int4, rightarg = int8, procedure = dolphin_catalog.dolphin_int48pl);

create function dolphin_catalog.dolphin_int48mi (
int4,
int8
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int48mi';
create operator dolphin_catalog.-(leftarg = int4, rightarg = int8, procedure = dolphin_catalog.dolphin_int48mi);

create function dolphin_catalog.dolphin_int48mul (
int4,
int8
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int48mul';
create operator dolphin_catalog.*(leftarg = int4, rightarg = int8, procedure = dolphin_catalog.dolphin_int48mul);

create function dolphin_catalog.dolphin_int48div (
int4,
int8
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int48div';
create operator dolphin_catalog./(leftarg = int4, rightarg = int8, procedure = dolphin_catalog.dolphin_int48div);

/* int4_uint4 */
create function dolphin_catalog.dolphin_int4_pl_uint4 (
int4,
uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4_pl_uint4';
create operator dolphin_catalog.+(leftarg = int4, rightarg = uint4, procedure = dolphin_catalog.dolphin_int4_pl_uint4);

create function dolphin_catalog.dolphin_int4_mi_uint4 (
int4,
uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4_mi_uint4';
create operator dolphin_catalog.-(leftarg = int4, rightarg = uint4, procedure = dolphin_catalog.dolphin_int4_mi_uint4);

create function dolphin_catalog.dolphin_int4_mul_uint4 (
int4,
uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4_mul_uint4';
create operator dolphin_catalog.*(leftarg = int4, rightarg = uint4, procedure = dolphin_catalog.dolphin_int4_mul_uint4);

create function dolphin_catalog.dolphin_int4_div_uint4 (
int4,
uint4
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int4_div_uint4';
create operator dolphin_catalog./(leftarg = int4, rightarg = uint4, procedure = dolphin_catalog.dolphin_int4_div_uint4);

/* int8_int2 */
create function dolphin_catalog.dolphin_int82pl (
int8,
int2
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int82pl';
create operator dolphin_catalog.+(leftarg = int8, rightarg = int2, procedure = dolphin_catalog.dolphin_int82pl);

create function dolphin_catalog.dolphin_int82mi (
int8,
int2
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int82mi';
create operator dolphin_catalog.-(leftarg = int8, rightarg = int2, procedure = dolphin_catalog.dolphin_int82mi);

create function dolphin_catalog.dolphin_int82mul (
int8,
int2
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int82mul';
create operator dolphin_catalog.*(leftarg = int8, rightarg = int2, procedure = dolphin_catalog.dolphin_int82mul);

create function dolphin_catalog.dolphin_int82div (
int8,
int2
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int82div';
create operator dolphin_catalog./(leftarg = int8, rightarg = int2, procedure = dolphin_catalog.dolphin_int82div);

/* int8_int4 */
create function dolphin_catalog.dolphin_int84pl (
int8,
int4
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int84pl';
create operator dolphin_catalog.+(leftarg = int8, rightarg = int4, procedure = dolphin_catalog.dolphin_int84pl);

create function dolphin_catalog.dolphin_int84mi (
int8,
int4
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int84mi';
create operator dolphin_catalog.-(leftarg = int8, rightarg = int4, procedure = dolphin_catalog.dolphin_int84mi);

create function dolphin_catalog.dolphin_int84mul (
int8,
int4
) RETURNS int8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'int84mul';
create operator dolphin_catalog.*(leftarg = int8, rightarg = int4, procedure = dolphin_catalog.dolphin_int84mul);

create function dolphin_catalog.dolphin_int84div (
int8,
int4
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int84div';
create operator dolphin_catalog./(leftarg = int8, rightarg = int4, procedure = dolphin_catalog.dolphin_int84div);

/* uint2_int2 */
create function dolphin_catalog.dolphin_uint2_pl_int2 (
uint2,
int2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2_pl_int2';
create operator dolphin_catalog.+(leftarg = uint2, rightarg = int2, procedure = dolphin_catalog.dolphin_uint2_pl_int2);

create function dolphin_catalog.dolphin_uint2_mi_int2 (
uint2,
int2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2_mi_int2';
create operator dolphin_catalog.-(leftarg = uint2, rightarg = int2, procedure = dolphin_catalog.dolphin_uint2_mi_int2);

create function dolphin_catalog.dolphin_uint2_mul_int2 (
uint2,
int2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2_mul_int2';
create operator dolphin_catalog.*(leftarg = uint2, rightarg = int2, procedure = dolphin_catalog.dolphin_uint2_mul_int2);

create function dolphin_catalog.dolphin_uint2_div_int2 (
uint2,
int2
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2_div_int2';
create operator dolphin_catalog./(leftarg = uint2, rightarg = int2, procedure = dolphin_catalog.dolphin_uint2_div_int2);

/* uint2 */
create function dolphin_catalog.dolphin_uint2pl (
uint2,
uint2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2pl';
create operator dolphin_catalog.+(leftarg = uint2, rightarg = uint2, procedure = dolphin_catalog.dolphin_uint2pl);

create function dolphin_catalog.dolphin_uint2mi (
uint2,
uint2
) RETURNS uint4 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2mi';
create operator dolphin_catalog.-(leftarg = uint2, rightarg = uint2, procedure = dolphin_catalog.dolphin_uint2mi);

create function dolphin_catalog.dolphin_uint2mul (
uint2,
uint2
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2mul';
create operator dolphin_catalog.*(leftarg = uint2, rightarg = uint2, procedure = dolphin_catalog.dolphin_uint2mul);

create function dolphin_catalog.dolphin_uint2div (
uint2,
uint2
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint2div';
create operator dolphin_catalog./(leftarg = uint2, rightarg = uint2, procedure = dolphin_catalog.dolphin_uint2div);

/* uint4 */
create function dolphin_catalog.dolphin_uint4pl (
uint4,
uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4pl';
create operator dolphin_catalog.+(leftarg = uint4, rightarg = uint4, procedure = dolphin_catalog.dolphin_uint4pl);

create function dolphin_catalog.dolphin_uint4mi (
uint4,
uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4mi';
create operator dolphin_catalog.-(leftarg = uint4, rightarg = uint4, procedure = dolphin_catalog.dolphin_uint4mi);

create function dolphin_catalog.dolphin_uint4mul (
uint4,
uint4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4mul';
create operator dolphin_catalog.*(leftarg = uint4, rightarg = uint4, procedure = dolphin_catalog.dolphin_uint4mul);

create function dolphin_catalog.dolphin_uint4div (
uint4,
uint4
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4div';
create operator dolphin_catalog./(leftarg = uint4, rightarg = uint4, procedure = dolphin_catalog.dolphin_uint4div);

/* uint4_int4 */
create function dolphin_catalog.dolphin_uint4_pl_int4 (
uint4,
int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4_pl_int4';
create operator dolphin_catalog.+(leftarg = uint4, rightarg = int4, procedure = dolphin_catalog.dolphin_uint4_pl_int4);

create function dolphin_catalog.dolphin_uint4_mi_int4 (
uint4,
int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4_mi_int4';
create operator dolphin_catalog.-(leftarg = uint4, rightarg = int4, procedure = dolphin_catalog.dolphin_uint4_mi_int4);

create function dolphin_catalog.dolphin_uint4_mul_int4 (
uint4,
int4
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4_mul_int4';
create operator dolphin_catalog.*(leftarg = uint4, rightarg = int4, procedure = dolphin_catalog.dolphin_uint4_mul_int4);

create function dolphin_catalog.dolphin_uint4_div_int4 (
uint4,
int4
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint4_div_int4';
create operator dolphin_catalog./(leftarg = uint4, rightarg = int4, procedure = dolphin_catalog.dolphin_uint4_div_int4);

/* float4 */
create function dolphin_catalog.dolphin_float4pl (
float4,
float4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_float4pl';
create operator dolphin_catalog.+(leftarg = float4, rightarg = float4, procedure = dolphin_catalog.dolphin_float4pl);

create function dolphin_catalog.dolphin_float4mi (
float4,
float4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_float4mi';
create operator dolphin_catalog.-(leftarg = float4, rightarg = float4, procedure = dolphin_catalog.dolphin_float4mi);

create function dolphin_catalog.dolphin_float4mul (
float4,
float4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_float4mul';
create operator dolphin_catalog.*(leftarg = float4, rightarg = float4, procedure = dolphin_catalog.dolphin_float4mul);

create function dolphin_catalog.dolphin_float4div (
float4,
float4
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_float4div';
create operator dolphin_catalog./(leftarg = float4, rightarg = float4, procedure = dolphin_catalog.dolphin_float4div);

/* float4_float8 */
create function dolphin_catalog.dolphin_float48pl (
float4,
float8
) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float48pl';
create operator dolphin_catalog.+(leftarg = float4, rightarg = float8, procedure = dolphin_catalog.dolphin_float48pl);

create function dolphin_catalog.dolphin_float48mi (
float4,
float8
) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float48mi';
create operator dolphin_catalog.-(leftarg = float4, rightarg = float8, procedure = dolphin_catalog.dolphin_float48mi);

create function dolphin_catalog.dolphin_float48mul (
float4,
float8
) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float48mul';
create operator dolphin_catalog.*(leftarg = float4, rightarg = float8, procedure = dolphin_catalog.dolphin_float48mul);

create function dolphin_catalog.dolphin_float48div (
float4,
float8
) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float48div';
create operator dolphin_catalog./(leftarg = float4, rightarg = float8, procedure = dolphin_catalog.dolphin_float48div);

/* float8_float4 */
create function dolphin_catalog.dolphin_float84pl (
float8,
float4
) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float84pl';
create operator dolphin_catalog.+(leftarg = float8, rightarg = float4, procedure = dolphin_catalog.dolphin_float84pl);

create function dolphin_catalog.dolphin_float84mi (
float8,
float4
) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float84mi';
create operator dolphin_catalog.-(leftarg = float8, rightarg = float4, procedure = dolphin_catalog.dolphin_float84mi);

create function dolphin_catalog.dolphin_float84mul (
float8,
float4
) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float84mul';
create operator dolphin_catalog.*(leftarg = float8, rightarg = float4, procedure = dolphin_catalog.dolphin_float84mul);

create function dolphin_catalog.dolphin_float84div (
float8,
float4
) RETURNS float8 LANGUAGE INTERNAL IMMUTABLE STRICT as 'float84div';
create operator dolphin_catalog./(leftarg = float8, rightarg = float4, procedure = dolphin_catalog.dolphin_float84div);

/* uint8_int8 */
CREATE FUNCTION dolphin_catalog.dolphin_uint8_pl_int8 (
uint8,
int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_pl_int8';
CREATE OPERATOR dolphin_catalog.+(leftarg = uint8, rightarg = int8, procedure = dolphin_catalog.dolphin_uint8_pl_int8);

CREATE FUNCTION dolphin_catalog.dolphin_uint8_mi_int8 (
uint8,
int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mi_int8';
CREATE OPERATOR dolphin_catalog.-(leftarg = uint8, rightarg = int8, procedure = dolphin_catalog.dolphin_uint8_mi_int8);

CREATE FUNCTION dolphin_catalog.dolphin_uint8_mul_int8 (
uint8,
int8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'uint8_mul_int8';
CREATE OPERATOR dolphin_catalog.*(leftarg = uint8, rightarg = int8, procedure = dolphin_catalog.dolphin_uint8_mul_int8);

CREATE FUNCTION dolphin_catalog.dolphin_uint8_div_int8 (
uint8,
int8
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_uint8_div_int8';
CREATE OPERATOR dolphin_catalog./(leftarg = uint8, rightarg = int8, procedure = dolphin_catalog.dolphin_uint8_div_int8);

/* int8_uint8 */
CREATE FUNCTION dolphin_catalog.dolphin_int8_pl_uint8 (
int8,
uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_pl_uint8';
CREATE OPERATOR dolphin_catalog.+(leftarg = int8, rightarg = uint8, procedure = dolphin_catalog.dolphin_int8_pl_uint8);

CREATE FUNCTION dolphin_catalog.dolphin_int8_mi_uint8 (
int8,
uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mi_uint8';
CREATE OPERATOR dolphin_catalog.-(leftarg = int8, rightarg = uint8, procedure = dolphin_catalog.dolphin_int8_mi_uint8);

CREATE FUNCTION dolphin_catalog.dolphin_int8_mul_uint8 (
int8,
uint8
) RETURNS uint8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'int8_mul_uint8';
CREATE OPERATOR dolphin_catalog.*(leftarg = int8, rightarg = uint8, procedure = dolphin_catalog.dolphin_int8_mul_uint8);

create function dolphin_catalog.dolphin_int8_div_uint8 (
int8,
uint8
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dolphin_int8_div_uint8';
create operator dolphin_catalog./(leftarg = int8, rightarg = uint8, procedure = dolphin_catalog.dolphin_int8_div_uint8);

/* float8 */
create operator dolphin_catalog.+(leftarg = float8, rightarg = float8, procedure = float8pl);
create operator dolphin_catalog.-(leftarg = float8, rightarg = float8, procedure = float8mi);
create operator dolphin_catalog.*(leftarg = float8, rightarg = float8, procedure = float8mul);
create operator dolphin_catalog./(leftarg = float8, rightarg = float8, procedure = float8div);

/* uint8 */
create operator dolphin_catalog.+(leftarg = uint8, rightarg = uint8, procedure = uint8pl);
create operator dolphin_catalog.-(leftarg = uint8, rightarg = uint8, procedure = uint8mi);
create operator dolphin_catalog.*(leftarg = uint8, rightarg = uint8, procedure = uint8mul);
create function dolphin_catalog.dolphin_uint8div (
uint8,
uint8
) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'dolphin_uint8div';
create operator dolphin_catalog./(leftarg = uint8, rightarg = uint8, procedure = dolphin_catalog.dolphin_uint8div);

/* numeric */
create operator dolphin_catalog.+(leftarg = numeric, rightarg = numeric, procedure = numeric_add);
create operator dolphin_catalog.-(leftarg = numeric, rightarg = numeric, procedure = numeric_sub);
create operator dolphin_catalog.*(leftarg = numeric, rightarg = numeric, procedure = numeric_mul);
create operator dolphin_catalog./(leftarg = numeric, rightarg = numeric, procedure = numeric_div);

CREATE FUNCTION pg_catalog.op_datexor(date, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.datexor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = date, rightarg = date, procedure = pg_catalog.op_datexor);
CREATE FUNCTION pg_catalog.op_timexor(time, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timexor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = time, rightarg = time, procedure = pg_catalog.op_timexor);
CREATE FUNCTION pg_catalog.op_date_time_xor(date, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_time_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = date, rightarg = time, procedure = pg_catalog.op_date_time_xor);
CREATE FUNCTION pg_catalog.op_time_date_xor(time, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.time_date_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = time, rightarg = date, procedure = pg_catalog.op_time_date_xor);
CREATE FUNCTION pg_catalog.op_time_text_xor(time, text) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.time_text_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = time, rightarg = text, procedure = pg_catalog.op_time_text_xor);
CREATE FUNCTION pg_catalog.op_text_time_xor(text, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.text_time_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = text, rightarg = time, procedure = pg_catalog.op_text_time_xor);
CREATE FUNCTION pg_catalog.op_date_text_xor(date, text) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_text_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = date, rightarg = text, procedure = pg_catalog.op_date_text_xor);
CREATE FUNCTION pg_catalog.op_text_date_xor(text, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.text_date_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = text, rightarg = date, procedure = pg_catalog.op_text_date_xor);
CREATE FUNCTION pg_catalog.op_date_int8_xor(date, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_int8_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = date, rightarg = int8, procedure = pg_catalog.op_date_int8_xor);
CREATE FUNCTION pg_catalog.op_int8_date_xor(int8, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_date_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = int8, rightarg = date, procedure = pg_catalog.op_int8_date_xor);
CREATE FUNCTION pg_catalog.op_time_int8_xor(time, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.time_int8_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = time, rightarg = int8, procedure = pg_catalog.op_time_int8_xor);
CREATE FUNCTION pg_catalog.op_int8_time_xor(int8, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_time_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = int8, rightarg = time, procedure = pg_catalog.op_int8_time_xor);
CREATE FUNCTION pg_catalog.op_date_float8_xor(date, float8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.date_float8_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = date, rightarg = float8, procedure = pg_catalog.op_date_float8_xor);
CREATE FUNCTION pg_catalog.op_float8_date_xor(float8, date) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.float8_date_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = float8, rightarg = date, procedure = pg_catalog.op_float8_date_xor);
CREATE FUNCTION pg_catalog.op_timestampxor(timestamp, timestamp) 
RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestampxor($1, $2)::uint8';
CREATE OPERATOR pg_catalog.^(leftarg = timestamp, rightarg = timestamp, procedure = pg_catalog.op_timestampxor);
CREATE FUNCTION pg_catalog.op_timestamp_int8_xor(timestamp, int8) 
RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamp_int8_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = timestamp, rightarg = int8, procedure = pg_catalog.op_timestamp_int8_xor);
CREATE FUNCTION pg_catalog.op_int8_timestamp_xor(int8, timestamp) 
RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_timestamp_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = int8, rightarg = timestamp, procedure = pg_catalog.op_int8_timestamp_xor);
CREATE FUNCTION pg_catalog.op_timestamp_float8_xor(timestamp, float8) 
RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamp_float8_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = timestamp, rightarg = float8, procedure = pg_catalog.op_timestamp_float8_xor);
CREATE FUNCTION pg_catalog.op_float8_timestamp_xor(float8, timestamp) 
RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.float8_timestamp_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = float8, rightarg = timestamp, procedure = pg_catalog.op_float8_timestamp_xor);
CREATE FUNCTION pg_catalog.op_timestamp_text_xor(timestamp, text) 
RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamp_text_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = timestamp, rightarg = text, procedure = pg_catalog.op_timestamp_text_xor);
CREATE FUNCTION pg_catalog.op_text_timestamp_xor(text, timestamp) 
RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.text_timestamp_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = text, rightarg = timestamp, procedure = pg_catalog.op_text_timestamp_xor);
CREATE FUNCTION pg_catalog.op_timestamptzxor(timestampTz, timestampTz) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamptzxor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = timestampTz, rightarg = timestampTz, procedure = pg_catalog.op_timestamptzxor);
CREATE FUNCTION pg_catalog.op_timestamptz_int8_xor(timestampTz, int8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamptz_int8_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = timestampTz, rightarg = int8, procedure = pg_catalog.op_timestamptz_int8_xor);
CREATE FUNCTION pg_catalog.op_int8_timestamptz_xor(int8, timestampTz) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_timestamptz_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = int8, rightarg = timestampTz, procedure = pg_catalog.op_int8_timestamptz_xor);
CREATE FUNCTION pg_catalog.op_timestamptz_float8_xor(timestampTz, float8) 
RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamptz_float8_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = timestampTz, rightarg = float8, procedure = pg_catalog.op_timestamptz_float8_xor);
CREATE FUNCTION pg_catalog.op_float8_timestamptz_xor(float8, timestampTz) 
RETURNS uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.float8_timestamptz_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = float8, rightarg = timestampTz, procedure = pg_catalog.op_float8_timestamptz_xor);
CREATE FUNCTION pg_catalog.op_timestamptz_text_xor(timestampTz, text) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.timestamptz_text_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = timestampTz, rightarg = text, procedure = pg_catalog.op_timestamptz_text_xor);
CREATE FUNCTION pg_catalog.op_text_timestamptz_xor(text, timestampTz) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.text_timestamptz_xor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = text, rightarg = timestampTz, procedure = pg_catalog.op_text_timestamptz_xor);
CREATE FUNCTION pg_catalog.op_textxor(text, text) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.textxor($1, $2)::uint8';
create operator pg_catalog.^(leftarg = text, rightarg = text, procedure = pg_catalog.op_textxor);
CREATE FUNCTION pg_catalog.op_boolxor(boolean, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int1xor($1::int1, $2::int1)::uint8';
CREATE OPERATOR pg_catalog.^(leftarg = boolean, rightarg = boolean, procedure = pg_catalog.op_boolxor);
CREATE FUNCTION pg_catalog.booltofloat8(bool) RETURNS float8 LANGUAGE C STRICT AS  '$libdir/dolphin',  'bool_float8';
CREATE CAST (bool AS float8) WITH FUNCTION pg_catalog.booltofloat8(bool) AS ASSIGNMENT;
CREATE FUNCTION pg_catalog.op_bool_float8_xor(boolean, float8) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.dpow($1::float8, $2)::uint8';
create operator pg_catalog.^(leftarg = boolean, rightarg = float8, procedure = pg_catalog.op_bool_float8_xor);
CREATE FUNCTION pg_catalog.op_float8_bool_xor(float8, boolean) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.dpow($1, $2::float8)::uint8';
create operator pg_catalog.^(leftarg = float8, rightarg = boolean, procedure = pg_catalog.op_float8_bool_xor);
