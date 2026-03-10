create schema partition_test5;
set current_schema to 'partition_test5';

CREATE TABLE IF NOT EXISTS t1 (
"SOURCE_ID" uint8,
"OCCURRENCE_TIME" timestamp(6) without time zone NOT NULL
)
CHARACTER SET = "UTF8" COLLATE = "utf8_general_ci"                                                                                                                
WITH (orientation=row, compression=no)
PARTITION BY RANGE (to_days("OCCURRENCE_TIME"))
(
PARTITION p20251201 VALUES LESS THAN (739952),
PARTITION p20251202 VALUES LESS THAN (739953),
PARTITION p20251203 VALUES LESS THAN (739954)
)
ENABLE ROW MOVEMENT;

CREATE TABLE IF NOT EXISTS t2 (
"SOURCE_ID" uint8,
"OCCURRENCE_TIME" timestamp(6) without time zone NOT NULL
)
CHARACTER SET = "UTF8" COLLATE = "utf8_general_ci"                                                                                                                
WITH (orientation=row, compression=no)
PARTITION BY RANGE (to_days("OCCURRENCE_TIME") + 1)
(
PARTITION p20251201 VALUES LESS THAN (739952),
PARTITION p20251202 VALUES LESS THAN (739953),
PARTITION p20251203 VALUES LESS THAN (739954)
)
ENABLE ROW MOVEMENT;

CREATE TABLE IF NOT EXISTS t3 (
"SOURCE_ID" uint8,
"OCCURRENCE_TIME" timestamp(6) without time zone NOT NULL
)
CHARACTER SET = "UTF8" COLLATE = "utf8_general_ci"                                                                                                                
WITH (orientation=row, compression=no)
PARTITION BY RANGE (to_days("OCCURRENCE_TIME" + 1))
(
PARTITION p20251201 VALUES LESS THAN (739952),
PARTITION p20251202 VALUES LESS THAN (739953),
PARTITION p20251203 VALUES LESS THAN (739954)
)
ENABLE ROW MOVEMENT;

CREATE TABLE IF NOT EXISTS t4 (
"SOURCE_ID" uint8,
"OCCURRENCE_TIME" timestamp(6) without time zone NOT NULL
)
CHARACTER SET = "UTF8" COLLATE = "utf8_general_ci"                                                                                                                
WITH (orientation=row, compression=no)
PARTITION BY RANGE (to_days(abs("OCCURRENCE_TIME" + 1)))
(
PARTITION p20251201 VALUES LESS THAN (739952),
PARTITION p20251202 VALUES LESS THAN (739953),
PARTITION p20251203 VALUES LESS THAN (739954)
)
ENABLE ROW MOVEMENT;

select to_days(20251201000000);

explain (costs off) select count(*) from t1 where OCCURRENCE_TIME BETWEEN '2025-12-01 00:00:00' AND '2025-12-01 23:59:59';
explain (costs off) select count(*) from t2 where OCCURRENCE_TIME BETWEEN '2025-12-01 00:00:00' AND '2025-12-01 23:59:59';
explain (costs off) select count(*) from t3 where OCCURRENCE_TIME < '2025-12-01 00:00:00';
explain (costs off) select count(*) from t4 where OCCURRENCE_TIME < '2025-12-01 00:00:00';
explain (costs off) select count(*) from t1 where OCCURRENCE_TIME is NULL;
explain (costs off) select count(*) from t1 where OCCURRENCE_TIME is not NULL;

drop table if exists t1 cascade;
drop table if exists t2 cascade;
drop table if exists t3 cascade;
drop table if exists t4 cascade;

drop schema partition_test5 cascade;
reset current_schema;