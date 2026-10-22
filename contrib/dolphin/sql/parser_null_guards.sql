CREATE SCHEMA parser_null_guards;
SET current_schema = parser_null_guards;
SET client_min_messages = warning;

-- Keep normal UPSERT UPDATE and NOTHING behavior working.
CREATE TABLE upsert_guard_t (id integer PRIMARY KEY, v integer);
INSERT INTO upsert_guard_t VALUES (1, 10), (2, 30);
INSERT INTO upsert_guard_t VALUES (1, 20) ON DUPLICATE KEY UPDATE v = VALUES(v);
INSERT INTO upsert_guard_t VALUES (2, 99) ON DUPLICATE KEY UPDATE NOTHING;
SELECT id, v FROM upsert_guard_t ORDER BY id;

-- Valid qualified names must still work in both grammar entries.
CREATE FUNCTION parser_null_guards.inc(integer) RETURNS integer
AS $$ SELECT $1 + 1 $$ LANGUAGE SQL;
CREATE FUNCTION parser_null_guards.series() RETURNS SETOF integer
AS $$ SELECT 7 $$ LANGUAGE SQL;
SELECT parser_null_guards.inc(41) AS qualified_function_value;
SELECT * FROM TABLE(parser_null_guards.series) AS t(table_function_value);

-- Non-string indirection nodes must be rejected before name conversion.
SELECT d[1][1](1);
SELECT d.a.*(1);
SELECT * FROM TABLE(d[1][1]);
SELECT * FROM TABLE(d.a[1:2]);
SELECT parser_null_guards.inc(41) AS session_alive;

RESET current_schema;
DROP SCHEMA parser_null_guards CASCADE;
RESET client_min_messages;
