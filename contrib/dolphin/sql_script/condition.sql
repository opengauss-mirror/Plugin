create or replace function pg_catalog.strcmp("any", "any") returns int language C immutable strict as '$libdir/dolphin', 'gs_strcmp';

create or replace function pg_catalog.gs_interval(variadic arr "any") returns int language C immutable as '$libdir/dolphin', 'gs_interval';

