create schema db_func_call1;
set current_schema to 'db_func_call1';


CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  CONTAINS SQL AS $$ select 1 $$ ;

call f1('a');
drop function f1; 

CREATE FUNCTION f1 () RETURNS int  no SQL AS $$ select 1 $$ ;

call f1;

call f1; 

drop function f1;

create table tb_object0015(data int,name varchar(25) ,"deptId" int,salary float);
create table tb_object0015_01(data int,name varchar(25) ,"deptId" int,salary float);

CREATE OR REPLACE FUNCTION tri_1() RETURNS TRIGGER AS
$$
DECLARE
BEGIN
INSERT
INTO tb_object0015_01
VALUES (NEW.data, U&"\006e\0065\0077".name, U&"\006e\0065\0077"."deptId", 2 * NEW.salary);
RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER tr_object0015_01
AFTER INSERT ON tb_object0015
FOR EACH ROW
EXECUTE PROCEDURE tri_1();

insert into tb_object0015 values(1,'1000',1,1.1),(2,'2000',2,2.2);
select * from tb_object0015 order by data;
select * from tb_object0015_01 order by data;

CREATE OR REPLACE FUNCTION f_2() RETURNS void AS
$$
DECLARE
BEGIN
insert into tb_object0015(U&"d\0041t\+000061",salary) values(3,3.3);
END;
$$ LANGUAGE plpgsql;
call f_2();
select * from tb_object0015 order by data;
select * from tb_object0015_01 order by data;

CREATE OR REPLACE FUNCTION f_3() RETURNS nchar AS
$$
DECLARE a nchar;
BEGIN
select into a n'A';
return a;
END;
$$ LANGUAGE plpgsql;
call f_3();

drop schema db_func_call1 cascade;
reset current_schema;

