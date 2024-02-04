--
-- Test All Time type under 'b' compatibility
--

drop database if exists b_time_type;
create database b_time_type dbcompatibility 'b';
\c b_time_type

-- test datetime add/substract interval
-- when b_compatibility_mode = true
set dolphin.b_compatibility_mode = true;
SELECT '1997-12-31 23:59:59' + INTERVAL 1 SECOND;
SELECT datetime'1997-12-31 23:59:59' + INTERVAL 1 SECOND;
SELECT '1997-12-31 23:59:59' - INTERVAL 1 SECOND;
SELECT datetime'1997-12-31 23:59:59' - INTERVAL 1 SECOND;
SELECT INTERVAL 1 SECOND + '1997-12-31 23:59:59';
SELECT INTERVAL 1 SECOND + datetime'1997-12-31 23:59:59';
-- when b_compatibility_mode = false;
set dolphin.b_compatibility_mode = false;
SELECT '1997-12-31 23:59:59' + INTERVAL 1 SECOND;
SELECT datetime'1997-12-31 23:59:59' + INTERVAL 1 SECOND;
SELECT '1997-12-31 23:59:59' - INTERVAL 1 SECOND;
SELECT datetime'1997-12-31 23:59:59' - INTERVAL 1 SECOND;
SELECT INTERVAL 1 SECOND + '1997-12-31 23:59:59';
SELECT INTERVAL 1 SECOND + datetime'1997-12-31 23:59:59';

-- test date
-- 'YYYY-MM-DD' 'YY-MM-DD'
SELECT date'2001-01-01';
SELECT date'999-01-01';
SELECT date'1-01-01';
SELECT date'10120-01-01';
SELECT date'2000-2-29';
SELECT date'2001-2-29';
SELECT date'2100-2-29';
SELECT date'0000-1-1';
SELECT date'01-01';
SELECT date'9-01-01';
SELECT date'99-01-01';
SELECT date'999-01-01';
SELECT date'1000-01-1';

-- 'YYYYMMDD' or 'YYMMDD'
SELECT date'99991231';
SELECT date'100000101';
SELECT date'10000101';
SELECT date'9990101';
SELECT date'701010';
SELECT date'691010';
SELECT date'691310';
SELECT date'691131';
SELECT date'10101';
SELECT date'10130';
SELECT date'00120101';

-- YYYYMMDD or YYYYMMDD
SELECT 99990101::date;
SELECT 990101::date;
SELECT 100::date;
SELECT 101::date;
SELECT 10228::date;
SELECT 10229::date;
SELECT 991231::date;
SELECT 99991231::date;
SELECT 21000228::date;
SELECT 21000229::date;
SELECT 100000000::date;
SELECT 100101231::date;

-- test time
-- '[-][D] hh:mm:ss.fsec'
SELECT time(7)'12:12:12.123456';
SELECT time(6)'12:12:12.123456';
SELECT time(5)'12:12:12.123456';
SELECT time(4)'12:12:12.123456';
SELECT time(3)'12:12:12.123456';
SELECT time(2)'12:12:12.123456';
SELECT time(1)'12:12:12.123456';
SELECT time(0)'12:12:12.123456';
SELECT time'12:12:12.123456';
SELECT time'34 22:59:59.999999';
SELECT time'12:60:12.123456';
SELECT time'12:12:60.123456';
SELECT time'34 23:00:00';
SELECT time'-34 23:00:00';
SELECT time'34 22:59:59';
SELECT time'-34 22:59:59';
SELECT time'34 22:59:59.999999';
SELECT time'34 22:59:59.9999999999999999999999';
SELECT time'-34 22:59:59.9999999999999999999999';
SELECT time'-34 22:59:59.9999999999999999999999';
SELECT time'838:59:59';
SELECT time'839:00:00';
SELECT time'59:59';
SELECT time'59:59.123';
SELECT time'59';
SELECT time'24 24';
SELECT time'24 12:13';
SELECT time'59.12';
SELECT time'';
SELECT time'1 09';
SELECT time'0 099';
SELECT time'-2 00000009';

