create schema tinyint_operator;
set current_schema to 'tinyint_operator';

select 1::int1 + 1::int1;
select (-1)::int1 + (-1)::int1;
select 127::int1 + 127::int1;
select (-128)::int1 + (-128)::int1;

select 1::int1 - 1::int1;
select (-1)::int1 - (-1)::int1;
select 127::int1 - (-1)::int1;
select (-128)::int1 - 1::int1;

select 1::int1 * 1::int1;
select (-1)::int1 * (-1)::int1;
select 127::int1 * 127::int1;
select (-128)::int1 * (-128)::int1;

select 1::int1 / 1::int1;
select (-1)::int1 / (-1)::int1;
select 127::int1 / 127::int1;
select (-128)::int1 / (-128)::int1;

select 1::int1 % 1::int1;
select (-1)::int1 % (-1)::int1;
select 127::int1 % 127::int1;
select (-128)::int1 % (-128)::int1;

select 1::int1 & 1::int1;
select (-1)::int1 & (-1)::int1;
select 127::int1 & 127::int1;
select (-128)::int1 & (-128)::int1;

select 1::int1 | 1::int1;
select (-1)::int1 | (-1)::int1;
select 127::int1 | 127::int1;
select (-128)::int1 | (-128)::int1;

select 1::int1 # 1::int1;
select (-1)::int1 # (-1)::int1;
select 127::int1 # 127::int1;
select (-128)::int1 # (-128)::int1;

select ~1::int1;
select ~(-1)::int1;
select ~127::int1;
select ~(-128)::int1;

select -1::int1;
select -(-1)::int1;
select -127::int1;
select -(-128)::int1;

select +1::int1;
select +(-1)::int1;
select +127::int1;
select +(-128)::int1;

select @1::int1;
select @(-1)::int1;
select @127::int1;
select @(-128)::int1;



create table t11(c11 tinyint(1), c12 smallint(2), c13 mediumint(3), c14 bigint(4), c15 int1(1), c16 int2(2), c17 int4(3), c18 int8(4), c19 uint1(1), c20 uint2(2), c21 uint4(3), c22 uint8(4));
select atttypmod from pg_attribute where attname in ('c11', 'c12', 'c13', 'c14', 'c15', 'c16', 'c17', 'c18', 'c19', 'c20', 'c21', 'c22') order by 1;
create table all_int_test(a tinyint(255), b smallint(255), c mediumint, d mediumint(255), e int(255), f bigint(255));
create table all_int_test1(a tinyint(256), b smallint(255), c mediumint, d mediumint(255), e int(255), f bigint(255));
drop table t11;
drop table all_int_test;

drop schema tinyint_operator cascade;
reset current_schema;