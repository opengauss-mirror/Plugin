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

drop table if exists having_alias_info_schema_test;
create table having_alias_info_schema_test(id int);

select
    table_schema as table_cat,
    null as table_schem,
    table_name,
    case
        when table_type = 'BASE TABLE' then 'TABLE'
        else table_type
    end as table_type
from information_schema.tables
where table_schema = 'infomation_schema_test'
    and table_name = 'having_alias_info_schema_test'
group by table_schema, table_name, information_schema.tables.table_type
having table_type in ('TABLE', null)
order by table_type, table_schema, table_name;

drop table if exists having_alias_grouping_test;
create table having_alias_grouping_test(a int, b int);
insert into having_alias_grouping_test values (1, 10), (2, 20);
select a + 1 as a, sum(b)
from having_alias_grouping_test
group by rollup(a)
having grouping(a) = 0
order by 1;
drop table having_alias_grouping_test;

drop table if exists having_alias_outer_test;
drop table if exists having_alias_inner_test;
create table having_alias_outer_test(id int);
create table having_alias_inner_test(id int);
insert into having_alias_outer_test values (1);
insert into having_alias_inner_test values (1), (2);
select count(*)
from (
    select id as id
    from having_alias_outer_test o
    where exists (
        select 1
        from having_alias_inner_test i
        group by i.id
        having id > 1
    )
) s;
drop table having_alias_outer_test;
drop table having_alias_inner_test;

drop table having_alias_info_schema_test;

drop table employees;
drop table employees2;
drop table employees3;
drop table employees4;

reset current_schema;
drop schema infomation_schema_test;