-- 'hhmmss.fsec'
SELECT time(1)'1.123456';
SELECT time(2)'12.123456';
SELECT time(3)'123.123456';
SELECT time(4)'1234.123456';
SELECT time(5)'12345.123456';
SELECT time'123456.123456';

-- hhmmss.fsec
SELECT 2::time;
SELECT 20::time;
SELECT 201::time;
SELECT 2011::time;
SELECT 2115.12::time;
SELECT 20115.23::time;
SELECT 233445.123::time;
SELECT 233445.123::time;
SELECT 8385959.999999::time;
SELECT 8385959.9999999::time;
SELECT -232323::time;
SELECT -8385959::time;
CREATE TABLE time_tbl (v time);
INSERT INTO time_tbl VALUES(-561234);
INSERT INTO time_tbl VALUES(-1202334);
INSERT INTO time_tbl VALUES(99991231235959);
INSERT INTO time_tbl VALUES(99991231235959.999999);
INSERT INTO time_tbl VALUES('20220-01-01');
-- test table's dafault typmod
INSERT INTO time_tbl VALUES(232323.123);
SELECT * FROM time_tbl;
-- test alter table add [column]
ALTER TABLE time_tbl ADD COLUMN dt datetime;
ALTER TABLE time_tbl ADD ts timestamp;
INSERT INTO time_tbl VALUES(232323.555, 20201212101010.555, 20201212101010.555);
SELECT * FROM time_tbl;
DROP TABLE time_tbl;
-- test alter table modify column
CREATE TABLE time_tbl (v time(2));
INSERT INTO time_tbl VALUES(232323.555);
SELECT * FROM time_tbl;
ALTER TABLE time_tbl MODIFY v time;
SELECT * FROM time_tbl;
DROP TABLE time_tbl;

-- test function
SELECT time '830:12:34' + interval '3 hours';
SELECT time '830:12:34' + interval '3 minutes';
SELECT time '830:12:34' + interval '3 seconds';
SELECT time '830:12:34' + interval '26 seconds';
SELECT time '830:12:34' - interval '3 hours';
SELECT time '837:00:00' + interval '2 hours';
SELECT time '838:59:59' + interval '1 second';
SELECT time '-837:00:00' - interval '3 hours';
SELECT time '1:00:00' - interval '3 hours';

-- test datetime
-- 'YYYY-MM-DD hh:mm:ss.fsec' or 'YY-MM-DD hh:mm:ss.fsec'
SELECT datetime(2)'2001-01-01  23:59:59.999999';
SELECT datetime(3)'2001-01-01  23:59:59.9999999';
SELECT datetime(4)'9999-12-31  23:59:59.9999999';
SELECT datetime(5)'999-12-31  23:59:59.9999999';
SELECT datetime'99-01-01  23:59:59.123';
SELECT datetime'2001-2-28  23:59:59.123';
SELECT datetime'2000-2-28  23:59:59.123';
SELECT datetime'2000-2-28  12:23:34.5678';

-- 'YYYYMMDDhhmmss.fsec' or 'YYMMDDhhmmss.fsec'
SELECT datetime(1)'20010101010101.1278';
SELECT datetime(1)'010101010101.1234';
SELECT datetime(2)'0101010101';
SELECT datetime(1)'01010101';
SELECT datetime(3)'0101017';
SELECT datetime(1)'010101';
SELECT datetime(1)'10101';
SELECT datetime(1)'71101';
SELECT datetime(1)'0101';

-- YYYYMMDDhhmmss or YYMMDDhhmmss
SELECT 11::datetime;
SELECT 111::datetime;
SELECT 1111::datetime;
SELECT 11111::datetime;
SELECT 111111::datetime;
SELECT 1111111::datetime;
SELECT 11111111::datetime;
SELECT 111111111::datetime;
SELECT 1111111111::datetime;
SELECT 11111111111::datetime;
SELECT 111111111111::datetime;
SELECT 1111111111111::datetime;
SELECT 11111111111111::datetime;

