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

DROP FUNCTION IF EXISTS pg_catalog.year(integer);
DROP FUNCTION IF EXISTS pg_catalog.year(anyset);

DROP FUNCTION IF EXISTS pg_catalog.hour(bit);
DROP FUNCTION IF EXISTS pg_catalog.hour(blob);
DROP FUNCTION IF EXISTS pg_catalog.hour(boolean);
DROP FUNCTION IF EXISTS pg_catalog.hour(json);
DROP FUNCTION IF EXISTS pg_catalog.hour(integer);

DROP FUNCTION IF EXISTS pg_catalog.minute(bit);
DROP FUNCTION IF EXISTS pg_catalog.minute(blob);
DROP FUNCTION IF EXISTS pg_catalog.minute(boolean);
DROP FUNCTION IF EXISTS pg_catalog.minute(json);
DROP FUNCTION IF EXISTS pg_catalog.minute(integer);

DROP FUNCTION IF EXISTS pg_catalog.second(bit);
DROP FUNCTION IF EXISTS pg_catalog.second(blob);
DROP FUNCTION IF EXISTS pg_catalog.second(boolean);
DROP FUNCTION IF EXISTS pg_catalog.second(json);
DROP FUNCTION IF EXISTS pg_catalog.second(integer);

DROP FUNCTION IF EXISTS pg_catalog.microsecond(year);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(bit);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(blob);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(boolean);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(json);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(integer);
DROP FUNCTION IF EXISTS pg_catalog.microsecond(float);

CREATE OR REPLACE FUNCTION pg_catalog.year(int4) RETURNS int8 LANGUAGE SQL STABLE STRICT as 'SELECT year($1::text)';

CREATE FUNCTION pg_catalog.unknown_concat(unknown, unknown) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.unknown_int_concat(unknown, integer) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';
CREATE FUNCTION pg_catalog.int_unknown_concat(integer, unknown) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select concat($1, $2)';

CREATE OPERATOR pg_catalog.||(leftarg=unknown, rightarg=unknown, procedure=pg_catalog.unknown_concat);
CREATE OPERATOR pg_catalog.||(leftarg=unknown, rightarg=integer, procedure=pg_catalog.unknown_int_concat);
CREATE OPERATOR pg_catalog.||(leftarg=integer, rightarg=unknown, procedure=pg_catalog.int_unknown_concat);

DROP FUNCTION IF EXISTS pg_catalog.chara(variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.chara(variadic arr "any") returns text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'm_char';

DROP FUNCTION IF EXISTS pg_catalog.quarter (timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.quarter (timetz);
DROP FUNCTION IF EXISTS pg_catalog.quarter (abstime);
DROP FUNCTION IF EXISTS pg_catalog.quarter (date);
DROP FUNCTION IF EXISTS pg_catalog.quarter (time);
DROP FUNCTION IF EXISTS pg_catalog.quarter (timestamp(0) with time zone);
DROP FUNCTION IF EXISTS pg_catalog.quarter (year);
DROP FUNCTION IF EXISTS pg_catalog.quarter (binary);
DROP FUNCTION IF EXISTS pg_catalog.quarter (text;
DROP CAST IF EXISTS (binary as timestamp without time zone);
DROP CAST IF EXISTS (binary as timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.binary_timestamp (binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_timestamptz (binary);
CREATE CAST ("binary" AS timestamp without time zone) WITH FUNCTION pg_catalog.Varlena2Datetime(anyelement) AS ASSIGNMENT;
CREATE CAST ("binary" AS timestamp with time zone) WITH FUNCTION pg_catalog.Varlena2Timestamptz(anyelement) AS ASSIGNMENT;
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamptz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timetz) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (abstime) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (date) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (time) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';
CREATE OR REPLACE FUNCTION pg_catalog.quarter (timestamp(0) with time zone) RETURNS float8 LANGUAGE SQL STABLE STRICT as 'select pg_catalog.date_part(''quarter'', $1)';