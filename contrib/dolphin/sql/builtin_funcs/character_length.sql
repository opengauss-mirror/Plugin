drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select character_length(1234);
select character_length( '1234');
select character_length( 'abcd');
select character_length(1.1);
select character_length(null);
select character_length(1);
select character_length(B'101010101');
select character_length(1,1);
select 1+character_length(1);
select 1.1+character_length(1);
select character_length(111111111111111111111111111111111111111111111111111111111111111111111111111111111);

\c postgres
drop database if exists format_test;
