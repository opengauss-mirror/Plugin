drop database if exists db_alter_func_2;
create database db_alter_func_2 dbcompatibility 'B';
\c db_alter_func_2

CREATE OR REPLACE PROCEDURE proc1() 
AS
BEGIN
RAISE NOTICE '%','aaa';
END; 
/

call proc1();

ALTER PROCEDURE proc1() NO SQL;

ALTER PROCEDURE proc1() CONTAINS SQL;
	 
ALTER PROCEDURE proc1() NO SQL CONTAINS SQL ;
	  
ALTER PROCEDURE proc1() CONTAINS SQL LANGUAGE SQL ;
	  
ALTER PROCEDURE proc1() CONTAINS SQL MODIFIES SQL DATA;
	 
ALTER PROCEDURE proc1() READS SQL DATA;
	  
ALTER PROCEDURE proc1() SQL SECURITY INVOKER;
	 
ALTER  PROCEDURE proc1() SQL SECURITY DEFINER;

-- error 
ALTER PROCEDURE proc1() DETERMINISTIC;

ALTER PROCEDURE proc1() NOT DETERMINISTIC;

-- no params
ALTER PROCEDURE proc1  READS SQL DATA;

\sf proc1
ALTER PROCEDURE proc1  READS SQL DATA FENCED;

\sf proc1
-- 存储过程本身不支持重载
CREATE  PROCEDURE proc1(a int)
AS
BEGIN
RAISE NOTICE '%','aaa';
END;
/

-- 修改不存在的存储过程
ALTER PROCEDURE proc2  READS SQL DATA;

\c postgres
drop database db_alter_func_2;
