--
-- Test All Time type under 'b' compatibility
--

drop database if exists b_time_type;
create database b_time_type dbcompatibility 'b';
\c b_time_type

-- test date
-- 'YYYY-MM-DD' 'YY-MM-DD'
SELECT date'2001-01-01';
SELECT date'999-01-01';
SELECT date'1-01-01';
SELECT date'10120-01-01';
SELECT date'2000-2-29';
SELECT date'2001-2-29';
SELECT date'2100-2-29';
SELECT date'0000-2-29';
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

-- test time
CREATE TABLE time_tbl (v time);

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

-- test timestamp with[out] timezone
select timestamp with time zone'1994-11-10 23:12:34';
select timestamp without time zone'1994-11-10 23:12:34';

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
INSERT INTO timestamp_tbl VALUES (11::timestamp);
INSERT INTO timestamp_tbl VALUES (111::timestamp);
INSERT INTO timestamp_tbl VALUES (1111::timestamp);
INSERT INTO timestamp_tbl VALUES (11111::timestamp);
INSERT INTO timestamp_tbl VALUES (111111::timestamp);
INSERT INTO timestamp_tbl VALUES (1111111::timestamp);
INSERT INTO timestamp_tbl VALUES (11111111::timestamp);
INSERT INTO timestamp_tbl VALUES (111111111::timestamp);
INSERT INTO timestamp_tbl VALUES (1111111111::timestamp);
INSERT INTO timestamp_tbl VALUES (11111111111::timestamp);
INSERT INTO timestamp_tbl VALUES (111111111111::timestamp);
INSERT INTO timestamp_tbl VALUES (1111111111111::timestamp);
INSERT INTO timestamp_tbl VALUES (11111111111111::timestamp);

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
insert into t_year0035 values('1902','2');
insert into t_year0035 values('1907','5');
select ts1 + ts2 from t_year0035;
select ts1 - ts2 from t_year0035;
drop table t_year0035;

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

\c postgres
DROP DATABASE b_time_type;