drop database if exists b;
create database b dbcompatibility 'b';
\c b
drop user if exists tester CASCADE;
create user tester password 'Password123';

-- create temporary tables
revoke create temporary tables on database b from tester;
revoke create temporary tables on database b from public;
SET SESSION AUTHORIZATION tester PASSWORD 'Password123';
create temporary table test(a int);
\c b
grant create temporary tables on database b to tester;
SET SESSION AUTHORIZATION tester PASSWORD 'Password123';
create temporary table test(a int);
select * from test;
\c b

-- create routine
revoke create routine from tester;
revoke create on schema tester from tester;
SET SESSION AUTHORIZATION tester PASSWORD 'Password123';
CREATE OR REPLACE FUNCTION tester.test()
returns text
as
$$
begin
    return '1';
end;
$$
language plpgsql;
\c b
grant create routine to tester;
SET SESSION AUTHORIZATION tester PASSWORD 'Password123';
CREATE OR REPLACE FUNCTION tester.test()
returns text
as
$$
begin
    return '1';
end;
$$
language plpgsql;

-- alter routine
\c b
CREATE OR REPLACE FUNCTION test2()
returns text
as
$$
begin
    return '1';
end;
$$
language plpgsql;
grant all privileges on schema public to tester;
revoke alter routine on function test2() from tester;
revoke alter routine on function test2() from public;
SET SESSION AUTHORIZATION tester PASSWORD 'Password123';
alter function test2() rename to test1;
\c b
grant alter routine on function test2() to tester;
SET SESSION AUTHORIZATION tester PASSWORD 'Password123';
alter function test2() rename to test1;
select test1();
alter function test1() rename to test2;
select test2();

-- CREATE USER
create user tester_2 PASSWORD 'Password123';
\c b
grant create user on *.* to tester;
SET SESSION AUTHORIZATION tester PASSWORD 'Password123';
create user tester_2 PASSWORD 'Password123';

-- INDEX
\c b
drop user tester_2;
CREATE SCHEMA pri_index_schema;
set search_path=pri_index_schema;
create table pri_index_schema.t1(i int);
revoke index from tester;
SET SESSION AUTHORIZATION tester PASSWORD 'Password123';
create index idx1 on t1(i);
\c b
grant index to tester;
SET SESSION AUTHORIZATION tester PASSWORD 'Password123';
set search_path=pri_index_schema;
create index idx1 on t1(i);

--grammar test
\c b
grant EXECUTE,ALTER ROUTINE on tester.* to tester;
grant CREATE ROUTINE,INDEX,CREATE ANY TABLE on *.* to tester;
grant CREATE TEMPORARY TABLES,TEMP on b.* to tester;
grant CREATE TABLESPACE ON *.* to tester;
revoke create user on *.* from tester;
revoke create tablespace on *.* from tester;
revoke index on *.* from tester;
grant alter routine,alter routine on all functions in schema tester  to tester;

\c postgres
drop database if exists b;
drop user if exists tester cascade;
