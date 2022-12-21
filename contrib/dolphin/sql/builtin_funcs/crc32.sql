create schema db_crc32;
set current_schema to 'db_crc32';

select crc32('abc');
select crc32('');
select crc32(1);

select crc32(10),crc32(-3.1415926),crc32(1.339E5),crc32('ab57'),crc32('HAF47');

drop schema db_crc32 cascade;
reset current_schema;
