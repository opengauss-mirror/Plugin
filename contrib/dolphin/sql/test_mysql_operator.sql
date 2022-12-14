drop database if exists test_op_and;
CREATE DATABASE test_op_and with dbcompatibility='B';
\c test_op_and
set dolphin.b_compatibility_mode = 1;
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group';

---create table
create table testforboolean(a boolean,b boolean);
create table testforint(a int,b int);
create table testforfloat(a float,b float);
create table testforbit(a bit(4),b bit(4));
create table testfordate(a date,b date);
create table testfortime(a time,b time);
create table testforstring(a varchar,b varchar);

---insert data
insert into testforboolean values(true,true);
insert into testforboolean values(true,false);
insert into testforboolean values(false,true);
insert into testforboolean values(false,false);
insert into testforboolean values(true,null);
insert into testforboolean values(false,null);
insert into testforboolean values(null,null);

insert into testforint values(1001,1001);
insert into testforint values(1001,-1010);
insert into testforint values(1001,0110);
insert into testforint values(1001,0);
insert into testforint values(0,1001);
insert into testforint values(0,0);
insert into testforint values(1001,null);
insert into testforint values(0,null);
insert into testforint values(null,null);

insert into testforfloat values(1.27,1.27);
insert into testforfloat values(1.27,-1.27);
insert into testforfloat values(1.27,0.01);
insert into testforfloat values(1.27,0.0);
insert into testforfloat values(0.0,1.27);
insert into testforfloat values(0.0,0.0);
insert into testforfloat values(1.27,null);
insert into testforfloat values(0.0,null);
insert into testforfloat values(null,null);

insert into testforbit values('1111','1111');
insert into testforbit values('1111','0111');
insert into testforbit values('1111','1011');
insert into testforbit values('1111','0000');
insert into testforbit values('0000','1111');
insert into testforbit values('0000','0000');
insert into testforbit values('1111',null);
insert into testforbit values('0000',null);
insert into testforbit values(null,null);

insert into testfordate values('2022-08-20','2022-08-20');
insert into testfordate values('2022-08-20','2022-08-21');
insert into testfordate values('2022-08-20','2022-07-20');
insert into testfordate values('2022-08-20','2021-08-20');
insert into testfordate values('2022-08-20',null);
insert into testfordate values(null,null);

insert into testfortime values('11:11:11','11:11:11');
insert into testfortime values('11:11:11','11:11:00');
insert into testfortime values('11:11:11','11:00:11');
insert into testfortime values('11:11:11','00:11:11');
insert into testfortime values('11:11:11','00:00:00');
insert into testfortime values('00:00:00','11:11:11');
insert into testfortime values('00:00:00','00:00:00');
insert into testfortime values('11:11:11',null);
insert into testfortime values('00:00:00',null);
insert into testfortime values(null,null);

insert into testforstring values('10','10');
insert into testforstring values('-10','+01');
insert into testforstring values('123a','123a');


select a&&b from testforboolean;
select a||b from testforboolean;

select a&&b from testforint;
select a||b from testforint;

select a&&b from testforfloat;
select a||b from testforfloat;

select a&&b from testforbit;
select a||b from testforbit;

select a&&b from testfordate;
select a||b from testfordate;

select a&&b from testfortime;
select a||b from testfortime;

select a&&b from testforstring;
select a||b from testforstring;

---drop table
drop table testforboolean;
drop table testforint;
drop table testforfloat;
drop table testforbit;
drop table testfordate;
drop table testfortime;
drop table testforstring;

---create table with (orientation=column)
create table testforboolean(a boolean,b boolean) with (orientation=column);

---create temp table
create local temp table testforint(a int,b int);
create global temp table testforfloat(a float,b float);

---create unlogged table
create unlogged table testforbit(a bit(4),b bit(4));

---create ustore table
create table testforboolean_u(a boolean, b boolean) with(storage_type=ustore);

---create segemnt table
create table testforboolean_s(a boolean,b boolean) with(segment=on);

