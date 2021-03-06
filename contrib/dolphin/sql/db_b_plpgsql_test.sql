drop database if exists db_b_plpgsql_test;
create database db_b_plpgsql_test dbcompatibility 'b';
\c db_b_plpgsql_test

create table tb_b_grammar_0038(a text(10)) engine = 表1;

create or replace procedure proc_01()
as
begin
insert tb_b_grammar_0038 values('tom') ;
end;
/

create or replace procedure PEOC_165()
as
begin
CREATE TABLE t1 (a binary) engine INNODB;
end;
/

CREATE TABLE J1_TBL (
i integer,
j integer,
t text
);

CREATE TABLE J2_TBL (
i integer,
k integer
);
INSERT INTO J1_TBL VALUES (1, 4, 'one');
INSERT INTO J1_TBL VALUES (2, 3, 'two');
INSERT INTO J1_TBL VALUES (0, NULL, 'zero');
INSERT INTO J2_TBL VALUES (1, -1);
INSERT INTO J2_TBL VALUES (NULL, 0);

create or replace procedure PEOC_165()
as
begin
SELECT '' AS "xxx", * FROM J1_TBL JOIN J2_TBL;
end;
/

SELECT * from proc_01();
SELECT * from tb_b_grammar_0038;

\c postgres
drop database if exists db_b_plpgsql_test;