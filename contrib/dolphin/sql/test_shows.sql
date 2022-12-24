create schema db_show;
set current_schema to 'db_show';
show processlist;
show full processlist;

drop schema db_show cascade;
reset current_schema;
