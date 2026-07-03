CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(number, name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select db_b_format($1::anyelement , $2::bigint)';

DO $for_og_rollback$
DECLARE
  ans boolean;
  v_isinplaceupgrade boolean;
BEGIN
    select case when count(*)=1 then true else false end as ans
      from (select setting from pg_settings where name = 'upgrade_mode' and setting != '0') into ans;
    show isinplaceupgrade into v_isinplaceupgrade;
    if ans and v_isinplaceupgrade then
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
    end if;
END $for_og_rollback$;
