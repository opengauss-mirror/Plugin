DROP FUNCTION IF EXISTS pg_catalog.dolphin_invoke();
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS pg_catalog.date_cast(cstring, boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_cast(cstring, oid, integer, boolean);

DROP OPERATOR IF EXISTS pg_catalog.=(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<>(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<=(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.<(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>=(time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.>(time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_eq_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_ne_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_le_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_lt_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_ge_timestamp (time, timestamp without time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_gt_timestamp (time, timestamp without time zone);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp without time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_eq_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_ne_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_le_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_lt_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_ge_time (timestamp without time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_gt_time (timestamp without time zone, time);

DROP OPERATOR IF EXISTS pg_catalog.=(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.<>(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.<=(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.<(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.>=(time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.>(time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_eq_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_ne_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_le_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_lt_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_ge_timestamptz (time, timestamp with time zone);
DROP FUNCTION IF EXISTS pg_catalog.time_gt_timestamptz (time, timestamp with time zone);
DROP OPERATOR IF EXISTS pg_catalog.=(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<>(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<=(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.<(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.>=(timestamp with time zone, time);
DROP OPERATOR IF EXISTS pg_catalog.>(timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_eq_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_ne_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_le_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_lt_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_ge_time (timestamp with time zone, time);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_gt_time (timestamp with time zone, time);
CREATE OR REPLACE FUNCTION pg_catalog.tinyblob_rawout (
tinyblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

CREATE OR REPLACE FUNCTION pg_catalog.mediumblob_rawout (
mediumblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';

CREATE OR REPLACE FUNCTION pg_catalog.longblob_rawout (
longblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteaout';
