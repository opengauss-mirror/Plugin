DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;
DROP FUNCTION IF EXISTS pg_catalog.ord(varbit);
DROP FUNCTION IF EXISTS pg_catalog.oct(bit);

DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", numeric) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, text, numeric) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
text,
text,
numeric
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index';

DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, text, numeric) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
boolean,
text,
numeric
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_1';

DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, boolean, numeric) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
text,
boolean,
numeric
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_2';

DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, boolean, numeric) CASCADE;
CREATE FUNCTION pg_catalog.substring_index (
boolean,
boolean,
numeric
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_2bool';