---create table partition
create table testforint2_p1(a int2,b int2) partition by range(a)(partition p1 values less than(0),partition p2 values less than(2),partition p3 values less than(3));
create table testforint2_p2(a int2,b int2) with (orientation=column) partition by range(a)(partition p1 values less than(0),partition p2 values less than(2),partition p3 values less than(3));
create table testforint2_p3(a int2,b int2,c date) partition by range(c) interval('1day')(partition p1 values less than('2022-08-01'),partition p2 values less than('2022-08-02'),partition p3 values less than('2022-08-03'));
create table testforint2_p4(a int2,b int2) partition by list(a)(partition p1 values(-1),partition p2 values(1),partition p3 values(2));
create table testforint2_p5(a int2,b int2) partition by hash(a)(partition p1,partition p2,partition p3);

---insert data
insert into testforboolean values(true,true);
insert into testforboolean values(true,false);
insert into testforboolean values(false,true);
insert into testforboolean values(false,false);
insert into testforboolean values(true,null);
insert into testforboolean values(false,null);
insert into testforboolean values(null,null);

insert into testforint values(1001,1001);
insert into testforint values(1001,-1010);
insert into testforint values(1001,0110);
insert into testforint values(1001,0);
insert into testforint values(0,1001);
insert into testforint values(0,0);
insert into testforint values(1001,null);
insert into testforint values(0,null);
insert into testforint values(null,null);

insert into testforfloat values(1.27,1.27);
insert into testforfloat values(1.27,-1.27);
insert into testforfloat values(1.27,0.01);
insert into testforfloat values(1.27,0.0);
insert into testforfloat values(0.0,1.27);
insert into testforfloat values(0.0,0.0);
insert into testforfloat values(1.27,null);
insert into testforfloat values(0.0,null);
insert into testforfloat values(null,null);

insert into testforbit values('1111','1111');
insert into testforbit values('1111','0111');
insert into testforbit values('1111','1011');
insert into testforbit values('1111','0000');
insert into testforbit values('0000','1111');
insert into testforbit values('0000','0000');
insert into testforbit values('1111',null);
insert into testforbit values('0000',null);
insert into testforbit values(null,null);

insert into testforboolean_u values(true,true);
insert into testforboolean_u values(true,false);
insert into testforboolean_u values(false,true);
insert into testforboolean_u values(false,false);
insert into testforboolean_u values(true,null);
insert into testforboolean_u values(false,null);
insert into testforboolean_u values(null,null);

insert into testforboolean_s values(true,true);
insert into testforboolean_s values(true,false);
insert into testforboolean_s values(false,true);
insert into testforboolean_s values(false,false);
insert into testforboolean_s values(true,null);
insert into testforboolean_s values(false,null);
insert into testforboolean_s values(null,null);

insert into testforint2_p1 values(-1,-1);
insert into testforint2_p1 values(1,0);
insert into testforint2_p1 values(2,123);

insert into testforint2_p2 values(-1,-1);
insert into testforint2_p2 values(1,0);
insert into testforint2_p2 values(2,123);

insert into testforint2_p3 values(-1,-1,'2022-07-31');
insert into testforint2_p3 values(1,0,'2022-08-01');
insert into testforint2_p3 values(2,123,'2022-08-02');

insert into testforint2_p4 values(-1,-1);
insert into testforint2_p4 values(1,0);
insert into testforint2_p4 values(2,123);

insert into testforint2_p5 values(-1,-1);
insert into testforint2_p5 values(1,0);
insert into testforint2_p5 values(2,123);


select a&&b from testforboolean;
select a||b from testforboolean;

select a&&b from testforint;
select a||b from testforint;

select a&&b from testforfloat;
select a||b from testforfloat;

select a&&b from testforbit;
select a||b from testforbit;

select a&&b from testforboolean_u;
select a||b from testforboolean_u;

select a&&b from testforboolean_s;
select a||b from testforboolean_s;

select a&&b from testforint2_p1;
select a||b from testforint2_p1;

select a&&b from testforint2_p2;
select a||b from testforint2_p2;

select a&&b from testforint2_p3;
select a||b from testforint2_p3;

select a&&b from testforint2_p4;
select a||b from testforint2_p4;

select a&&b from testforint2_p5;
select a||b from testforint2_p5;

---create view
create view testforboolean_v as select * from testforboolean;
select a&&b from testforboolean_v;
select a||b from testforboolean_v;
drop view testforboolean_v;

--- test for function
select count(tem) from (select a&&b tem from testforboolean);
select a&&b,b from testforboolean order by b;

