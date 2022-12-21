create schema schema_test;
set current_schema to 'schema_test';

SELECT SCHEMA();

drop schema schema_test cascade;
reset current_schema;
