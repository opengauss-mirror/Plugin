create schema show_create;
set current_schema to 'show_create';
CREATE USER test_showcreate WITH PASSWORD 'openGauss@123';
GRANT ALL PRIVILEGES TO test_showcreate;
SET ROLE test_showcreate PASSWORD 'openGauss@123';
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group';
create schema test_get_def;
set current_schema=test_get_def;
-- tests for table
create table table_function_export_def_base (
    id integer primary key,
    name varchar(100)
);
create table table_function_export_def (
    id integer primary key,
    fid integer,
    constraint table_export_base_fkey foreign key (fid) references table_function_export_def_base(id)
);
show create table table_function_export_def_base;
show create table test_get_def.table_function_export_def_base;
show create table table_function_export_def;
drop table table_function_export_def;
drop table table_function_export_def_base;
--range table
create table table_range1 (id int, a date, b varchar)
partition by range (id)
(
    partition table_range1_p1 values less than(10),
    partition table_range1_p2 values less than(50),
    partition table_range1_p3 values less than(100),
    partition table_range1_p4 values less than(maxvalue)
);
show create table table_range1;
drop table table_range1;
--list table
create table table_list1 (id int, a date, b varchar)
partition by list (id)
(
    partition table_list1_p1 values (1, 2, 3, 4),
    partition table_list1_p2 values (5, 6, 7, 8),
    partition table_list1_p3 values (9, 10, 11, 12)
);
show create table table_list1;
drop table table_list1;
--hash table
create table table_hash1 (id int, a date, b varchar)
partition by hash (id)
(
    partition table_hash1_p1,
    partition table_hash1_p2,
    partition table_hash1_p3
);
show create table table_hash1;
drop table table_hash1;
--subpartition table
CREATE TABLE list_range_1 (
    col_1 integer primary key,
    col_2 integer,
    col_3 character varying(30) unique,
    col_4 integer
)
WITH (orientation=row, compression=no)
PARTITION BY LIST (col_1) SUBPARTITION BY RANGE (col_2)
(
    PARTITION p_list_1 VALUES (-1,-2,-3,-4,-5,-6,-7,-8,-9,-10)
    (
        SUBPARTITION p_range_1_1 VALUES LESS THAN (-10),
        SUBPARTITION p_range_1_2 VALUES LESS THAN (0),
        SUBPARTITION p_range_1_3 VALUES LESS THAN (10),
        SUBPARTITION p_range_1_4 VALUES LESS THAN (20),
        SUBPARTITION p_range_1_5 VALUES LESS THAN (50)
    ),
    PARTITION p_list_2 VALUES (1,2,3,4,5,6,7,8,9,10),
    PARTITION p_list_3 VALUES (11,12,13,14,15,16,17,18,19,20)
    (
        SUBPARTITION p_range_3_1 VALUES LESS THAN (15),
        SUBPARTITION p_range_3_2 VALUES LESS THAN (MAXVALUE)
    ),
    PARTITION p_list_4 VALUES (21,22,23,24,25,26,27,28,29,30)
    (
        SUBPARTITION p_range_4_1 VALUES LESS THAN (-10),
        SUBPARTITION p_range_4_2 VALUES LESS THAN (0),
        SUBPARTITION p_range_4_3 VALUES LESS THAN (10),
        SUBPARTITION p_range_4_4 VALUES LESS THAN (20),
        SUBPARTITION p_range_4_5 VALUES LESS THAN (50)
    ),
    PARTITION p_list_5 VALUES (31,32,33,34,35,36,37,38,39,40),
    PARTITION p_list_6 VALUES (41,42,43,44,45,46,47,48,49,50)
    (
        SUBPARTITION p_range_6_1 VALUES LESS THAN (-10),
        SUBPARTITION p_range_6_2 VALUES LESS THAN (0),
        SUBPARTITION p_range_6_3 VALUES LESS THAN (10),
        SUBPARTITION p_range_6_4 VALUES LESS THAN (20),
        SUBPARTITION p_range_6_5 VALUES LESS THAN (50)
    ),
    PARTITION p_list_7 VALUES (DEFAULT)
);
show create table list_range_1;
drop table list_range_1;
CREATE TABLE list_hash_2 (
    col_1 integer primary key,
    col_2 integer,
    col_3 character varying(30) unique,
    col_4 integer
)
WITH (orientation=row, compression=no)
PARTITION BY LIST (col_2) SUBPARTITION BY HASH (col_3)
(
    PARTITION p_list_1 VALUES (-1,-2,-3,-4,-5,-6,-7,-8,-9,-10)
    (
        SUBPARTITION p_hash_1_1,
        SUBPARTITION p_hash_1_2,
        SUBPARTITION p_hash_1_3
    ),
    PARTITION p_list_2 VALUES (1,2,3,4,5,6,7,8,9,10),
    PARTITION p_list_3 VALUES (11,12,13,14,15,16,17,18,19,20)
    (
        SUBPARTITION p_hash_3_1,
        SUBPARTITION p_hash_3_2
    ),
    PARTITION p_list_4 VALUES (21,22,23,24,25,26,27,28,29,30)
    (
        SUBPARTITION p_hash_4_1,
        SUBPARTITION p_hash_4_2,
        SUBPARTITION p_hash_4_3,
        SUBPARTITION p_hash_4_4,
        SUBPARTITION p_hash_4_5
    ),
    PARTITION p_list_5 VALUES (31,32,33,34,35,36,37,38,39,40),
    PARTITION p_list_6 VALUES (41,42,43,44,45,46,47,48,49,50)
    (
        SUBPARTITION p_hash_6_1,
        SUBPARTITION p_hash_6_2,
        SUBPARTITION p_hash_6_3,
        SUBPARTITION p_hash_6_4,
        SUBPARTITION p_hash_6_5
    ),
    PARTITION p_list_7 VALUES (DEFAULT)
);
create unique index list_hash_2_idx1 on list_hash_2(col_2, col_3, col_4) local;
create index list_hash_2_idx2 on list_hash_2(col_3, col_1) local;
create index list_hash_2_idx3 on list_hash_2(col_4) global;
show create table list_hash_2;
drop table list_hash_2;
--interval table
create table table_interval1 (id int, a date, b varchar)
partition by range (a)
interval ('1 day')
(
    partition table_interval1_p1 values less than('2020-03-01'),
    partition table_interval1_p2 values less than('2020-05-01'),
    partition table_interval1_p3 values less than('2020-07-01'),
    partition table_interval1_p4 values less than(maxvalue)
);
show create table table_interval1;
drop table table_interval1;
--generate table
create table generated_test(a int, b int generated always as (length((a)::text)) stored);
show create table generated_test;
-- tests for view
create view tt19v as
select 'foo'::text = any(array['abc','def','foo']::text[]) c1,
       'foo'::text = any((select array['abc','def','foo']::text[])::text[]) c2;
