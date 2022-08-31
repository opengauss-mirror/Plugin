drop database if exists db_character_length;
create database db_character_length dbcompatibility 'B';
\c db_character_length

select character_length(1234);
select character_length(1234.5);
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
select character_length('测试');
select character_length('测试123.45');
select character_length(true);

\c postgres
drop database if exists db_character_length;
