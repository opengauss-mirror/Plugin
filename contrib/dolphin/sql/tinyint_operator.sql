drop database if exists tinyint_operator;
create database tinyint_operator dbcompatibility 'b';
\c tinyint_operator

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

\c postgres
drop database tinyint_operator;