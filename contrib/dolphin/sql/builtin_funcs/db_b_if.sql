create schema db_b_if;
set current_schema to 'db_b_if';


select if(TRUE, 1, 2);
select if(FALSE, 1, 2);
-- string type for first param
select if('abc', 1, 2);
select if('ASD', 1, 2);
select if('ASD', 1, 2);

-- '2022-01-30' is text, date '2022-01-30' is date
CREATE VIEW test_view as select '2022-01-30' as text_type, date '2022-01-30' as date_type;
\d+ test_view

-- string to date
select if (true, '2022-01-30', date '2022-01-30') as a, if (false, '2022-01-30', date '2022-01-30') as b;

-- string to numeric
select if (true, '2022-01-30', 1.1::numeric(10, 2)) as a, if (false, '2022-01-30', 1.1::numeric(10, 2)) as b;

-- string to float
select if (true, '2022-01-30', 1.1::float8) as a, if (false, '2022-01-30', 1.1::float8) as b;

-- string to integer
select if (true, '2022-01-30', 1::int4) as a, if (false, '2022-01-30', 1::int4) as b;

-- date to numeric
select if (true, date '2022-01-30', 1.1::numeric(10, 2)) as a, if (false, date '2022-01-30', 1.1::numeric(10, 2)) as b;

-- date to float
select if (true, date '2022-01-30', 1.1::float8) as a, if (false, date '2022-01-30', 1.1::float8) as b;

-- date to integer
select if (true, date '2022-01-30', 1::int4) as a, if (false, date '2022-01-30', 1::int4) as b;

-- numeric to float
select if (true, 2.2::numeric(10, 2), 1.1::float8) as a, if (false, 2.2::numeric(10, 2), 1.1::float8) as b;

-- numeric to integer
select if (true, 2.2::numeric(10, 2), 1::int4) as a, if (false, 2.2::numeric(10, 2), 1::int4) as b;

-- float to integer
select if (true, 2.2::float8, 1::int4) as a, if (false, 2.2::float8, 1::int4) as b;

-- TODO
-- binary to date
select if (true, 'aaa'::binary(5), date '2022-01-30') as a, if (false, 'aaa'::binary(5), date '2022-01-30') as b;
-- binary to numeric
select if (true, 'aaa'::binary(5), 1.1::numeric(10, 2)) as a, if (false, 'aaa'::binary(5), 1.1::numeric(10, 2)) as b;
-- binary to float
select if (true, 'aaa'::binary(5), 1.1::float8) as a, if (false, 'aaa'::binary(5), 1.1::float8) as b;
-- binary to integer
select if (true, 'aaa'::binary(5), 1::int4) as a, if (false, 'aaa'::binary(5), 1::int4) as b;
-- varbinary to date
select if (true, 'aaa'::varbinary(5), date '2022-01-30') as a, if (false, 'aaa'::varbinary(5), date '2022-01-30') as b;
-- varbinary to numeric
select if (true, 'aaa'::varbinary(5), 1.1::numeric(10, 2)) as a, if (false, 'aaa'::varbinary(5), 1.1::numeric(10, 2)) as b;
-- varbinary to float
select if (true, 'aaa'::varbinary(5), 1.1::float8) as a, if (false, 'aaa'::varbinary(5), 1.1::float8) as b;
-- varbinary to integer
select if (true, 'aaa'::varbinary(5), 1::int4) as a, if (false, 'aaa'::varbinary(5), 1::int4) as b;
-- blob to date
select if (true, 'aaa'::blob, date '2022-01-30') as a, if (false, 'aaa'::blob, date '2022-01-30') as b;
-- blob to numeric
select if (true, 'aaa'::blob, 1.1::numeric(10, 2)) as a, if (false, 'aaa'::blob, 1.1::numeric(10, 2)) as b;
-- blob to float
select if (true, 'aaa'::blob, 1.1::float8) as a, if (false, 'aaa'::blob, 1.1::float8) as b;
-- blob to integer
select if (true, 'aaa'::blob, 1::int4) as a, if (false, 'aaa'::blob, 1::int4) as b;

-- ERROR
-- string to boolean
select if (true, '2022-01-30', true) as a, if (false, '2022-01-30', true) as b;

-- float to boolean
select if (true, 1.1::float8, true) as a, if (false, 1.1::float8, true) as b;

-- numeric to boolean
select if (true, 2.2::numeric(10, 2), true) as a, if (false, 2.2::numeric(10, 2), true) as b;

-- int to connproc
select pg_typeof(case when conproc < 5000 then conproc else 0 end) from pg_conversion limit 1;


drop table if exists t_if_001;
create table t_if_001(c_01 int primary key,c_02 text,c_03 int,c_04 bool,c_05 date,c_06 float);
insert into t_if_001 values(1,null,2,true,null,12.3),
(2,'abc',123,false,'2022-03-08',0.00),
(3,'ADB',-123,false,'9999-12-31',1.005),
(4,'aAzZ',12.3,false,'2022-03-08',-1.00),
(5,'&*$#@',-2147483647,true,'2022-03-08',-12.6),
(7,'',2147483647,true,'2022-03-08',-12.300);
select if(c_02,c_03,c_04) from t_if_001 where c_01=2;
select if(c_02,c_03,c_04) from t_if_001 where c_01=3;
select if(c_02,c_03,c_04) from t_if_001 where c_01=4;
select if(1,c_03,c_04) from t_if_001;
select if(1,c_04,c_03) from t_if_001;
select if(0,c_03,c_04) from t_if_001;
select if(0,c_04,c_03) from t_if_001;

set dolphin.sql_mode = '';
create table t_if_005_1(c_01 int,c_02 varchar(20),c_03 int,c_04 bool);
insert into t_if_005_1 values(1,'leishuai',25,1),(2,'lizhuo',30,1),(3,'liyazhou',24,1),(4,'zhanghuan',18,0),(5,null,23,0),(6,null,null,1),(7,'ningjie',null,0);
select c_02,c_03,c_04 from t_if_005_1 group by if(1,c_04,c_02);
select c_02,c_03,c_04 from t_if_005_1 group by if(0,c_04,c_02);
select c_02,c_03,c_04 from t_if_005_1 group by if(1,c_02,c_04);
select c_02,c_03,c_04 from t_if_005_1 group by if(0,c_02,c_04);

reset dolphin.sql_mode;
select if(1, c_02, c_04) from t_if_005_1;
select if(1, c_02,c_04) from t_if_005_1;
select if(0, c_02,c_04) from t_if_005_1;
select if(0, c_04,c_02) from t_if_005_1;


create table test_datetime(c1 datetime, c2 bool);
insert into test_datetime values ('2024-01-01 10:10:10', 0);
insert into test_datetime values ('2024-01-02 10:10:10', 1);
select if (1, c1, c2) from test_datetime;
select if (0, c1, c2) from test_datetime;
select if (1, c2, c1) from test_datetime;
select if (0, c2, c1) from test_datetime;

select if(1,5 <> 3.14 or 0.5 >= 4,2) result;

drop table test_datetime;
drop table t_if_005_1;
drop table t_if_001;

drop schema db_b_if cascade;
reset current_schema;