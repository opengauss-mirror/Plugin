drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select soundex('abc');
select soundex(10);
select soundex('afdsbfdlsafs');
select soundex('dslfaalskf222122');
select soundex('dadfa353dadf333ff');
select soundex('hell');


\c postgres
drop database if exists format_test;
