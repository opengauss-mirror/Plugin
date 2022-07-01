drop database if exists db_regexp;
create database db_regexp dbcompatibility 'b';
\c db_regexp
select regexp('a', true);

\c postgres
drop database if exists db_regexp;