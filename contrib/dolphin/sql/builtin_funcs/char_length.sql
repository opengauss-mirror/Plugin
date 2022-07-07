drop database if exists db_char_length;
create database db_char_length dbcompatibility 'B';
\c db_char_length

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
drop database if exists db_char_length;
