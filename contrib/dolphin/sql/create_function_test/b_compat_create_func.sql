create schema b_compat_create_func;
set current_schema to 'b_compat_create_func';
DROP ROLE if EXISTS us1;
CREATE ROLE us1 IDENTIFIED BY 'Aa@123456';
GRANT ALL ON SCHEMA b_compat_create_func TO us1;
SET ROLE us1 PASSWORD 'Aa@123456';

create function func(n int) returns varchar(50) return (select n+1);
select func(100);
drop function if exists func(int);
create function func(n int) returns varchar(50) return (select (n+1)::text);
select func(100);
drop function if exists func(int);
create function func(s char(20)) returns char(50) return concat('Hello, ', s, '!');
select func('me');
drop function if exists func(char(20));
create function func(s date) returns timestamp return current_timestamp;
select func('2023-01-28'::date);
drop function if exists func(date);
create function func(p1 datetime, p2 int) returns time return null;
select func('2023-01-28 00:00:00'::datetime, 1);
drop function if exists func(datetime, int);

-- test for duplicated options
create function func(s int)
    returns char(50)
    deterministic
    sql security invoker
sql security invoker
return s;
select func(20);
drop function if exists func;
create function func(s int)
returns char(50)
deterministic
sql security invoker
language C
not deterministic
language sql
sql security definer
return s;
select func(20);
drop function if exists func;

create table test11(
    name varchar(50) not null,
    current_value int not null,
    increment int not null default 1,
    primary key (name)
) engine=innodb;
insert into test11 values ('a', 999, 999);

drop function if exists currval(varchar(50));
delimiter /
create function currval(seq_name varchar(50)) returns integer
language sql
deterministic
contains sql
sql security definer
comment ''
begin
    declare value integer;
    set value = 0;
    select current_value into value from test11 where name = seq_name;
    return value;
end/
delimiter ;
select currval('a');

drop function if exists currval(varchar(50));
delimiter //
create function currval(seq_name varchar(50)) returns integer
comment 'abcdefghijk'
begin
    declare value integer;
    set value = 0;
    select current_value into value from test11 where name = seq_name;
    return value;
end //
delimiter ;
select currval('a');
show create function currval;

drop function if exists currval(varchar(50));
delimiter /
create function currval(seq_name varchar(50)) returns integer
language sql
begin
    declare value integer;
    set value = 0;
    select current_value into value from test11 where name = seq_name;
    return value;
end /
delimiter ;
select currval('a');

drop function if exists currval(varchar(50));
delimiter /
create function currval(seq_name varchar(50)) returns integer
deterministic
begin
    declare value integer;
    set value = 0;
    select current_value into value from test11 where name = seq_name;
    return value;
end /
delimiter ;
select currval('a');

drop function if exists currval(varchar(50));
delimiter /
create function currval(seq_name varchar(50)) returns integer
contains sql
begin
    declare value integer;
    set value = 0;
select current_value into value from test11 where name = seq_name;
return value;
end /
delimiter ;
select currval('a');

drop function if exists currval(varchar(50));
delimiter /
create function currval(seq_name varchar(50)) returns integer
sql security definer
begin
    declare value integer;
    set value = 0;
select current_value into value from test11 where name = seq_name;
return value;
end /
delimiter ;
select currval('a');

drop function if exists simplecompare;
delimiter /
create function simplecompare(n int, m int) returns varchar(20)
begin
    declare s varchar(20);
    if n > m then set s = '>';
    elseif n = m then set s = '=';
    else set s = '<';
    end if;
    set s = concat(n, ' ', s, ' ', m);
    return s;
end/
delimiter ;
select simplecompare(1, 2);

drop function if exists func_ddl_0011;
delimiter |
create function func_ddl_0011(n1 int, n2 int) returns int
begin
    declare num int;
    set num = n1 + n2;
    return num;
END|
delimiter ;
call func_ddl_0011(1, 5);

drop function if exists simplecompare;
delimiter //
create function simplecompare(n int, m int) returns varchar(20)
begin
    declare s varchar(20);
    if n > m then set s = '>';
    elseif n = m then set s = '=';
    else set s = '<';
    end if;

    set s = concat(n, ' ', s, ' ', m);

    return s;
