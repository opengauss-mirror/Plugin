create schema db_show_status;
set current_schema to 'db_show_status';
set dolphin.sql_mode='';
show status;
show dolphin.sql_mode;
reset dolphin.sql_mode;

drop schema db_show_status cascade;
reset current_schema;
