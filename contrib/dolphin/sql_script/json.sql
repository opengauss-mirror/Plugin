CREATE OR REPLACE FUNCTION pg_catalog.json_quote(text) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_quote';

CREATE OR REPLACE FUNCTION pg_catalog.json_array(variadic "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_array';

create or replace function pg_catalog.json_object_mysql(variadic arr "any") returns json language C immutable as '$libdir/dolphin', 'json_object_mysql';
create or replace function pg_catalog.json_object_noarg() returns json language C immutable as '$libdir/dolphin', 'json_object_noarg';

CREATE OR REPLACE FUNCTION pg_catalog.json_contains(json, json, text) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_contains';
CREATE OR REPLACE FUNCTION pg_catalog.json_contains(json, json) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_contains';

CREATE OR REPLACE FUNCTION pg_catalog.json_contains_path(json, text, variadic text[]) RETURNS boolean LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_contains_path';

DROP FUNCTION IF EXISTS pg_catalog.json_extract(json, variadic text[]) CASCADE;

CREATE FUNCTION pg_catalog.json_extract(json, variadic text[]) RETURNS json LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'json_extract';

CREATE OR REPLACE FUNCTION pg_catalog.json_keys(json) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_keys';
CREATE OR REPLACE FUNCTION pg_catalog.json_keys(json,text) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_keys';

CREATE OR REPLACE FUNCTION pg_catalog.json_search(json,text,"any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_search';
CREATE OR REPLACE FUNCTION pg_catalog.json_search(json,text,"any","any") RETURNS text LANGUAGE C STABLE as '$libdir/dolphin', 'json_search';
CREATE OR REPLACE FUNCTION pg_catalog.json_search(json,text,"any","any",variadic text[]) RETURNS text LANGUAGE C STABLE as '$libdir/dolphin', 'json_search';

CREATE OR REPLACE FUNCTION pg_catalog.json_array_append(json, VARIADIC "any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_array_append';

CREATE OR REPLACE FUNCTION pg_catalog.json_append(json, VARIADIC "any") RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_append';

CREATE OR REPLACE FUNCTION pg_catalog.json_unquote(text) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_unquote';
CREATE OR REPLACE FUNCTION pg_catalog.json_unquote(json) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin','json_unquote';

CREATE OR REPLACE FUNCTION pg_catalog.json_merge_preserve(variadic json[]) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_merge_preserve';

CREATE OR REPLACE FUNCTION pg_catalog.json_merge(variadic json[]) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_merge';

CREATE OR REPLACE FUNCTION pg_catalog.json_merge_patch(variadic json[]) RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_merge_patch';

CREATE OR REPLACE FUNCTION pg_catalog.json_insert(variadic "any") RETURNS json LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_insert';

CREATE OR REPLACE FUNCTION pg_catalog.json_depth(json) RETURNS int LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'json_depth';

CREATE OR REPLACE FUNCTION pg_catalog.json_replace(variadic arr "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_replace';

CREATE OR REPLACE FUNCTION pg_catalog.json_remove(json, variadic "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_remove';

CREATE OR REPLACE FUNCTION pg_catalog.json_array_insert(json, variadic "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_array_insert';

CREATE OR REPLACE FUNCTION pg_catalog.json_set( variadic "any") RETURNS json LANGUAGE C STABLE as '$libdir/dolphin', 'json_set';

