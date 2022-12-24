create schema db_show_4;
set current_schema to 'db_show_4';
show master status;
show slave hosts;
drop schema db_show_4 cascade;
reset current_schema;