select char_length('asbjhc')&&char_length('askjdhkj');
select left('1023jasdzlxc',5)&&left('1023jasdnzxc',5);
select substring('as1dz34lcas',3)&&substring('zxcbkj1shd',5);
select replace('123456789','234','asd')&&replace('123456789','234','asd');

select count(tem) from (select a||b tem from testforboolean);
select a||b,b from testforboolean order by b;

select char_length('asbjhc')||char_length('askjdhkj');
select left('1023jasdzlxc',5)||left('1023jasdnzxc',5);
select substring('as1dz34lcas',3)||substring('zxcbkj1shd',5);
select replace('123456789','234','asd')||replace('123456789','234','asd');

--- test for arithmetic op
select 1+1 && 1;
select 1+1 && 0;
select 1+1 || 1;
select 1+1 || 0;

select 1-1 && 1;
select 0-1 && 1;
select 1-1 || 1;
select 0-1 || 1;

select 1*2 && 1;
select 0*3 && 1;
select 1*2 || 1;
select 0*3 || 1;

select cast(4/2 as int) && 1;
select cast(0/3 as int) && 1;
select cast(4/2 as int) || 1;
select cast(0/3 as int) || 1;

select 6%3 && 1;
select 6%4 && 1;
select 6%3 || 1;
select 6%4 || 1;

--- test for comparison op
select 1<1 && 1;
select 0<1 && 1;
select 1<1 || 1;
select 0<1 || 1;

select 1>1 && 1;
select 1>0 && 1;
select 1>1 || 1;
select 1>0 || 1;

select 1=1 && 1;
select 0=1 && 1;
select 1=1 || 1;
select 0=1 || 1;

select 1!=1 && 1;
select 1!=0 && 1;
select 1!=1 || 1;
select 1!=0 || 1;

select 1<>1 && 1;
select 1<>0 && 1;
select 1<>1 || 1;
select 1<>0 || 1;

select 1>=1 && 1;
select 1>=2 && 1;
select 1>=1 || 1;
select 1>=2 || 1;

select 1<=1 && 1;
select 1<=0 && 1;
select 1<=1 || 1;
select 1<=0 || 1;

---create function
create function test (boolean,boolean) returns boolean
    as 'select $1&&$2;'
    language sql
    returns null on null input;
select test(true,true);
select test(true,false);
drop function test;

create function test (boolean,boolean) returns boolean
    as 'select $1||$2;'
    language sql
    returns null on null input;
select test(true,true);
select test(true,false);
drop function test;

---create procedure
create procedure test1(in a boolean,in b boolean)
as
begin
   raise notice '%', a&&b;
end;
/
call test1(true,true);
call test1(true,false);
call test1(false,null);

drop procedure test1;

create procedure test1(in a boolean,in b boolean)
as
begin
   raise notice '%', a||b;
end;
/
call test1(true,true);
call test1(true,false);
call test1(false,null);

drop procedure test1;

---drop table
drop table testforboolean;
drop table testforint;
drop table testforfloat;
drop table testforbit;
drop table testforboolean_u;
drop table testforboolean_s;
drop table testforint2_p1;
drop table testforint2_p2;
drop table testforint2_p3;
drop table testforint2_p4;
drop table testforint2_p5;

---drop database
set dolphin.b_compatibility_mode = 0;
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat';
\c postgres
drop database test_op_and;


drop database if exists test_op_xor;
CREATE DATABASE test_op_xor with dbcompatibility='B';
\c test_op_xor
set dolphin.b_compatibility_mode = 1;

select null^1;

create table testforbit(a bit,b bit);
insert into testforbit values('1','1');
insert into testforbit values('1','0');
select a^b from testforbit where (select 0^1);
drop table testforbit;


create table testforint2(a int2,b int2);
insert into testforint2 values(1001,1001);
insert into testforint2 values(1001,-1010);
insert into testforint2 values(1001,0110);
select a^b from testforint2 where (select 0^1);
drop table testforint2;


create table testforint4(a int4,b int4);
insert into testforint4 values(1001,1001);
insert into testforint4 values(1001,-1010);
insert into testforint4 values(1001,0110);
select a^b from testforint4 where (select 0^1);
drop table testforint4;


create table testforint8(a int8,b int8);
insert into testforint8 values(1001,1001);
insert into testforint8 values(1001,-1010);
insert into testforint8 values(1001,0110);
select a^b from testforint8 where (select 0^1);
drop table testforint8;


