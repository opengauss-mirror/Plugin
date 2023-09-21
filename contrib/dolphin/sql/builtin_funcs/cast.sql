create schema db_cast;
set current_schema to 'db_cast';

select cast('$2'::money as unsigned);
select cast(cast('$2'::money as unsigned) as money);

select current_timestamp;
select cast('2022-11-10 18:03:20'::timestamp as unsigned);
select cast(current_timestamp::timestamp as unsigned);
select cast(cast('2022-11-10 18:03:20'::timestamp as unsigned) as timestamp);

SELECT CAST('test' AS CHAR CHARACTER SET utf8);
SELECT CAST('test' AS CHAR CHARACTER SET not_exist);
SELECT CAST('test' AS nchar CHARACTER SET not_exist);

select 2.5::float::int16;
select 2.4::float::int16;
select 2.4::double::int16;
select 2.5::double::int16;

set dolphin.b_compatibility_mode to on;
select CAST(9/2 as char);
select CAST('0.12000' AS CHAR);
select CAST('1.203010' AS CHAR);
select CAST('-0.20' AS CHAR);
select CAST(2.090 AS char);
select CAST('2.090'::float AS char);
select CAST('4.100000'::double precision AS char);
select CAST(-6.40100 as char);

select 0+CAST(9/2 as char);
SELECT 0+CAST('0.12000' AS CHAR),0+CAST('1.203010' AS CHAR),0+CAST('-0.20' AS CHAR);
SELECT (0 + CAST(2.090 AS char))/2;
SELECT (0 + CAST('2.090'::float AS char))/2;
SELECT (0 + CAST('4.100000'::double precision AS char))/2;
select 0+CAST(-6.40100 as char) as res;

drop schema db_cast cascade;
reset current_schema;
