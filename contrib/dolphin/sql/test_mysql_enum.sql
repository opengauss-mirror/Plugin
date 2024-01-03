create schema test_enum;
set current_schema to 'test_enum';
-- create extension dolphin;
show sql_compatibility;
CREATE TABLE test (
    age INT,
    myname anonymous_enum('a','b')
);
CREATE TABLE test (
    age INT,
    myname ttanonymous_enum('a','b')
);
CREATE TABLE test (
    age INT,
    myname ttanonymous_enumtt('a','b')
);

-- when drop a column using an auto created enum type, the created enum type will also be dropped.
CREATE TABLE test (
    age INT,
    myname enum('a','b')
);

SELECT COUNT(*) FROM pg_type WHERE typname like '%anonymous_enum%';
ALTER TABLE test DROP myname;
SELECT COUNT(*) FROM pg_type WHERE typname like '%anonymous_enum%';
DROP TABLE test;

-- when drop a table using an auto created enum type, the created enum type will also be dropped.
CREATE TABLE test (
    age INT,
    myname enum('a','b')
);
SELECT COUNT(*) FROM pg_type WHERE typname like '%anonymous_enum%';
DROP TABLE test;
SELECT COUNT(*) FROM pg_type WHERE typname like '%anonymous_enum%';

-- when drop a column using an hand created enum type, the created enum type will not be dropped.
CREATE TYPE job AS enum('x','y');
CREATE TABLE test (
    age INT,
    myjob job
);
SELECT typname FROM pg_type WHERE typname = 'job';
ALTER TABLE test DROP myjob;
SELECT typname FROM pg_type WHERE typname = 'job';
DROP TABLE test;

-- when alter column type to others, the created enum type will not be dropped.
CREATE TABLE test (
    age INT,
    myjob enum('x','y')
);
SELECT COUNT(*) FROM pg_type WHERE typname like '%anonymous_enum%';

insert into test values(1, 'x');
insert into test values(2, 'y');
insert into test values(4, 'y');

alter table test alter myjob type text;
SELECT COUNT(*) FROM pg_type WHERE typname like '%anonymous_enum%';

DROP TABLE test;


-- rename a type to a type contain 'anonymous_enum' is not allowed

create type test as enum('a','b');
ALTER type test RENAME TO anonymous_enum;
drop type test;

-- make name more the 64

CREATE TABLE testtttttttttttttttttttttttttttttttttt (
    age INT,
    myjobbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb enum('x','y')
);

SELECT count(*) FROM pg_type WHERE typname like 'testtttttttttttttttt_myjobbbbbbbbbbbbbbb%_anonymous_enum_1';
drop table testtttttttttttttttttttttttttttttttttt;


-- when drop a table using an hand created enum type, the created enum type will not be dropped.
CREATE TABLE test (
    age INT,
    myjob job
);
SELECT typname FROM pg_type WHERE typname = 'job';
DROP TABLE test;
SELECT typname FROM pg_type WHERE typname = 'job';
DROP TYPE job;

-- test function
drop table if exists t1;
create table t1(c_id int, c_name varchar(20), c_age enum('20', '30'));
--插入1条数据
insert into t1 values(1, 'name+' || 1, 2);
--创建存储过程，插入10条数据
create or replace procedure p_enum1 
as 
begin 
    for i in 1..3 loop
        insert into t1 values(i, 'name+' || i, 1);
    end loop;
end;
/

--调用
call p_enum1();
select * from t1;

-- not allowed to create a type with type name contained 'anonymous_enum'
CREATE TYPE country_anonymous_enum_1 AS enum('CHINA','USA');

-- insert with existing index
CREATE TABLE test (
    age INT,
    myname enum('a','b','c')
);
INSERT INTO test (myname) VALUES (1);

-- insert with index 0 resulting in a null value
INSERT INTO test (myname) VALUES (0);

-- insert with out-of-range index
INSERT INTO test (myname) VALUES (4);

-- query with existing index
SELECT * FROM test WHERE myname=1;

-- query with index 0
SELECT * FROM test WHERE myname=0;

-- query with out-of-range index
SELECT * FROM test WHERE myname=4;

-- delete with existing index
DELETE FROM test WHERE myname=1;

-- delete with index 0
DELETE FROM test WHERE myname=0;

-- delete with out-of-range index
DELETE FROM test WHERE myname=4;
DROP TABLE test;

-- foreigh key is not allowed
drop table if exists t_enum_0011_01;
CREATE TABLE t_enum_0011_01
(
W_CITY varchar(60) PRIMARY KEY,
W_ADDRESS TEXT
);

 CREATE TABLE t_enum_0011_02
(
W_WAREHOUSE_SK INTEGER NOT NULL,
W_WAREHOUSE_ID CHAR(16) NOT NULL,
W_WAREHOUSE_NAME VARCHAR(20) ,
W_WAREHOUSE_SQ_FT INTEGER ,
W_STREET_NUMBER CHAR(10) ,
W_STREET_NAME VARCHAR(60) ,
W_STREET_TYPE CHAR(15) ,
W_SUITE_NUMBER CHAR(10) ,
W_CITY ENUM('xian','gansu','yanan') REFERENCES t_enum_0011_01(W_CITY),
W_COUNTY VARCHAR(30) ,
W_STATE CHAR(2) ,
W_ZIP CHAR(10) ,
W_COUNTRY VARCHAR(20) ,
W_GMT_OFFSET DECIMAL(5,2)
);

drop table if exists chameleon_case6;
create table chameleon_case6(c1 enum('男','女','无'));
insert into chameleon_case6 values('女'), ('男');
select * from chameleon_case6 order by 1;
update chameleon_case6 set c1 ='无' where c1 = '女';
select * from chameleon_case6 order by 1;

