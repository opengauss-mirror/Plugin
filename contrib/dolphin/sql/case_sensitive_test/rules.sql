\c table_name_test_db;
set dolphin.lower_case_table_names TO 0;
--
-- RULES
-- From Jan's original setup_ruletest.sql and run_ruletest.sql
-- - thomas 1998-09-13
--

--
-- Tables and rules for the view test
--
create table rtest_T1 (a int4, b int4);
create table rtest_T2 (a int4, b int4);
create table rtest_T3 (a int4, b int4);

create view rtest_v1 as select * from rtest_T1;
create rule rtest_v1_ins as on insert to rtest_v1 do instead
	insert into rtest_T1 values (new.a, new.b);
create rule rtest_v1_upd as on update to rtest_v1 do instead
	update rtest_T1 set a = new.a, b = new.b
	where a = old.a;
create rule rtest_v1_del as on delete to rtest_v1 do instead
	delete from rtest_T1 where a = old.a;
-- Test comments
COMMENT ON RULE rtest_v1_bad ON rtest_v1 IS 'bad rule';
COMMENT ON RULE rtest_v1_del ON rtest_v1 IS 'delete rule';
COMMENT ON RULE rtest_v1_del ON rtest_v1 IS NULL;
--
-- Tables and rules for the constraint update/delete test
--
-- Note:
-- 	Now that we have multiple action rule support, we check
-- 	both possible syntaxes to define them (The last action
--  can but must not have a semicolon at the end).
--
create table rtest_System (sysname text, sysdesc text);
create table rtest_Interface (sysname text, ifname text);
create table rtest_Person (pname text, pdesc text);
create table rtest_Admin (pname text, sysname text);

create rule rtest_sys_upd as on update to rtest_System do also (
	update rtest_Interface set sysname = new.sysname
		where sysname = old.sysname;
	update rtest_Admin set sysname = new.sysname
		where sysname = old.sysname
	);

create rule rtest_sys_del as on delete to rtest_System do also (
	delete from rtest_Interface where sysname = old.sysname;
	delete from rtest_Admin where sysname = old.sysname;
	);

create rule rtest_pers_upd as on update to rtest_Person do also
	update rtest_Admin set pname = new.pname where pname = old.pname;

create rule rtest_pers_del as on delete to rtest_Person do also
	delete from rtest_Admin where pname = old.pname;

--
-- Tables and rules for the logging test
--
create table rtest_Emp (ename char(20), salary money);
create table rtest_Emplog (ename char(20), who name, action char(10), newsal money, oldsal money);
create table rtest_Empmass (ename char(20), salary money);

create rule rtest_emp_ins as on insert to rtest_Emp do
	insert into rtest_Emplog values (new.ename, current_user,
			'hired', new.salary, '0.00');

create rule rtest_emp_upd as on update to rtest_Emp where new.salary != old.salary do
	insert into rtest_Emplog values (new.ename, current_user,
			'honored', new.salary, old.salary);

create rule rtest_emp_del as on delete to rtest_Emp do
	insert into rtest_Emplog values (old.ename, current_user,
			'fired', '0.00', old.salary);

--
-- Tables and rules for the multiple cascaded qualified instead
-- rule test
--
create table rtest_t4 (a int4, b text);
create table rtest_t5 (a int4, b text);
create table rtest_t6 (a int4, b text);
create table rtest_t7 (a int4, b text);
create table rtest_t8 (a int4, b text);
create table rtest_t9 (a int4, b text);

create rule rtest_t4_ins1 as on insert to rtest_t4
		where new.a >= 10 and new.a < 20 do instead
	insert into rtest_t5 values (new.a, new.b);

create rule rtest_t4_ins2 as on insert to rtest_t4
		where new.a >= 20 and new.a < 30 do
	insert into rtest_t6 values (new.a, new.b);

create rule rtest_t5_ins as on insert to rtest_t5
		where new.a > 15 do
	insert into rtest_t7 values (new.a, new.b);

create rule rtest_t6_ins as on insert to rtest_t6
		where new.a > 25 do instead
	insert into rtest_t8 values (new.a, new.b);

--
-- Tables and rules for the rule fire order test
--
-- As of PG 7.3, the rules should fire in order by name, regardless
-- of INSTEAD attributes or creation order.
--
create table rtest_order1 (a int4);
create table rtest_order2 (a int4, b int4, c text);

create sequence rtest_seq;

create rule rtest_order_r3 as on insert to rtest_order1 do instead
	insert into rtest_order2 values (new.a, nextval('rtest_seq'),
		'rule 3 - this should run 3rd');

create rule rtest_order_r4 as on insert to rtest_order1
		where a < 100 do instead
	insert into rtest_order2 values (new.a, nextval('rtest_seq'),
		'rule 4 - this should run 4th');

