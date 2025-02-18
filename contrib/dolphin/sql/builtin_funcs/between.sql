create schema db_between;
set current_schema to 'db_between';
set timezone to 'PRC';
select 2 between 2 and 23;
select 2.1 between 2.1 and 12.3;
select true between false and true;
select 'abc' between 'abc' and 'deg';
select 11 between 1.1 and 12.5;
select true between 0 and 12;
select true between 0.9 and 12.6;
select true between '0.5' and '12.3';
select true between 'a0.5' and '12.3aa';
select 1 between 'a0.5' and '12.3aa';
select 1 between '0.5' and '12.3';
select 1 between 0 and '12.3';
select 1.1 between 0 and '12.3';
select 1.1 between 0.3 and '12.3a';
select '1a' between 'B' and '12.3b';
select 'a2.2a' between 2.1 and '4';
select 12.7 between '11@#&%*' and '19.9hd';
select 'a2.2a' between 0 and B'101';
select 'a2.2a' between 1 and B'101';
select '2.2' between 1 and B'101';
select 2.2 between 1 and B'101';
select B'001' between 1 and B'101';
select '©12gdja12' between '©11.1a' and 15.4;
select B'001' between 0.1 and true;
select 2022-06-07 between 2022-06-06 and 2022-06-10;
select b'1111111111111111111111111' between 0 and 999999999;
select 0 between '测' and '15.2';
select 1 between '测1' and '1';
select 1 between '1测' and '1';
select 'xyzhi'::text between 'xyzhi'::text and 'xyzhi'::text;
select 'xyzhi'::text between 'xyzhi'::text and 'xyzhi';
select 'xyzhi'::text between 'xyzhi' and 'xyzhi'::text;
select repeat('Pg', 4) between 0 and 'PgPgPgPg';
select replace('mm','mm',1) between 0 and 1;
select lpad('hi', 5, 'xyza') between 'xyzhi' and 10;
select 11 between '' and 0;
select '' between 34 and 110;
select 11 between 66 and '';
select '2023-01-01 12:30:00'::datetime between '2023-06-28 11:30:30'::datetime and '2023-07-28 17:30:30'::datetime;
select '2023-01-01 00:00:00' between date_add(now(),interval-30 minute) and now();
select '2023-01-01 12:30:00'::time between '2023-06-28 11:30:30'::time and '2023-07-28 17:30:30'::time;
select '12:30:00'::time between '11:30:30'::time and '2023-07-28 17:30:30'::time;
select '12:30:00' between '12:30:30' and '17:30:30';
select 1 between 0 and cast(18446744073709551615 as unsigned);
select cast(0 as unsigned) between -1 and 1;
select cast(0 as unsigned) between 0 and -1;
select 1.5::numeric between 0::numeric and 2::numeric;
select 0.5::float between 0::float and 2::float;
select b'11' between 1::float and 2.5::decimal;
select 100 between '-Inf'::float and '+Inf'::float;
select 2 between symmetric 23 and 2;
select 2.1 between symmetric 12.3 and 2.0;
select true between symmetric true and false;
select 'abc' between symmetric 'deg' and 'abc';
select (sqrt(81) between log10(100) and 20)+('2' between log2(4) and sqrt(4));
select (sqrt(81) between log10(100) and 20)-('22' between log2(4) and sqrt(4));
select sqrt(1000) > 3.14 and sqrt(2000) <= 4 between (sqrt(1212) <= 3) and (sqrt(5757) = sqrt(5757));
drop table if exists t_between_and_0015;
create table t_between_and_0015(c_0015_nn numeric);
insert into t_between_and_0015 values(0.123),(21.215),(12.5),(268/7),(5.0001),(-11.111);
select c_0015_nn from t_between_and_0015 where abs(c_0015_nn) between 10 and 20 and c_0015_nn is not null order by c_0015_nn;
select c_0015_nn from t_between_and_0015 where abs(c_0015_nn) not between 10 and 20 and c_0015_nn is not null order by c_0015_nn;
drop table t_between_and_0015;
drop table if exists t_between_and_0007;
create table t_between_and_0007(
c_id int(11) not null,
c_name varchar(255) not null,
c_age int(11) not null,
c_create_time datetime default null,
c_birthday date default null,
primary key (c_id));
insert into t_between_and_0007(c_id,c_name,c_age,c_create_time,c_birthday) values
(1,'andy',17,'2021-02-01 00:00:00','2021-01-30'),
(2,'ben',18,'2021-02-01 00:00:01','20210711'),
(3,'cindy',16,'2021-02-01 12:01:54','2021-02-01'),
(4,'davc_id',18,'2021-02-01 23:59:59','2021-02-02'),
(5,'cindy2',18,'2021-02-02 00:00:00','2021-january-03'),
(6,'cindy3',17,'2021-02-02 00:00:01','2021-august-08'),
(7,'cindy4',19,'2021-11-22 12:19:33','2021-02-28');
select * from t_between_and_0007;
select * from t_between_and_0007 where c_birthday between 20210201::date and 20210202::date;
select * from t_between_and_0007 where c_create_time between 20210115::datetime and 20210215::datetime;
select * from t_between_and_0007 where c_create_time between 20210201::datetime and 20210201::datetime;
select * from t_between_and_0007 where c_create_time between 20210201::datetime and 20210202::datetime;
select * from t_between_and_0007 where c_create_time between 20210115::timestamp and 20210215::timestamp;
select * from t_between_and_0007 where c_create_time between 20210201::timestamp and 20210201::timestamp;
select * from t_between_and_0007 where c_create_time between 20210201::timestamp and 20210202::timestamp;
create table t_between_and_0023(
c_id serial,
c_title char(4),
c_fname varchar(32),
c_lname varchar(32) not null,
c_addressline varchar(64),
c_town varchar(32),
c_zipcode char(10) not null,
c_phone varchar(16));
insert into t_between_and_0023(c_title, c_fname, c_lname, c_addressline, c_town, c_zipcode, c_phone)values
('miss','jenny','stones','27 rowan avenue','highc_town','nt2 1aq','023 9876'),
('mr','andrew','stones','52 the willows','lowc_town','lt5 7ra','876 3527'),
('miss','alex','matthew','4 the street','nicec_town','nt2 2tx','010 4567'),
('mr','adrian','matthew','the barn','yuleville','yv67 2wr','487 3871'),
('mr','simon','cozens','7 shady lane','oakenham','oa3 6qw','514 5926'),
('mr','neil','matthew','5 pasture lane','nicec_town','nt3 7rt','267 1232'),
('mr','richard','stones','34 holly way','bingham','bg4 2we','342 5982'),
('mrs','christine','hickman','36 queen street','histon','ht3 5em','342 5432'),
('mr','mike','howard','86 dysart street','tibsville','tb3 7fg','505 5482'),
('mr','dave','jones','54 vale rise','bingham','bg3 8gd','342 8264'),
('mr','richard','neill','42 thatched way','winersby','wb3 6gq','505 6482');
select * from t_between_and_0023;
select distinct c_town from t_between_and_0023 where c_town between 'b' and 'n' order by c_town;
select distinct c_town from t_between_and_0023 where c_town between 'b' and 'nz' order by c_town;
drop table t_between_and_0023;
drop table if exists test_collate;
create table test_collate(c1 text, c2 text, c3 text)charset 'utf8' collate 'utf8_unicode_ci';
insert into test_collate values
('t','T','T'),
('T','t','t'),
('TT','tT','Tt');
select c1 between c2 and c3 from test_collate ;
drop table test_collate;
drop table if exists test_nocollate;
create table test_nocollate(c1 text, c2 text, c3 text);
insert into test_nocollate values
('t','T','T'),
('T','t','t'),
('TT','tT','Tt');
select c1 between c2 and c3 from test_nocollate ;
drop table test_nocollate;
-- test with openGauss unique datatype
select '2'::money between '1' and '5'::money;
select '2' not between '1'::money and '5'::money;
select '2' between SYMMETRIC '5' and '1'::money;
select '2'::money not between SYMMETRIC '5' and '1';
-- test with 3 arg has same type
set enable_seqscan to off;
set enable_bitmapscan to off;
create table t_b1(a int1, b int2, c int4, d float, e double, f numeric, g text, h varchar(10), i char(10));
create index i_t_b1_a on t_b1(a);
create index i_t_b1_b on t_b1(b);
create index i_t_b1_c on t_b1(c);
create index i_t_b1_d on t_b1(d);
create index i_t_b1_e on t_b1(e);
create index i_t_b1_f on t_b1(f);
create index i_t_b1_g on t_b1(g);
create index i_t_b1_h on t_b1(h);
create index i_t_b1_i on t_b1(i);
analyze t_b1;
explain (costs off) select * from t_b1 where a between 1 and 2;
explain (costs off) select * from t_b1 where b between 1 and 2;
explain (costs off) select * from t_b1 where c between 1 and 2;
explain (costs off) select * from t_b1 where d between 1 and 2;
explain (costs off) select * from t_b1 where e between 1 and 2;
explain (costs off) select * from t_b1 where f between 1 and 2;
explain (costs off) select * from t_b1 where g between '1' and '2';
explain (costs off) select * from t_b1 where h between '1' and '2';
explain (costs off) select * from t_b1 where i between '1' and '2';
-- test the cmp of time and date
select date'2018-12-31' between time'23:56:59' and timestamp'2018-12-31 23:56:59';
select date'2018-12-31' not between time'23:56:59' and timestamp'2018-12-31 23:56:59';
select date'2018-12-31' between symmetric time'23:56:59' and timestamp'2018-12-31 23:56:59';
select date'2018-12-31' not between symmetric time'23:56:59' and timestamp'2018-12-31 23:56:59';
select time'23:56:59' between date'2018-12-31' and timestamp'2018-12-31 23:56:59';
select time'23:56:59' not between date'2018-12-31' and timestamp'2018-12-31 23:56:59';
select time'23:56:59' between datetime'2018-12-31' and timestamptz'2018-12-31 23:56:59';
select time'23:56:59' not between datetime'2018-12-31' and timestamptz'2018-12-31 23:56:59';
-- test the cmp of str and date/time/datetime
select '2019-12-31' between date'2018-12-31' and date'2020-12-31';
select '2019-12-31' between datetime'2018-12-31 23:56:59' and datetime'2020-12-31 23:56:59';
select '2019-12-31' between timestamp'2018-12-31 23:56:59' and timestamp'2020-12-31 23:56:59';
select '2019abc' between date'2018-12-31' and date'2020-12-31';
select '2019abc' between datetime'2018-12-31 23:56:59' and datetime'2020-12-31 23:56:59';
select '2019abc' between timestamp'2018-12-31 23:56:59' and timestamp'2020-12-31 23:56:59';
-- test the cmp of number and date/time/datetime
select period_add(202201,2) between '2022-01-01' and date'2022-03-31';
select sec_to_time(4396) between sec_to_time(4395) and 4396;
select 10 between to_days('0000-01-01') and date'0000-01-04';
select 10 between to_days('0000-01-01') and time'12:30:00';
select 10 between to_days('0000-01-01') and datetime'0000-01-04 12:30:00';
select 10 between to_days('0000-01-01') and timestamp'0000-01-04 12:30:00';
select 102 between '0000-01-01' and date'0000-01-03';
select 102 between time'00:01:01' and date'0000-01-03';
select 102 between date'0000-01-01' and timestamp'0000-01-03 00:00:00';
-- use index when DATEOID/TIMESTAMPOID/TIMESTAMPTZOID with string
create table test(time time, timetz time with time zone, date date, datetime datetime, timestamp timestamp);
insert into test values('12:00:00','12:00:00','2024-12-12','2024-12-12','2024-12-12');
create index on test(time);
create index on test(timetz);
create index on test(date);
create index on test(datetime);
create index on test(timestamp);

