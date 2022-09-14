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
drop database if exists db_character_length_gbk;
create database db_character_length_gbk dbcompatibility 'B' encoding 'GBK' LC_CTYPE 'zh_CN.gbk' lc_collate 'zh_CN.gbk';
\c db_character_length_gbk
\encoding gbk;
select character_length('测试');
select character_length('测试123');
\c postgres
drop database if exists db_character_length_gbk;
drop database if exists db_character_length_latin1;
create database db_character_length_latin1 dbcompatibility 'B' encoding 'latin1' LC_CTYPE 'en_US' lc_collate 'en_US';
\c db_character_length_latin1
\encoding latin1;
select character_length('测试');
select character_length('测试123');
\c postgres
drop database if exists db_character_length_latin1;
drop database if exists db_character_length_utf8;
create database db_character_length_utf8 dbcompatibility 'b' encoding 'utf8';
\c db_character_length_utf8
\encoding utf8;
select character_length('测试');
select character_length('测试123');
\c postgres
drop database if exists db_character_length_utf8;
drop database if exists db_character_length_ascii;
create database db_character_length_ascii dbcompatibility 'b' encoding 'sql_ascii';
\c db_character_length_ascii
\encoding sql_ascii;
select character_length('测试');
select character_length('测试123');
\c postgres
drop database if exists db_character_length_ascii;