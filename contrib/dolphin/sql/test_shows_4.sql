drop database if exists db_show_4;
create database db_show_4 dbcompatibility 'b';
\c db_show_4
show master status;
show slave hosts;
\c postgres
drop database if exists db_show_4;
