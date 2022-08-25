
drop database if exists db_func_3;
create database db_func_3 dbcompatibility 'B';
\c db_func_3

CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  CONTAINS SQL AS $$ select 1 $$ ;


CREATE FUNCTION f2 (s int) RETURNS int CONTAINS SQL DETERMINISTIC  AS $$ select s; $$ ;


CREATE FUNCTION f3 (s int) RETURNS int  CONTAINS SQL LANGUAGE SQL AS $$ select s; $$ ;


CREATE FUNCTION f4 (s int) RETURNS int  CONTAINS SQL LANGUAGE SQL AS $$ select s; $$ ;

call f1(1);

call f2(2);

call f3(3);

call f4(4);

drop function f1;

drop function f2;

drop function f3;

drop function f4;

CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  CONTAINS SQL NOT DETERMINISTIC  AS $$ select 1 $$ ;


CREATE FUNCTION f2 (s int) RETURNS int CONTAINS SQL NO SQL   AS $$ select s; $$ ;


CREATE FUNCTION f3 (s int) RETURNS int  CONTAINS SQL LANGUAGE SQL NO SQL AS $$ select s; $$ ;


CREATE FUNCTION f4 (s int) RETURNS int  CONTAINS SQL LANGUAGE SQL DETERMINISTIC AS $$ select s; $$ ;

call f1(1);

call f2(2);

call f3(3);

call f4(4);

drop function f1;

drop function f2;

drop function f3;

drop function f4;

CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  CONTAINS SQL NOT DETERMINISTIC  READS SQL DATA AS $$ select 1 $$ ;


CREATE FUNCTION f2 (s int) RETURNS int CONTAINS SQL NO SQL READS SQL DATA  AS $$ select s; $$ ;


CREATE FUNCTION f3 (s int) RETURNS int  CONTAINS SQL LANGUAGE SQL NO SQL  MODIFIES SQL DATA AS $$ select s; $$ ;


CREATE FUNCTION f4 (s int) RETURNS int  CONTAINS SQL LANGUAGE SQL DETERMINISTIC MODIFIES SQL DATA AS $$ select s; $$ ;

call f1(1);

call f2(2);

call f3(3);

call f4(4);

\c postgres
drop database if exists db_func_3;

