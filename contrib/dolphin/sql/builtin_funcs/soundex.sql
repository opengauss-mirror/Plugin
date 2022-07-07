drop database if exists db_soundex;
create database db_soundex dbcompatibility 'B';
\c db_soundex

select soundex('abc');
select soundex(10);
select soundex('afdsbfdlsafs');
select soundex('dslfaalskf222122');
select soundex('dadfa353dadf333ff');
select soundex('hell');


\c postgres
drop database if exists db_soundex;