-- test datetime operation
create table test_dt(dt datetime);
insert into test_dt values('1997-11-10');
insert into test_dt values('2000-11-10');
insert into test_dt values('1997-11-10');
insert into test_dt values('2012-11-10');
insert into test_dt values('2008-11-10');
insert into test_dt values('1994-11-10');
select max(dt) from test_dt;
select min(dt) from test_dt;
select distinct(dt) from test_dt;
select * from test_dt;
create index on test_dt(dt);
select * from test_dt where dt > '2007-01-01';
select * from test_dt where dt = '1997-11-10';
select * from test_dt where dt < '1997-11-10';

-- test now() and current_timestamp
create table test_datetime(c1 datetime);
insert into test_datetime values(now());
insert into test_datetime values(current_timestamp);
drop table test_datetime;

-- test timestamp with[out] timezone
select timestamp with time zone'1994-11-10 23:12:34.5678';
select timestamp without time zone'1994-11-10 23:12:34.5678';
select timestamp'1994-11-10 23:12:34.5678';

-- test timestamp
set time zone 'PRC';
CREATE TABLE timestamp_tbl (v timestamp(2));

-- 'YYYY-MM-DD hh:mm:ss.fsec' or 'YY-MM-DD hh:mm:ss.fsec'
INSERT INTO timestamp_tbl VALUES ('2001-01-01  23:59:59.999999');
INSERT INTO timestamp_tbl VALUES ('2001-01-01  23:59:59.9999999');
INSERT INTO timestamp_tbl VALUES ('9999-12-31  23:59:59.9999999');
INSERT INTO timestamp_tbl VALUES ('999-12-31  23:59:59.9999999');
INSERT INTO timestamp_tbl VALUES ('99-01-01  23:59:59.123');
INSERT INTO timestamp_tbl VALUES ('2001-2-28  23:59:59.123');
INSERT INTO timestamp_tbl VALUES ('2000-2-28  23:59:59.123');

-- 'YYYYMMDDhhmmss.fsec' or 'YYMMDDhhmmss.fsec'
INSERT INTO timestamp_tbl VALUES ('20010101010101.1278');
INSERT INTO timestamp_tbl VALUES ('010101010101.1234');
INSERT INTO timestamp_tbl VALUES ('0101010101');
INSERT INTO timestamp_tbl VALUES ('01010101');
INSERT INTO timestamp_tbl VALUES ('0101017');
INSERT INTO timestamp_tbl VALUES ('010101');
INSERT INTO timestamp_tbl VALUES ('0101');

-- YYYYMMDDhhmmss or YYMMDDhhmmss
INSERT INTO timestamp_tbl VALUES (11);
INSERT INTO timestamp_tbl VALUES (111);
INSERT INTO timestamp_tbl VALUES (1111);
INSERT INTO timestamp_tbl VALUES (11111);
INSERT INTO timestamp_tbl VALUES (111111);
INSERT INTO timestamp_tbl VALUES (1111111);
INSERT INTO timestamp_tbl VALUES (11111111);
INSERT INTO timestamp_tbl VALUES (111111111);
INSERT INTO timestamp_tbl VALUES (1111111111);
INSERT INTO timestamp_tbl VALUES (11111111111);
INSERT INTO timestamp_tbl VALUES (111111111111);
INSERT INTO timestamp_tbl VALUES (1111111111111);
INSERT INTO timestamp_tbl VALUES (11111111111111);

SELECT * FROM timestamp_tbl;
set time zone 'UTC';
SELECT * FROM timestamp_tbl;
set time zone 'PRC';

DROP TABLE timestamp_tbl;

-- test YEAR
DROP TABLE IF EXISTS year_tbl;

-- test invalid display width
CREATE TABLE year_tbl (y YEAR(-1));
CREATE TABLE year_tbl (y YEAR(0));
CREATE TABLE year_tbl (y YEAR(1));
CREATE TABLE year_tbl (y YEAR(3));
CREATE TABLE year_tbl (y YEAR(5));
CREATE TABLE year_tbl (y YEAR(4294967297));

