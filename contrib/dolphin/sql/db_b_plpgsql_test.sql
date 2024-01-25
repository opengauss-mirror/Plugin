create schema db_b_plpgsql_test;
set current_schema to 'db_b_plpgsql_test';

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

-- test "set"
create procedure proc1(x int) as
v int;
begin
set x=x+1;
set v=x+1;
raise notice '%',v;
end;
/
call proc1(1);
call proc1(2);
drop procedure proc1;

declare
i int;
begin
set j=1;
raise notice '%',j;
end;
/

declare
i int;
begin
set j=1;
raise notice '%',j;
end;
/

begin
set enable_set_variable_b_format = on;
end;
/
show enable_set_variable_b_format;

begin
set @i=0;
end;
/
select @i;

set dolphin.b_compatibility_mode to on;
create table t1(c1 varchar(20));
DO $$ 
DECLARE
   hs_sql text;
BEGIN
   
   SELECT 'ALTER TABLE t1 MODIFY COLUMN c1 varchar(40) DEFAULT ''b'' NOT NULL;'
    INTO hs_sql
   FROM dual
   WHERE (
     SELECT COUNT(1)
     FROM information_schema.columns
     WHERE 
      table_name = 't1'
     AND column_name = 'c1'
	 AND DATA_TYPE = 'character varying'
     AND replace(COLUMN_DEFAULT,'::character varying','') = 'b'
     AND character_maximum_length=40
     AND IS_NULLABLE='NO'
    ) = 0;
    
   if hs_sql is not null then 
     RAISE NOTICE '执行SQL：%', hs_sql;
     EXECUTE hs_sql;
   end if; 
END $$;
drop table t1;

reset enable_set_variable_b_format;

drop schema db_b_plpgsql_test cascade;
reset current_schema;
