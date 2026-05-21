create schema mod_zero;
set current_schema to 'mod_zero';

select 5 % 0;
select int1mod(3, 0);
select int2mod(3, 0);
select int4mod(3, 0);
select int8mod(3, 0);
select numeric_mod(1234.5678, 0.0);

drop schema mod_zero cascade;
