\c table_name_test_db;
set dolphin.lower_case_table_names TO 0;

--
-- CREATE_TABLE
--
-- CREATE TABLE SYNTAX
--
-- CLASS DEFINITIONS
--
CREATE TABLE Src(a int) with(autovacuum_enabled = off);
insert into Src values(1);

CREATE TABLE Hobbies_r (
	name		text,
	person 		text
) with(autovacuum_enabled = off);

CREATE TABLE Equipment_r (
	name 		text,
	hobby		text
) with(autovacuum_enabled = off);

CREATE TABLE Onek (
	unique1		int4,
	unique2		int4,
	two			int4,
	four		int4,
	ten			int4,
	twenty		int4,
	hundred		int4,
	thousand	int4,
	twothousand	int4,
	fivethous	int4,
	tenthous	int4,
	odd			int4,
	even		int4,
	stringu1	name,
	stringu2	name,
	string4		name
) with(autovacuum_enabled = off);

CREATE TABLE Onek2 AS SELECT * FROM Onek;

CREATE TABLE Tenk1 (
	unique1		int4,
	unique2		int4,
	two			int4,
	four		int4,
	ten			int4,
	twenty		int4,
	hundred		int4,
	thousand	int4,
	twothousand	int4,
	fivethous	int4,
	tenthous	int4,
	odd			int4,
	even		int4,
	stringu1	name,
	stringu2	name,
	string4		name
) with(autovacuum_enabled = off);

CREATE TABLE Tenk2 (
	unique1 	int4,
	unique2 	int4,
	two 	 	int4,
	four 		int4,
	ten			int4,
	twenty 		int4,
	hundred 	int4,
	thousand 	int4,
	twothousand int4,
	fivethous 	int4,
	tenthous	int4,
	odd			int4,
	even		int4,
	stringu1	name,
	stringu2	name,
	string4		name
) with(autovacuum_enabled = off);


CREATE TABLE Person (
	name 		text,
	age			int4,
	location 	point
);


CREATE TABLE Emp (
	name            text,
	age		int4,
	location	point,
	salary 		int4,
	manager 	name
) with(autovacuum_enabled = off);


CREATE TABLE Student (
	name 		text,
	age			int4,
	location 	point,
	gpa		float8
);


CREATE TABLE Stud_Emp (
	name 		text,
	age			int4,
	location 	point,
	salary		int4,
	manager		name,
	gpa 		float8,
	percent		int4
) with(autovacuum_enabled = off);

CREATE TABLE City (
	name		name,
	location 	box,
	budget 		city_budget
) with(autovacuum_enabled = off);

CREATE TABLE Dept (
	dname		name,
	mgrname 	text
) with(autovacuum_enabled = off);

CREATE TABLE Slow_emp4000 (
	home_base	 box
) with(autovacuum_enabled = off);

CREATE TABLE Fast_emp4000 (
	home_base	 box
) with(autovacuum_enabled = off);

CREATE TABLE Road (
	name		text,
	thepath 	path
);

CREATE TABLE iHighway(
	name		text,
	thepath 	path
) with(autovacuum_enabled = off);

CREATE TABLE sHighway (
	surface		text,
	name		text,
	thepath 	path
) with(autovacuum_enabled = off);

CREATE TABLE Real_City (
	pop			int4,
	cname		text,
	outline 	path
) with(autovacuum_enabled = off);

--
-- test the "star" operators a bit more thoroughly -- this time,
-- throw in lots of NULL fields...
--
-- a is the type root
-- b and c Inherit from a (one-level single Inheritance)
-- d Inherits from b and c (two-level multiple Inheritance)
-- e Inherits from c (two-level single Inheritance)
-- f Inherits from e (three-level single Inheritance)
--
CREATE TABLE a_Star (
	class		char,
	a 			int4
) with(autovacuum_enabled = off);

CREATE TABLE b_Star (
	b 			text,
	class		char,
	a 			int4
) with(autovacuum_enabled = off);

CREATE TABLE c_Star (
	c 			name,
	class		char,
	a 			int4
) with(autovacuum_enabled = off);

CREATE TABLE d_Star (
	d 			float8,
	b 			text,
	class		char,
	a 			int4,
	c 			name
) with(autovacuum_enabled = off);

CREATE TABLE e_Star (
	e 			int2,
	c 			name,
	class		char,
	a 			int4
) with(autovacuum_enabled = off);

CREATE TABLE f_Star (
	f 			polygon,
	e 			int2,
	c 			name,
	class		char,
	a 			int4
) with(autovacuum_enabled = off);

CREATE TABLE AggTest (
	a 			int2,
	b			float4
) with(autovacuum_enabled = off);

CREATE TABLE Hash_i4_heap (
	seqno 		int4,
	random 		int4
) with(autovacuum_enabled = off);

