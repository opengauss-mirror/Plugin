drop database if exists greatest_least;
create database greatest_least dbcompatibility 'b';
\c greatest_least
--return null if input include null
select GREATEST(null,1,2), GREATEST(null,1,2) is null;
select GREATEST(1,2);
--return null if input include null
select LEAST(null,1,2), LEAST(null,1,2) is null;
select LEAST(1,2);

\c postgres
drop database if exists greatest_least;
