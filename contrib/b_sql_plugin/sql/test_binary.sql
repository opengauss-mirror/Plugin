drop database if exists test;
create database test dbcompatibility 'B';
\c test
create extension b_sql_plugin;
create table binary_templates (a bytea, b binary(5), c varbinary(5));

-- invalid typmod
create table invalid_table (b binary(-1));
create table invalid_table (b binary(256));
create table invalid_table (b varbinary(-1));
create table invalid_table (b varbinary(65536));

insert into binary_templates values ('aaa', 'aaa', 'aaa');
insert into binary_templates values ('aaa', 'aaa', 'aaaaa');

-- invalid insert
insert into binary_templates values ('aaaaaa', 'aaaaaa', 'aaa');
insert into binary_templates values ('aaaaaa', 'aaa', 'aaaaaa');

select * from binary_templates;

create table test_bytea (a bytea);
create table test_binary (a binary(5));
create table test_varbinary (a varbinary(5));

insert into test_bytea select a from binary_templates;
insert into test_bytea select b from binary_templates;
insert into test_bytea select c from binary_templates;

insert into test_binary select a from binary_templates;
insert into test_binary select b from binary_templates;
insert into test_binary select c from binary_templates;

insert into test_varbinary select a from binary_templates;
insert into test_varbinary select b from binary_templates;
insert into test_varbinary select c from binary_templates;

create table shorter_binary (a binary(3));
create table shorter_varbinary (a varbinary(3));
insert into shorter_binary select b from binary_templates;
insert into shorter_varbinary select c from binary_templates;

create table test_index (a binary(10), b varbinary(10));
create index on test_index using btree (a);
create index on test_index using hash (a);
create index on test_index using gin (to_tsvector(a::text));
create index on test_index using btree (b);
create index on test_index using hash (b);
create index on test_index using gin (to_tsvector(b::text));
\d test_index

create cast (text as binary) without function as implicit;
create cast (text as varbinary) without function as implicit;

create table test_text2binary (a binary(255), b varbinary(65535));
insert into test_text2binary values(repeat('a', 256), repeat('a', 65536));
insert into test_text2binary values(repeat('a', 255), repeat('a', 65535));
select octet_length(a) as binary_length, octet_length(b) as varbinary_length from test_text2binary;
drop cast if exists (text as binary);
drop cast if exists (text as varbinary);
drop table binary_templates;
drop table test_binary;
drop table test_varbinary;
drop table shorter_binary;
drop table shorter_varbinary;
drop table test_index;
drop table test_text2binary;

drop table if exists t_binary_061;
create table t_binary_061(id int, field_name binary(10));
PREPARE insert_binary(int,binary(10)) as insert into t_binary_061 values($1,$2);
EXECUTE insert_binary(1, 'aaa'::bytea);  -- length 3
EXECUTE insert_binary(1, 'aaaaaaaaaaa'::bytea);  -- length 11

drop table if exists t_varbinary_061;
create table t_varbinary_061(id int, field_name varbinary(10));
PREPARE insert_varbinary(int,varbinary(10)) as insert into t_varbinary_061 values($1,$2);
EXECUTE insert_varbinary(1, 'aaa'::bytea);  -- length 3
EXECUTE insert_varbinary(1, 'aaaaaaaaaaa'::bytea); -- length 11

select * from t_binary_061;
select * from t_varbinary_061;

drop table if exists t_binary_061;
drop table if exists t_varbinary_061;

\c postgres
drop database test;