drop function if exists pg_catalog.yearweek(timestamp without time zone);
drop function if exists pg_catalog.yearweek(timestamp with time zone);
drop function if exists pg_catalog.yearweek(timestamp without time zone, int8);
drop function if exists pg_catalog.yearweek(timestamp with time zone, int8);

DROP FUNCTION IF EXISTS pg_catalog.date_format (date, text);
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp without time zone, text);
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp with time zone, text); 