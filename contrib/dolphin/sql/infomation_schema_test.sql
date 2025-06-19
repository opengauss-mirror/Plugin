create schema infomation_schema_test;

set current_schema = infomation_schema_test;

CREATE TABLE employees (emp_id INT, emp_name VARCHAR(50) NOT NULL) COMMENT='公司员工基本信息表';

CREATE TABLE employees2 (emp_id INT, emp_name VARCHAR(50) NOT NULL) COMMENT='Basic Information Form of Company Employees';

CREATE TABLE employees3 (emp_id INT primary key  AUTO_INCREMENT, emp_name VARCHAR(50) NOT NULL COMMENT 'this is a comment1');

CREATE TABLE employees4 (emp_id INT primary key  AUTO_INCREMENT COMMENT 'this is a comment2', emp_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);

select * from information_schema.tables where table_schema = 'infomation_schema_test' and table_name = 'employees';
select * from information_schema.tables where table_schema = 'infomation_schema_test' and table_name = 'employees2';

select * from information_schema.columns where table_schema = 'infomation_schema_test' and table_name = 'employees3' and column_name = 'emp_id';
select * from information_schema.columns where table_schema = 'infomation_schema_test' and table_name = 'employees4' and column_name = 'emp_id';

select * from information_schema.columns where table_schema = 'infomation_schema_test' and table_name = 'employees3' and column_name = 'emp_name';
select * from information_schema.columns where table_schema = 'infomation_schema_test' and table_name = 'employees4' and column_name = 'emp_time';

drop table employees;
drop table employees2;
drop table employees3;
drop table employees4;

reset current_schema;
drop schema infomation_schema_test;

