CREATE OR REPLACE FUNCTION pg_catalog.json_quote(text) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_quote';

CREATE OR REPLACE FUNCTION pg_catalog.json_array(variadic "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_array';

create or replace function pg_catalog.json_object_mysql(variadic arr "any") returns json language C immutable as '$libdir/dolphin', 'json_object_mysql';
create or replace function pg_catalog.json_object_noarg() returns json language C immutable as '$libdir/dolphin', 'json_object_noarg';

CREATE OR REPLACE FUNCTION pg_catalog.json_contains("any", "any", text) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_contains';
CREATE OR REPLACE FUNCTION pg_catalog.json_contains("any", "any") RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_contains';

CREATE OR REPLACE FUNCTION pg_catalog.json_contains_path("any", text, variadic text[]) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_contains_path';

CREATE OR REPLACE FUNCTION pg_catalog.json_extract("any", variadic text[]) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'json_extract';

CREATE OR REPLACE FUNCTION pg_catalog.json_keys("any") RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_keys';
CREATE OR REPLACE FUNCTION pg_catalog.json_keys("any",text) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_keys';

CREATE OR REPLACE FUNCTION pg_catalog.json_search("any",text,"any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_search';
CREATE OR REPLACE FUNCTION pg_catalog.json_search("any",text,"any","any") RETURNS text LANGUAGE C STABLE as '$libdir/dolphin', 'json_search';
CREATE OR REPLACE FUNCTION pg_catalog.json_search("any",text,"any","any",variadic text[]) RETURNS text LANGUAGE C STABLE as '$libdir/dolphin', 'json_search';

CREATE OR REPLACE FUNCTION pg_catalog.json_array_append("any", VARIADIC "any") RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_array_append';

CREATE OR REPLACE FUNCTION pg_catalog.json_append("any", VARIADIC "any") RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_append';

CREATE OR REPLACE FUNCTION pg_catalog.json_unquote("any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_unquote';

CREATE OR REPLACE FUNCTION pg_catalog.json_merge_preserve(variadic arr "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_merge_preserve';

CREATE OR REPLACE FUNCTION pg_catalog.json_merge(variadic arr "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_merge';

CREATE OR REPLACE FUNCTION pg_catalog.json_merge_patch(variadic arr "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_merge_patch';

CREATE OR REPLACE FUNCTION pg_catalog.json_insert(variadic "any") RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_insert';

CREATE OR REPLACE FUNCTION pg_catalog.json_depth("any") RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_depth';

CREATE OR REPLACE FUNCTION pg_catalog.json_replace(variadic arr "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_replace';

CREATE OR REPLACE FUNCTION pg_catalog.json_remove(variadic arr "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_remove';

CREATE OR REPLACE FUNCTION pg_catalog.json_array_insert(variadic arr "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_array_insert';

CREATE OR REPLACE FUNCTION pg_catalog.json_set(variadic "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_set';

CREATE OR REPLACE FUNCTION pg_catalog.json_length("any") RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_length';
CREATE OR REPLACE FUNCTION pg_catalog.json_length("any",text) RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_length';
create aggregate pg_catalog.json_arrayagg("any") (SFUNC = json_agg_transfn, STYPE = internal, finalfunc = json_agg_finalfn);

CREATE OR REPLACE FUNCTION pg_catalog.json_objectagg_finalfn(
    internal
) RETURNS json LANGUAGE C as '$libdir/dolphin', 'json_objectagg_finalfn';

create aggregate pg_catalog.json_objectagg("any", "any") (SFUNC = json_object_agg_transfn, STYPE = internal, finalfunc = json_objectagg_finalfn);

CREATE OR REPLACE FUNCTION pg_catalog.json_valid("any") RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_valid';

CREATE OR REPLACE FUNCTION pg_catalog.json_storage_size("any") RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_storage_size';

CREATE OR REPLACE FUNCTION pg_catalog.json_pretty("any") RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_pretty';

CREATE OR REPLACE FUNCTION pg_catalog.json_type("any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_type';


