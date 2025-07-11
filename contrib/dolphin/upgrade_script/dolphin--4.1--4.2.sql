CREATE or replace FUNCTION pg_catalog.replace(json, text, text) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.replace($1::text, $2, $3)';

-- xor between bool and bit,time
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_bit_xor(bool, bit) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8xor($1::int8, $2::int8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bit_bool_xor(bit, bool) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8xor($1::int8, $2::int8)::uint8';
CREATE OPERATOR pg_catalog.^ (leftarg = bool, rightarg = bit, procedure = pg_catalog.op_bool_bit_xor);
CREATE OPERATOR pg_catalog.^ (leftarg = bit, rightarg = bool, procedure = pg_catalog.op_bit_bool_xor);

CREATE OR REPLACE FUNCTION pg_catalog.op_time_bool_xor(time, bool) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.time_int8_xor($1, $2::uint8)::uint8';
CREATE OR REPLACE FUNCTION pg_catalog.op_bool_time_xor(bool, time) returns uint8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.int8_time_xor($1::uint8, $2)::uint8';
CREATE OPERATOR pg_catalog.^ (leftarg = time without time zone, rightarg = bool, procedure = pg_catalog.op_time_bool_xor);
CREATE OPERATOR pg_catalog.^ (leftarg = bool, rightarg = time without time zone, procedure = pg_catalog.op_bool_time_xor);

CREATE OPERATOR CLASS uint1_ops_1
    FOR TYPE uint1 USING hash family integer_ops AS
        OPERATOR        1       =(int2, uint1),
        OPERATOR        1       =(int, uint1);


