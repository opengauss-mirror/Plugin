\c show_open_tables_b
set search_path to show_open_tables_scm;
set role user1 password 'Show@123';
-- 1 and 1+ locks waiting on table
lock tables show_open_tables_test_1 read;
SHOW OPEN TABLES IN show_open_tables_scm;
select pg_sleep(2);
SHOW OPEN TABLES IN show_open_tables_scm;
unlock tables;
SHOW OPEN TABLES IN show_open_tables_scm;
select pg_sleep(2);
begin;
lock table show_open_tables_test_1 in ACCESS EXCLUSIVE mode;
SHOW OPEN TABLES IN show_open_tables_scm;
select pg_sleep(2);
SHOW OPEN TABLES IN show_open_tables_scm;
commit;
SHOW OPEN TABLES IN show_open_tables_scm;