-- test YEAR YEAR(2) YEAR(4)
CREATE TABLE year_tbl (y YEAR, y4 YEAR(4), y2 YEAR(2));
INSERT INTO year_tbl VALUES (1900, 1900, 1900);
INSERT INTO year_tbl VALUES ('1900', '1900', '1900');
INSERT INTO year_tbl VALUES (2156, 2156, 2156);
INSERT INTO year_tbl VALUES ('2156', '2156', '2156');

INSERT INTO year_tbl VALUES (1901, 1901, 1901);
INSERT INTO year_tbl VALUES ('1901', '1901', '1901');
INSERT INTO year_tbl VALUES (2155, 2155, 2155);
INSERT INTO year_tbl VALUES ('2155', '2155', '2155');

INSERT INTO year_tbl VALUES (70, 70, 70);
INSERT INTO year_tbl VALUES ('70', '70', '70');
INSERT INTO year_tbl VALUES (69, 69, 69);
INSERT INTO year_tbl VALUES ('69', '69', '69');
INSERT INTO year_tbl VALUES (2156, 2156, 2156);
INSERT INTO year_tbl VALUES ('2156', '2156', '2156');
INSERT INTO year_tbl VALUES (-1, -1, -1);
INSERT INTO year_tbl VALUES ('-1', '-1', '-1');

INSERT INTO year_tbl VALUES (0, 0, 0);
INSERT INTO year_tbl VALUES ('0', '0', '0');
INSERT INTO year_tbl VALUES ('00', '00', '00');
INSERT INTO year_tbl VALUES ('000', '000', '000');
INSERT INTO year_tbl VALUES ('0000', '0000', '0000');
INSERT INTO year_tbl VALUES (1, 1, 1);
INSERT INTO year_tbl VALUES ('1', '1', '1');

SELECT * FROM year_tbl;
-- test order by
SELECT * FROM year_tbl ORDER BY y ASC;
-- test max, min, distinct
SELECT max(y), min(y4) FROM year_tbl;
SELECT distinct(y) FROM year_tbl;
-- test index
CREATE INDEX test_b_tree_idx ON year_tbl(y);
DROP INDEX test_b_tree_idx;

DROP TABLE year_tbl;

-- test operation of YEAR and YEAR(2)
SELECT ('2010')::YEAR(2)<(2001)::YEAR;
SELECT ('2010')::YEAR(2)>(2001)::YEAR;
SELECT ('2010')::YEAR(2)>(2001);
SELECT (2010)>(2001)::YEAR;
SELECT (2001)=(2001)::YEAR;
SELECT (2001)=(2001)::YEAR(2);

-- test operation of year and YEAR(2)
SELECT (2010)::YEAR-(2001)::YEAR;
SELECT (2010)::YEAR-(2111)::YEAR;
SELECT (2010)::YEAR-(2111)::YEAR;

SELECT (2010)::YEAR-(interval '2' year);
SELECT (2010)::YEAR+(interval '2' year);

SELECT (1970)::YEAR-(interval '69' year);
SELECT (1970)::YEAR-(interval '70' year);
SELECT (2069)::YEAR+(interval '86' year);
SELECT (2069)::YEAR+(interval '87' year);

SELECT (2069)::YEAR+(interval'366day');

SELECT (2010)::YEAR(2)-(2001)::YEAR(2);
SELECT (2010)::YEAR(2)-(2111)::YEAR;
SELECT (2010)::YEAR-(2111)::YEAR(2);

SELECT (2010)::YEAR(2)-(interval '2' year);
SELECT (2010)::YEAR(2)+(interval '2' year);

SELECT (1970)::YEAR(2)-(interval '69' year);
SELECT (1970)::YEAR(2)-(interval '70' year);
SELECT (2069)::YEAR(2)+(interval '86' year);
SELECT (2069)::YEAR(2)+(interval '87' year);

SELECT (2069)::YEAR(2)+(interval'366day');

