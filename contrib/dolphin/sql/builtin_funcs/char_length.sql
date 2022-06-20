drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select char_length(1234);
select char_length( '1234');
select char_length( 'abcd');
select char_length(1.1);
select char_length(null);
select char_length(1);
select char_length(B'101010101');
select char_length(1,1);
select 1+char_length(1);
select 1.1+char_length(1);
select char_length(111111111111111111111111111111111111111111111111111111111111111111111111111111111);

\c postgres
drop database if exists format_test;
