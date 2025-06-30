DROP FUNCTION IF EXISTS pg_catalog.replace(json, text, text);

-- xor between bool and bit,time
DROP OPERATOR IF EXISTS pg_catalog.^(bool, bit);
DROP OPERATOR IF EXISTS pg_catalog.^(bit, bool);
DROP OPERATOR IF EXISTS pg_catalog.^(time without time zone, bool);
DROP OPERATOR IF EXISTS pg_catalog.^(bool, time without time zone);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_bit_xor(bool, bit);
DROP FUNCTION IF EXISTS pg_catalog.op_bit_bool_xor(bit, bool);
DROP FUNCTION IF EXISTS pg_catalog.op_time_bool_xor(time, bool);
DROP FUNCTION IF EXISTS pg_catalog.op_bool_time_xor(bool, time);
