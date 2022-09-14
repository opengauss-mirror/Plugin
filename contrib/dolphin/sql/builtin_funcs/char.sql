drop database if exists db_char;
create database db_char dbcompatibility 'B';
\c db_char
select chara(67,66,67);
select chara('65','66','67');
select chara('A','B','C');
select chara(65.4,66.3,67.3);
select chara(null);
select chara(65);
SELECT chara(77,77.3,'77.3','78.8',78.8);
select chara(65,66.4);
select chara(65,'66.4');
SELECT chara(77,77.3,'77.3a','78.8',78.8);
select chara(65,null,66);
select chara(-18446744073709551616);
select chara(18446744073709551615);
select chara(0);
select chara(18446744073709551615);
select chara(true);
select chara('hiu158','-156n测试fjl',155.99,'146');
select chara('hiu78','-156nfjl',175.99,'测试');
select chara('侧四',-156,55.99,'ceshi');
select chara('hi测试u158','ceshi',135.99,146);
select chara('hiu158','测试',125.99,146);
\c postgres
drop database if exists db_char;