CREATE TABLE Hash_name_heap (
	seqno 		int4,
	random 		name
) with(autovacuum_enabled = off);

CREATE TABLE Hash_txt_heap (
	seqno 		int4,
	random 		text
) with(autovacuum_enabled = off);

-- PGXC: Here replication is used to ensure correct index creation
-- when a non-shippable expression is used.
-- PGXCTODO: this should be removed once global constraints are supported
CREATE TABLE Hash_f8_heap (
	seqno		int4,
	random 		float8
)  with(autovacuum_enabled = off);

-- don't include the hash_ovfl_heap stuff in the distribution
-- the data set is too large for what it's worth
--
-- CREATE TABLE hash_ovfl_heap (
--	x			int4,
--	y			int4
-- );

CREATE TABLE BT_i4_heap (
	seqno 		int4,
	random 		int4
) with(autovacuum_enabled = off);

CREATE TABLE BT_name_heap (
	seqno 		name,
	random 		int4
) with(autovacuum_enabled = off);

CREATE TABLE BT_txt_heap (
	seqno 		text,
	random 		int4
);

CREATE TABLE BT_f8_heap (
	seqno 		float8,
	random 		int4
) with(autovacuum_enabled = off);

CREATE TABLE Array_op_test (
	seqno		int4,
	i			int4[],
	t			text[]
) with(autovacuum_enabled = off);

CREATE TABLE Array_index_op_test (
	seqno		int4,
	i			int4[],
	t			text[]
) with(autovacuum_enabled = off);

CREATE TABLE IF NOT EXISTS Test_tsvector(
	t text,
	a tsvector
);

CREATE TABLE IF NOT EXISTS Test_tsvector(
	t text
) with(autovacuum_enabled = off);

CREATE UNLOGGED TABLE unlogged1 (a int primary key);			-- OK
INSERT INTO unlogged1 VALUES (42);
CREATE UNLOGGED TABLE public.unlogged2 (a int primary key);		-- also OK
CREATE UNLOGGED TABLE pg_temp.unlogged3 (a int primary key);	-- not OK
CREATE TABLE pg_temp.implicitly_temp (a int primary key);		-- OK
CREATE TEMP TABLE explicitly_temp (a int primary key);			-- also OK
CREATE TEMP TABLE pg_temp.doubly_temp (a int primary key);		-- also OK
CREATE TEMP TABLE public.temp_to_perm (a int primary key);		-- not OK
DROP TABLE unlogged1, public.unlogged2;

--
-- CREATE TABLE AS TEST CASE: Expect the column typemod info is not lost on DN
--
CREATE TABLE HW_create_as_test11(C_CHAR CHAR(102400));
CREATE TABLE HW_create_as_test12(C_CHAR) as SELECT C_CHAR FROM HW_create_as_test11;
CREATE TABLE HW_create_as_test13 (C_CHAR CHAR(102400));
ALTER TABLE HW_create_as_test13 InheRIT HW_create_as_test12;
DROP TABLE HW_create_as_test12;
DROP TABLE HW_create_as_test13;
DROP TABLE HW_create_as_test11;

CREATE TABLE HW_create_as_test11(C_INT int);
CREATE TABLE HW_create_as_test12(C_INT) as SELECT C_INT FROM HW_create_as_test11;
CREATE TABLE HW_create_as_test13 (C_INT int);
ALTER TABLE HW_create_as_test13 InheRIT HW_create_as_test12;
DROP TABLE HW_create_as_test13;
DROP TABLE HW_create_as_test12;
DROP TABLE HW_create_as_test11;

CREATE TABLE HW_create_as_test11(COL1 numeric(10,2));
CREATE TABLE HW_create_as_test12(COL1) as SELECT COL1 FROM HW_create_as_test11;
CREATE TABLE HW_create_as_test13 (COL1 numeric(10,2));
ALTER TABLE HW_create_as_test13 InheRIT HW_create_as_test12;
DROP TABLE HW_create_as_test13;
DROP TABLE HW_create_as_test12;
DROP TABLE HW_create_as_test11;

CREATE TABLE HW_create_as_test11(COL1 timestamp(1));
CREATE TABLE HW_create_as_test12(COL1) as SELECT COL1 FROM HW_create_as_test11;
CREATE TABLE HW_create_as_test13 (COL1 timestamp(1));
ALTER TABLE HW_create_as_test13 InheRIT HW_create_as_test12;
DROP TABLE HW_create_as_test13;
DROP TABLE HW_create_as_test12;
DROP TABLE HW_create_as_test11;

CREATE TABLE HW_create_as_test11(COL1 int[2][2]);
CREATE TABLE HW_create_as_test12(COL1) as SELECT COL1 FROM HW_create_as_test11;
CREATE TABLE HW_create_as_test13 (COL1 int[2][2]);
ALTER TABLE HW_create_as_test13 InheRIT HW_create_as_test12;
DROP TABLE HW_create_as_test13;
DROP TABLE HW_create_as_test12;
DROP TABLE HW_create_as_test11;

