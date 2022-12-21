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

drop schema tinyint_operator cascade;
reset current_schema;