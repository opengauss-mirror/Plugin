drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test
create extension b_sql_plugin;

select hex(int1(255));
select hex(int1(256));

-- max value for int32
select hex(2147483647);
-- max value for int64
select hex(9223372036854775807);
-- out of range for int64
select hex(9223372036854775808);

-- test for float
select hex(12.34);
select hex(12.55);


select hex(TRUE);
select hex(FALSE);

select hex('abcde');
select hex('12.34');
select hex(NULL);
select hex('');

create table bytea_to_hex_test(c1 bytea);
insert into bytea_to_hex_test values ('123');
insert into bytea_to_hex_test values ('abc');
select hex(c1) from bytea_to_hex_test;

\c postgres
drop database if exists format_test;
