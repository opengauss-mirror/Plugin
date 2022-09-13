drop database if exists db_char_length;
create database db_char_length dbcompatibility 'B';
\c db_char_length

select char_length(1234);
select char_length(1234.5);
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
select char_length('测试');
select char_length('测试123');
select char_length(true);

\c postgres
drop database if exists db_char_length;
drop database if exists db_char_length_gbk;
create database db_char_length_gbk dbcompatibility 'B' encoding 'GBK' LC_CTYPE 'zh_CN.gbk' lc_collate 'zh_CN.gbk';
\c db_char_length_gbk
select char_length('测试');
select char_length('测试123');
\c postgres
drop database if exists db_char_length_gbk;
drop database if exists db_char_length_latin1;
create database db_char_length_latin1 dbcompatibility 'B' encoding 'latin1' LC_CTYPE 'en_US' lc_collate 'en_US';
\c db_char_length_latin1
\encoding latin1;
select char_length('测试');
select char_length('测试123');
\c postgres
drop database if exists db_char_length_latin1;
drop database if exists db_char_length_utf8;
create database db_char_length_utf8 dbcompatibility 'b' encoding 'utf8';
\c db_char_length_utf8
\encoding utf8;
select char_length('测试');
select char_length('测试123');
\c postgres
drop database if exists db_char_length_utf8;
drop database if exists db_char_length_ascii;
create database db_char_length_ascii dbcompatibility 'b' encoding 'sql_ascii';
\c db_char_length_ascii
\encoding sql_ascii;
select char_length('测试');
select char_length('测试123');
\c postgres
drop database if exists db_char_length_ascii;