create table testforfloat(a float,b float);
insert into testforfloat values(1.27,1.27);
insert into testforfloat values(1.27,0.12);
insert into testforfloat values(1.27,0.01);
select a^b from testforfloat;
drop table testforfloat;


create table testforstring(a varchar,b varchar);
insert into testforstring values('10','10');
insert into testforstring values('-10','-01');
insert into testforstring values('+10','+01');
insert into testforstring values('123a','123a456');
select a^b from testforstring where (select 0^1);
drop table testforstring;


create table testforboolean(a boolean,b boolean);
insert into testforboolean values(true,true);
insert into testforboolean values(true,false);
insert into testforboolean values(false,false);
select a^b from testforboolean where (select 0^1);
drop table testforboolean;


create table testfordate(a date,b date);
insert into testfordate values('2011-08-13','2011-08-13');
insert into testfordate values('2022-08-13','2022-08-14');
select a^b from testfordate;
drop table testfordate;



create table testforbit(a bit,b bit) with (orientation=column);
insert into testforbit values('1','1');
insert into testforbit values('1','0');
select a^b from testforbit;


create local temp table testforint2(a int2,b int2);
insert into testforint2 values(1001,1001);
insert into testforint2 values(1001,-1010);
insert into testforint2 values(1001,0110);
select a^b from testforint2;


create global temp table testforint4(a int4,b int4);
insert into testforint4 values(1001,1001);
insert into testforint4 values(1001,-1010);
insert into testforint4 values(1001,0110);
select a^b from testforint4;


create unlogged table testforint8(a int8,b int8);
insert into testforint8 values(1001,1001);
insert into testforint8 values(1001,-1010);
insert into testforint8 values(1001,0110);
select a^b from testforint8;

create table testforbit_u(a bit, b bit) with(storage_type=ustore);
insert into testforbit_u values(b'1',b'1');
insert into testforbit_u values(b'1',b'0');
select a^b from testforbit_u;


create table testforbit_s(a bit,b bit) with(segment=on);
insert into testforbit_s values(b'1',b'1');
insert into testforbit_s values(b'1',b'0');
select a^b from testforbit_s;


create table testforint2_p1(a int2,b int2) partition by range(a)(partition p1 values less than(0),partition p2 values less than(2),partition p3 values less than(3));
insert into testforint2_p1 values(-1,-1);
insert into testforint2_p1 values(1,0);
insert into testforint2_p1 values(2,123);
select a^b from testforint2_p1;

create table testforint2_p2(a int2,b int2) with (orientation=column) partition by range(a)(partition p1 values less than(0),partition p2 values less than(2),partition p3 values less than(3));
insert into testforint2_p2 values(-1,-1);
insert into testforint2_p2 values(1,0);
insert into testforint2_p2 values(2,123);
select a^b from testforint2_p2;

create table testforint2_p3(a int2,b int2,c date) partition by range(c) interval('1day')(partition p1 values less than('2022-08-01'),partition p2 values less than('2022-08-02'),partition p3 values less than('2022-08-03'));
insert into testforint2_p3 values(-1,-1,'2022-07-31');
insert into testforint2_p3 values(1,0,'2022-08-01');
insert into testforint2_p3 values(2,123,'2022-08-02');
select a^b from testforint2_p3;

create table testforint2_p4(a int2,b int2) partition by list(a)(partition p1 values(-1),partition p2 values(1),partition p3 values(2));
insert into testforint2_p4 values(-1,-1);
insert into testforint2_p4 values(1,0);
insert into testforint2_p4 values(2,123);
select a^b from testforint2_p4;

create table testforint2_p5(a int2,b int2) partition by hash(a)(partition p1,partition p2,partition p3);
insert into testforint2_p5 values(-1,-1);
insert into testforint2_p5 values(1,0);
insert into testforint2_p5 values(2,123);
select a^b from testforint2_p5;


create view testforbit_v as select * from testforbit;
select a^b from testforbit_v;
drop view testforbit_v;


create function test (bit(4),bit(4)) returns bit(4)
    as 'select $1^$2;'
    language sql
    returns null on null input;
select test(b'1001',b'1010');
drop function test;


create procedure test(in a bit,in b bit)
as
begin
   raise notice '%', a^b;
end;
/
call test(b'1',b'1');
drop procedure test;


select abs(1)^abs(1);
select abs(1^1);


