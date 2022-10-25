drop database if exists db_char;
create database db_char dbcompatibility 'B';
\c db_char
select char(67,66,67);
select char('65','66','67');
select char('A','B','C');
select char(65.4,66.3,67.3);
select char(null);
select char(65);
SELECT char(77,77.3,'77.3','78.8',78.8);
select char(65,66.4);
select char(65,'66.4');
SELECT char(77,77.3,'77.3a','78.8',78.8);
select char(65,null,66);
select char(-18446744073709551616);
select char(18446744073709551615);
select char(0);
select char(18446744073709551615);
select char(true);
select char('hiu158','-156n测试fjl',155.99,'146');
select char('hiu78','-156nfjl',175.99,'测试');
select char('侧四',-156,55.99,'ceshi');
select char('hi测试u158','ceshi',135.99,146);
select char('hiu158','测试',125.99,146);
\c postgres
drop database if exists db_char;
