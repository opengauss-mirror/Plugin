create schema db_do_stmt;
set current_schema to 'db_do_stmt';

create table t1 (a int);
insert into t1 values(1),(4),(7);
select a from t1;

--simple expr;
do 1;
do 1+1;
do 1+3;
do 2*4;


--multi expr
do 1,1,1,1;
do 1+1,2,3+3;
do 1+3,6;
do 2*4,9;

-- function
do sin(1);
do pg_sleep(1);
do exp(x);

-- function multi
do sin(1),exp(1);
do pg_sleep(1),sin(1);

--error stmt
do sin(1) limit 1;

--origin do stmt
DO $$
BEGIN
  perform  2,3,4;
END;
$$
;

DO $$
BEGIN
  perform  2;
END;
$$
LANGUAGE SQL;

-- error when has table
do sin(a) from t1;

-- bug fix do in block

create table time_table4(id int,c1 char(8),c3 varchar(16));
insert into time_table4 values(1,'xx','xx');
insert into time_table4 values(2,'yy','yy');
insert into time_table4 values(3,'zz','zz');
insert into time_table4 values(4,'ss','ss');

CREATE OR REPLACE PROCEDURE pro_do_(n int,a1 in char(8),a2 in int,a3 in varchar(16))
AS
BEGIN
for i in 1..n loop
a3:=a3||i;
insert into time_table4 values(4+i,a1,a3);
end loop;
select count(*) into a2 from time_table4;
END;
/


--直接调用，值为常量
CREATE OR REPLACE PROCEDURE pro2_do()
as
begin
do pro_do_(3,'dd',5,'ll');
END;
/
call pro2_do();
select * from time_table4 order by id;

CREATE function func_do()return void
as
begin
do pro_do_(3,'dd',5,'ll');
return;
END;
/

call func_do();
select * from time_table4 order by id;
drop table time_table4;

create table time_table4(id int,c1 char(8),c3 varchar(16));
insert into time_table4 values(1,'xx','xx');
insert into time_table4 values(2,'yy','yy');
insert into time_table4 values(3,'zz','zz');
insert into time_table4 values(4,'ss','ss');

--自定义模式
create schema call_schema;

--创建存储过程
CREATE OR REPLACE PROCEDURE call_schema.pro_do_(n int,a1 in char(8),a2 in int,a3 in varchar(16))
AS
BEGIN
for i in 1..n loop
a3:=a3||i;
insert into time_table4 values(4+i,a1,a3);
end loop;
select count(*) into a2 from time_table4;
END;
/
 

select * from time_table4 order by id;
--将表的模式改为call_schema
alter table time_table4 set schema call_schema;
declare
b1 char(8);
b2 int;
b3 varchar(16);
begin
b1:='oo';
b2:=3;
b3:='pp';
do call_schema.pro_do_(2,b1,b2,b3);
raise notice '%',b1;
raise notice '%',b2;
raise notice '%',b3;
end;
/

select * from call_schema.time_table4 order by id;


drop schema db_do_stmt cascade;
reset current_schema;