end //
delimiter ;
call simplecompare(1, 2);

-- test for function body with flow control stmt
drop function if exists func1;
delimiter |
create function func1(b int, c int) returns int
    if b < c then return b;
    else return c;
end if|
select func1(2,3)|

drop function if exists func2|
create function func2(b int) returns int
    case b
	when 1 then return 9;
    when 2 then return 99;
    when 3 then return 999;
    else return 0;
end case|
select func2(3)|

drop function if exists func3|
create function func3(b int) returns int
loop
    if b < 10 then set b = b + 10;
    return b;
    end if;
end loop|
select func3(3)|

drop function if exists func3|
create function func3(b int) returns int
    label1:loop
    if b < 10 then set b = b + 10;
return b;
end if;
end loop|
select func3(3)|

drop function if exists func3|
create function func3(b int) returns int
    label1: loop
    if b < 10 then set b = b + 10;
return b;
end if;
end loop|
select func3(3)|

drop function if exists func4|
create function func4(b int) returns int
    repeat
	set b = b + 10;
    return b;
until b > 10 end repeat|
select func4(3)|

drop function if exists func4|
create function func4(b int) returns int
    label2:repeat
	set b = b + 10;
return b;
until b > 10 end repeat|
select func4(3)|

drop function if exists func4|
create function func4(b int) returns int
    label2: repeat
	set b = b + 10;
return b;
until b > 10 end repeat|
select func4(3)|

drop function if exists func5|
create function func5(b int) returns int
while b < 10 do
    set b = b + 10;
    return b;
end while|
select func5(3)|

drop function if exists func5|
create function func5(b int) returns int
    label3:while b < 10 do
    set b = b + 10;
return b;
end while|
select func5(3)|

drop function if exists func5|
create function func5(b int) returns int
    label3: while b < 10 do
    set b = b + 10;
return b;
end while|
select func5(3)|

-- test for begin-end statement compound by flow control statement
drop function if exists func6|
create function func6(b int) returns int
loop
    begin
        if b > 0 then
            return b + 10;
        else return -1;
        end if;
    end;
end loop|
select func6(3)|

-- test for nested begin-end within begin-end
drop function if exists func7 |
create function func7(b int) returns int
begin
    declare num int;
    set num = 0;
    begin
        num = num - b;
        num = num * 10;
    end;
    return num;
end|
select func7(5)|

drop function if exists func8|
create function func8(b int) returns int
BEGIN
    IF b = 1 THEN
        BEGIN
            set b = b + 10;
            return b;
        END;
    ELSE
        BEGIN
            set b = b - 10;
            return b;
        END;
    END IF;
END|
select func8(3)|

drop function if exists func9|
create function func9(b int) returns int
BEGIN
    BEGIN
        set b = b + 10;
        return b;
    END;
END|
select func9(5)|

-- test for function body with begin-end stmt
drop function if exists currval(varchar(50))|
delimiter /
create function currval(seq_name varchar(50))
    returns integer
begin
    declare value integer;
    set value = 0;
    select current_value into value from test11 where name = seq_name;
    return value;
end/
delimiter ;
select currval('a');

-- test for flow control statement compound by begin-end stmt
drop function if exists func10;
delimiter //
create function func10(b int) returns int
begin
    if b > 0 then return b + 10;
    else return -1;
    end if;
end//
delimiter ;
select func10(9);

drop function if exists hello;
CREATE FUNCTION hello (s CHAR(20)) RETURNS CHAR(50)
    RETURN CONCAT('Hello, ', s, '!');
select hello('me');

drop function if exists hello;
delimiter |
CREATE FUNCTION hello (s CHAR(20)) RETURNS CHAR(50)
    RETURN CONCAT('Hello, ', s, '!')|
delimiter ;
select hello('me');

drop function if exists hello;
delimiter /
CREATE FUNCTION hello (s CHAR(20)) RETURNS CHAR(50)
BEGIN
RETURN CONCAT('Hello, ', s, '!');
END/
delimiter ;
select hello('me');

