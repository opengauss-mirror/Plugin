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

drop schema make_set cascade;
reset current_schema;

