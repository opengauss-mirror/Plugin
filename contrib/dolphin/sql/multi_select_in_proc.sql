create schema multi_select_proc;
set current_schema to 'multi_select_proc';

--open parameter sql_mode
set dolphin.sql_mode = 'block_return_multi_results';

create table test_1(a int,b int);
create table t (a int);
insert into test_1 values(1,2),(3,4);
insert into t values(123),(789);

--one select
CREATE   PROCEDURE proc_a_1 ()  as 
begin
	select * from t;
end;
/

call proc_a_1();


-- two select
CREATE   PROCEDURE proc_a_2 ()  as 
begin
	select * from t;
	select * from test_1;
end;
/

call proc_a_2();

-- with input params 
CREATE   PROCEDURE proc_a_3 (aa int)  as 
begin
	select aa from t;
	select * from test_1;
end;
/

call proc_a_3(1);

-- with while and out param
CREATE   PROCEDURE proc_b_1 (aa int, out re1  int,out re2  int)  as 
declare i int default 1;
begin
   re1 = aa +100;
   re2 = aa + 1000;
   while i<=2 do
    i := i+1;
	select aa + 1,a from test_1;
	end while;
	select * from t;
end;
/

--user var 
call proc_b_1(1,@a,@b);

set enable_set_variable_b_format = 1;

call proc_b_1(1,@a,@b);

select @a;

select @b;


CREATE   PROCEDURE proc_b_2 (aa int, out re1  int,out re2  int)  as 
declare i int default 1;
begin
   re1 = aa +100;
   re2 = aa + 1000;
   while i<=2 do
    i := i+1;
	insert into test_1 values(6,7);
	end while;
	select * from test_1;
	
end;
/

call proc_b_2 (102,@c,@d);

--check params
select @c;

select @d;

set enable_set_variable_b_format = 0;

--half wrong

create table tab_1145173(id int,pid int,a1 char(8));
create table a_1145173(id int,a1 char(8));
create table b_1145173(id int,a1 char(8));
--insert;
insert into tab_1145173 values(1,2,'s'),(2,3,'b'),(3,4,'c'),(4,5,'d');
insert into a_1145173 values(1,'s'),(2,'b');
insert into b_1145173 values(2,'s'),(3,'b');

create or replace procedure pro_1145173()
as
begin
select * from a_1145173 union select * from b_1145173 order by id;
select * from tab_1145173;
select tt_114;
end;
/

call pro_1145173();

create table tab_1144052(id int,pid int,a1 char(8));
--insert;
insert into tab_1144052 values(1,2,'s'),(2,3,'b'),(3,4,'c'),(4,5,'d');

--proc;
create or replace procedure pro_1144052()
as
begin
with temp_1144052(a1,a2) as (select id,a1 from tab_1144052 where id > 1) select * from temp_1144052;
select * from tab_1144052 start with pid = 4 connect by prior id = pid order by a1;
select avg(id),a1 from tab_1144052 group by a1 having avg(id) > 1;
end;
/
--func;
create or replace function fun_1144052()return int
as
b int;
begin
select count(*) into b from tab_1144052;
return b;
end;
/

call pro_1144052();

call fun_1144052();

select fun_1144052();

select pro_1144052();

-- mysql format 
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,block_return_multi_results';

delimiter //

CREATE   PROCEDURE proc_a_m ()   
begin
	select * from t;
end
//

delimiter ;

call proc_a_m();

create table testtyp (a int8, b varchar ,c date,d bytea);
insert into testtyp values(123,'abv','2020-01-01','a');
insert into testtyp values(1123,'abcv','2022-01-01','c');
insert into testtyp values(NULL,NULL,NULL,NULL);

delimiter //

CREATE   PROCEDURE proc_a_m1 ()   
begin
	select a,b,c,d from testtyp;

	select a,c,d,b from testtyp;
end
//

delimiter ;

call proc_a_m1();

create or replace procedure pro_11451713()
as
begin

end;
/

call pro_11451713();

--bug fix in call in call procedure 
set dolphin.sql_mode=block_return_multi_results;

create table tab_1145533(id int,pid int,a1 char(8));
--insert;
insert into tab_1145533 values(1,2,'s'),(2,3,'b'),(3,4,'c'),(4,5,'d');
--create proc;
create or replace procedure pro_1145533()
as
begin
select * from tab_1145533 order by id;
end;
/
create or replace procedure pro1_1145533(n in int)
as
begin
if n>3 then
call pro_1145533();
else
select * from tab_1145533 where id >2;
end if;
end;
/
create or replace procedure pro2_1145533(n in int)
as
begin
call pro1_1145533(n);
end;
/
call pro2_1145533(2);
call pro2_1145533(5);

--fill up default value in the end 
create procedure proc_def_1(a int , b int = 1) as
begin
select b,a ;
end;
/
call proc_def_1(2);

set enable_set_variable_b_format = 1;

create procedure proc_def_2(a out int , b int = 1) as
begin
a = 1234 + b;
select b,a ;
end;
/

set @out = 123;

call proc_def_2(@out);

call proc_def_2(@out,11);

set  dolphin.sql_mode=default;

drop schema multi_select_proc cascade;
reset current_schema;