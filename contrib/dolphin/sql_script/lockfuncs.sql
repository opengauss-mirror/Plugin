DROP FUNCTION IF EXISTS pg_catalog.get_lock(text, text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.get_lock(text, double) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.get_lock(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.release_lock(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_free_lock(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_used_lock(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.release_all_locks() CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.get_all_locks() CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.clear_all_invalid_locks() CASCADE;

CREATE FUNCTION pg_catalog.get_lock (
text,
text
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'GetAdvisoryLockWithtimeTextFormat';

CREATE FUNCTION pg_catalog.get_lock (
text,
double
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'GetAdvisoryLockWithtime';

CREATE FUNCTION pg_catalog.get_lock (
text
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'GetAdvisoryLock';

CREATE FUNCTION pg_catalog.release_lock (
text
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ReleaseAdvisoryLock';

CREATE FUNCTION pg_catalog.is_free_lock (
text
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'IsFreeAdvisoryLock';

CREATE FUNCTION pg_catalog.is_used_lock (
text
) RETURNS bigint LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'IsUsedAdvisoryLock';

CREATE FUNCTION pg_catalog.release_all_locks (
) RETURNS bigint LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ReleaseAllAdvisoryLock';

CREATE FUNCTION pg_catalog.get_all_locks (
lockname out text,
sessionid out bigint
) RETURNS setof record LANGUAGE C STRICT as '$libdir/dolphin',  'GetAllAdvisoryLock';

CREATE FUNCTION pg_catalog.clear_all_invalid_locks (
) RETURNS bigint LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'ClearInvalidLockName';
