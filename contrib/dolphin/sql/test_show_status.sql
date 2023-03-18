create schema db_show_status;
set current_schema to 'db_show_status';
set dolphin.sql_mode='';
show status;
show session status;
show global status;
show session status like 'buffers_backend';
show global status where variable_name = 'buffers_backend';
show dolphin.sql_mode;
reset dolphin.sql_mode;

drop schema db_show_status cascade;
reset current_schema;