create rule rtest_order_r2 as on insert to rtest_order1 do
	insert into rtest_order2 values (new.a, nextval('rtest_seq'),
		'rule 2 - this should run 2nd');

create rule rtest_order_r1 as on insert to rtest_order1 do instead
	insert into rtest_order2 values (new.a, nextval('rtest_seq'),
		'rule 1 - this should run 1st');

--
-- Tables and rules for the instead nothing test
--
create table rtest_Nothn1 (a int4, b text);
create table rtest_Nothn2 (a int4, b text);
create table rtest_Nothn3 (a int4, b text);
create table rtest_Nothn4 (a int4, b text);

create rule rtest_nothn_r1 as on insert to rtest_Nothn1
	where new.a >= 10 and new.a < 20 do instead nothing;

create rule rtest_nothn_r2 as on insert to rtest_Nothn1
	where new.a >= 30 and new.a < 40 do instead nothing;

create rule rtest_nothn_r3 as on insert to rtest_Nothn2
	where new.a >= 100 do instead
	insert into rtest_Nothn3 values (new.a, new.b);

create rule rtest_nothn_r4 as on insert to rtest_Nothn2
	do instead nothing;

--
-- Tests on a view that is select * of a table
-- and has insert/update/delete instead rules to
-- behave close like the real table.
--

--
-- We need test date later
--
insert into rtest_T2 values (1, 21);
insert into rtest_T2 values (2, 22);
insert into rtest_T2 values (3, 23);

insert into rtest_T3 values (1, 31);
insert into rtest_T3 values (2, 32);
insert into rtest_T3 values (3, 33);
insert into rtest_T3 values (4, 34);
insert into rtest_T3 values (5, 35);

-- insert values
insert into rtest_v1 values (1, 11);
insert into rtest_v1 values (2, 12);
select * from rtest_v1;

-- delete with constant expression
delete from rtest_v1 where a = 1;
select * from rtest_v1;
insert into rtest_v1 values (1, 11);
delete from rtest_v1 where b = 12;
select * from rtest_v1;
insert into rtest_v1 values (2, 12);
insert into rtest_v1 values (2, 13);
select * from rtest_v1;
** Remember the delete rule on rtest_v1: It says
** DO INSTEAD DELETE FROM rtest_T1 WHERE a = old.a
** So this time both rows with a = 2 must get deleted
\p
\r
delete from rtest_v1 where b = 12;
select * from rtest_v1;
delete from rtest_v1;

-- insert select
insert into rtest_v1 select * from rtest_T2;
select * from rtest_v1;
delete from rtest_v1;

-- same with swapped targetlist
insert into rtest_v1 (b, a) select b, a from rtest_T2;
select * from rtest_v1;

-- now with only one target attribute
insert into rtest_v1 (a) select a from rtest_T3;
select * from rtest_v1;
select * from rtest_v1 where b isnull;

-- let attribute a differ (must be done on rtest_T1 - see above)
update rtest_T1 set a = a + 10 where b isnull;
delete from rtest_v1 where b isnull;
select * from rtest_v1;

-- now updates with constant expression
update rtest_v1 set b = 42 where a = 2;
select * from rtest_v1;
update rtest_v1 set b = 99 where b = 42;
select * from rtest_v1;
update rtest_v1 set b = 88 where b < 50;
select * from rtest_v1;
delete from rtest_v1;
insert into rtest_v1 select rtest_T2.a, rtest_T3.b
    from rtest_T2, rtest_T3
    where rtest_T2.a = rtest_T3.a;
select * from rtest_v1;

-- updates in a mergejoin
update rtest_v1 set b = rtest_T2.b from rtest_T2 where rtest_v1.a = rtest_T2.a;
select * from rtest_v1;
insert into rtest_v1 select * from rtest_T3;
select * from rtest_v1;
update rtest_T1 set a = a + 10 where b > 30;
select * from rtest_v1;
update rtest_v1 set a = rtest_T3.a + 20 from rtest_T3 where rtest_v1.b = rtest_T3.b;
select * from rtest_v1;

--
-- Test for constraint updates/deletes
--
insert into rtest_System values ('orion', 'Linux Jan Wieck');
insert into rtest_System values ('notjw', 'WinNT Jan Wieck (notebook)');
insert into rtest_System values ('neptun', 'Fileserver');

insert into rtest_Interface values ('orion', 'eth0');
insert into rtest_Interface values ('orion', 'eth1');
insert into rtest_Interface values ('notjw', 'eth0');
insert into rtest_Interface values ('neptun', 'eth0');

insert into rtest_Person values ('jw', 'Jan Wieck');
insert into rtest_Person values ('bm', 'Bruce Momjian');

insert into rtest_Admin values ('jw', 'orion');
insert into rtest_Admin values ('jw', 'notjw');
insert into rtest_Admin values ('bm', 'neptun');

