drop database if exists db_cast;
create database db_cast dbcompatibility 'B';
\c db_cast

select cast('$2'::money as unsigned);
select cast(cast('$2'::money as unsigned) as money);

select current_timestamp;
select cast('2022-11-10 18:03:20'::timestamp as unsigned);
select cast(current_timestamp::timestamp as unsigned);
select cast(cast('2022-11-10 18:03:20'::timestamp as unsigned) as timestamp);

\c postgres
drop database if exists db_cast;
