DROP FUNCTION IF EXISTS pg_catalog.bin(bit) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.bin(bit) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bin_bit';
DROP FUNCTION IF EXISTS pg_catalog.bin(text) cascade;
DROP FUNCTION IF EXISTS pg_catalog.bin(numeric) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.bin(text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bin_string';
CREATE OR REPLACE FUNCTION pg_catalog.bin(numeric) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bin_integer';

DROP FUNCTION IF EXISTS pg_catalog.elt(text, variadic arr "any") cascade;
DROP FUNCTION IF EXISTS pg_catalog.elt(int8, variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.elt(text, variadic arr "any") returns text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'elt_string';
CREATE OR REPLACE FUNCTION pg_catalog.elt(int8, variadic arr "any") returns text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'elt_integer';
DROP FUNCTION IF EXISTS pg_catalog.elt(bit, variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.elt(bit, variadic arr "any") returns text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'elt_bit';

DROP FUNCTION IF EXISTS pg_catalog.field(variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.field(variadic arr "any") returns int8 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'field';

DROP FUNCTION IF EXISTS pg_catalog.find_in_set(text, text) cascade;
DROP FUNCTION IF EXISTS pg_catalog.find_in_set(numeric, text) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.find_in_set(text, text) returns int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'find_in_set_string';
CREATE OR REPLACE FUNCTION pg_catalog.find_in_set(numeric, text) returns int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'find_in_set_integer';

DROP FUNCTION IF EXISTS pg_catalog.soundex(text) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.soundex(text) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'soundex';
DROP FUNCTION IF EXISTS pg_catalog.soundex(boolean) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.soundex(boolean) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'soundex_bool';
DROP FUNCTION IF EXISTS pg_catalog.soundex(bit) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.soundex(bit) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'soundex_bit';

DROP FUNCTION IF EXISTS pg_catalog.soundex_difference(text, text) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.soundex_difference(text, text) returns integer LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'soundex_difference';

DROP FUNCTION IF EXISTS pg_catalog.space(int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.space(text) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.space(int4) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'space_integer';
CREATE OR REPLACE FUNCTION pg_catalog.space(text) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'space_string';
DROP FUNCTION IF EXISTS pg_catalog.space(bit) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.space(bit) returns text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'space_bit';

DROP FUNCTION IF EXISTS pg_catalog.chara(variadic arr "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.chara(variadic arr "any") returns text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'm_char';

DROP FUNCTION IF EXISTS pg_catalog.insert(text, integer, integer, text) CASCADE;
CREATE FUNCTION pg_catalog.insert (text, bigint, bigint, text) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'text_insert';

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

DROP FUNCTION IF EXISTS pg_catalog.hex(int8) cascade;
DROP FUNCTION IF EXISTS pg_catalog.hex(text) cascade;
DROP FUNCTION IF EXISTS pg_catalog.hex(bytea) cascade;
DROP FUNCTION IF EXISTS pg_catalog.hex(bit) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.hex(int8) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'int_to_hex';
CREATE OR REPLACE FUNCTION pg_catalog.hex(text) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'text_to_hex';
CREATE OR REPLACE FUNCTION pg_catalog.hex(bytea) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bytea_to_hex';
CREATE OR REPLACE FUNCTION pg_catalog.hex(bit) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bit_to_hex';

DROP FUNCTION IF EXISTS pg_catalog.uuid() cascade;
CREATE OR REPLACE FUNCTION pg_catalog.uuid() RETURNS varchar LANGUAGE C VOLATILE STRICT as '$libdir/dolphin', 'uuid_generate';

DROP FUNCTION IF EXISTS pg_catalog.db_b_format("any", int4) cascade;
DROP FUNCTION IF EXISTS pg_catalog.db_b_format("any", int4, "any") cascade;
CREATE OR REPLACE FUNCTION pg_catalog.db_b_format("any", int4) RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'db_b_format';
CREATE OR REPLACE FUNCTION pg_catalog.db_b_format("any", int4, "any") RETURNS text LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'db_b_format_locale';

CREATE OR REPLACE FUNCTION pg_catalog.float8_bool(float8) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'float8_bool';
CREATE CAST (float8 as boolean) WITH FUNCTION float8_bool(float8) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.float4_bool(float4) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'float4_bool';
CREATE CAST (float4 as boolean) WITH FUNCTION float4_bool(float4) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.date_bool(date) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'date_bool';
CREATE CAST (date as boolean) WITH FUNCTION date_bool(date) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.time_bool(time) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'time_bool';
CREATE CAST (time as boolean) WITH FUNCTION time_bool(time) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.bit_bool(bit) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'bit_bool';
CREATE CAST (bit as boolean) WITH FUNCTION bit_bool(bit) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.text_bool(text) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'text_bool';
CREATE CAST (text as boolean) WITH FUNCTION text_bool(text) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.varchar_bool(varchar) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'varchar_bool';
CREATE CAST (varchar as boolean) WITH FUNCTION varchar_bool(varchar) AS IMPLICIT;

CREATE OR REPLACE FUNCTION pg_catalog.char_bool(char) returns boolean LANGUAGE C immutable strict as '$libdir/dolphin', 'char_bool';
CREATE CAST (char as boolean) WITH FUNCTION char_bool(char) AS IMPLICIT;

CREATE FUNCTION pg_catalog.boolboollike(
bool,
bool
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'boolboollike';

CREATE OPERATOR pg_catalog.~~(leftarg = bool, rightarg = bool, procedure = pg_catalog.boolboollike);
CREATE OPERATOR pg_catalog.~~*(leftarg = bool, rightarg = bool, procedure = pg_catalog.boolboollike);

CREATE FUNCTION pg_catalog.booltextlike(
bool,
text
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'booltextlike';

CREATE OPERATOR pg_catalog.~~(leftarg = bool, rightarg = text, procedure = pg_catalog.booltextlike);
CREATE OPERATOR pg_catalog.~~*(leftarg = bool, rightarg = text, procedure = pg_catalog.booltextlike);

CREATE FUNCTION pg_catalog.textboollike(
text,
bool
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textboollike';

CREATE OPERATOR pg_catalog.~~(leftarg = text, rightarg = bool, procedure = pg_catalog.textboollike);
CREATE OPERATOR pg_catalog.~~*(leftarg = text, rightarg = bool, procedure = pg_catalog.textboollike);

CREATE FUNCTION pg_catalog.boolboolnlike(
bool,
bool
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'boolboolnlike';

CREATE OPERATOR pg_catalog.!~~(leftarg = bool, rightarg = bool, procedure = pg_catalog.boolboolnlike);
CREATE OPERATOR pg_catalog.!~~*(leftarg = bool, rightarg = bool, procedure = pg_catalog.boolboolnlike);

CREATE FUNCTION pg_catalog.booltextnlike(
bool,
text
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'booltextnlike';

CREATE OPERATOR pg_catalog.!~~(leftarg = bool, rightarg = text, procedure = pg_catalog.booltextnlike);
CREATE OPERATOR pg_catalog.!~~*(leftarg = bool, rightarg = text, procedure = pg_catalog.booltextnlike);

CREATE FUNCTION pg_catalog.textboolnlike(
text,
bool
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textboolnlike';

CREATE OPERATOR pg_catalog.!~~(leftarg = text, rightarg = bool, procedure = pg_catalog.textboolnlike);
CREATE OPERATOR pg_catalog.!~~*(leftarg = text, rightarg = bool, procedure = pg_catalog.textboolnlike);

CREATE FUNCTION pg_catalog.bitlike(
VarBit,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitlike';

CREATE FUNCTION pg_catalog.bitnlike(
VarBit,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitnlike';

CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=VarBit, procedure=pg_catalog.bitnlike);
CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=VarBit, procedure=pg_catalog.bitnlike);

CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=VarBit, procedure=pg_catalog.bitlike);
CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=VarBit, procedure=pg_catalog.bitlike);


CREATE FUNCTION pg_catalog.bitothernlike(
VarBit,
bool
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=bool, procedure=pg_catalog.bitothernlike);
CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=bool, procedure=pg_catalog.bitothernlike);

CREATE FUNCTION pg_catalog.bitothern2like(
bool,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

CREATE OPERATOR pg_catalog.!~~(leftarg=bool, rightarg=VarBit, procedure=pg_catalog.bitothern2like);
CREATE OPERATOR pg_catalog.!~~*(leftarg=bool, rightarg=VarBit, procedure=pg_catalog.bitothern2like);

CREATE FUNCTION pg_catalog.bitothern3like(
VarBit,
text
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=text, procedure=pg_catalog.bitothern3like);
CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=text, procedure=pg_catalog.bitothern3like);

CREATE FUNCTION pg_catalog.bitothern4like(
text,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

CREATE OPERATOR pg_catalog.!~~(leftarg=text, rightarg=VarBit, procedure=pg_catalog.bitothern4like);
CREATE OPERATOR pg_catalog.!~~*(leftarg=text, rightarg=VarBit, procedure=pg_catalog.bitothern4like);

CREATE FUNCTION pg_catalog.bitothern5like(
VarBit,
bytea
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=bytea, procedure=pg_catalog.bitothern5like);
CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=bytea, procedure=pg_catalog.bitothern5like);

CREATE FUNCTION pg_catalog.bitothern6like(
bytea,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

CREATE OPERATOR pg_catalog.!~~(leftarg=bytea, rightarg=VarBit, procedure=pg_catalog.bitothern6like);
CREATE OPERATOR pg_catalog.!~~*(leftarg=bytea, rightarg=VarBit, procedure=pg_catalog.bitothern6like);

CREATE FUNCTION pg_catalog.bitothern7like(
VarBit,
blob
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

CREATE OPERATOR pg_catalog.!~~(leftarg=VarBit, rightarg=blob, procedure=pg_catalog.bitothern7like);
CREATE OPERATOR pg_catalog.!~~*(leftarg=VarBit, rightarg=blob, procedure=pg_catalog.bitothern7like);

CREATE FUNCTION pg_catalog.bitothern8like(
blob,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitothernlike';

CREATE OPERATOR pg_catalog.!~~(leftarg=blob, rightarg=VarBit, procedure=pg_catalog.bitothern8like);
CREATE OPERATOR pg_catalog.!~~*(leftarg=blob, rightarg=VarBit, procedure=pg_catalog.bitothern8like);

CREATE FUNCTION pg_catalog.bitotherlike(
VarBit,
bool
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=bool, procedure=pg_catalog.bitotherlike);
CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=bool, procedure=pg_catalog.bitotherlike);

CREATE FUNCTION pg_catalog.bitother2like(
bool,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

CREATE OPERATOR pg_catalog.~~(leftarg=bool, rightarg=VarBit, procedure=pg_catalog.bitother2like);
CREATE OPERATOR pg_catalog.~~*(leftarg=bool, rightarg=VarBit, procedure=pg_catalog.bitother2like);

CREATE FUNCTION pg_catalog.bitother3like(
VarBit,
text
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=text, procedure=pg_catalog.bitother3like);
CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=text, procedure=pg_catalog.bitother3like);

CREATE FUNCTION pg_catalog.bitother4like(
text,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

CREATE OPERATOR pg_catalog.~~(leftarg=text, rightarg=VarBit, procedure=pg_catalog.bitother4like);
CREATE OPERATOR pg_catalog.~~*(leftarg=text, rightarg=VarBit, procedure=pg_catalog.bitother4like);

CREATE FUNCTION pg_catalog.bitother5like(
VarBit,
bytea
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=bytea, procedure=pg_catalog.bitother5like);
CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=bytea, procedure=pg_catalog.bitother5like);

CREATE FUNCTION pg_catalog.bitother6like(
bytea,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

CREATE OPERATOR pg_catalog.~~(leftarg=bytea, rightarg=VarBit, procedure=pg_catalog.bitother6like);
CREATE OPERATOR pg_catalog.~~*(leftarg=bytea, rightarg=VarBit, procedure=pg_catalog.bitother6like);

CREATE FUNCTION pg_catalog.bitother7like(
VarBit,
blob
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

CREATE OPERATOR pg_catalog.~~(leftarg=VarBit, rightarg=blob, procedure=pg_catalog.bitother7like);
CREATE OPERATOR pg_catalog.~~*(leftarg=VarBit, rightarg=blob, procedure=pg_catalog.bitother7like);

CREATE FUNCTION pg_catalog.bitother8like(
blob,
VarBit
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'bitotherlike';

CREATE OPERATOR pg_catalog.~~(leftarg=blob, rightarg=VarBit, procedure=pg_catalog.bitother8like);
CREATE OPERATOR pg_catalog.~~*(leftarg=blob, rightarg=VarBit, procedure=pg_catalog.bitother8like);

CREATE OPERATOR pg_catalog.~~*(leftarg = bytea, rightarg = bytea, procedure = pg_catalog.bytealike);
CREATE OPERATOR pg_catalog.!~~*(leftarg = bytea, rightarg = bytea, procedure = pg_catalog.byteanlike);

CREATE OPERATOR pg_catalog.~~*(leftarg=raw, rightarg=raw, procedure=pg_catalog.rawlike);
CREATE OPERATOR pg_catalog.!~~*(leftarg=raw, rightarg=raw, procedure=pg_catalog.rawnlike);