---正常报错：^不能作为表名
create table ^;
---正常报错：^不能作为表的列名
create table test(^ int);
---正常报错：^不能作为函数名
create function ^;
drop function if exists ^;
---正常报错：^不能作为存储过程名
create procedure ^;
---正常报错：^不能作为视图名
create view ^;
---正常报错：^不能作为数据库名
create database ^;
---正常报错：^不能作为目录名
create index ^;
---正常报错：^不能作为序列名
create sequence ^ increment by 1 minvalue 1 no maxvalue start with 1;
---正常报错：^不能作为用户名
create user ^ identified by 'hw123456';
---正常报错：^不能作为角色名
create role ^ identified by 'hw123456';


select * from testforbit;
select max(tem) from (select a^b tem from testforbit);
select sum(tem) from (select a^b tem from testforbit);
select min(tem) from (select a^b tem from testforbit);
select avg(tem) from (select a^b tem from testforbit);
select count(tem) from (select a^b tem from testforbit);

update testforbit set a=b'1'^b'1' where b=0;
insert into testforbit values(b'1'^b'1',b'0'^b'1');
delete testforbit where b=b'1'^b'1';

select a^b,b from testforbit order by b;
select a^b from testforbit group by a,b having a>0;
select A1.a^A1.b,A2.a^A2.b from testforbit A1 join testforint2 A2 on A1.b>0 and A2.b>0;
select a^b,
   (case b when b>0 then 'good'
    when b<=0 then 'bad' end) as quality
from testforbit;

select a^b from testforbit where b%a^1=1;
select a^b from testforbit where a/b^1<1;
select a^b from testforbit where a*b^1!=1;
select a^b from testforbit where a+b^1>=1;
select a^b from testforbit where b^1.0^1<=1;

select char_length('asbjhc')^char_length('askjdhkj');
select left('1023jasdzlxc',5)^left('1023jasdnzxc',5);
select substring('as1dz34lcas',3)^substring('zxcbkj1shd',5);
select replace('123456789','234','asd')^replace('123456789','234','asd');




---drop table
drop table testforbit;
drop table testforint2;
drop table testforint4;
drop table testforint8;
drop table testforbit_s;
drop table testforbit_u;
drop table testforint2_p1;
drop table testforint2_p2;
drop table testforint2_p3;
drop table testforint2_p4;
drop table testforint2_p5;

---drop database
set dolphin.b_compatibility_mode = 0;
\c postgres
drop database test_op_xor;


drop database if exists like_test;
create database like_test DBCOMPATIBILITY 'b';
\c like_test
set dolphin.b_compatibility_mode = 1;

select 'a' like 'A';
---正常报错，like右边缺参数
select 'a' like; 
---正常报错，like右边参数过多
select 'a' like 'A' 'a';



select 100 like 100;
select -100 like 100;
select -100 like -100;

select cast(1.252 as float) like cast(1.252 as float);
select cast(1.252 as float) like cast(1.253 as float);

select cast('2002-5-16' as date) like cast('2002-5-16' as date);
select cast('2002-5-16' as date) like cast('2002-5-17' as date);

select 'abc' like 'ABC';
select '@Af%' like '@aF%';
select 'abc' like binary 'ABC';
select '@Af%' like binary '@aF%';
select 'abc' like 'ab\%' escape '\' ;
select 'abc' like binary 'ab\%' escape '\' ;

select true like true;
select true like 'a';
select 'a' like true;
select 'a' not like true;
select true not like true;
select true not like 'a';




select char_length('asbjhc') like char_length('askjdhkj');
select left('1023jasdzlxc',5) like left('1023jasdnzxc',5);
select substring('as1dz34lcas',3) like substring('zxcbkj1shd',5);
select replace('123456789','234','asd') like replace('123456789','234','asd');

---正常报错，表名不能为like
create table like (id int);
---正常报错，like不能为列名
create table test1 (like char(10));
drop table if exists like;
drop table if exists test1;

create function test (varchar,varchar) returns boolean
    as 'select $1 like $2;'
    language sql
    returns null on null input;
select test('ab','ab');
drop function test;

