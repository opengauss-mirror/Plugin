create schema db_b_insert_test;
set current_schema to 'db_b_insert_test';

select insert('abcdefg', 2, 4, 'yyy');
select insert(1234567, 2, 4, 'yyy');
select insert('abcdefg', 2, 4, 123);
select insert('abcdefg', -100, 4, 'yyy');
select insert('abcdefg', 100, 4, 'yyy');
select insert('abcdefg', 2, 200, 'yyy');
select insert('abcdefg', -100, 200, 'yyy');
select insert('abcdefg', 100, 200, 'yyy');
select insert('abcdefg', 412321321421, 4, 'yyy');
select insert('abcdefg', -412321321421, 4, 'yyy');
select insert('abcdefg', 1147483640, 1147483640, 'yyy');
select insert('abcdefg', 2.3213, 4.123123, 'yyy');
select insert(5.1234, 2, 4, 'yyy');
select insert('abcdefg', 2, 4, 5.21);
set dolphin.sql_mode='';
select insert('abcdefg', '2aa', 4, 'yyy');
select insert('abcdefg', 2, '3abc', 'yyy');
select insert('abcdefg', 'aa2', 4, 'yyy');
select insert('abcdefg', 2, 'aa2', 'yyy');
select insert('abcdefg', 4123213214212123123123123, 4, 'yyy');
select insert('abcdefg', -4123213214212123123123123, 4, 'yyy');
select insert('abcdefg', 412321321421, 4, 'yyy');
select insert('abcdefg', -412321321421, 4, 'yyy');

drop schema db_b_insert_test cascade;
reset current_schema;