create schema db_b_parser4;
set current_schema to 'db_b_parser4';
--验证text类型
drop table if exists tb_db_b_parser_0001;
create table tb_db_b_parser_0001(a text(10),b tinytext,c mediumtext,d longtext);
--验证默认精度类型 decimal/number/dec/numeric/fixed
drop table if exists tb_db_b_parser_0002;
create table tb_db_b_parser_0002(a decimal, b number, c dec, d numeric, e fixed);

\d tb_db_b_parser_0002
--验证float4(n)和double类型
drop table if exists tb_default_float;
create table tb_default_float(a float4(10));

\d tb_default_float
drop table if exists tb_default_double;
create table tb_default_double(a double);

\d tb_default_double
--验证real和float类型
drop table if exists tb_real_float;
create table tb_real_float(a real, b float);

\d tb_real_float

drop table if exists tb_db_b_parser_0002;
drop table if exists tb_default_float;
drop table if exists tb_default_double;
drop table if exists tb_real_float;

--syntax error message
create table    t1(123abc   int);
create table    t1(123   int);
create table    t1(123   abc int);
create table    123(abc   int);
create table    123abc(abc   int);
create table    123 abc(abc   int);
create table if  not  exists   t1(123abc   int);
create table if  not  exists   t1(123   int);
create table if  not  exists   t1(123   abc int);
create table if  not  exists   123(abc   int);
create table if  not  exists   123abc(abc   int);
create table if  not  exists   123 abc(abc   int);
123abc;

--bconst
select 0b01;
select 0b01+1;
select 1+0b01;

select 0b01 = b'01';
select 0b01+1 = b'01'+1;
select 1+0b01 = 1+b'01';

--wrong usage
select 0b02;
select 0b2;
--not report error, but not bconst actually
select 0b; -- same as select 0 b;
select 0bf; -- same as select 0 bf;
select 0b0f; -- same as select 0b0 f;

drop schema db_b_parser4 cascade;
reset current_schema;