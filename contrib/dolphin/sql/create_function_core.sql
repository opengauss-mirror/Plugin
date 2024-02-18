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
reset current_schema;
drop schema create_function_core;
