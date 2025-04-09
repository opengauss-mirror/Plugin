create schema test_current_user;
set current_schema to 'test_current_user';
select current_user;
select current_user();
select pg_typeof(current_user);
select pg_typeof(current_user());
create user u1 password 'Gauss123';
rename user u1 to current_user;
rename user u1 to current_user();
create table test (myid int, myname text);
grant select on test to current_user;
grant select on test to current_user();
revoke select on test from current_user;
revoke select on test from current_user();
delimiter //
CREATE DEFINER = current_user FUNCTION test_func()
RETURNS integer
LANGUAGE sql
AS $function$SELECT myid FROM test$function$//
drop function test_func//
CREATE DEFINER = current_user() FUNCTION test_func()
RETURNS integer
LANGUAGE sql
AS $function$SELECT myid FROM test$function$//
drop function test_func//
CREATE DEFINER = current_user PROCEDURE test_proc()
AS  DECLARE 
BEGIN
   raise info 'create definer procedure.';
END//
drop procedure test_proc//
CREATE DEFINER = current_user() PROCEDURE test_proc()
AS  DECLARE 
BEGIN
   raise info 'create definer procedure.';
END//
drop procedure test_proc//
delimiter ;
CREATE DEFINER = current_user VIEW test_view AS SELECT myid FROM test;
drop view test_view;
CREATE DEFINER = current_user() VIEW test_view AS SELECT myid FROM test;
ALTER DEFINER = current_user VIEW test_view AS SELECT myid FROM test;
ALTER DEFINER = current_user() VIEW test_view AS SELECT myid FROM test;
drop view test_view;
ALTER TABLE test OWNER TO current_user;
ALTER TABLE test OWNER TO current_user();
create database tmp_db dbcompatibility 'b';
CLEAN CONNECTION TO ALL FOR DATABASE tmp_db TO USER current_user;
CLEAN CONNECTION TO ALL FOR DATABASE tmp_db TO USER current_user();
set dolphin.b_compatibility_mode to off;
drop database tmp_db;
set dolphin.b_compatibility_mode to on;
drop user current_user;
drop user current_user();
CREATE TYPE mood AS ENUM ('sad', 'ok', 'happy');
ALTER TYPE mood OWNER TO u1;
ALTER TYPE mood OWNER TO CURRENT_USER;
ALTER TYPE mood OWNER TO CURRENT_USER();
ALTER TYPE mood OWNER TO SESSION_USER;
drop type mood;
CREATE ROW LEVEL SECURITY POLICY test_policy ON test TO current_user USING(FALSE);
CREATE ROW LEVEL SECURITY POLICY test_policy_2 ON test TO current_user() USING(FALSE);
CREATE ROW LEVEL SECURITY POLICY test_policy_3 ON test TO u1 USING(FALSE);
drop table test;
CREATE FOREIGN DATA WRAPPER foo;
CREATE SERVER s1 FOREIGN DATA WRAPPER foo;
CREATE USER MAPPING FOR current_user SERVER s1;
DROP USER MAPPING FOR current_user SERVER s1;
CREATE USER MAPPING FOR current_user() SERVER s1;
DROP USER MAPPING FOR current_user() SERVER s1;
CREATE USER MAPPING FOR USER SERVER s1;
DROP USER MAPPING FOR USER SERVER s1;
CREATE USER MAPPING FOR u1 SERVER s1;
DROP USER MAPPING FOR u1 SERVER s1;
drop user u1;
drop schema test_current_user cascade;
reset current_schema;