---正常报错，like不能为序列
create sequence like increment by 1 minvalue 1 no maxvalue start with 1;
---正常报错，like不能为数据库名
create database like;
---正常报错，like不能为用户名
create user like identified by 'hw123456';
---正常报错，like不能为角色名
create role like identified by 'hw123456';
create table hotel (id int,name char(10),cin date,cout date,hotel char(10),room int);
insert into hotel values
(1,'Alice','2022-5-7','2022-5-8','Vienna',5),
(2,'Anna','2022-7-9','2022-7-12','Vienna',2),
(3,'Bob','2022-6-17','2022-6-25','Holiday',1),
(4,'Claris','2022-5-9','2022-6-1','Holiday',1),
(5,'Band','2022-3-6','2022-3-9','Vienna',3);
---正常报错，like不能为索引名
create index like on hotel(id);
---正常报错，like不能为视图名
create view like as select * from hotel;

select count(cout like '2022%') from hotel group by hotel ;
select max(cout like '2022%') from hotel group by hotel ;
select min(cout like '2021%') from hotel group by hotel ;
select avg(cout like '2022%') from hotel group by hotel ;
select sum(cout like '2022%') from hotel group by hotel ;
select count(cout like binary '2022%') from hotel group by hotel ;
select max(cout like binary '2022%') from hotel group by hotel ;
select min(cout like binary '2021%') from hotel group by hotel ;
select avg(cout like binary '2022%') from hotel group by hotel ;
select sum(cout like binary '2022%') from hotel group by hotel ;

select * from hotel where name like 'a%';
update hotel set hotel='Holiday' where cin like '2022-3%';
create table hotel2 (id int,name char(10),cin date,cout date,hotel char(10),room int);
insert into hotel2 select * from hotel where hotel like 'vienna%';

select max(cout) from hotel group by hotel having hotel like 'v%';
create table price(hotelname char(10),price int);
insert into price values
('Vienna',500),
('Holiday',700);
select * from hotel natural inner join price where name like 'b%';
select * from hotel natural inner join price where name like binary 'b%';
select * from hotel natural inner join price where name like 'b/%' escape '/';
select * from hotel natural inner join price where name like binary 'b/%' escape '/';
select * from hotel natural inner join price where name not like 'b%';
select * from hotel natural inner join price where name not like binary 'b%';
select * from hotel natural inner join price where name not like 'b/%' escape '/';
select * from hotel natural inner join price where name not like binary 'b/%' escape '/';



drop table if exists hotel;
drop table if exists hotel2;
drop table if exists price;
create table hotel (id int,name char(10),cin date,cout date,hotel char(10),room int) with (orientation = column);
insert into hotel values
(1,'Alice','2022-5-7','2022-5-8','Vienna',5),
(2,'Anna','2022-7-9','2022-7-12','Vienna',2),
(3,'Bob','2022-6-17','2022-6-25','Holiday',1),
(4,'Claris','2022-5-9','2022-6-1','Holiday',1),
(5,'Band','2022-3-6','2022-3-9','Vienna',3);


select count(cout like '2022%') from hotel group by hotel ;
select max(cout like '2022%') from hotel group by hotel ;
select min(cout like '2021%') from hotel group by hotel ;
select avg(cout like '2022%') from hotel group by hotel ;
select sum(cout like '2022%') from hotel group by hotel ;
select count(cout like binary '2022%') from hotel group by hotel ;
select max(cout like binary '2022%') from hotel group by hotel ;
select min(cout like binary '2021%') from hotel group by hotel ;
select avg(cout like binary '2022%') from hotel group by hotel ;
select sum(cout like binary '2022%') from hotel group by hotel ;

select * from hotel where name like 'a%';
update hotel set hotel='Holiday' where cin like '2022-3%';
create table hotel2 (id int,name char(10),cin date,cout date,hotel char(10),room int) with (orientation = column);
insert into hotel2 select * from hotel where hotel like 'vienna%';

select max(cout) from hotel group by hotel having hotel like 'v%';

create table price(hotelname char(10),price int) with (orientation = column);
insert into price values
('Vienna',500),
('Holiday',700);
select * from hotel natural inner join price where name like 'b%';
select * from hotel natural inner join price where name like binary 'b%';
select * from hotel natural inner join price where name like 'b/%' escape '/';
select * from hotel natural inner join price where name like binary 'b/%' escape '/';
select * from hotel natural inner join price where name not like 'b%';
select * from hotel natural inner join price where name not like binary 'b%';
select * from hotel natural inner join price where name not like 'b/%' escape '/';
select * from hotel natural inner join price where name not like binary 'b/%' escape '/';


