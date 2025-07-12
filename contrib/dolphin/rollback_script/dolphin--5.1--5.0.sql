DROP FUNCTION IF EXISTS pg_catalog.mod(varchar, int4);
DROP FUNCTION IF EXISTS pg_catalog.mod(int4, varchar);
DROP FUNCTION IF EXISTS pg_catalog.mod(int8, varchar);
DROP FUNCTION IF EXISTS pg_catalog.mod(varchar, int8);
DROP FUNCTION IF EXISTS pg_catalog.mod(char, char);
DROP FUNCTION IF EXISTS pg_catalog.mod(varchar, varchar);

DROP FUNCTION IF EXISTS pg_catalog.b_extract (text, date);

DROP FUNCTION IF EXISTS pg_catalog.datediff(date, date);

DROP OPERATOR CLASS IF EXISTS uint1_ops_1 USING hash;
