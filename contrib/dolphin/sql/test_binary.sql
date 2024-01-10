create schema test_binary;
set current_schema to 'test_binary';
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

-- binary test enhance
select binary '\t';
select binary '\\';
select binary '\a';
select binary '\b';
select binary '\n';
select binary '\r';
select binary '\v';
select binary '\f';
select binary '\"';
select binary '\%';
select binary '\_';
select binary '\0';
select binary '\z';
select binary '\pqy';
select binary '数据库';
select binary E'\t';
select binary E'\\';
select binary E'\a';
select binary E'\b';
select binary E'\n';
select binary E'\r';
select binary E'\v';
select binary E'\f';
select binary E'\"';
select binary E'\%';
select binary E'\_';
select binary E'\0';
select binary E'\z';
select binary E'\pqy';
select binary E'数据库';
-- binary type cast test
select 'abc'::binary;
select 'abcdefgh'::binary;
select 'abc'::binary(20);
select 'a啊填啊'::binary;
-- other type
select 123::binary;
select 123.456::binary;
select '2020-01-01'::date::binary;
select '12:13:13'::time::binary;

--errreport
select 'abc'::binary(-1);
select 'abc'::binary(1);
--binary in like and escape

select 'abcd' like binary 'abc%';
select 'abcd' like binary 'abc\%';
select 'abcd' like binary 'abc\%' escape '\';
select 'abcd' like binary 'abc|%' escape '|';
select 'abc%' like binary 'abc|%' escape '|';
select 'abcd' like binary 'abc_';
select 'abcd' like binary 'abc\_';
select 'abcd' like binary 'abc\_' escape '\';
select 'abcd' like binary 'abc|_' escape '|';
select 'abc%' like binary 'abc|_' escape '|';

-- test binary expr gram;
select binary sin(1);
drop table if exists t1;
create table t1(a int, b text);
insert into t1 values(1,'test');
select binary a from t1;
select binary b from t1;
select binary a =  binary '3' from t1;

--test create binary table 

drop table if exists t1;
create table t1(a binary, b text);
insert into t1 values('a','name1');
insert into t1 values('ad','name2');
select * from t1;

-- enhase origin request 
SELECT BINARY 'Geeksforgeeks';
select binary repeat('a', 3);
create table test33 (c binary(3));
insert into test33 set c = 'a';
select hex(c), c = 'a', c = 'a\0\0' from test33;
-- binary operator
select c > 'a\0\0' from test33;
select c <> 'a\0\0' from test33;
select c >= 'a\0\0' from test33;
select c <= 'a\0\0' from test33;
select c != 'a\0\0' from test33;

--test binary core dump bug
select cast('a' as binary(0));
select cast(''	as binary(0));
select cast('ab' as binary(12));
create table test34 (a binary(0));
insert into test34 values('a');
insert into test34 values('');

SELECT 'ab'::binary = 'ab';
SELECT 'ab'::binary < 'ab';
SELECT 'ab'::binary <= 'ab';
SELECT 'ab'::binary > 'ab';
SELECT 'ab'::binary >= 'ab';
SELECT 'ab'::binary <> 'ab';

SELECT 'ab' = 'ab'::binary;
SELECT 'ab' < 'ab'::binary;
SELECT 'ab' <= 'ab'::binary;
SELECT 'ab' > 'ab'::binary;
SELECT 'ab' >= 'ab'::binary;
SELECT 'ab' <> 'ab'::binary;

SELECT 'ab'::binary = 'ab'::binary;
SELECT 'ab'::binary < 'ab'::binary;
SELECT 'ab'::binary <= 'ab'::binary;
SELECT 'ab'::binary > 'ab'::binary;
SELECT 'ab'::binary >= 'ab'::binary;
SELECT 'ab'::binary <> 'ab'::binary;

SELECT 'ab'::binary = 'ab'::varbinary(2);
SELECT 'ab'::binary < 'ab'::varbinary(2);
SELECT 'ab'::binary <= 'ab'::varbinary(2);
SELECT 'ab'::binary > 'ab'::varbinary(2);
SELECT 'ab'::binary >= 'ab'::varbinary(2);
SELECT 'ab'::binary <> 'ab'::varbinary(2);

SELECT 'ab'::varbinary(2) = 'ab';
SELECT 'ab'::varbinary(2) < 'ab';
SELECT 'ab'::varbinary(2) <= 'ab';
SELECT 'ab'::varbinary(2) > 'ab';
SELECT 'ab'::varbinary(2) >= 'ab';
SELECT 'ab'::varbinary(2) <> 'ab';

SELECT 'ab' = 'ab'::varbinary(2);
SELECT 'ab' < 'ab'::varbinary(2);
SELECT 'ab' <= 'ab'::varbinary(2);
SELECT 'ab' > 'ab'::varbinary(2);
SELECT 'ab' >= 'ab'::varbinary(2);
SELECT 'ab' <> 'ab'::varbinary(2);

SELECT 'ab'::varbinary(2) = 'ab'::varbinary(2);
SELECT 'ab'::varbinary(2) < 'ab'::varbinary(2);
SELECT 'ab'::varbinary(2) <= 'ab'::varbinary(2);
SELECT 'ab'::varbinary(2) > 'ab'::varbinary(2);
SELECT 'ab'::varbinary(2) >= 'ab'::varbinary(2);
SELECT 'ab'::varbinary(2) <> 'ab'::varbinary(2);

SELECT 'ab'::varbinary(2) = 'ab'::binary;
SELECT 'ab'::varbinary(2) < 'ab'::binary;
SELECT 'ab'::varbinary(2) <= 'ab'::binary;
SELECT 'ab'::varbinary(2) > 'ab'::binary;
SELECT 'ab'::varbinary(2) >= 'ab'::binary;
SELECT 'ab'::varbinary(2) <> 'ab'::binary;

set dolphin.b_compatibility_mode to on;
create table binary_operator(c1 binary(10), c2 numeric(10, 2), c3 time(6), c4 uint1, c5 uint2, c6 uint4, c7 uint8);
insert into binary_operator values('34', '234.5', '234.5', 234, 234, 234, 234), ('1234', '234.5', '234.5', 234, 234, 234, 234);

select c1 < c2, c1 > c2, c1 <= c2, c1 >= c2 from binary_operator;
select c1 < c3, c1 > c3, c1 <= c3, c1 >= c3 from binary_operator;
select c1 < c4, c1 > c4, c1 <= c4, c1 >= c4 from binary_operator;
select c1 < c5, c1 > c5, c1 <= c5, c1 >= c5 from binary_operator;
select c1 < c6, c1 > c6, c1 <= c6, c1 >= c6 from binary_operator;
select c1 < c7, c1 > c7, c1 <= c7, c1 >= c7 from binary_operator;

-- binary to bigint
select (20220101)::binary(30)::bigint;
select (-2075)::binary(30)::bigint;

-- binary about concat
DROP TABLE IF EXISTS t1;
SET dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero,treat_bxconst_as_binary';
CREATE TABLE t1 (s1 binary(2), s2 varbinary(2));
INSERT INTO t1 VALUES (0x4100, 0x4100);
SELECT LENGTH(concat('*', s1, '*', s2, '*')) FROM t1;
SELECT HEX(concat('*', s1, '*', s2, '*')) FROM t1;
SELECT HEX(concat('*', s1, '*', s2, '*')) FROM t1;
SELECT HEX(s1), HEX(s2), HEX('*') FROM t1;
DROP TABLE t1;

drop table if exists binary_operator;
reset dolphin.b_compatibility_mode;

drop schema test_binary cascade;
reset current_schema;
