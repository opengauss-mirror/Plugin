create schema uint_procedure_col;
set current_schema to 'uint_procedure_col';

 create procedure test_p1(uint2, uint4)
SHIPPABLE VOLATILE
as
begin
    insert into test1 values($1 + $2);
end;
/

create table test1(a uint4);
select test_p1(1, 3);
select * from test1;

create table test2(a uint1) with(orientation = column);
create table test2(a int1, b uint1) with(orientation = column);
create table test2(a uint2) with(orientation = column);
create table test2(a int2, b uint2) with(orientation = column);
create table test2(a uint4) with(orientation = column);
create table test2(a int4, b uint4) with(orientation = column);
create table test2(a uint8) with(orientation = column);
create table test2(a int8, b uint8) with(orientation = column);

set enable_opfusion = on;
create table bypass(a uint2, b uint4);
insert into bypass values(1, 10);
insert into bypass values(1, 2);
explain(costs off, verbose) insert into bypass values(1, 10);
explain(costs off, verbose) select b from bypass where a = 1;
explain(costs off, verbose) delete from bypass where b = 10;
explain(costs off, verbose) update bypass set b = b + 1 where a = 1;

drop schema uint_procedure_col cascade;
reset current_schema;