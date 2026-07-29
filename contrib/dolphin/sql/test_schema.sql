create schema schema_test;
set current_schema to 'schema_test';

SELECT SCHEMA();
SELECT pg_typeof(SCHEMA());

drop schema schema_test cascade;
reset current_schema;

-- test not exist schema
-- 不存在的一个schema
set current_schema to 'test_ansi_quotes';  
CREATE TABLE test_quotes (a text); 
desc test_quotes; 

set current_schema to 'public';
DROP TABLE IF EXISTS test_quotes;
CREATE TABLE test_quotes (a text); 
desc test_quotes; 

set current_schema to 'test_ansi_quotes'; 
desc test_quotes;


