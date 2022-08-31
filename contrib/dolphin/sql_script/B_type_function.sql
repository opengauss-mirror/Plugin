DROP FUNCTION IF EXISTS pg_catalog.bin(text) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bin(numeric) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.bin(text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bin_string';
CREATE OR REPLACE FUNCTION pg_catalog.bin(numeric) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bin_integer';

DROP FUNCTION IF EXISTS pg_catalog.elt(text, variadic arr "any") cascade;
DROP FUNCTION IF EXISTS pg_catalog.elt(int8, variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.elt(text, variadic arr "any") returns text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'elt_string';
CREATE OR REPLACE FUNCTION pg_catalog.elt(int8, variadic arr "any") returns text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'elt_integer';

DROP FUNCTION IF EXISTS pg_catalog.field(variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.field(variadic arr "any") returns int8 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'field';

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
CREATE OR REPLACE FUNCTION pg_catalog.chara(variadic arr "any") returns text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'm_char';

DROP FUNCTION IF EXISTS pg_catalog.insert(text, integer, integer, text) CASCADE;
CREATE FUNCTION pg_catalog.insert (text, bigint, bigint, text) RETURNS text LANGUAGE C IMMUTABLE  as '$libdir/dolphin',  'text_insert';

DROP FUNCTION IF EXISTS pg_catalog.char_length(bool) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.char_length(bool) returns int4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'boolcharlen';
DROP FUNCTION IF EXISTS pg_catalog.char_length(bit) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.char_length(bit) returns int4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'c_bitoctetlength';

DROP FUNCTION IF EXISTS pg_catalog.character_length(bool) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.character_length(bool) returns int4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'boolcharlen';
DROP FUNCTION IF EXISTS pg_catalog.character_length(bit) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.character_length(bit) returns int4 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'c_bitoctetlength';
DROP FUNCTION IF EXISTS pg_catalog.make_set(bigint, VARIADIC ARR "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.make_set(bigint, VARIADIC ARR "any") RETURNS text LANGUAGE C as '$libdir/dolphin', 'make_set';
