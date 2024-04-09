CREATE OR REPLACE FUNCTION pg_catalog.length(boolean) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::integer)';
CREATE OR REPLACE FUNCTION pg_catalog.length(tinyblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(blob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(mediumblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(longblob) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(anyenum) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';
CREATE OR REPLACE FUNCTION pg_catalog.length(json) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.length($1::text)';

CREATE OR REPLACE FUNCTION pg_catalog.position(boolean, text) RETURNS integer LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.position($1::text, $2)';

DROP FUNCTION IF EXISTS pg_catalog.acos(bit);
CREATE OR REPLACE FUNCTION pg_catalog.acos(bit) RETURNS double precision LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'acos_bit';

DROP FUNCTION IF EXISTS pg_catalog.cos(bit);
CREATE OR REPLACE FUNCTION pg_catalog.cos(bit) RETURNS double precision LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'cos_bit';
DROP FUNCTION IF EXISTS pg_catalog.cos(boolean);
CREATE OR REPLACE FUNCTION pg_catalog.cos(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.cos(cast($1 as float))';

DROP FUNCTION IF EXISTS pg_catalog.asin(bit);
CREATE OR REPLACE FUNCTION pg_catalog.asin(bit) RETURNS double precision LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'asin_bit';
DROP FUNCTION IF EXISTS pg_catalog.asin(boolean);
CREATE OR REPLACE FUNCTION pg_catalog.asin(boolean) RETURNS double precision LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.asin(cast($1 as float))';

DO $for_upgrade_only$
DECLARE
  ans boolean;
  v_isinplaceupgrade boolean;
BEGIN
    select case when count(*)=1 then true else false end as ans from (select setting from pg_settings where name = 'upgrade_mode' and setting != '0') into ans;
    show isinplaceupgrade into v_isinplaceupgrade;
    -- we can do drop operator only during upgrade. Just for 6.0.0-RC1
    if ans = true and v_isinplaceupgrade = true then
        DROP OPERATOR IF EXISTS pg_catalog.||(unknown, unknown);
        DROP OPERATOR IF EXISTS pg_catalog.||(unknown, integer);
        DROP OPERATOR IF EXISTS pg_catalog.||(integer, unknown);

        DROP FUNCTION IF EXISTS pg_catalog.unknown_concat(unknown, unknown);
        DROP FUNCTION IF EXISTS pg_catalog.unknown_int_concat(unknown, integer);
        DROP FUNCTION IF EXISTS pg_catalog.int_unknown_concat(integer, unknown);
    end if;
END
$for_upgrade_only$;