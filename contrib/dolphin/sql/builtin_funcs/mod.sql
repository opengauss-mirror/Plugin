create schema test_mod;
set current_schema to 'test_mod';

set dolphin.b_compatibility_mode = on;

select pg_typeof(mod(100.1, 1));
select mod(100.1, 1);

drop function if exists get_msec;
create or replace function get_msec ( n1 double precision)
returns bigint
language 'plpgsql'
as $$
declare
val bigint;
v1 double precision;
begin
set v1=mod(n1, 1);
raise notice '%', v1;
return 12;
end$$;

call get_msec(100.1);

drop function if exists get_msec;
drop PROCEDURE if exists get_msec;
create or replace PROCEDURE get_msec ( n1 double precision)
is
v1 double precision;
v2 text;
begin
set v1=mod(n1, 1);
set v2= pg_typeof(mod(n1, 1));
raise notice 'v1变量的值%', v1;
raise notice 'v1变量的类型%', v2;
raise notice '取模运算的值%', mod(n1, 1);
raise notice '取模运算的类型%', pg_typeof(mod(n1, 1));
end;
/

call get_msec(100.1);
drop PROCEDURE if exists get_msec;

drop schema test_mod cascade;
reset current_schema;

