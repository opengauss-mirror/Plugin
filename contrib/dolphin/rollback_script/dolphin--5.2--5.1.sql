DROP CAST IF EXISTS (varchar AS bytea);
DROP FUNCTION IF EXISTS pg_catalog.varchar_bytea(varchar);

DROP VIEW IF EXISTS performance_schema.threads;

DROP FUNCTION IF EXISTS pg_catalog.db_b_format(number, name);
DROP FUNCTION IF EXISTS pg_catalog.db_b_format(unknown, unknown);

drop function if exists pg_catalog.yearweek(timestamp without time zone);
drop function if exists pg_catalog.yearweek(timestamp with time zone);
drop function if exists pg_catalog.yearweek(timestamp without time zone, int8);
drop function if exists pg_catalog.yearweek(timestamp with time zone, int8);
