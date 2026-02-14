CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp without time zone, int8) RETURNS int8 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'yearweek_timestamp';
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp with time zone, int8) RETURNS int8 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'yearweek_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp without time zone) RETURNS int8 AS $$ SELECT pg_catalog.yearweek($1, null) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp with time zone) RETURNS int8 AS $$ SELECT pg_catalog.yearweek($1, null) $$ LANGUAGE SQL;