show create view tt19v;
show create view test_get_def.tt19v;
-- table view exchange show create test
show create view generated_test;
show create table tt19v;
drop view tt19v;
drop table generated_test;
-- This test checks that proper typmods are assigned in a multi-row VALUES
CREATE VIEW tt1 AS
  SELECT * FROM (
    VALUES
       ('abc'::varchar(3), '0123456789', 42, 'abcd'::varchar(4)),
       ('0123456789', 'abc'::varchar(3), 42.12, 'abc'::varchar(4))
  ) vv(a,b,c,d);
show create view tt1;
show create table tt1;
drop view tt1;
-- check display of assorted RTE_FUNCTION expressions
create view tt20v as
select * from
  coalesce(1,2) as c,
  pg_collation_for ('x'::text) col,
  current_date as d,
  cast(1+2 as int4) as i4,
  cast(1+2 as int8) as i8;
show create view tt20v;
show create table tt20v;
drop view tt20v;
-- Check reverse listing
CREATE TABLE department (
	id INTEGER PRIMARY KEY,
	parent_department INTEGER ,
	name TEXT
);
WITH RECURSIVE subdepartment AS
(
	SELECT name as root_name, * FROM department WHERE name = 'A'
	UNION ALL
	SELECT sd.root_name, d.* FROM department AS d, subdepartment AS sd
		WHERE d.parent_department = sd.id
)
SELECT * FROM subdepartment ORDER BY name;
CREATE VIEW vsubdepartment AS
	WITH RECURSIVE subdepartment AS
	(
		SELECT * FROM department WHERE name = 'A'
		UNION ALL
		SELECT d.* FROM department AS d, subdepartment AS sd
			WHERE d.parent_department = sd.id
	)
	SELECT * FROM subdepartment;
