DROP DATABASE IF EXISTS b_compatibility_auto_parameterization;
CREATE DATABASE b_compatibility_auto_parameterization DBCOMPATIBILITY 'B';
\c b_compatibility_auto_parameterization

SET enable_query_parameterization = ON;
SET enable_set_variable_b_format TO ON;
SET dolphin.sql_mode = 'sql_mode_strict, treat_bxconst_as_binary';

CREATE TABLE t1 (s1 binary(2), s2 varbinary(2));
INSERT INTO t1 VALUES (0x4100, 0x4100);
SELECT HEX(concat('*', s1, '*', s2, '*')) FROM t1;
DROP TABLE t1;

SET dolphin.sql_mode='sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length';
SET dolphin.b_compatibility_mode = OFF;
DROP TABLE IF EXISTS t_bit;
CREATE TABLE t_bit(c_bit1 bit(1), c_bit10 bit(10));
INSERT INTO t_bit VALUES (null, 12);
INSERT INTO t_bit VALUES (1, 123);
INSERT INTO t_bit VALUES (null, 123.4);
INSERT INTO t_bit VALUES (null, 12.345);
INSERT INTO t_bit VALUES (null, 123.45);
INSERT INTO t_bit VALUES (null, 123.45678);
INSERT INTO t_bit VALUES (1, b'0101111');
select * FROM t_bit ORDER BY c_bit10 DESC;

SET dolphin.bit_output = DEC;
SELECT * FROM t_bit;
DROP TABLE t_bit;

SET dolphin.sql_mode = 'pad_char_to_full_length';
CREATE TABLE test_char(a char);
INSERT INTO test_char(a) VALUES('adsfsdaf');
SELECT * FROM test_char;
DROP TABLE test_char;

SET dolphin.sql_mode = 'pad_char_to_full_length';
CREATE TABLE test_varchar7(a varchar(7));
INSERT INTO test_varchar7(a) VALUES('adsfsdaf');
SELECT * FROM test_varchar7;
DROP TABLE test_varchar7;

CREATE TABLE t1(a datetime, b timestamp with time zone);
SET dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
INSERT INTO t1 VALUES ('3.1415926', '3.1415926');
SELECT * FROM t1;
DROP TABLE t1;

drop table if exists update_arithmetic_expr_t;
create table update_arithmetic_expr_t(
    n int,
    a int,
    b int8
);
insert into update_arithmetic_expr_t values(1, null, null);
insert into update_arithmetic_expr_t values(2, null, null);
insert into update_arithmetic_expr_t values(3, null, null);
update update_arithmetic_expr_t set b=9223372036854775000 + 666 where n = 1;
select * from update_arithmetic_expr_t;
drop table update_arithmetic_expr_t;

SET enable_query_parameterization = off;

\c postgres
DROP DATABASE b_compatibility_auto_parameterization;