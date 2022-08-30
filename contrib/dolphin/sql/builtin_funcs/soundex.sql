drop database if exists db_soundex;
create database db_soundex dbcompatibility 'B';
\c db_soundex

select soundex('abc');
select soundex(10);
select soundex('afdsbfdlsafs');
select soundex('dslfaalskf222122');
select soundex('dadfa353dadf333ff');
select soundex('hell');

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
