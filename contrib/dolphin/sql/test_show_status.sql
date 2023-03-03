create schema db_show_status;
set current_schema to 'db_show_status';
show status;

drop schema db_show_status cascade;
reset current_schema;
