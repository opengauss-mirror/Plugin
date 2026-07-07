CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(number, name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select db_b_format($1::"any", $2::bigint)';

DROP OPERATOR IF EXISTS pg_catalog.<>(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.<>(binary, text);
DROP OPERATOR IF EXISTS pg_catalog.>(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.>(binary, text);
DROP OPERATOR IF EXISTS pg_catalog.<(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.<(binary, text);
DROP OPERATOR IF EXISTS pg_catalog.>=(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, text);
DROP OPERATOR IF EXISTS pg_catalog.<=(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.<=(binary, text);

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_eq(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT ($1::binary = $2)';

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_eq(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT ($1 = $2::binary)';

DROP FUNCTION IF EXISTS pg_catalog.text_binary_ne(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_ne(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.text_binary_gt(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_gt(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.text_binary_lt(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_lt(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.text_binary_ge(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_ge(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.text_binary_le(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_le(binary, text);

DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp without time zone, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ;
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp with time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp with time zone, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ;

CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_eq(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_eq(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_ne(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_ne(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_gt(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_gt(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_lt(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_lt(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_ge(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_ge(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_tinyblob_le(boolean, tinyblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_boolean_le(tinyblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_eq(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_eq(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_ne(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_ne(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_gt(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_gt(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_lt(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_lt(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_ge(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_ge(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_mediumblob_le(boolean, mediumblob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_boolean_le(mediumblob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_eq(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_eq(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text = $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_ne(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_ne(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text != $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_gt(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_gt(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text > $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_lt(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_lt(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text < $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_ge(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_ge(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text >= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_blob_le(boolean, blob) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.blob_boolean_le(blob, boolean) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS 'SELECT ($1::text <= $2::text)';

CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_eq(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text = $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_eq(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text = $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_ne(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text != $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_ne(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text != $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_gt(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text > $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_gt(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text > $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_lt(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text < $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_lt(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text < $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_ge(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text >= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_ge(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text >= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.boolean_longblob_le(boolean, longblob) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text <= $2::text)';
CREATE OR REPLACE FUNCTION pg_catalog.longblob_boolean_le(longblob, boolean) returns bool LANGUAGE SQL IMMUTABLE STRICT as 'select ($1::text <= $2::text)';
