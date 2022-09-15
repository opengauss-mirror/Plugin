drop database if exists test_quote;
create database test_quote dbcompatibility'B';
\c test_quote
select json_quote(E'a\tb');
select json_quote('a    b');
select json_quote('"ab"');
select json_quote('a
b');
select json_quote(null);
select json_quote(E'ab\'');
select json_quote(E'\'ab\'');
select json_quote(''ab'');
select json_quote('\"ab\"');
select json_quote('');
select json_quote(ab);
select json_quote('~!ada221');
select json_quote('null');
create table student(name VARCHAR(20) PRIMARY KEY);
insert into student (name) value(json_quote('lc'));
select name from student;
drop table student;
\c postgres
drop database test_quote;