-- test year +/- integer
create table t_year0035(ts1 year(4) ,ts2 integer);
insert into t_year0035 values('1904','1');
insert into t_year0035 values('1902','2');
insert into t_year0035 values('1907','5');
select ts1 + ts2 from t_year0035;
select ts2 + ts1 from t_year0035;
select ts1 - ts2 from t_year0035;
select ts2 - ts1 from t_year0035;
drop table t_year0035;
-- test overflow
select integer'2147481650' + year'1998';
select year'1998' + integer'2147481650';
select integer'-2147481651' - year'1997';

-- test year input
select year '20$#12';
select year '20中文12';
select year '    1997    ';
select year '  1997';
select year '1997  ';
select year ' 1997  89';
select year ' 1997  #@#';
select year '&%2122';
select year '中文2122';
select year '98.3';
select year '98.5';
select year '00001';
select year '-0.2';
select year '-000';
select year '';
select year ' ';
select year 'abcd';
select year '001a';
select year 'a001';
select year 'a000';
select year '000a';
select ''::longblob::year;

-- test partition key
CREATE TABLESPACE b_time_type_example RELATIVE LOCATION 'tablespace1/tablespace_1';
-- year range key
CREATE TABLE y_range
(
    y year
)
TABLESPACE b_time_type_example
PARTITION BY RANGE(y)
(
    PARTITION P1 VALUES LESS THAN(2000),
    PARTITION P2 VALUES LESS THAN(2050)
)
ENABLE ROW MOVEMENT;
INSERT INTO y_range VALUES(1997);
INSERT INTO y_range VALUES(2030);
INSERT INTO y_range VALUES(2060);
SELECT * FROM y_range;
SELECT * FROM y_range PARTITION(P1);
SELECT * FROM y_range PARTITION(P2);
DROP TABLE y_range;
-- time range key
CREATE TABLE ti_range
(
    t time
)
TABLESPACE b_time_type_example
PARTITION BY RANGE(t)
(
        PARTITION P1 VALUES LESS THAN('-12:00:00'),
        PARTITION P2 VALUES LESS THAN('48:00:00')
)
ENABLE ROW MOVEMENT;
INSERT INTO ti_range VALUES('-20:00:00');
INSERT INTO ti_range VALUES('20:00:00');
INSERT INTO ti_range VALUES('120:00:00');
SELECT * FROM ti_range;
SELECT * FROM ti_range PARTITION(P1);
SELECT * FROM ti_range PARTITION(P2);
DROP TABLE ti_range;
-- year list key
CREATE TABLE y_list
(
    y year
)
TABLESPACE b_time_type_example
PARTITION BY LIST(y)
(
    PARTITION P1 VALUES IN(1991, 1997, 2000),
    PARTITION P2 VALUES IN(1992, 1998, 2010)
)
ENABLE ROW MOVEMENT;
INSERT INTO y_list VALUES(1991);
INSERT INTO y_list VALUES(1997);
INSERT INTO y_list VALUES(2000);
INSERT INTO y_list VALUES(1992);
INSERT INTO y_list VALUES(1998);
INSERT INTO y_list VALUES(2010);
INSERT INTO y_list VALUES(2011);
INSERT INTO y_list VALUES(1990);
SELECT * FROM y_list ORDER BY y;
SELECT * FROM y_list PARTITION(P1);
SELECT * FROM y_list PARTITION(P2);
DROP TABLE y_list;
-- time list key
CREATE TABLE ti_list
(
    t time
)
TABLESPACE b_time_type_example
PARTITION BY LIST(t)
(
        PARTITION P1 VALUES IN('12:00:00', '60000', '-3:0:0'),
        PARTITION P2 VALUES IN('2:0:0', '-5:0:0')
)
ENABLE ROW MOVEMENT;
INSERT INTO ti_list VALUES('12:00:00');
INSERT INTO ti_list VALUES('02:00:00');
INSERT INTO ti_list VALUES('6:00:00');
INSERT INTO ti_list VALUES('-30000');
INSERT INTO ti_list VALUES('-50000');
INSERT INTO ti_list VALUES('3:12:12');
SELECT * FROM ti_list;
SELECT * FROM ti_list PARTITION(P1);
SELECT * FROM ti_list PARTITION(P2);
DROP TABLE ti_list;

