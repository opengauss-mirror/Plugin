drop database if exists db_b_insert_test;
create database db_b_insert_test dbcompatibility 'B';
\c db_b_insert_test

select insert('abcdefg', 2, 4, 'yyy');
select insert('abcdefg', -100, 4, 'yyy');
select insert('abcdefg', 100, 4, 'yyy');

\c postgres
drop database db_b_insert_test;