DROP FUNCTION IF EXISTS pg_catalog.length(boolean);
DROP FUNCTION IF EXISTS pg_catalog.length(tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.length(blob);
DROP FUNCTION IF EXISTS pg_catalog.length(mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.length(longblob);
DROP FUNCTION IF EXISTS pg_catalog.length(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.length(json);

DROP FUNCTION IF EXISTS pg_catalog.position(boolean, text);

DROP FUNCTION IF EXISTS pg_catalog.acos(bit);
DROP FUNCTION IF EXISTS pg_catalog.cos(bit);
DROP FUNCTION IF EXISTS pg_catalog.cos(boolean);
DROP FUNCTION IF EXISTS pg_catalog.asin(bit);
DROP FUNCTION IF EXISTS pg_catalog.asin(boolean);

CREATE FUNCTION pg_catalog.unknown_concat(unknown, unknown) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.unknown_int_concat(unknown, integer) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.int_unknown_concat(integer, unknown) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';

CREATE OPERATOR pg_catalog.||(leftarg=unknown, rightarg=unknown, procedure=pg_catalog.unknown_concat);
CREATE OPERATOR pg_catalog.||(leftarg=unknown, rightarg=integer, procedure=pg_catalog.unknown_int_concat);
CREATE OPERATOR pg_catalog.||(leftarg=integer, rightarg=unknown, procedure=pg_catalog.int_unknown_concat);