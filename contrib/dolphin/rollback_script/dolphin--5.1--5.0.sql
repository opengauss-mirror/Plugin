DROP FUNCTION IF EXISTS pg_catalog.datediff(date, date);

DROP OPERATOR CLASS IF EXISTS uint1_ops_1 USING hash;

DROP FUNCTION IF EXISTS pg_catalog.date_add_time_interval_return_time (time, interval, boolean);
DROP FUNCTION IF EXISTS pg_catalog.date_sub_time_interval_return_time (time, interval, boolean);
