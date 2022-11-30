\c table_name_test_db;
set dolphin.lower_case_table_names TO 0;
--
-- drop.source
--

--
-- this will fail if the user is not the postgres superuser.
-- if it does, don't worry about it (you can turn usersuper
-- back on as "postgres").  too many people don't follow
-- directions and run this as "postgres", though...
--
UPDATE pg_user
   SET usesuper = 't'::bool
   WHERE usename = 'postgres';


--
-- FUNCTION REMOVAL
--
DROP FUNCTION hobbies("Person");

DROP FUNCTION hobby_construct(text,text);

DROP FUNCTION equipment("Hobbies_r");

DROP FUNCTION user_relns();

DROP FUNCTION widget_in(cstring);

DROP FUNCTION widget_out(widget);

DROP FUNCTION pt_in_widget(point,widget);

DROP FUNCTION overpaid(Emp);

DROP FUNCTION boxarea(box);

DROP FUNCTION interpt_pp(path,path);

DROP FUNCTION reverse_name(name);

DROP FUNCTION oldstyle_length(int4, text);

--
-- OPERATOR REMOVAL
--
DROP OPERATOR ## (path, path);

DROP OPERATOR <% (point, widget);

-- left unary
DROP OPERATOR @#@ (none, int4);

-- right unary
DROP OPERATOR #@# (int4, none);

-- right unary
DROP OPERATOR #%# (int4, none);


--
-- ABSTRACT DATA TYPE REMOVAL
--
DROP TYPE city_budget;

DROP TYPE widget;


--
-- RULE REMOVAL
--	(is also tested in queries.source)
--

--
-- AGGREGATE REMOVAL
--
DROP AGGREGATE newavg (int4);

DROP AGGREGATE newsum (int4);

DROP AGGREGATE newcnt (int4);


--
-- CLASS REMOVAL
--	(Inheritance hierarchies are deleted in reverse order)
--

--
-- DROP ancillary data structures (i.e. indices)
--
DROP INDEX onek_unique1;

DROP INDEX onek_unique2;

DROP INDEX onek_hundred;

DROP INDEX onek_stringu1;

DROP INDEX tenk1_unique1;

DROP INDEX tenk1_unique2;

DROP INDEX tenk1_hundred;

DROP INDEX tenk2_unique1;

DROP INDEX tenk2_unique2;

DROP INDEX tenk2_hundred;

-- DROP INDEX onek2_u1_prtl;

-- DROP INDEX onek2_u2_prtl;

-- DROP INDEX onek2_stu1_prtl;

DROP INDEX rix;

DROP INDEX iix;

DROP INDEX six;

DROP INDEX hash_i4_index;

DROP INDEX hash_name_index;

DROP INDEX hash_txt_index;

DROP INDEX hash_f8_index;

-- DROP INDEX hash_ovfl_index;

DROP INDEX bt_i4_index;

DROP INDEX bt_name_index;

DROP INDEX bt_txt_index;

DROP INDEX bt_f8_index;


DROP TABLE  Onek;

DROP TABLE  Onek2;

DROP TABLE  Tenk1;

DROP TABLE  Tenk2;

DROP TABLE  Hobbies_r;

DROP TABLE  Equipment_r;

DROP TABLE  AggTest;

DROP TABLE  f_Star;

DROP TABLE  e_Star;

DROP TABLE  d_Star;

DROP TABLE  c_Star;

DROP TABLE  b_Star;

DROP TABLE  a_Star;


--
-- must be in reverse Inheritance order
--
DROP TABLE  Stud_Emp;

DROP TABLE  Student;

DROP TABLE  Slow_emp4000;

DROP TABLE  Fast_emp4000;

DROP TABLE  Emp;

DROP TABLE  Person;


DROP TABLE  ramp;

DROP TABLE  Real_City;

DROP TABLE  Dept;

DROP TABLE  iHighway;

DROP TABLE  sHighway;

DROP TABLE  Road;

DROP TABLE  City;


DROP TABLE  Hash_i4_heap;

DROP TABLE  Hash_name_heap;

DROP TABLE  Hash_txt_heap;

DROP TABLE  Hash_f8_heap;

-- DROP TABLE  hash_ovfl_heap;

DROP TABLE  BT_i4_heap;

DROP TABLE  BT_name_heap;

DROP TABLE  BT_txt_heap;

DROP TABLE  BT_f8_heap;
