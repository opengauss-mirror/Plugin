drop database if exists db_optimize;
create database db_optimize dbcompatibility 'b';
\c db_optimize
create table doc(id serial primary key, content varchar(255));
insert into doc(content) select 'abcd1234' from generate_series(1,10000) as content;
delete from doc where id < 9000;
optimize no_write_to_binlog table doc;
drop table doc;
set xc_maintenance_mode = on;
optimize table pg_class;
set xc_maintenance_mode = off;
\c postgres
drop database if exists db_optimize;