drop table if exists test_abc;
create table test_abc (c1 enum('c', 'b', 'a'));
insert into test_abc values('a'), ('b'), ('c');
select c1,
c1 = 'b' AS "c1 = b", c1 <> 'b' AS "c1 <> b", c1 > 'b' AS "c1 > b", c1 >= 'b' AS "c1 >= b", c1 < 'b' AS "c1 < b", c1 <= 'b' AS "c1 <= b",
'b' = c1 AS "b = c1", 'b' <> c1 AS "b <> c1", 'b' > c1 AS "b > c1", 'b' >= c1 AS "b >= c1", 'b' < c1 AS "b < c1", 'b' <= c1 AS "b <= c1"
from test_abc order by 1;

create table enum_test_table(a enum('a', 'b', 'c'));
insert into enum_test_table values('a'), ('b'), ('c');
select * from enum_test_table where a = 1;
select * from enum_test_table where a = 2;
select * from enum_test_table where a = 3;
select * from enum_test_table where 1 = a;
select * from enum_test_table where 2 = a;
select * from enum_test_table where 3 = a;
select * from enum_test_table where 1 > a;
select * from enum_test_table where 2 > a;
select * from enum_test_table where 3 > a;
select * from enum_test_table where 1 >= a;
select * from enum_test_table where 2 >= a;
select * from enum_test_table where 3 >= a;
select * from enum_test_table where 1 < a;
select * from enum_test_table where 2 < a;
select * from enum_test_table where 3 < a;
select * from enum_test_table where 1 <= a;
select * from enum_test_table where 2 <= a;
select * from enum_test_table where 3 <= a;
select * from enum_test_table where 1 != a;
select * from enum_test_table where 2 != a;
select * from enum_test_table where 3 != a;

select a + 1 from enum_test_table;
select a - 1 from enum_test_table;
select a * 1 from enum_test_table;
select a / 1 from enum_test_table;

create table test_enum_d(ssl_type enum('','any','X509','SPECIFIED') not null default '');

\d test_enum_d
 
\d+ test_enum_d

-- test anonymous enum backup
create table student (
     id int not null auto_increment,
     name varchar(100),
     gender enum('male', 'female'),
     non_enum enum(),
     primary key(id)
);
insert into student(name,gender) values('bill','male');
select * from student;

create table student_bak as select * from student;
select * from student_bak;

alter table student drop column gender;
select * from student_bak;
alter table student drop column non_enum;
select * from student_bak;

select * from pg_get_tabledef('student_bak');

drop table student, student_bak cascade;

drop schema test_enum cascade;
reset current_schema;

create schema db_b_new_gram_test3;
set current_schema to 'db_b_new_gram_test3';
create table t_charset_enum_column(c enum('a', 'b', 'c') charset test_character_set);
drop table t_charset_enum_column;
create table t_enum_column_drop(c1 int, c2 enum('a', 'b'));
alter table t_enum_column_drop drop c2;
drop table t_enum_column_drop;
create table t_enum_column_alter_type(c1 int, c2 enum('a', 'b'));
alter table t_enum_column_alter_type drop c2;
drop table t_enum_column_alter_type;
create table t_drop_view(c1 int, c2 enum('a'));
create view my_view as select * from t_drop_view;
drop view my_view;
drop table t_drop_view;
set dolphin.sql_mode to 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero';
drop table if exists test;
create table test(color enum('red', 'green', 'blue', 'purple', 'yellow'));
insert into test values('red');
insert into test values('green');
insert into test values('orange');
insert into test values(0);
insert into test values('0');
insert into test values(1);
insert into test values('1');
insert into test values(1.2);
insert into test values('1.2');
insert into test values(1.4);
insert into test values('1.4');
insert into test values(1.5);
insert into test values('1.5');
insert into test values(1.6);
insert into test values('1.6');
insert into test values('a1');
insert into test values('1a');
insert into test values('a123');
insert into test values('123a');
select * from test;
delete from test;
insert ignore into test values('red');
insert ignore into test values('green');
insert ignore into test values('orange');
insert ignore into test values(0);
insert ignore into test values('0');
insert ignore into test values(1);
insert ignore into test values('1');
insert ignore into test values(1.2);
insert ignore into test values('1.2');
insert ignore into test values(1.4);
insert ignore into test values('1.4');
insert ignore into test values(1.5);
insert ignore into test values('1.5');
insert ignore into test values(1.6);
insert ignore into test values('1.6');
insert ignore into test values('a1');
insert ignore into test values('1a');
insert ignore into test values('a123');
insert ignore into test values('123a');
select * from test;
delete from test;
set dolphin.sql_mode to 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero';
insert into test values('red');
insert into test values('green');
insert into test values('orange');
insert into test values(0);
insert into test values('0');
insert into test values(1);
insert into test values('1');
insert into test values(1.2);
insert into test values('1.2');
insert into test values(1.4);
insert into test values('1.4');
insert into test values(1.5);
insert into test values('1.5');
insert into test values(1.6);
insert into test values('1.6');
insert into test values('a1');
insert into test values('1a');
insert into test values('a123');
insert into test values('123a');
select * from test;
drop table test cascade;
drop schema db_b_new_gram_test3 cascade;
reset current_schema;

--
--test tailing blank for enum value
--
--expect failure with with duplicate values
create table tabenum(a enum ('a','a ') );
--expect success
create table tabenum(a enum ('a ',' a'));
drop table tabenum;
--expect failure
CREATE TYPE typenum AS ENUM ( 'a', 'a ');
--expect success
CREATE TYPE typenum AS ENUM ( 'a', ' a');
DROP TYPE typenum;
