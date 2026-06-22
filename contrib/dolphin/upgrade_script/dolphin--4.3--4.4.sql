CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp without time zone, int8) RETURNS int8 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'yearweek_timestamp';
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp with time zone, int8) RETURNS int8 LANGUAGE C STABLE CALLED ON NULL INPUT as '$libdir/dolphin', 'yearweek_timestamptz';
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp without time zone) RETURNS int8 AS $$ SELECT pg_catalog.yearweek($1, null) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION pg_catalog.yearweek(timestamp with time zone) RETURNS int8 AS $$ SELECT pg_catalog.yearweek($1, null) $$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS pg_catalog.date_format (date, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (date, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ;
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp without time zone, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ;
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp with time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp with time zone, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ; 