show create view vsubdepartment;
show create table vsubdepartment;
drop view vsubdepartment;
drop table department;
CREATE TABLE customer (
       cid      int primary key,
       name     text not null,
       tel      text,
       passwd	text
);
CREATE VIEW my_property_normal AS
       SELECT * FROM customer WHERE name = current_user;
CREATE VIEW my_property_secure WITH (security_barrier) AS
       SELECT * FROM customer WHERE name = current_user;
show create view my_property_normal;
show create table my_property_normal;
show create view my_property_secure;
show create table my_property_secure;
drop view my_property_normal;
drop view my_property_secure;
drop table customer;
-- tests for function
-- ARGUMENT and RETURN TYPES
CREATE FUNCTION functest_A_1(text, date) RETURNS bool LANGUAGE 'sql'
       AS 'SELECT $1 = ''abcd'' AND $2 > ''2001-01-01''';
CREATE FUNCTION functest_A_2(text[]) RETURNS int LANGUAGE 'sql'
       AS 'SELECT $1[0]::int';
CREATE FUNCTION functest_A_3() RETURNS bool LANGUAGE 'sql'
       AS 'SELECT false';
show create function functest_A_1;
show create function test_get_def.functest_A_1;
show create function test_get_def.test_get_def.functest_A_1;
show create function functest_A_2;
show create function functest_A_3;
drop function functest_A_1;
drop function functest_A_2;
drop function functest_A_3;
-- IMMUTABLE | STABLE | VOLATILE
CREATE FUNCTION functest_B_1(int) RETURNS bool LANGUAGE 'sql'
       AS 'SELECT $1 > 0';
CREATE FUNCTION functest_B_2(int) RETURNS bool LANGUAGE 'sql'
       IMMUTABLE AS 'SELECT $1 > 0';
CREATE FUNCTION functest_B_3(int) RETURNS bool LANGUAGE 'sql'
       STABLE AS 'SELECT $1 = 0';
CREATE FUNCTION functest_B_4(int) RETURNS bool LANGUAGE 'sql'
       VOLATILE AS 'SELECT $1 < 0';
show create function functest_B_1;
show create function functest_B_2;
show create function functest_B_3;
show create function functest_B_4;
drop function functest_B_1;
drop function functest_B_2;
drop function functest_B_3;
drop function functest_B_4;
-- SECURITY DEFINER | INVOKER
CREATE FUNCTION functext_C_1(int) RETURNS bool LANGUAGE 'sql'
       AS 'SELECT $1 > 0';
CREATE FUNCTION functext_C_2(int) RETURNS bool LANGUAGE 'sql'
       SECURITY DEFINER AS 'SELECT $1 = 0';
CREATE FUNCTION functext_C_3(int) RETURNS bool LANGUAGE 'sql'
       SECURITY INVOKER AS 'SELECT $1 < 0';
show create function functext_C_1;
show create function functext_C_2;
show create function functext_C_3;
drop function functext_C_1;
drop function functext_C_2;
drop function functext_C_3;
-- LEAKPROOF
CREATE FUNCTION functext_E_1(int) RETURNS bool LANGUAGE 'sql'
       AS 'SELECT $1 > 100';
