drop database if exists db_b_insert_test;
create database db_b_insert_test dbcompatibility 'B';
\c db_b_insert_test

select insert('abcdefg', 2, 4, 'yyy');
select insert(1234567, 2, 4, 'yyy');
select insert('abcdefg', -100, 4, 'yyy');
select insert('abcdefg', 100, 4, 'yyy');
select insert('abcdefg', 2, 200, 'yyy');
select insert('abcdefg', -100, 200, 'yyy');
select insert('abcdefg', 100, 200, 'yyy');
set sql_mode='';
select insert('abcdefg', '2aa', 4, 'yyy');
select insert('abcdefg', 'aa2', 4, 'yyy');

\c postgres
drop database db_b_insert_test;