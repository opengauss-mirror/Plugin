create schema db_flush;
set current_schema to 'db_flush';
FLUSH BINARY LOGS;
drop schema db_flush cascade;
reset current_schema;