-- year hash key
CREATE TABLE y_hash
(
    y year
)
TABLESPACE b_time_type_example
PARTITION BY HASH(y)
(
    PARTITION P1,
    PARTITION P2,
    PARTITION P3,
    PARTITION P4
);
INSERT INTO y_hash VALUES(1991);
INSERT INTO y_hash VALUES(2010);
INSERT INTO y_hash VALUES(2054);
INSERT INTO y_hash VALUES(2056);
INSERT INTO y_hash VALUES(2000);
INSERT INTO y_hash VALUES(1992);
INSERT INTO y_hash VALUES(1998);
INSERT INTO y_hash VALUES(1970);
INSERT INTO y_hash VALUES(1980);
INSERT INTO y_hash VALUES(2133);
INSERT INTO y_hash VALUES(2100);
INSERT INTO y_hash VALUES(2122);
SELECT * FROM y_hash order by 1;
SELECT * FROM y_hash PARTITION(P1);
SELECT * FROM y_hash PARTITION(P2);
SELECT * FROM y_hash PARTITION(P3);
SELECT * FROM y_hash PARTITION(P4);
DROP TABLE y_hash;
-- time hash key
CREATE TABLE ti_hash
(
    ti time
)
TABLESPACE b_time_type_example
PARTITION BY HASH(ti)
(
    PARTITION P1,
    PARTITION P2
);
INSERT INTO ti_hash VALUES('12:1:1');
INSERT INTO ti_hash VALUES('12:1:2');
INSERT INTO ti_hash VALUES('12:1:3');
INSERT INTO ti_hash VALUES('12:1:4');
INSERT INTO ti_hash VALUES('12:1:5');
SELECT * FROM ti_hash order by 1;
SELECT * FROM ti_hash PARTITION(P1);
SELECT * FROM ti_hash PARTITION(P2);
DROP TABLE ti_hash;

-- test cast function
SELECT cast(-232323.555 as time);
SELECT cast('20121010101010.555' as datetime);
select cast(('2001-10-10 11:11:59.123456'::time(6)) as int);
select cast(('2001-10-10 11:11:59.123456'::date) as int);
select cast(('2001-10-10 11:11:59.123456'::datetime(6)) as bigint);

-- test time type operations
select ('2001-10-10 11:11:59.123456'::time(6) + 1);
select ('2001-10-10 11:11:59.123456'::time(6) - 1);
select ('2001-10-10 11:11:59.123456'::time(6) / 2.0);
select ('2001-10-10 11:11:59.123456'::datetime(6) + 1);
select ('2001-10-10 11:11:59.123456'::datetime(6) - 1);
select ('2001-10-10 11:11:59.123456'::datetime(6) / 2.0);

-- test time type operations in not 'public' schema
select pg_typeof(year'2010' + interval '2' year);
select pg_typeof(year'2010' - interval '2' year);
select pg_typeof(time'01:01:01' + 1);
select pg_typeof(time'01:01:01' - 1);
select pg_typeof(datetime'2000-01-01 01:01:01' + 1);
select pg_typeof(datetime'2000-01-01 01:01:01' - 1);
create schema if not exists test_schema;
set current_schema to 'test_schema';
select pg_typeof(year'2010' + interval '2' year);
select pg_typeof(year'2010' - interval '2' year);
select pg_typeof(time'01:01:01' + 1);
select pg_typeof(time'01:01:01' - 1);
select pg_typeof(datetime'2000-01-01 01:01:01' + 1);
select pg_typeof(datetime'2000-01-01 01:01:01' - 1);
set current_schema to 'public';
drop schema if exists test_schema;

-- no strict mode
set dolphin.sql_mode = '';
CREATE TABLE time_tbl (v time);
INSERT INTO time_tbl VALUES('20220-01-01');
select * from time_tbl;
DROP TABLE time_tbl;