update rtest_System set sysname = 'pluto' where sysname = 'neptun';

select * from rtest_Interface;
select * from rtest_Admin;

update rtest_Person set pname = 'jwieck' where pdesc = 'Jan Wieck';

-- Note: use ORDER BY here to ensure consistent output across all systems.
-- The above UPDATE affects two rows with equal keys, so they could be
-- updated in either order depending on the whim of the local qsort().

select * from rtest_Admin order by pname, sysname;

delete from rtest_System where sysname = 'orion';

select * from rtest_Interface;
select * from rtest_Admin;

--
-- Rule qualification test
--
insert into rtest_Emp values ('wiecc', '5000.00');
insert into rtest_Emp values ('gates', '80000.00');
update rtest_Emp set ename = 'wiecx' where ename = 'wiecc';
update rtest_Emp set ename = 'wieck', salary = '6000.00' where ename = 'wiecx';
update rtest_Emp set salary = '7000.00' where ename = 'wieck';
delete from rtest_Emp where ename = 'gates';

select ename, who = current_user as "matches user", action, newsal, oldsal from rtest_Emplog order by ename, action, newsal;
insert into rtest_Empmass values ('meyer', '4000.00');
insert into rtest_Empmass values ('maier', '5000.00');
insert into rtest_Empmass values ('mayr', '6000.00');
insert into rtest_Emp select * from rtest_Empmass;
select ename, who = current_user as "matches user", action, newsal, oldsal from rtest_Emplog order by ename, action, newsal;
update rtest_Empmass set salary = salary + '1000.00';
update rtest_Emp set salary = rtest_Empmass.salary from rtest_Empmass where rtest_Emp.ename = rtest_Empmass.ename;
select ename, who = current_user as "matches user", action, newsal, oldsal from rtest_Emplog order by ename, action, newsal;
delete from rtest_Emp using rtest_Empmass where rtest_Emp.ename = rtest_Empmass.ename;
select ename, who = current_user as "matches user", action, newsal, oldsal from rtest_Emplog order by ename, action, newsal;

--
-- Multiple cascaded qualified instead rule test
--
insert into rtest_t4 values (1, 'Record should go to rtest_t4');
insert into rtest_t4 values (2, 'Record should go to rtest_t4');
insert into rtest_t4 values (10, 'Record should go to rtest_t5');
insert into rtest_t4 values (15, 'Record should go to rtest_t5');
insert into rtest_t4 values (19, 'Record should go to rtest_t5 and t7');
insert into rtest_t4 values (20, 'Record should go to rtest_t4 and t6');
insert into rtest_t4 values (26, 'Record should go to rtest_t4 and t8');
insert into rtest_t4 values (28, 'Record should go to rtest_t4 and t8');
insert into rtest_t4 values (30, 'Record should go to rtest_t4');
insert into rtest_t4 values (40, 'Record should go to rtest_t4');

select * from rtest_t4;
select * from rtest_t5;
select * from rtest_t6;
select * from rtest_t7;
select * from rtest_t8;

delete from rtest_t4;
delete from rtest_t5;
delete from rtest_t6;
delete from rtest_t7;
delete from rtest_t8;

insert into rtest_t9 values (1, 'Record should go to rtest_t4');
insert into rtest_t9 values (2, 'Record should go to rtest_t4');
insert into rtest_t9 values (10, 'Record should go to rtest_t5');
insert into rtest_t9 values (15, 'Record should go to rtest_t5');
insert into rtest_t9 values (19, 'Record should go to rtest_t5 and t7');
insert into rtest_t9 values (20, 'Record should go to rtest_t4 and t6');
insert into rtest_t9 values (26, 'Record should go to rtest_t4 and t8');
insert into rtest_t9 values (28, 'Record should go to rtest_t4 and t8');
insert into rtest_t9 values (30, 'Record should go to rtest_t4');
insert into rtest_t9 values (40, 'Record should go to rtest_t4');

insert into rtest_t4 select * from rtest_t9 where a < 20;

select * from rtest_t4;
select * from rtest_t5;
select * from rtest_t6;
select * from rtest_t7;
select * from rtest_t8;

insert into rtest_t4 select * from rtest_t9 where b ~ 'and t8';

select * from rtest_t4;
select * from rtest_t5;
select * from rtest_t6;
select * from rtest_t7;
select * from rtest_t8;

insert into rtest_t4 select a + 1, b from rtest_t9 where a in (20, 30, 40);

select * from rtest_t4;
select * from rtest_t5;
select * from rtest_t6;
select * from rtest_t7;
select * from rtest_t8;

--
-- Check that the ordering of rules fired is correct
--
insert into rtest_order1 values (1);
select * from rtest_order2;

