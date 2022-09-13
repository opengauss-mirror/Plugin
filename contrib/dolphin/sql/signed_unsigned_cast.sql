drop database if exists signed_unsigned_cast;
create database signed_unsigned_cast dbcompatibility 'b';
\c signed_unsigned_cast
select cast(1-2 as unsigned);
select cast(3-5 as signed);
select cast(cast(1 - 5 as signed) as unsigned);
select cast(cast(1 + 5 as unsigned) as signed);
\c postgres
drop database signed_unsigned_cast;