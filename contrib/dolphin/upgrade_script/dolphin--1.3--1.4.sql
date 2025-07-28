CREATE OR REPLACE FUNCTION pg_catalog.mod(double precision, int) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(int, double precision) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.mod(cast($1 as number), cast($2 as number))';

DROP FUNCTION IF EXISTS pg_catalog.date_add(timestamp without time zone, interval);
CREATE OR REPLACE FUNCTION pg_catalog.date_add(timestamp without time zone, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1::text, $2)  $$ LANGUAGE SQL;
DROP FUNCTION IF EXISTS pg_catalog.date_sub(timestamp without time zone, interval);
CREATE OR REPLACE FUNCTION pg_catalog.date_sub(timestamp without time zone, interval) RETURNS text AS $$ SELECT pg_catalog.adddate($1::text, -$2)  $$ LANGUAGE SQL;