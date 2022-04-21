drop database if exists mysql_test;
create database mysql_test dbcompatibility 'b';
\c mysql_test
create extension b_sql_plugin;
--验证text类型
drop table if exists tb_db_b_parser_0001;
create table tb_db_b_parser_0001(a text(10),b tinytext,c mediumtext,d longtext);
--验证默认精度类型 decimal/number/dec/numeric
drop table if exists tb_db_b_parser_0002;
create table tb_db_b_parser_0002(a decimal, b number, c dec, d numeric);

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


\c postgres
drop database if exists mysql_test;