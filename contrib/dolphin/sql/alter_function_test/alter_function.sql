create schema db_alter_func_1;
set current_schema to 'db_alter_func_1';

CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  AS $$ select 1 $$ ;

ALTER FUNCTION f1 (s char(20)) NO SQL;

ALTER FUNCTION f1 (s char(20)) CONTAINS SQL;

ALTER FUNCTION f1 (s char(20)) NO SQL CONTAINS SQL ;

ALTER FUNCTION f1 (s char(20)) CONTAINS SQL LANGUAGE SQL ;

ALTER FUNCTION f1 (s char(20)) CONTAINS SQL MODIFIES SQL DATA;

ALTER FUNCTION f1 (s char(20)) READS SQL DATA;

ALTER FUNCTION f1 (s char(20)) SQL SECURITY INVOKER;

ALTER FUNCTION f1 (s char(20)) SQL SECURITY DEFINER;

-- error 
ALTER FUNCTION f1 (s char(20)) DETERMINISTIC;

ALTER FUNCTION f1 (s char(20)) NOT DETERMINISTIC;

-- no params

ALTER FUNCTION f1 READS SQL DATA;

-- 存在没有参数的情况，有一样的也报错
create  function f3  returns int 
as $$ 
begin
	insert into t1 values(1);
	return 1;
end;
$$ LANGUAGE PLPGSQL;

create  function f3(a int)  returns int 
as $$ 
begin
	insert into t1 values(1);
	return 1;
end;
$$ LANGUAGE PLPGSQL;

ALTER FUNCTION f3  fenced;

drop FUNCTION f3(a int);

\sf f3

-- 没有此函数报错

ALTER FUNCTION f4  NO SQL;

-- 有重载，没有没有参数的函数，报错

create  function f5(a int)  returns int
as $$
begin
        insert into t1 values(1);
        return 1;
end;
$$ LANGUAGE PLPGSQL;


create  function f5(a float)  returns int
as $$
begin
        insert into t1 values(1);
        return 1;
end;
$$ LANGUAGE PLPGSQL;


alter FUNCTION f5 no sql;


create  function f5  returns int
as $$
begin
        insert into t1 values(1);
        return 1;
end;
$$ LANGUAGE PLPGSQL;

alter FUNCTION f5 no sql;


create table t1 (a int);


select  f5(1);


select f5 (1.1);


select  f5() ;

select * from t1;

drop schema db_alter_func_1 cascade;
reset current_schema;
