create schema make_set;
set current_schema to 'make_set';
set dolphin.sql_mode = '';
select make_set(3, 'a', 'b', 'c');
select make_set(2,'a','b','c','d','e');
select make_set(1|4, 'hello', 'nice', 'world');
select make_set(1|4, 'hello', 'nice', NULL, 'world');
select make_set(0, 'a', 'b', 'c');
select make_set('', 'a', 'b', 'c');
select make_set(null, 'a', 'b', 'c');

select make_set(2147483649, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y');

select make_set(-1,'a','b','c','d','e');
select make_set(-2,'a','b','c','d','e');
select make_set(-3,'a','b','c','d','e');
select make_set(-4,'a','b','c','d','e');
select make_set(-5,'a','b','c','d','e');
select make_set(-6,'a','b','c','d','e');
select make_set(-7,'a','b','c','d','e');

select make_set(3,-1,4);

select make_set(7,-1,&#!#!@,'asfjoajfowqeknf,zncfiwuhrqnsdfhkasf');

select make_set(3,-1,&#!#!@);

select make_set(3,-1,'afasfasdfdsafsafasfdasdfsadfowijfqwjfowqfhasdhfsaqufhaksdfhskadfhasiufhakjfdahfiuwhfasjlajfowifjawefwfa');

select make_set(4294967296*1024*1024*1024,'1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66');

select make_set(-4294967296*1024*1024*1024,'1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66');

select make_set(3, true, false);
select make_set(3,01/02/03, false, true, false);

set dolphin.b_compatibility_mode = on;
select castcontext from pg_cast where castsource=(select oid from pg_type where typname = 'tinyblob') and casttarget= (select oid from pg_type where typname = 'anyset');
drop table if exists t_converse_case0001;
create table t_converse_case0001(c1 set('6heWuEd1rYf!@#', '1', '0', '2022-01-01', '2022-01-01 23:59:59', '010110', '73'));

insert into t_converse_case0001 values('6heWuEd1rYf!@#'::tinyblob);
insert into t_converse_case0001 values('1'::tinyblob);
insert into t_converse_case0001 values('2022-01-01'::tinyblob);
insert into t_converse_case0001 values('2022-01-01 11:11:11'::tinyblob);
insert into t_converse_case0001 values('0'::tinyblob);
insert into t_converse_case0001 values('73'::tinyblob);
insert into t_converse_case0001 values('010110'::tinyblob);

select * from t_converse_case0001;

reset dolphin.b_compatibility_mode;
drop table if exists t_converse_case0001;

drop schema make_set cascade;
reset current_schema;