/* index */
set enable_seqscan to off;
explain (costs off) select * from test where time between '12:00:00' and '12:00:00';
explain (costs off) select * from test where timetz between '12:00:00' and '12:00:00';
explain (costs off) select * from test where date between '2024-12-12' and '2024-12-12';
explain (costs off) select * from test where datetime between '2024-12-12' and '2024-12-12';
explain (costs off) select * from test where timestamp between '2024-12-12' and '2024-12-12';

explain (costs off) select * from test where time between '12:00:00'::text and '12:00:00'::text; -- N
explain (costs off) select * from test where timetz between '12:00:00'::text and '12:00:00'::text; -- N
explain (costs off) select * from test where date between '2024-12-12'::text and '2024-12-13'::text;
explain (costs off) select * from test where datetime between '2024-12-12'::text and '2024-12-13'::text;
explain (costs off) select * from test where timestamp between '2024-12-12'::text and '2024-12-13'::text;

explain (costs off) select * from test where time between '12:00:00'::char(100) and '12:00:00'::char(100); -- N
explain (costs off) select * from test where date between '2024-12-12'::char(100) and '2024-12-13'::char(100); -- N
explain (costs off) select * from test where datetime between '2024-12-12'::char(100) and '2024-12-13'::char(100);
explain (costs off) select * from test where timestamp between '2024-12-12'::char(100) and '2024-12-13'::char(100);

