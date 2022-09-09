CREATE OR REPLACE FUNCTION pg_catalog.pg_get_nonstrict_basic_value(typename text)
RETURNS text
AS
$$
BEGIN
    IF typename = 'timestamp' then
        return 'now';
    elsif typename = 'time' or typename = 'timetz' or typename = 'interval' or typename = 'reltime' then
        return '00:00:00';
    elsif typename = 'date' then
        return '1970-01-01';
    elsif typename = 'smalldatetime' then
        return '1970-01-01 08:00:00';
    elsif typename = 'abstime' then
        return '1970-01-01 08:00:00+08';
    elsif typename = 'uuid' then
        return '00000000-0000-0000-0000-000000000000';
    elsif typename = 'bool' then
        return 'false';
    elsif typename = 'point' or typename = 'polygon' then
        return '(0,0)';
    elsif typename = 'path' then
        return '((0,0))';
    elsif typename = 'circle' then
        return '(0,0),0';
    elsif typename = 'lseg' or typename = 'box' then
        return '(0,0),(0,0)';
    elsif typename = 'tinterval' then
        return '["1970-01-01 00:00:00+08" "1970-01-01 00:00:00+08"]';
    else
        return '0 or empty';
    end if;
end;
$$
LANGUAGE plpgsql;

CREATE VIEW public.pg_type_nonstrict_basic_value AS
    SELECT
            t.typname As typename,
            pg_get_nonstrict_basic_value(t.typname) As basic_value

    FROM pg_type t;
REVOKE ALL ON public.pg_type_nonstrict_basic_value  FROM PUBLIC;
GRANT SELECT, REFERENCES ON public.pg_type_nonstrict_basic_value  TO PUBLIC;

DROP FUNCTION IF EXISTS pg_catalog.gs_master_status(OUT "Xlog_File_Name" text, OUT "Xlog_File_Offset" int4, OUT "Xlog_Lsn" text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.gs_master_status (
OUT "Xlog_File_Name" text, OUT "Xlog_File_Offset" int4, OUT "Xlog_Lsn" text
) RETURNS setof record LANGUAGE C VOLATILE STRICT as '$libdir/dolphin', 'gs_master_status';

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