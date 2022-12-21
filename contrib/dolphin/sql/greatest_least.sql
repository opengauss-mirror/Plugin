create schema greatest_least;
set current_schema to 'greatest_least';
--return null if input include null
select GREATEST(null,1,2), GREATEST(null,1,2) is null;
select GREATEST(1,2);
--return null if input include null
select LEAST(null,1,2), LEAST(null,1,2) is null;
select LEAST(1,2);

drop schema greatest_least cascade;
reset current_schema;
