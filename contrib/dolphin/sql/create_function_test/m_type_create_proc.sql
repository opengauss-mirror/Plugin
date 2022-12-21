create schema m_create_proc_type;
set current_schema to 'm_create_proc_type';

--test create procedure 
delimiter //

CREATE PROCEDURE p()
  BEGIN
  DECLARE v INT DEFAULT 1;
  CASE v
  WHEN 2 THEN SELECT v;
  WHEN 3 THEN SELECT 0;
  ELSE
  BEGIN
  END;
  END CASE;
END;
//

CREATE PROCEDURE p()
  BEGIN
  END;
//

CREATE PROCEDURE proc1 ()
       BEGIN
         SELECT 1;
       END;
//
delimiter ;

-- test proc call

create table item(c1 int, c2 int);
create procedure procedure_1(a int, b int)
as
begin
   insert into item values(a, b);
end;
/

call procedure_1(1,2);


delimiter //

CREATE PROCEDURE procedure_1_m (c1 int, c2 int)
       BEGIN
         insert into item values(c1, c2);
       END;
//
delimiter ;

call procedure_1_m(6,9);

select * from item;

select procedure_1_m(6,9);

select * from procedure_1_m(6,9);


select * from item;


-- test declare

create table test (a int);
CREATE PROCEDURE testpro(a int) as
declare
b int;
begin
b  := 5;
insert into test values (a + b);
end;
/

call testpro (2);

--error
delimiter //
CREATE PROCEDURE testpro_m() 
declare
b int;
begin
b  := 5;
insert into test values (a + b);
end;
//

delimiter ;

--success
delimiter //
CREATE PROCEDURE testpro_m(a int)
begin
declare 
b int;
b  := 5;
insert into test values (a + b);
end;
//

delimiter ;



call testpro_m (5);

select * from test;

--testmulti declare

 delimiter //
 CREATE PROCEDURE testpro_m6(a int)
 begin
 declare b int;
 declare c int;
 b:= 9;
 insert into test values (a + b);
    begin 
    declare d int;
    insert into test values (a + b);
    end;
 end;
 //

 delimiter ;




 delimiter //
 CREATE PROCEDURE testpro_m7(a int)
 begin
 declare b int;
 declare c int;
 b := 9;
 insert into test values (a + b);
    begin 
    declare d int;
    insert into test values (a + b);
          begin
         declare e int;
         insert into test values (a + b);
         end;
         end;
 end;
 //

delimiter ;

call testpro_m6(10);

call testpro_m7(11);

select * from test;


-- test options 


delimiter //

CREATE PROCEDURE procedure_1_m_o (c1 int, c2 int) LANGUAGE SQL
       BEGIN
         insert into item values(88,88);
       END;
//
delimiter ;


delimiter //

CREATE PROCEDURE procedure_2_m_o() NO SQL READS SQL DATA LANGUAGE SQL
       BEGIN
         insert into item values(99, 99);
       END;
//
delimiter ;


-- issue fix 

-- test while
drop table if exists test9;
create table test9(
id int(11) unsigned not null auto_increment,
sname varchar(255) not null,
  primary key (id)
)engine=innodb default charset=utf8;

drop procedure if exists test_proc;
delimiter //
create procedure test_proc()
begin
    declare i int;
	i := 1;
    while i<=10 do
        insert into test9 values(null,rand()*10);
        i := i+1;
    end while;
end;
//
delimiter ;
call test_proc();


--test loop,if
delimiter //
create or replace procedure doiterate(p1 int)
begin
LABEL1:loop
p1 := p1+1;
if p1 < 10 then
raise notice '123';
end if;
exit LABEL1;
end loop LABEL1;
end;
//
delimiter ;
call doiterate(2);

--test repeat
delimiter //
CREATE or replace PROCEDURE dorepeat(p1 INT)
BEGIN
declare
i int = 0;
<<label>>
repeat
i = i + 1;
until i >p1 end repeat label;
raise notice '%',i;
end;
//
delimiter ;
select dorepeat(5);


-- test  case
delimiter //
CREATE or replace PROCEDURE docase(p1 INT)
BEGIN
declare
i int = p1;
<<label>>
case i
when i > 10 then raise notice '%','big';
when i < 1 then raise notice '%','small';
else  raise notice '%','good';
end case;
end;
//
delimiter ;
select docase(4);
select docase(0);
select docase(100);

--test empty 

drop procedure  if exists doempty;
delimiter //
CREATE or replace PROCEDURE doempty()
BEGIN
end;
//
delimiter ;

call doempty();





drop schema m_create_proc_type cascade;
reset current_schema;

