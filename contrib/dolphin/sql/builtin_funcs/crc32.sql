drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select crc32('abc');
select crc32('');
select crc32(1);

select crc32(10),crc32(-3.1415926),crc32(1.339E5),crc32('ab57'),crc32('HAF47');

\c postgres
drop database if exists format_test;