drop table if exists hotel;
drop table if exists hotel2;
drop table if exists price;

create unlogged table testlike1 (a char(10), b char(10));
create table testlike2 (a char(10), b char(10)) with(storage_type=ustore);
create table testlike3 (a char(10),b char(10)) with(segment=on);

insert into testlike1 values ('a','A');
select * from testlike1 where a like 'A%';
select * from testlike1 where a like binary 'A%';

insert into testlike2 values ('a','A');
select * from testlike2 where a like 'A%';
select * from testlike2 where a like binary 'A%';

insert into testlike3 values ('A','a');
select * from testlike3 where a like 'A%';
select * from testlike3 where a like binary 'A%';

create table testlike4(a char(10),b char(10)) partition by hash(a)(partition p1,partition p2,partition p3);
insert into testlike4 values ('A','a');
select * from testlike4 where a like 'A%';
select * from testlike4 where a like binary 'A%';

create table testlike5 (a int,b char(10)) partition by range(a)(partition p1 values less than(0),partition p2 values less than(2),partition p3 values less than(3));
insert into testlike5 values (1,'a');
select * from testlike5 where b like 'A%';
select * from testlike5 where b like binary 'A%';

create table testlike6(a int,b char(10)) partition by list(a)(partition p1 values(-1),partition p2 values(1),partition p3 values(2));
insert into testlike6 values (1,'a');
select * from testlike6 where b like 'A%';
select * from testlike6 where b like binary 'A%';

create table testlike7(a int,b char(10)) with (orientation=column) partition by range(a)(partition p1 values less than(0),partition p2 values less than(2),partition p3 values less than(3));
insert into testlike7 values (1,'a');
select * from testlike7 where b like 'A%';
select * from testlike7 where b like binary 'A%';

create table testlike8(a char(10),b date) partition by range(b) interval('1day')(partition p1 values less than('2022-08-01'),partition p2 values less than('2022-08-02'),partition p3 values less than('2022-08-03'));
insert into testlike8 values ('a','2022-8-01');
select * from testlike8 where a like 'A%';
select * from testlike8 where a like binary 'A%';

select b'1010' like b'0010';
select b'1010' like b'0011';
select b'1010' like b'1100';
select '1010'::bit(4) like '0010'::bit(4);
select '1010'::bit(4) like '0011'::bit(4);
select '1010'::bit(4) like '1100'::bit(4);

select b'1010' not like b'0010';
select b'1010' not like b'0011';
select b'1010' not like b'1100';
select '1010'::bit(4) not like '0010'::bit(4);
select '1010'::bit(4) not like '0011'::bit(4);
select '1010'::bit(4) not like '1100'::bit(4);

select 'deadbeef'::blob like 'deadbeef';
select 'deadbeef'::blob like 'DEADBEEF'::blob;
select 'DEADBEEF'::blob like 'DEADBEEF'::blob;
select 'deadbeef'::blob like null;
select '1010'::blob like '1100';
select '1010'::blob like '1100'::blob;
select '1010'::blob like null;

select 'deadbeef'::blob not like 'deadbeef';
select 'deadbeef'::blob not like 'DEADBEEF'::blob;
select 'DEADBEEF'::blob not like 'DEADBEEF'::blob;
select 'deadbeef'::blob not like null;
select '1010'::blob not like '1100';
select '1010'::blob not like '1100'::blob;
select '1010'::blob not like null;

drop table testlike1;
drop table testlike2;
drop table testlike3;
drop table testlike4;
drop table testlike5;
drop table testlike6;
drop table testlike7;
drop table testlike8;

-- test for '!' in place of NOT for expr
set dolphin.b_compatibility_mode = 0;
select !0;
select !!0;
select !!10;
select !!!0;
set dolphin.b_compatibility_mode = 1;
select !0;
select !9;
select !!0;
select !!9;
select !!!0;
select !(0);
select !(!(0));
select !(!(!(0)));
select !(10.345::numeric);
select !(10.345::float4);
select !(10.345::float8);
select !('2012-01-01'::date);
select !('23:59:59'::time);
select !b'1001';
select 10!;

set dolphin.b_compatibility_mode = 0;
select !10;
select !!10;
select 10!;

\c postgres
drop database if exists like_test;
