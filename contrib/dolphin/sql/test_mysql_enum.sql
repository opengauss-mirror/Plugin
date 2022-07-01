drop database if exists test_enum;
CREATE DATABASE test_enum with dbcompatibility='B';
\c test_enum
-- create extension dolphin;
show sql_compatibility;
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

SELECT * FROM pg_type WHERE typname like '%anonymous_enum%';
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

\c postgres
DROP DATABASE test_enum;
