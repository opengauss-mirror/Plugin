create schema create_function_core;
set current_schema = create_function_core;
delimiter |;
create function bug10015_2() returns int
begin
declare i, s int;
set i = (select min(id) from t1);
set s = (select max(id) from t1);
return (s - i);
end|
delimiter ;|
drop function bug10015_2;
-- test show open tables
drop table if exists t2;
create table t2 (a int unique);
drop function if exists f2;
delimiter | ;
create function f2(x int) returns int
begin
insert into t2 (a) values (x);
insert into t2 (a) values (x);
return x;
end |
delimiter ; |
insert into t2 (a) values (1014);
show open tables where (f2(14) = 14); --expect error
drop function f2;
drop table t2;
reset current_schema;
drop schema create_function_core;
