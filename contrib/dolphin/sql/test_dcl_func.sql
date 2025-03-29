create schema test_dcl_func;
set current_schema to 'test_dcl_func';

create table table1(a int);
insert into table1 values (1),(2);


-- save point and roll back
START TRANSACTION;
INSERT INTO table1 VALUES (3),(4);
SAVEPOINT my_savepoint;
INSERT INTO table1 VALUES (3),(4);
SAVEPOINT my_savepoint2;
ROLLBACK TO SAVEPOINT my_savepoint;
ROLLBACK TO SAVEPOINT my_savepoint2;
end;


START TRANSACTION;
INSERT INTO table1 VALUES (3),(4);
SAVEPOINT my_savepoint;
INSERT INTO table1 VALUES (3),(4);
SAVEPOINT my_savepoint;
ROLLBACK TO SAVEPOINT my_savepoint;
ROLLBACK TO SAVEPOINT my_savepoint;
end;


START TRANSACTION;
INSERT INTO table1 VALUES (3),(4);
SAVEPOINT my_savepoint;
INSERT INTO table1 VALUES (3),(4);
SAVEPOINT my_savepoint;
ROLLBACK TO SAVEPOINT my_savepoint3;
ROLLBACK TO SAVEPOINT my_savepoint3;
end;


START TRANSACTION;
INSERT INTO table1 VALUES (3),(4);
SAVEPOINT my_savepoint;
INSERT INTO table1 VALUES (3),(4);
SAVEPOINT my_savepoint;
ROLLBACK TO SAVEPOINT my_savepoint;
ROLLBACK TO SAVEPOINT my_savepoint;
ROLLBACK work to savepoint my_savepoint;
ROLLBACK to  my_savepoint;
ROLLBACK work to my_savepoint;
end;


create or replace procedure test_cursor_0
as
    a int;
    cursor c1 is SELECT * from table1;
begin
    INSERT INTO table1 VALUES (3),(4);
    SAVEPOINT my_savepoint;
    INSERT INTO table1 VALUES (3),(4);
    SAVEPOINT my_savepoint;
    ROLLBACK TO SAVEPOINT my_savepoint;
    ROLLBACK TO SAVEPOINT my_savepoint;
    ROLLBACK work to savepoint my_savepoint;
    ROLLBACK to  my_savepoint;
    ROLLBACK work to my_savepoint;
end;
/

--- close cursor
START TRANSACTION;
CURSOR cursor1 FOR SELECT * from table1;
CLOSE cursor1;
CLOSE cursor1;
end;

START TRANSACTION;
CURSOR cursor1 FOR SELECT * from table1;
commit;
CLOSE cursor1;
end;


START TRANSACTION;
CURSOR cursor1 FOR SELECT * from table1;
CLOSE cursor1;
FETCH FORWARD 2 FROM cursor1;
end;


START TRANSACTION;
CURSOR cursor1 FOR SELECT * from table1;
CLOSE ALL;
FETCH FORWARD 2 FROM cursor1;
end;


create or replace procedure test_cursor_1
as
    a int;
    cursor c1 is SELECT * from table1;
begin
    OPEN c1;
    close c1;
	fetch c1 into a;
end;
/
call test_cursor_1();


create or replace procedure test_cursor_2
as
    a int;
    cursor c1 is SELECT * from table1;
begin
    OPEN c1;
    close c1;
	open c1;
	fetch c1 into a;
	close c1;
	open c1;
	close c1;
end;
/
call test_cursor_2();


create or replace procedure test_cursor_3
as
    a int;
    cursor c1 is SELECT * from table1;
begin
    OPEN c1;
    close c1;
	close c1;
	open c1;
	fetch c1 into a;
end;
/
call test_cursor_3();


create or replace procedure test_cursor_4
as
    a int;
    cursor c1 is SELECT * from table1;
begin
    OPEN c1;
    close all;
	close c1;
	open c1;
	fetch c1 into a;
end;
/
call test_cursor_4();


-- desc
create table test1(id int, id1 int, id2 int, not_id int);
insert into test1 values (1, 2, 3);

create view v1 as select * from test1;
create MATERIALIZED view mv1 as select * from test1;
create index idx1 on test1(id);


desc test1;
desc test1 'id%';
desc test1 'id';
desc test1 '.%';
desc test1 '*%';
desc test1 '..%';
desc test1 '***%';
desc test2;
desc 'tes%' '.*%';
desc test1 'id1,id2';
desc v1;
desc v1 'id%';
desc mv1;
desc mv1 'id%';


describe test1;
describe test1 'id%';
describe test1 'id';
describe test1 '.%';
describe test1 '*%';
describe test1 '..%';
describe test1 '***%';
describe test2;
describe 'tes%' '.*%';
describe test1 'id1,id2';
describe v1;
describe v1 'id%';
describe mv1;
describe mv1 'id%';

EXPLAIN test1;
EXPLAIN test1 'id%';
EXPLAIN test1 'id';
EXPLAIN test1 '.%';
EXPLAIN test1 '*%';
EXPLAIN test1 '..%';
EXPLAIN test1 '***%';
EXPLAIN test2;
EXPLAIN 'tes%' '.*%';
EXPLAIN test1 'id1,id2';
EXPLAIN v1;
EXPLAIN v1 'id%';
EXPLAIN mv1;
EXPLAIN mv1 'id%';


drop view v1;
drop MATERIALIZED view mv1;
drop table test1;
drop table table1;
drop procedure test_cursor_0;
drop procedure test_cursor_1;
drop procedure test_cursor_2;
drop procedure test_cursor_3;
drop procedure test_cursor_4;

drop schema test_dcl_func cascade;
reset current_schema;
