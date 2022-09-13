drop database if exists db_soundex;
create database db_soundex dbcompatibility 'B';
\c db_soundex
select soundex('abc');
select soundex(10);
select soundex('afdsbfdlsafs');
select soundex('dslfaalskf222122');
select soundex('dadfa353dadf333ff');
select soundex('hell');
select soundex('测试');
select soundex('dbakbdasjcbsdk,kdsjbcksd,cnsk');
select soundex('!!!jhdgj');
select soundex('!!!!!！！jhdgj');
select soundex(null);
select soundex('12,xa');
select soundex('dkh测试fkdsjs12');
select soundex(false),soundex(true);
select soundex('颜'),soundex('谚');
select soundex('颜 '),soundex('谚0');
select soundex('遥'),soundex('摇'),soundex('瑶');
select soundex('遥!@#@$#'),soundex('摇0.00001'),soundex('瑶nhgchgh');
select soundex('人'),soundex('入');
select soundex('00人'),soundex('a00入'),soundex('baa入'),soundex('0b0入');
select soundex('0000食不果腹'),soundex('食不裹腹');
select soundex('气概'),soundex('气慨');
select soundex('辐射'),soundex('幅射');
select 'ibc' sounds like 'abc';
select 'aws' sounds like '10';
select 'aftcbtlxpz' sounds like 'afdsbfdlsafs';
select 'zie' sounds like 'dslfaalskf222122';
select 'adfidf' sounds like 'dadfa353dadf333ff';
select 'hill' sounds like 'hell';
select '中Adfab' sounds like '@adp';
select 1 sounds like 2;
select true sounds like 3;
select 2.2 sounds like false;

\c postgres
drop database if exists db_soundex;
