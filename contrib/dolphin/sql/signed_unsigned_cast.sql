create schema signed_unsigned_cast;
set current_schema to 'signed_unsigned_cast';
select cast(1-2 as unsigned);
select cast(3-5 as signed);
select cast(cast(1 - 5 as signed) as unsigned);
select cast(cast(1 + 5 as unsigned) as signed);
drop schema signed_unsigned_cast cascade;
reset current_schema;