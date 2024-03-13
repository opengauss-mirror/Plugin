create schema test_char;
set current_schema to 'test_char';
set dolphin.b_compatibility_mode=1;
set dolphin.sql_mode = '';

---create table
create table testfortext(a text,b int);
create table testforchar(a char(10),b int);
create table testforvarchar(a varchar(10),b int);

---insert data
insert into testfortext values('00100abc',1);
insert into testfortext values('00100abc',0);
insert into testfortext values('00100abc',null);
insert into testfortext values('-1abc',1);
insert into testfortext values('-1abc',0);
insert into testfortext values('-1abc',null);
insert into testfortext values('00200',1);
insert into testfortext values('00200',0);
insert into testfortext values('00200',null);
insert into testfortext values('abc',1);
insert into testfortext values('abc',0);
insert into testfortext values('abc',null);
insert into testfortext values('abc1',1);
insert into testfortext values('abc1',0);
insert into testfortext values('abc1',null);
insert into testfortext values('0.0100abc',1);
insert into testfortext values('0.0100abc',0);
insert into testfortext values('0.0100abc',null);

insert into testforchar values('00100abc',1);
insert into testforchar values('00100abc',0);
insert into testforchar values('00100abc',null);
insert into testforchar values('-1abc',1);
insert into testforchar values('-1abc',0);
insert into testforchar values('-1abc',null);
insert into testforchar values('00200',1);
insert into testforchar values('00200',0);
insert into testforchar values('00200',null);
insert into testforchar values('abc',1);
insert into testforchar values('abc',0);
insert into testforchar values('abc',null);
insert into testforchar values('abc1',1);
insert into testforchar values('abc1',0);
insert into testforchar values('abc1',null);
insert into testforchar values('0.0100abc',1);
insert into testforchar values('0.0100abc',0);
insert into testforchar values('0.0100abc',null);

insert into testforvarchar values('00100abc',1);
insert into testforvarchar values('00100abc',0);
insert into testforvarchar values('00100abc',null);
insert into testforvarchar values('-1abc',1);
insert into testforvarchar values('-1abc',0);
insert into testforvarchar values('-1abc',null);
insert into testforvarchar values('00200',1);
insert into testforvarchar values('00200',0);
insert into testforvarchar values('00200',null);
insert into testforvarchar values('abc',1);
insert into testforvarchar values('abc',0);
insert into testforvarchar values('abc',null);
insert into testforvarchar values('abc1',1);
insert into testforvarchar values('abc1',0);
insert into testforvarchar values('abc1',null);
insert into testforvarchar values('0.0100abc',1);
insert into testforvarchar values('0.0100abc',0);
insert into testforvarchar values('0.0100abc',null);

select a and b from testfortext;
select a && b from testfortext;
select a or b from testfortext;
select a || b from testfortext;

select a and b from testforchar;
select a && b from testforchar;
select a or b from testforchar;
select a || b from testforchar;

select a and b from testforvarchar;
select a && b from testforvarchar;
select a or b from testforvarchar;
select a || b from testforvarchar;

---drop table
drop table testfortext;
drop table testforchar;
drop table testforvarchar;

set dolphin.b_compatibility_mode to on;
create table t1(a varchar(10));
insert into t1 values(1);
select * from t1 where a=true;
create table t2(a char(10));
insert into t2 values(1);
select * from t2 where a=true;
create table t3(a text);
insert into t3 values(1);
select * from t3 where a=true;
drop table t1;
drop table t2;
drop table t3;

select '00100abc' && 1;
select '00100abc' && 0;
select '00100abc' && null;
select '-1abc' && 1;
select '-1abc' && 0;
select '-1abc' && null;
select '00200' && 1;
select '00200' && 0;
select '00200' && null;
select 'abc' && 1;
select 'abc' && 0;
select 'abc' && null;
select 'abc1' && 1;
select 'abc1' && 0;
select 'abc1' && null;
select '0.0100abc' && 1;
select '0.0100abc' && 0;
select '0.0100abc' && null;

select '00100abc' || 1;
select '00100abc' || 0;
select '00100abc' || null;
select '-1abc' || 1;
select '-1abc' || 0;
select '-1abc' || null;
select '00200' || 1;
select '00200' || 0;
select '00200' || null;
select 'abc' || 1;
select 'abc' || 0;
select 'abc' || null;
select 'abc1' || 1;
select 'abc1' || 0;
select 'abc1' || null;
select '0.0100abc' || 1;
select '0.0100abc' || 0;
select '0.0100abc' || null;
select true and 'true' AS false;
select true and 'false' AS false;
select true and '1.0 false' AS true;
select true and '0.0 true' AS false;
select false or 'true' AS false;
select false or 'false' AS false;
select false or '1.0 false' AS true;
select false or '0.0 true' AS false;
select true and 'true'::text AS false;
select true and 'true'::char AS false;
select true and 'true'::varchar AS false;

drop schema test_char cascade;
reset current_schema;
