drop database if exists m_create_proc_type;
create database m_create_proc_type dbcompatibility 'B';
\c m_create_proc_type

--test create procedure 


delimiter //

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



\c postgres

drop database m_create_proc_type;