/* all no index. */
explain (costs off) select * from test where time between '12:00:00'::blob and '12:00:00'::blob; -- failed
explain (costs off) select * from test where timetz between '12:00:00'::blob and '12:00:00'::blob; -- N
explain (costs off) select * from test where date between '2024-12-12'::blob and '2024-12-13'::blob; -- N
explain (costs off) select * from test where datetime between '2024-12-12'::blob and '2024-12-13'::blob;
explain (costs off) select * from test where timestamp between '2024-12-12'::blob and '2024-12-13'::blob;


/*  all no index. */
explain (costs off) select * from test where time between '12:00:00'::VARBINARY(100) and '12:00:00'::VARBINARY(100); -- N
explain (costs off) select * from test where timetz between '12:00:00'::VARBINARY(100) and '12:00:00'::VARBINARY(100); -- N
explain (costs off) select * from test where date between '2024-12-12'::VARBINARY(100) and '2024-12-13'::VARBINARY(100); -- N
explain (costs off) select * from test where datetime between '2024-12-12'::VARBINARY(100) and '2024-12-13'::VARBINARY(100);
explain (costs off) select * from test where timestamp between '2024-12-12'::VARBINARY(100) and '2024-12-13'::VARBINARY(100);

select * from test where time between '12:00:00' and '12:00:00'; /* exptected 1 */
select * from test where time between '11:00:00' and '12:00:00'; /* exptected 1 */
select * from test where timetz between '12:00:00' and '12:00:00'; /* exptected 1 */
select * from test where date between '2024-12-12 12:00:00' and '2024-12-12'; /* exptected 0 */
select * from test where date between '2024-12-12' and '2024-12-12 12:00:00'; /* exptected 1 */
select * from test where date between datetime and timestamp; /* exptected 1 */
select * from test where datetime between date and timestamp; /* exptected 1 */

reset timezone;
drop schema db_between cascade;
reset current_schema;