drop function if exists func(int);
create function func(n int) returns varchar(50) return (select (n+1)::text);

-- test for function options
drop function if exists currval(varchar(50));
delimiter /
create function currval(seq_name varchar(50))
    returns integer
language sql
deterministic
comment 'me'
begin
    declare value integer;
    set value = 0;
    select current_value into value from test11 where name = seq_name;
    return value;
end/
delimiter ;

-- test for invalid function options
drop function if exists currval(varchar(50));
delimiter /
create function currval(seq_name varchar(50)) returns integer
strict
comment 'test'
immutable
return 1/
delimiter ;

-- testcase for mysql
drop table if exists t1;
create table t1(c int);
delimiter |
create function m_func(n int) returns int(11)
begin
    insert into t1 values (null);
    return n;
end|
delimiter ;
call m_func(6);
drop function if exists m_func(int);

insert into t1 values(1);
delimiter |
create function m_func() returns int
begin
    declare j int;
    select c from t1 where c = 1 into j;
    return j;
end|
delimiter ;
call m_func();
drop function if exists m_func();

drop table if exists t1;
create table t1(c int);
CREATE OR REPLACE PROCEDURE proc1()
AS
BEGIN
    INSERT INTO t1 VALUES (1);
END;
/
delimiter |
create function func_call_proc1(a int) returns int
begin
    call proc1();
    return 1;
end|
delimiter ;
call func_call_proc1(1);
drop function if exists func_call_proc1(int);

-- test for begin-end surrounded body statement without return
drop table if exists t_create_begin_end_func_no_return;
create table t_create_begin_end_func_no_return(c1 int);
drop function if exists t_create_begin_end_func_no_return;
delimiter |
create function t_create_begin_end_func_no_return(num int) returns int
    no sql
BEGIN
declare i int;
set i = 0;
while i < num do
insert into t_create_begin_end_func_no_return values(i);
set i = i + 1;
end while;
end|
delimiter ;

-- test for flow control body statement without return
delimiter |
drop function if exists t_create_flow_control_func_no_return|
create function t_create_flow_control_func_no_return(b int) returns int
    repeat
	set b = b + 10;
    until b > 10 end repeat|
delimiter ;
drop table if exists test_table_030;
create table test_table_030(ID int,NAME text);
insert into test_table_030 values(1,'a'),(2,'b');
drop function if exists test_function_030;
DELIMITER |
CREATE FUNCTION test_function_030(canshu VARCHAR(16))
returns INT READS SQL DATA
BEGIN
DECLARE test_table_030a int;
SELECT ID into test_table_030a FROM test_table_030 WHERE NAME = canshu;
return test_table_030a;
END |
DELIMITER ;
--test trigger
drop function if exists f_create_function_0033;
delimiter |
create function f_create_function_0033() returns trigger
begin
insert into t_create_function_0033 values(new.id1, new.id2, new.id3);
return new;
end|
delimiter ;

CREATE TABLE test_trigger_src_tbl(id1 INT, id2 INT, id3 INT);
CREATE TABLE test_trigger_des_tbl(id1 INT, id2 INT, id3 INT);
delimiter /
CREATE OR REPLACE FUNCTION tri_insert_funcx() RETURNS TRIGGER 
BEGIN
INSERT INTO test_trigger_des_tbl VALUES(NEW.id1, NEW.id2, NEW.id3);
RETURN NEW;
END
/
delimiter ;


CREATE TRIGGER insert_triggerx
BEFORE INSERT ON test_trigger_src_tbl
FOR EACH ROW
EXECUTE PROCEDURE tri_insert_funcx();

insert into test_trigger_src_tbl values(1,2,3);
select * from test_trigger_src_tbl;
select * from test_trigger_des_tbl;

drop table test_trigger_src_tbl cascade;
drop table test_trigger_des_tbl cascade;
select test_function_030('aaa');
select test_function_030('aaa') is null;
drop table if exists test_table_030;
drop function if exists test_function_030;

-- expect error: there is no parameter $1, other error is wrong!!!
select :lable;
select :loop;
select :repeat;
select :while;
drop schema b_compat_create_func cascade;
reset current_schema;
reset role;
drop role us1;