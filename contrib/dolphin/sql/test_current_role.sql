create schema test_current_role;
set current_schema to 'test_current_role';
select current_role;
select current_role();
drop schema test_current_role cascade;
reset current_schema;