-- test sql_mode NO_ZERO_DATE
CREATE TABLE t_NO_ZERO_DATE_date(v date);
CREATE TABLE t_NO_ZERO_DATE_datetime(v datetime);
CREATE TABLE t_NO_ZERO_DATE_timestamp(v timestamp);

INSERT INTO t_NO_ZERO_DATE_date(v) VALUES(0);
INSERT INTO t_NO_ZERO_DATE_date(v) VALUES('0000-00-00');
INSERT INTO t_NO_ZERO_DATE_date(v) VALUES('0000-00-01');
INSERT INTO t_NO_ZERO_DATE_date(v) VALUES('0000-01-00');
INSERT INTO t_NO_ZERO_DATE_date(v) VALUES('2000-00-00');
INSERT INTO t_NO_ZERO_DATE_datetime(v) VALUES('0000-00-00 10:01');
INSERT INTO t_NO_ZERO_DATE_datetime(v) VALUES('0000-00-01 10:01');
INSERT INTO t_NO_ZERO_DATE_datetime(v) VALUES('0000-01-00 10:01');
INSERT INTO t_NO_ZERO_DATE_datetime(v) VALUES('2000-00-00 10:01');
INSERT INTO t_NO_ZERO_DATE_timestamp(v) VALUES('0000-00-00 10:01');
INSERT INTO t_NO_ZERO_DATE_timestamp(v) VALUES('0000-00-01 10:01');
INSERT INTO t_NO_ZERO_DATE_timestamp(v) VALUES('0000-01-00 10:01');
INSERT INTO t_NO_ZERO_DATE_timestamp(v) VALUES('2000-00-00 10:01');

SET dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group';
INSERT INTO t_NO_ZERO_DATE_date(v) VALUES(0);
INSERT INTO t_NO_ZERO_DATE_date(v) VALUES('0000-00-00');
INSERT INTO t_NO_ZERO_DATE_date(v) VALUES('0000-00-01');
INSERT INTO t_NO_ZERO_DATE_date(v) VALUES('0000-01-00');
INSERT INTO t_NO_ZERO_DATE_date(v) VALUES('2000-00-00');
INSERT INTO t_NO_ZERO_DATE_datetime(v) VALUES('0000-00-00 10:01');
INSERT INTO t_NO_ZERO_DATE_datetime(v) VALUES('0000-00-01 10:01');
INSERT INTO t_NO_ZERO_DATE_datetime(v) VALUES('0000-01-00 10:01');
INSERT INTO t_NO_ZERO_DATE_datetime(v) VALUES('2000-00-00 10:01');
INSERT INTO t_NO_ZERO_DATE_timestamp(v) VALUES('0000-00-00 10:01');
INSERT INTO t_NO_ZERO_DATE_timestamp(v) VALUES('0000-00-01 10:01');
INSERT INTO t_NO_ZERO_DATE_timestamp(v) VALUES('0000-01-00 10:01');
INSERT INTO t_NO_ZERO_DATE_timestamp(v) VALUES('2000-00-00 10:01');

SELECT * FROM t_NO_ZERO_DATE_date order by v;
SELECT * FROM t_NO_ZERO_DATE_datetime order by v;
SELECT * FROM t_NO_ZERO_DATE_timestamp order by v;

DROP TABLE t_NO_ZERO_DATE_date;
DROP TABLE t_NO_ZERO_DATE_datetime;
DROP TABLE t_NO_ZERO_DATE_timestamp;
RESET dolphin.sql_mode;

drop table if exists t_date;
drop table if exists t_datetime;
create table t_date(a date);
create table t_datetime(a datetime);

reset dolphin.sql_mode;
insert into t_date values('0000-00-00');
insert into t_datetime values('0000-00-00');
insert ignore into t_date values('0000-00-00');
insert ignore into t_datetime values('0000-00-00');
select * from t_date;
select * from t_datetime;