CREATE FUNCTION functext_E_2(int) RETURNS bool LANGUAGE 'sql'
       LEAKPROOF AS 'SELECT $1 > 100';
show create function functext_E_1;
show create function functext_E_2;
drop function functext_E_1;
drop function functext_E_2;
-- CALLED ON NULL INPUT | RETURNS NULL ON NULL INPUT | STRICT
CREATE FUNCTION functext_F_1(int) RETURNS bool LANGUAGE 'sql'
       AS 'SELECT $1 > 50';
CREATE FUNCTION functext_F_2(int) RETURNS bool LANGUAGE 'sql'
       CALLED ON NULL INPUT AS 'SELECT $1 = 50';
CREATE FUNCTION functext_F_3(int) RETURNS bool LANGUAGE 'sql'
       RETURNS NULL ON NULL INPUT AS 'SELECT $1 < 50';
CREATE FUNCTION functext_F_4(int) RETURNS bool LANGUAGE 'sql'
       STRICT AS 'SELECT $1 = 50';
show create function functext_F_1;
show create function functext_F_2;
show create function functext_F_3;
show create function functext_F_4;
drop function functext_F_1;
drop function functext_F_2;
drop function functext_F_3;
drop function functext_F_4;
-- tests for procedure
CREATE PROCEDURE test_proc_define
(
	in_1  	IN VARCHAR2,
	in_2    VARCHAR2,
	out_1  	OUT VARCHAR2,
	inout_1  IN OUT VARCHAR
)
IS
BEGIN  
	out_1 	:= in_1;
	inout_1 := inout_1 || in_2;
END;
/
show create procedure test_proc_define;
CREATE PROCEDURE   proc_no_parameter
AS
	ret1 int;
BEGIN  
	ret1 := 0;
END;
/
show create procedure proc_no_parameter;
show create procedure test_get_def.proc_no_parameter;
show create procedure test_get_def.test_get_def.proc_no_parameter;
drop procedure proc_no_parameter;
-- Test Replace procedure
--Replace parameter number
CREATE OR REPLACE PROCEDURE   test_proc_define
(
   in_1  IN VARCHAR2,
   in_2    VARCHAR2,
   out_1  OUT VARCHAR2
   -- inout_1  IN OUT VARCHAR
)
IS
BEGIN  
	out_1 := in_1;
	
END;
/
show create procedure test_proc_define;
--Replace parameter type
CREATE OR REPLACE PROCEDURE   test_proc_define
(
   in_1  IN INTEGER,
   in_2    INTEGER,
   out_1  OUT INTEGER
)
IS
BEGIN  
	out_1 := in_1;
END;
/
show create procedure test_proc_define;
--Replace parameter NAME
CREATE OR REPLACE PROCEDURE   test_proc_define
(
   in1  IN INTEGER,
   in2    INTEGER,
   out1  OUT INTEGER
)
IS
BEGIN  
	out1 := in1;
END;
/
show create procedure test_proc_define;
drop procedure test_proc_define;
-- Test Nested BEGIN/END
CREATE OR REPLACE PROCEDURE test_multi_level
AS
	 ret INT;
begin 
	  ret := 0;
	  BEGIN
		  ret := ret + 1 ;
		  BEGIN
			 ret := ret + 1;
		  END;
	  END;
END;
/
show create procedure test_multi_level;
drop procedure test_multi_level;
-- tests fot trigger
CREATE TABLE main_table (a int, b int);
ALTER TABLE MAIN_TABLE ADD PRIMARY KEY(A, B);
CREATE FUNCTION trigger_func() RETURNS trigger LANGUAGE plpgsql AS '
BEGIN
	RAISE NOTICE ''trigger_func(%) called: action = %, when = %, level = %'', TG_ARGV[0], TG_OP, TG_WHEN, TG_LEVEL;
	RETURN NULL;