--
-- Check if instead nothing w/without qualification works
--
insert into rtest_Nothn1 values (1, 'want this');
insert into rtest_Nothn1 values (2, 'want this');
insert into rtest_Nothn1 values (10, 'don''t want this');
insert into rtest_Nothn1 values (19, 'don''t want this');
insert into rtest_Nothn1 values (20, 'want this');
insert into rtest_Nothn1 values (29, 'want this');
insert into rtest_Nothn1 values (30, 'don''t want this');
insert into rtest_Nothn1 values (39, 'don''t want this');
insert into rtest_Nothn1 values (40, 'want this');
insert into rtest_Nothn1 values (50, 'want this');
insert into rtest_Nothn1 values (60, 'want this');

select * from rtest_Nothn1;

insert into rtest_Nothn2 values (10, 'too small');
insert into rtest_Nothn2 values (50, 'too small');
insert into rtest_Nothn2 values (100, 'OK');
insert into rtest_Nothn2 values (200, 'OK');

select * from rtest_Nothn2;
select * from rtest_Nothn3;

delete from rtest_Nothn1;
delete from rtest_Nothn2;
delete from rtest_Nothn3;

insert into rtest_Nothn4 values (1, 'want this');
insert into rtest_Nothn4 values (2, 'want this');
insert into rtest_Nothn4 values (10, 'don''t want this');
insert into rtest_Nothn4 values (19, 'don''t want this');
insert into rtest_Nothn4 values (20, 'want this');
insert into rtest_Nothn4 values (29, 'want this');
insert into rtest_Nothn4 values (30, 'don''t want this');
insert into rtest_Nothn4 values (39, 'don''t want this');
insert into rtest_Nothn4 values (40, 'want this');
insert into rtest_Nothn4 values (50, 'want this');
insert into rtest_Nothn4 values (60, 'want this');

insert into rtest_Nothn1 select * from rtest_Nothn4;

select * from rtest_Nothn1;

delete from rtest_Nothn4;

insert into rtest_Nothn4 values (10, 'too small');
insert into rtest_Nothn4 values (50, 'too small');
insert into rtest_Nothn4 values (100, 'OK');
insert into rtest_Nothn4 values (200, 'OK');

insert into rtest_Nothn2 select * from rtest_Nothn4;

select * from rtest_Nothn2;
select * from rtest_Nothn3;

create table rtest_view1 (a int4, b text, v bool);
create table rtest_view2 (a int4);
create table rtest_view3 (a int4, b text);
create table rtest_view4 (a int4, b text, c int4);
create view rtest_vview1 as select a, b from rtest_view1 X
	where 0 < (select count(*) from rtest_view2 Y where Y.a = X.a);
create view rtest_vview2 as select a, b from rtest_view1 where v;
create view rtest_vview3 as select a, b from rtest_vview2 X
	where 0 < (select count(*) from rtest_view2 Y where Y.a = X.a);
create view rtest_vview4 as select X.a, X.b, count(Y.a) as refcount
	from rtest_view1 X, rtest_view2 Y
	where X.a = Y.a
	group by X.a, X.b;
create function rtest_viewfunc1(int4) returns int4 as
	'select count(*)::int4 from rtest_view2 where a = $1'
	language sql;
create view rtest_vview5 as select a, b, rtest_viewfunc1(a) as refcount
	from rtest_view1;

insert into rtest_view1 values (1, 'item 1', 't');
insert into rtest_view1 values (2, 'item 2', 't');
insert into rtest_view1 values (3, 'item 3', 't');
insert into rtest_view1 values (4, 'item 4', 'f');
insert into rtest_view1 values (5, 'item 5', 't');
insert into rtest_view1 values (6, 'item 6', 'f');
insert into rtest_view1 values (7, 'item 7', 't');
insert into rtest_view1 values (8, 'item 8', 't');

insert into rtest_view2 values (2);
insert into rtest_view2 values (2);
insert into rtest_view2 values (4);
insert into rtest_view2 values (5);
insert into rtest_view2 values (7);
insert into rtest_view2 values (7);
insert into rtest_view2 values (7);
insert into rtest_view2 values (7);

select * from rtest_vview1;
select * from rtest_vview2;
select * from rtest_vview3;
select * from rtest_vview4 order by a, b;
select * from rtest_vview5;

insert into rtest_view3 select * from rtest_vview1 where a < 7;
select * from rtest_view3;
delete from rtest_view3;

insert into rtest_view3 select * from rtest_vview2 where a != 5 and b !~ '2';
select * from rtest_view3;
delete from rtest_view3;

insert into rtest_view3 select * from rtest_vview3;
select * from rtest_view3;
delete from rtest_view3;

insert into rtest_view4 select * from rtest_vview4 where 3 > refcount;
select * from rtest_view4 order by a, b;
delete from rtest_view4;

insert into rtest_view4 select * from rtest_vview5 where a > 2 and refcount = 0;
select * from rtest_view4;
delete from rtest_view4;