DROP FUNCTION IF EXISTS pg_catalog.bin(text) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bin(int8) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.bin(text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bin_string';
CREATE OR REPLACE FUNCTION pg_catalog.bin(int8) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bin_integer';

DROP FUNCTION IF EXISTS pg_catalog.elt(text, variadic arr "any") cascade;
DROP FUNCTION IF EXISTS pg_catalog.elt(int8, variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.elt(text, variadic arr "any") returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'elt_string';
CREATE OR REPLACE FUNCTION pg_catalog.elt(int8, variadic arr "any") returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'elt_integer';

DROP FUNCTION IF EXISTS pg_catalog.field(text, variadic arr "any") cascade;
DROP FUNCTION IF EXISTS pg_catalog.field(numeric , variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.field(text, variadic arr "any") returns int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'field_string';
CREATE OR REPLACE FUNCTION pg_catalog.field(numeric, variadic arr "any") returns int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'field_integer';

DROP FUNCTION IF EXISTS pg_catalog.find_in_set(text, text) cascade;
DROP FUNCTION IF EXISTS pg_catalog.find_in_set(numeric, text) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.find_in_set(text, text) returns int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'find_in_set_string';
CREATE OR REPLACE FUNCTION pg_catalog.find_in_set(numeric, text) returns int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'find_in_set_integer';

DROP FUNCTION IF EXISTS pg_catalog.soundex(text) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.soundex(text) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'soundex';

DROP FUNCTION IF EXISTS pg_catalog.soundex_difference(text, text) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.soundex_difference(text, text) returns integer LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'soundex_difference';

DROP FUNCTION IF EXISTS pg_catalog.space(int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.space(text) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.space(int4) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'space_integer';
CREATE OR REPLACE FUNCTION pg_catalog.space(text) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'space_string';

DROP FUNCTION IF EXISTS pg_catalog.chara(variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.chara(variadic arr "any") returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'm_char';

DROP FUNCTION IF EXISTS pg_catalog.insert(text, integer, integer, text) CASCADE;
CREATE FUNCTION pg_catalog.insert (text, bigint, bigint, text) RETURNS text LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'text_insert';