END;';
-- tests for per-statement triggers
CREATE TRIGGER before_ins_stmt_trig BEFORE INSERT ON main_table
FOR EACH STATEMENT EXECUTE PROCEDURE trigger_func('before_ins_stmt');
CREATE TRIGGER after_ins_stmt_trig AFTER INSERT ON main_table
FOR EACH STATEMENT EXECUTE PROCEDURE trigger_func('after_ins_stmt');
show create trigger before_ins_stmt_trig;
show create trigger test_get_def.before_ins_stmt_trig;
show create trigger after_ins_stmt_trig;
-- if neither 'FOR EACH ROW' nor 'FOR EACH STATEMENT' was specified,
-- CREATE TRIGGER should default to 'FOR EACH STATEMENT'
CREATE TRIGGER after_upd_stmt_trig AFTER UPDATE ON main_table
EXECUTE PROCEDURE trigger_func('after_upd_stmt');
CREATE TRIGGER after_upd_row_trig AFTER UPDATE ON main_table
FOR EACH ROW EXECUTE PROCEDURE trigger_func('after_upd_row');
show create trigger after_upd_stmt_trig;
show create trigger after_upd_row_trig;
-- test triggers with WHEN clause
CREATE TRIGGER modified_a BEFORE UPDATE OF a ON main_table
FOR EACH ROW WHEN (OLD.a <> NEW.a) EXECUTE PROCEDURE trigger_func('modified_a');
CREATE TRIGGER modified_any BEFORE UPDATE OF a ON main_table
FOR EACH ROW WHEN (OLD.* IS DISTINCT FROM NEW.*) EXECUTE PROCEDURE trigger_func('modified_any');
CREATE TRIGGER insert_a AFTER INSERT ON main_table
FOR EACH ROW WHEN (NEW.a = 123) EXECUTE PROCEDURE trigger_func('insert_a');
CREATE TRIGGER delete_a AFTER DELETE ON main_table
FOR EACH ROW WHEN (OLD.a = 123) EXECUTE PROCEDURE trigger_func('delete_a');
CREATE TRIGGER insert_when BEFORE INSERT ON main_table
FOR EACH STATEMENT WHEN (true) EXECUTE PROCEDURE trigger_func('insert_when');
CREATE TRIGGER delete_when AFTER DELETE ON main_table
FOR EACH STATEMENT WHEN (true) EXECUTE PROCEDURE trigger_func('delete_when');
show create trigger modified_a;
show create trigger modified_any;
show create trigger insert_a;
show create trigger delete_a;
show create trigger insert_when;
show create trigger delete_when;
-- Test column-level triggers
CREATE TRIGGER before_upd_a_row_trig BEFORE UPDATE OF a ON main_table
FOR EACH ROW EXECUTE PROCEDURE trigger_func('before_upd_a_row');
CREATE TRIGGER after_upd_b_row_trig AFTER UPDATE OF b ON main_table
FOR EACH ROW EXECUTE PROCEDURE trigger_func('after_upd_b_row');
CREATE TRIGGER after_upd_a_b_row_trig AFTER UPDATE OF a, b ON main_table
FOR EACH ROW EXECUTE PROCEDURE trigger_func('after_upd_a_b_row');
CREATE TRIGGER before_upd_a_stmt_trig BEFORE UPDATE OF a ON main_table
FOR EACH STATEMENT EXECUTE PROCEDURE trigger_func('before_upd_a_stmt');
CREATE TRIGGER after_upd_b_stmt_trig AFTER UPDATE OF b ON main_table
FOR EACH STATEMENT EXECUTE PROCEDURE trigger_func('after_upd_b_stmt');
show create trigger before_upd_a_row_trig;
show create trigger after_upd_b_row_trig;
show create trigger after_upd_a_b_row_trig;
show create trigger before_upd_a_stmt_trig;
show create trigger after_upd_b_stmt_trig;
drop table main_table;
drop function trigger_func;
-- Check no existence
show create view aa;
show create table aa;
show create function aa;
show create procedure aa;
show create trigger aa;
reset current_schema;
drop schema test_get_def cascade;
RESET ROLE;
DROP USER test_showcreate;
drop schema show_create cascade;
reset current_schema;