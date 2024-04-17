create schema test_subquery;
set current_schema to test_subquery;
set dolphin.b_compatibility_mode = on;

drop table if exists test_type_table cascade;
create table test_type_table
(
  `int1`         tinyint,
  `uint1`        tinyint unsigned,
  `int2`         smallint,
  `uint2`        smallint unsigned,
  `int4`         integer,
  `uint4`        integer unsigned,
  `int8`         bigint,
  `uint8`        bigint unsigned,
  `float4`       float4,
  `float8`       float8,
  `numeric`      decimal(20, 6),

  `date`         date,
  `time`         time,
  `time(4)`      time(4),
  `datetime`     datetime,
  `datetime(4)`  datetime(4) default '2022-11-11 11:11:11',
  `timestamp`    timestamp,
  `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
  `year`         year,

  `char`         char(100),
  `varchar`      varchar(100),
  `text`         text
);
insert into test_type_table
values (111, 111, 11111, 11111, 1111111111, 1111111111, 11111111111111, 11111111111111, 111, 1111111111111.1, 111,
        '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
        '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023',
        '2023-02-05', '2023-02-05', '2023-02-05');

-- test (subquery)
drop table if exists t1;
drop table if exists t2;
create table t1(s1 int);
insert into t1 values(1);
create table t2(s1 int);
insert into t2 values(2);

select (select s1 from t2) from t1;
select (table t2) from t1;

drop view if exists res_view;
create view res_view as select 
(select `int1` from test_type_table) + (select `numeric` from test_type_table) as 'int1+numeric',
(select `uint1` from test_type_table) + (select `float8` from test_type_table) as 'uint1+float8',
(select `int2` from test_type_table) + (select `float4` from test_type_table) as 'int2+float4',
(select `uint2` from test_type_table) + (select `uint8` from test_type_table) as 'uint2+uint8',
(select `int4` from test_type_table) + (select `int8` from test_type_table) as 'int4+int8',
(select `uint4` from test_type_table) + (select `uint4` from test_type_table) as 'uint4+uint4',
concat((select `char` from test_type_table),(select `text` from test_type_table)) as 'concat(char,text)',
concat((select `varchar` from test_type_table),(select `text` from test_type_table)) as 'concat(varchar,text)',
extract(day from (select `date` from test_type_table)) as 'extract(date)',
extract(hour from (select `time` from test_type_table)) as 'extract(time)',
extract(hour from (select `time(4)` from test_type_table)) as 'extract(time(4))',
extract(day from (select `datetime` from test_type_table)) as 'extract(datetime)',
extract(day from (select `datetime(4)` from test_type_table)) as 'extract(datetime(4))',
(select `year` from test_type_table) + (select `year` from test_type_table) as 'year+year';
\x
select * from res_view;
\x
desc res_view;

-- test abnormal case
select (select * from test_type_table) from t1;

-- no strict mode
set dolphin.sql_mode = '';
drop table if exists t3;
drop table if exists t4;
create table t3 as select (select * from t1) from t1; 
create table t4 as select (select * from test_type_table) from t1; 
reset dolphin.sql_mode;

-- strict mode
set dolphin.sql_mode = 'sql_mode_strict';
drop table if exists t3;
drop table if exists t4;
create table t3 as select (select * from t1) from t1; 
create table t4 as select (select * from test_type_table) from t1; 
reset dolphin.sql_mode;

-- test EXISTS (subquery)
drop view if exists res_view;
create view res_view as select 
exists (select `int1` from test_type_table) as 'int1',
exists (select `uint1` from test_type_table) as 'uint1',
exists (select `int2` from test_type_table) as 'int2',
exists (select `uint2` from test_type_table) as 'uint2',
exists (select `int4` from test_type_table) as 'int4',
exists (select `uint4` from test_type_table) as 'uint4',
exists (select `int8` from test_type_table) as 'int8',
exists (select `uint8` from test_type_table) as 'uint8',
exists (select `float4` from test_type_table) as 'float4',
exists (select `float8` from test_type_table) as 'float8',
exists (select `numeric` from test_type_table) as 'numeric',
exists (select `date` from test_type_table) as 'date',
exists (select `time` from test_type_table) as 'time',
exists (select `time(4)` from test_type_table) as 'time(4)',
exists (select `datetime` from test_type_table) as 'datetime',
exists (select `datetime(4)` from test_type_table) as 'datetime(4)',
exists (select `timestamp` from test_type_table) as 'timestamp',
exists (select `timestamp(4)` from test_type_table) as 'timestamp(4)',
exists (select `year` from test_type_table) as 'year',
exists (select `char` from test_type_table) as 'char',
exists (select `varchar` from test_type_table) as 'varchar',
exists (select `text` from test_type_table) as 'text';
\x
select * from res_view;
\x
desc res_view;

-- no strict mode
set dolphin.sql_mode = '';
drop table if exists emp_tbl;
drop table if exists nul_tbl;
create table emp_tbl(s1 int);
create table nul_tbl(s1 int);
insert into nul_tbl values(null);

drop table if exists t3;
drop table if exists t4;
create table t3 as select exists (select * from emp_tbl); 
create table t4 as select exists (select * from nul_tbl); 
select * from t3;
select * from t4;
reset dolphin.sql_mode;

-- strict mode
set dolphin.sql_mode = 'sql_mode_strict';
drop table if exists t3;
drop table if exists t4;
create table t3 as select exists (select * from emp_tbl); 
create table t4 as select exists (select * from nul_tbl); 
select * from t3;
select * from t4;
reset dolphin.sql_mode;


drop view if exists res_view;
drop table if exists t1;
drop table if exists t2;
drop table if exists t3;
drop table if exists t4;
drop table if exists emp_tbl;
drop table if exists nul_tbl;
drop table if exists test_type_table;
reset dolphin.b_compatibility_mode;
drop schema test_subquery cascade;
reset current_schema;
