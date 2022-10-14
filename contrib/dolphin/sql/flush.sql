drop database if exists db_flush;
create database db_flush dbcompatibility 'b';
\c db_flush
FLUSH BINARY LOGS;
\c postgres
drop database if exists db_flush;