create table HW_create_as_test11(col1 int);
insert into HW_create_as_test11 values(1);
insert into HW_create_as_test11 values(2);
create table HW_create_as_test12 as select * from HW_create_as_test11 with no data;
select count(*) from HW_create_as_test12;
explain (analyze on, costs off) create table HW_create_as_test13 as select * from HW_create_as_test11 with no data;
drop table HW_create_as_test11;
drop table HW_create_as_test12;
drop table HW_create_as_test13;

CREATE TABLE HW_create_as_test11(COL1 int);
insert into HW_create_as_test11 values(1);
insert into HW_create_as_test11 values(2);
CREATE TABLE HW_create_as_test12 as SELECT '001' col1, COL1 col2 FROM HW_create_as_test11;
select * from HW_create_as_test12 order by 1, 2;
DROP TABLE HW_create_as_test12;
DROP TABLE HW_create_as_test11;

-- Zero column table is not supported any more.
CREATE TABLE Zero_column_table_test1();
CREATE TABLE Zero_column_table_test2();

CREATE TABLE Zero_column_table_test3(a INT);
ALTER TABLE Zero_column_table_test3 DROP COLUMN a;
DROP TABLE Zero_column_table_test3;

CREATE TABLE Zero_column_table_test6() with (orientation = column);
CREATE TABLE Zero_column_table_test7() with (orientation = column);
CREATE TABLE Zero_column_table_test8(a INT) with (orientation = column);
ALTER TABLE Zero_column_table_test8 DROP COLUMN a;
DROP TABLE Zero_column_table_test8;

--test create table of pg_node_tree type
create table PG_node_tree_tbl11(id int,name pg_node_tree);
create table PG_node_tree_tbl12 as select * from pg_type;

-- test unreserved keywords for table name
CREATE TABLE App(a int);
CREATE TABLE Movement(a int);
CREATE TABLE Pool(a int);
CREATE TABLE Profile(a int);
CREATE TABLE Resource(a int);
CREATE TABLE Store(a int);
CREATE TABLE Than(a int);
CREATE TABLE Workload(a int);

DROP TABLE App;
DROP TABLE Movement;
DROP TABLE Pool;
DROP TABLE Profile;
DROP TABLE Resource;
DROP TABLE Store;
DROP TABLE Than;
DROP TABLE Workload;

-- test orientation
CREATE TABLE Orientation_test_1 (c1 int) WITH (orientation = column);
CREATE TABLE Orientation_test_2 (c1 int) WITH (orientation = 'column');
CREATE TABLE Orientation_test_3 (c1 int) WITH (orientation = "column");
CREATE TABLE Orientation_test_4 (c1 int) WITH (orientation = row);
CREATE TABLE Orientation_test_5 (c1 int) WITH (orientation = 'row');
CREATE TABLE Orientation_test_6 (c1 int) WITH (orientation = "row");

DROP TABLE Orientation_test_1;
DROP TABLE Orientation_test_2;
DROP TABLE Orientation_test_3;
DROP TABLE Orientation_test_4;
DROP TABLE Orientation_test_5;
DROP TABLE Orientation_test_6;

CREATE SCHEMA "TEST";
CREATE SCHEMA "SCHEMA_TEST";

CREATE TABLE "SCHEMA_TEST"."Table" (
    column1 bigint,
    column2 bigint
);

CREATE TABLE "TEST"."Test_Table"(
    clm1 "SCHEMA_TEST"."Table",
    clm2 bigint);
    
select * from "TEST"."Test_Table";

set current_schema=public;
create table Test_info(a int, b int);
insert into Test_info values(1,2),(2,3),(3,4),(4,5);
\d+ "Test_info"

explain (verbose on, costs off) select * from Test_info;
select count(*) from Test_info;
drop table Test_info;
reset current_schema;

create table t_Serial(a int, b serial);
create temp table t_Tmp(a int, b serial);
create temp table t_Tmp(like t_Serial);
set enable_beta_features = on;
create temp table t_Tmp(like t_Serial);
select nextval('"t_Tmp_b_seq"');
reset enable_beta_features;
set default_statistics_target = -50;
analyze t_Serial;
drop table t_Serial;
drop table t_Tmp;

-- schema.table
CREATE TABLE public.Test_indirection1(a int);
-- catalog.schema.table
CREATE TABLE table_name_test_db.public.Test_indirection2(a int);
-- error
CREATE TABLE unknown_obj.table_name_test_db.public.Test_indirection3(a int);

drop table "TEST"."Test_Table";
drop table "SCHEMA_TEST"."Table";
drop schema "TEST";
drop schema "SCHEMA_TEST";
