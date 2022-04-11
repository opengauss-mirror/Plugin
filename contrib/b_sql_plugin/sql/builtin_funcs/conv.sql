drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test
create extension b_sql_plugin;

select conv('a',16,2);
select conv('6e',18,8);
select conv(-17,10,-18);
select conv('10'+10,16,10);
select conv('ffffffffffffffff',16,10);
select conv('ffffffffffffffff',16,-10);
select conv('-ffffffffffffffff',16,10);
select conv('-ffffffffffffffff',16,-10);
select conv('ffffffffffffffff1',16,10);
select conv('ffffffffffffffff1',16,-10);
select conv('-ffffffffffffffff1',16,10);
select conv('-ffffffffffffffff1',16,-10);