create schema db_regexp;
set current_schema to 'db_regexp';
select regexp('a', true);

drop schema db_regexp cascade;
reset current_schema;