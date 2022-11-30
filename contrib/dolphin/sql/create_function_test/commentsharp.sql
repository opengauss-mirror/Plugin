drop database if exists db_comment_sharp;
create database db_comment_sharp dbcompatibility 'B';
\c db_comment_sharp

create table t1(a int,b int);

insert into t1 values(10,11);

insert into t1 values(12,13);

set dolphin.b_compatibility_mode  = 0;

select a # b from t1;

select a -- b from t1;
;

set dolphin.b_compatibility_mode  = 1;

select 2 # b from t1 ;

select a +# b from t1;
;

#select a + b from t1;


select a + b from t1;#comment aaa

select a + b from t1;#+===- comment aaa


select a + b from t1;=#- comment aaa
;

set dolphin.b_compatibility_mode  = 0;

create table t1#t2 (a int);

insert into t1#t2 values(1),(2);

select * from t1#t2;

set dolphin.b_compatibility_mode  = 1;

create table t3#t2
(a int);

insert  into t3 values(1),(2);

select * from t3;

create table t4 (a#b
int );

insert into t4 values(1),(3);

select * from t4;

set dolphin.b_compatibility_mode  = 0;

--test inside plpgsql 
set dolphin.b_compatibility_mode  = 1;

create table test(a varchar(10));

CREATE FUNCTION testfunc3() return int as
begin
insert into test values ('bbbbbb');# 
insert into test values ('c#
d');
return 1;
end;
/

call testfunc3();


CREATE FUNCTION testfunc2() return int as
declare
a#b int := 5
int :=4;
begin
insert into test values (a);
return 1;
end;
/

call testfunc2();

set dolphin.b_compatibility_mode  = 0;


CREATE FUNCTION testfunc12() return int as
declare
a#b int := 5
int :=4;
begin
insert into test values (a);
return 1;
end;
/

call testfunc12();

--bug fix when use sharp after  keyword without space
set dolphin.b_compatibility_mode = on;

create or replace function testfunc() returns trigger as
$$
declare
begin
insert into t_trig values(new.id);
return new;
end
$$ language plpgsql;

create table t_test (a int);

create table t_test2 (a int);
-- after keyword 

create trigger test_trig
after insert on t_test
for each row#
execute procedure testfunc();


create trigger test_trig2
after insert on t_test2
for each row# 123123123
execute procedure testfunc();

create#
table t_test3 (a int);

create trigger test_trig3
after insert on t_test3
for each row
execute# 
procedure testfunc();

--turn off parameter,has error 
set dolphin.b_compatibility_mode = off;

create trigger test_trig4
after insert on t_test
for each row#
execute procedure testfunc();

--clear enverment

drop table t_test;
drop table t_test2;
drop table t_test3;


\c postgres

drop database if exists db_comment_sharp;
