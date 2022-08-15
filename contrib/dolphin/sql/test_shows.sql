drop database if exists db_show;
create database db_show dbcompatibility 'b';
\c db_show
show processlist;
show full processlist;

\c postgres
drop database if exists db_show;
