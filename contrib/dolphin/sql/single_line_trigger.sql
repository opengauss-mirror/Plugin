--create trigger
-- test mysql compatibility trigger 
drop database if exists db_mysql;
create database db_mysql dbcompatibility 'B';
\c db_mysql
create table t (id int);
create table t1 (id int);
create table animals (id int, name char(30));
create table food (id int, foodtype varchar(32), remark varchar(32), time_flag timestamp);

--definer test 
create definer=d_user1 trigger animal_trigger1
after insert on t
for each row
begin 
    insert into t1 values(3);
end;
/
/*
create user test with sysadmin password 'Gauss@123';
create user test1 password 'Gauss@123';

grant all on t ,t1 to test;
grant all on t ,t1 to test1;
set role test password 'Gauss@123';

create definer = test1 trigger animal_trigger1
after insert on t
for each row
begin 
    insert into t1 values(3);
end;
/
insert into t values(3);
set role d_user1 password 'Aa123456';
alter role d_user1 identified by '123456Aa' replace 'Aa123456';
insert into t values(3);
select * from t1;

reset role;
drop trigger animal_trigger1 on t;
*/
-- trigger_order{follows|precedes} && begin ... end test
create trigger animal_trigger1
after insert on animals
for each row
begin
    insert into food(id, foodtype, remark, time_flag) values (1,'ice cream', 'sdsdsdsd', now());
end;
/

create trigger animal_trigger2
after insert on animals
for each row
follows animal_trigger1
begin
    insert into food(id, foodtype, remark, time_flag) values (2,'chocolate', 'sdsdsdsd', now());
end;
/

create trigger animal_trigger3
after insert on animals
for each row
follows animal_trigger1
begin
    insert into food(id, foodtype, remark, time_flag) values (3,'cake', 'sdsdsdsd', now());
end;
/

create trigger animal_trigger4
after insert on animals
for each row
follows animal_trigger1
begin
    insert into food(id, foodtype, remark, time_flag) values (4,'sausage', 'sdsdsdsd', now());
end;
/

insert into animals (id, name) values(1,'lion');
select id, foodtype, remark from food;
delete from food;

create trigger animal_trigger5
after insert on animals
for each row
precedes animal_trigger3
begin
    insert into food(id, foodtype, remark, time_flag) values (5,'milk', 'sdsds', now());
end;
/

create trigger animal_trigger6
after insert on animals
for each row
precedes animal_trigger2
begin
    insert into food(id, foodtype, remark, time_flag) values (6,'strawberry', 'sdsds', now());
end;
/
insert into animals (id, name) values (2, 'dog');
select id, foodtype, remark from food;
delete from food;

create trigger animal_trigger7
after insert on animals
for each row
follows animal_trigger5
begin
    insert into food(id, foodtype, remark, time_flag) values (7,'jelly', 'sdsds', now());
end;
/
insert into animals (id,name) values(3,'cat');
select id, foodtype, remark from food;

-- if not exists test
create trigger animal_trigger1
after insert on animals
for each row
begin
    insert into food(id, foodtype, remark, time_flag) values (1,'ice cream', 'sdsdsdsd', now());
end;
/

create trigger if not exists animal_trigger1
after insert on animals
for each row
begin
    insert into food(id, foodtype, remark, time_flag) values (1,'ice cream', 'sdsdsdsd', now());
end;
/

drop trigger animal_trigger1 on animals;
drop trigger animal_trigger2 on animals;
drop trigger animal_trigger3 on animals;
drop trigger animal_trigger4 on animals;
drop trigger animal_trigger5 on animals;
drop trigger animal_trigger6 on animals;
drop trigger animal_trigger7 on animals;
delete food, animals;

--definer test 
create definer=d_user1 trigger animal_trigger1
after insert on t
for each row
insert into t1 values(3);
-- trigger_order{follows|precedes} && begin ... end test
create trigger animal_trigger1
after insert on animals
for each row
insert into food(id, foodtype, remark, time_flag) values (1,'ice cream', 'sdsdsdsd', now());

create trigger animal_trigger2
after insert on animals
for each row
follows animal_trigger1
insert into food(id, foodtype, remark, time_flag) values (2,'chocolate', 'sdsdsdsd', now());

create trigger animal_trigger3
after insert on animals
for each row
follows animal_trigger1
insert into food(id, foodtype, remark, time_flag) values (3,'cake', 'sdsdsdsd', now());

create trigger animal_trigger4
after insert on animals
for each row
follows animal_trigger1
insert into food(id, foodtype, remark, time_flag) values (4,'sausage', 'sdsdsdsd', now());

insert into animals (id, name) values(1,'lion');
select id, foodtype, remark from food;
delete from food;

create trigger animal_trigger5
after insert on animals
for each row
precedes animal_trigger3
insert into food(id, foodtype, remark, time_flag) values (5,'milk', 'sdsds', now());

create trigger animal_trigger6
after insert on animals
for each row
precedes animal_trigger2
insert into food(id, foodtype, remark, time_flag) values (6,'strawberry', 'sdsds', now());
insert into animals (id, name) values (2, 'dog');
select id, foodtype, remark from food;
delete from food;

create trigger animal_trigger7
after insert on animals
for each row
follows animal_trigger5
insert into food(id, foodtype, remark, time_flag) values (7,'jelly', 'sdsds', now());
insert into animals (id,name) values(3,'cat');
select id, foodtype, remark from food;

-- if not exists test
create trigger animal_trigger1
after insert on animals
for each row
insert into food(id, foodtype, remark, time_flag) values (1,'ice cream', 'sdsdsdsd', now());

create trigger if not exists animal_trigger1
after insert on animals
for each row
insert into food(id, foodtype, remark, time_flag) values (1,'ice cream', 'sdsdsdsd', now());

drop table food;
drop table animals;

set enable_set_variable_b_format = on;
set @sum = 0;
CREATE TRIGGER ins_sum BEFORE INSERT ON t FOR EACH ROW SET @sum = @sum + NEW.id;
CREATE TRIGGER ins_insert BEFORE INSERT ON t FOR EACH ROW insert into t1 values(NEW.id);
insert into t values(1);
insert into t values(2);
select @sum;
select * from t1;

drop TRIGGER ins_insert on t;
CREATE TRIGGER ins_update after INSERT ON t FOR EACH ROW update t1 set id = 333;
insert into t values(3);
select * from t1;

drop TRIGGER ins_update on t;
CREATE TRIGGER ins_delete after INSERT ON t FOR EACH ROW delete t1;
insert into t values(4);
select * from t1;

drop TRIGGER ins_delete on t;
create procedure proc(a int) as begin insert into t1 values($1+100);end;
/
CREATE TRIGGER if not exists ins_call100 after INSERT ON t FOR EACH ROW call proc(NEW.id);
insert into t values(5);
select * from t1;
select @sum;

CREATE TRIGGER if not exists ins_call100 after INSERT ON t FOR EACH ROW call proc(NEW.id);
drop table t, t1;
drop procedure proc;

reset enable_set_variable_b_format;

\c postgres
drop database db_mysql;