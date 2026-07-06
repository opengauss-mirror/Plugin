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