set dolphin.sql_mode = 'no_zero_date';
insert into t_date values('0000-00-00');
insert into t_datetime values('0000-00-00');
insert ignore into t_date values('0000-00-00');
insert ignore into t_datetime values('0000-00-00');
select * from t_date;
select * from t_datetime;

set dolphin.sql_mode = 'sql_mode_strict';
insert into t_date values('0000-00-00');
insert into t_datetime values('0000-00-00');
insert ignore into t_date values('0000-00-00');
insert ignore into t_datetime values('0000-00-00');
select * from t_date;
select * from t_datetime;

set dolphin.sql_mode = '';
insert into t_date values('0000-00-00');
insert into t_datetime values('0000-00-00');
insert ignore into t_date values('0000-00-00');
insert ignore into t_datetime values('0000-00-00');
select * from t_date;
select * from t_datetime;

SET TIME ZONE PRC;
CREATE TABLE test_timestamp(ts timestamp(2));
INSERT INTO test_timestamp VALUES ('2012-10-21 23:55:23-12:12');
INSERT INTO test_timestamp VALUES (201112234512);
SELECT * FROM test_timestamp;
SET TIME ZONE UTC;
SELECT * FROM test_timestamp;

drop table if exists t_date;
drop table if exists t_datetime;
drop table if exists test_timestamp;

set dolphin.b_compatibility_mode = true;
select cast(true as date) as result;
select cast(false as date) as result;
select cast(100::int1 as date) as result;
select cast(100::uint1 as date) as result;
select cast(100::int2 as date) as result;
select cast(100::uint2 as date) as result;
select cast(100::int4 as date) as result;
select cast(100::uint4 as date) as result;
select cast(100::int4 as date) as result;
select cast(100::uint8 as date) as result;
select cast(100::float4 as date) as result;
select cast(100::float8 as date) as result;
select cast(100::numeric as date) as result;


select cast(100::int1 as datetime) as result;
select cast(100::uint1 as datetime) as result;
select cast(100::int2 as datetime) as result;
select cast(100::uint2 as datetime) as result;
select cast(100::int4 as datetime) as result;
select cast(100::uint4 as datetime) as result;
select cast(100::int8 as datetime) as result;
select cast(100::uint8 as datetime) as result;
select cast(100::float4 as datetime) as result;
select cast(100::float8 as datetime) as result;
select cast(100::numeric as datetime) as result;

select cast(100::int1 as timestamp with time zone) as result;
select cast(100::uint1 as timestamp with time zone) as result;
select cast(100::int2 as timestamp with time zone) as result;
select cast(100::uint2 as timestamp with time zone) as result;
select cast(100::int4 as timestamp with time zone) as result;
select cast(100::uint4 as timestamp with time zone) as result;
select cast(100::int8 as timestamp with time zone) as result;
select cast(100::uint8 as timestamp with time zone) as result;
select cast(100::float4 as timestamp with time zone) as result;
select cast(100::float8 as timestamp with time zone) as result;
select cast(100::numeric as timestamp with time zone) as result;

select unix_timestamp(1.5);

select cast('4714-11-24 10:10:10 BC' as date);
select cast('4714-11-24 10:10:10 BC' as timestamp with time zone);
select cast('4714-11-24 10:10:10 BC' as datetime);
select cast('-100-11-24 BC' as date);
select cast('4715-11-24 10:10:10 BC' as date);
select cast('4715-11-24 10:10:10 BC' as timestamp with time zone);
select cast('4715-11-24 10:10:10 BC' as datetime);

create table t1(c1 date, c2 datetime, c3 timestamp with time zone);
insert into t1 values ('4714-11-24 BC', '4714-11-24 10:10:10 BC', '4714-11-24 10:10:10 BC');
insert ignore into t1 values ('4715-11-24 BC', '4715-11-24 10:10:10 BC', '4715-11-24 10:10:10 BC');
select * from t1 order by 1;

drop table t1;

\c postgres
DROP DATABASE b_time_type;
DROP TABLESPACE b_time_type_example;
