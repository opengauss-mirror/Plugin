-- b compatibility case
drop database if exists db_test_condition;
-- create database db_test_condition dbcompatibility 'b';
create database db_test_condition with DBCOMPATIBILITY = 'B';
\c db_test_condition
set dolphin.sql_mode = '';
create table test_bccf (t1 int ,t2 float, t3 char, t4 text);
insert into test_bccf values(1,3,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(1,null,'b','asdf');
insert into test_bccf values(3,2.0,'b','sdf');
insert into test_bccf values(4,6.1,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(5,null,'b','asdf');
insert into test_bccf values(6,6.0,'b','sdf');
insert into test_bccf values(7,9,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(1,null,'b','asdf');
insert into test_bccf values(1,4.0,'b','sdf');
insert into test_bccf values(1,7.1,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(11,null,'b','asdf');
insert into test_bccf values(10,6.0,'b','sdf');
insert into test_bccf values(11,14.6,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(13,null,'b','asdf');
insert into test_bccf values(12,2.0,'b','sdf');
insert into test_bccf values(9,2.2,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(8,null,'b','asdf');
insert into test_bccf values(7,2.3,'b','sdf');
insert into test_bccf values(123,3,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(51,null,'b','asdf');
insert into test_bccf values(73,72.0,'b','sdf');
insert into test_bccf values(84,96.1,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(5,null,'b','asdf');
insert into test_bccf values(6,6.7,'b','sdf');
insert into test_bccf values(7,null,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(1,null,'b','asdf');
insert into test_bccf values(2,4.0,'b','sdf');
insert into test_bccf values(1,7.1,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(13,null,'b','asdf');
insert into test_bccf values(10,6.7,'b','sdf');
insert into test_bccf values(11,24.6,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(13,null,'b','asdf');
insert into test_bccf values(12,2.7,'b','sdf');
insert into test_bccf values(9,12.2,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(8,null,'b','asdf');
insert into test_bccf values(7,42.3,'b','sdf');

----------------Coalesce function----------------------
select coalesce(null, null);
select coalesce(null, 'other');
select coalesce('something','other');
select coalesce(null,321);
select coalesce(null, null,123,'321');
select coalesce(null, null,123::int2,321::int2);
select coalesce(null, null,123::int4,321::int4);
select coalesce(null, null,123::int8,321::int8);
select coalesce(null, null,123::float4,321::float4);
select coalesce(null, null,null,123::float8,321::float8);
select coalesce(null, null,null,''::text,'hello!'::text);
select coalesce(null, null,null,'100.001'::numeric,134);
select coalesce(null, null,null,'2001-01-01 01:01:01.3654'::date,'2012-08-02 15:57:54.6365'::date);
select coalesce(null, null,null,'2001-01-01 01:01:01.3654'::timestamp,'2012-08-02 15:57:54.6365'::timestamp);
select coalesce(null, null);
select coalesce(null, 'other');
select coalesce('something','other');
select coalesce(null,321);
select coalesce(123,'321');
select coalesce(123::int2,null, null,321::int2);
select coalesce(123::int4,null, null,321::int4);
select coalesce(123::int8,null, null,321::int8);
select coalesce(123::float4,null, null,321::float4);
select coalesce(123::float8,null, null,321::float8);
select coalesce(null, null,'2001-01-01 01:01:01.3654'::date,'2012-08-02 15:57:54.6365'::date);
select coalesce('2001-01-01 01:01:01.3654'::timestamp,null, null,'2012-08-02 15:57:54.6365'::timestamp);
SELECT coalesce(null,1, 'k');
SELECT coalesce(null,1, '6');
SELECT coalesce(null,'j', '');
SELECT coalesce(null,'j', null);
SELECT coalesce(null,'', 'k');
SELECT coalesce(null, 'k');
SELECT coalesce(null,t1, 12) from test_bccf;
SELECT coalesce(null,t2, 213.4) from test_bccf;
SELECT coalesce(null,t3, 'c') from test_bccf;
SELECT coalesce(null,t4, 'hello') from test_bccf;
SELECT coalesce(null,t1,t2) from test_bccf;
SELECT coalesce(null,t2,t1) from test_bccf;

----------------IfNULL function------------------------
select ifnull(null, null);
select ifnull(null, 'other');
select ifnull('something','other');
select ifnull(null,321);
select ifnull(123,'321');
select ifnull(123::int2,321::int2);
select ifnull(123::int4,321::int4);
select ifnull(123::int8,321::int8);
select ifnull(123::float4,321::float4);
select ifnull(123::float8,321::float8);
select ifnull('2001-01-01 01:01:01.3654'::date,'2012-08-02 15:57:54.6365'::date);
select ifnull('2001-01-01 01:01:01.3654'::timestamp,'2012-08-02 15:57:54.6365'::timestamp);


--ifnull(unknown, int)
SELECT ifnull('j', 1);
--ifnull(int, unknown)
SELECT ifnull(1, 'k');
SELECT ifnull(1, '6');
--ifnull(char, int)
SELECT ifnull('o'::char, 1);
--ifnull(int, char)
SELECT ifnull(1, 'o'::char);
--ifnull(text, int2)
SELECT ifnull('hello!'::text, '100'::int2);
SELECT ifnull('100'::int2, 'hello!'::text);
--ifnull(text, int4)
SELECT ifnull('hello!'::text, '100'::int4);
SELECT ifnull('100'::int4, 'hello!'::text);
--ifnull(text, int8)
SELECT ifnull('hello!'::text, '100'::int8);
SELECT ifnull('100'::int8, 'hello!'::text);
--ifnull(text, float4)
SELECT ifnull('hello!'::text, '100.001'::float4);
SELECT ifnull('100.001'::float4, 'hello!'::text);
--ifnull(text, float8)
SELECT ifnull('hello!'::text, '100.001'::float8);
SELECT ifnull('100.001'::float8, 'hello!'::text);
--ifnull(text, number)
SELECT ifnull('hello!'::text, '100.001'::numeric);
SELECT ifnull('100.001'::numeric, 'hello!'::text);

--null
SELECT ifnull('j', '');
SELECT ifnull('j', null);
SELECT ifnull('', 'k');
SELECT ifnull(null, 'k');
SELECT ifnull(''::text, '100'::int2);
SELECT ifnull(''::int2, 'hello!'::text)L;
SELECT ifnull('hello!'::text, ''::int2);
SELECT ifnull('100'::int2, ''::text);
SELECT ifnull(''::text, '100.001'::float8);
SELECT ifnull('100.001'::float8, ''::text);
SELECT ifnull(''::text, '100.001'::numeric);
SELECT ifnull(''::numeric, 'hello!'::text);
SELECT ifnull('hello!'::text, ''::numeric);
SELECT ifnull('100.001'::numeric, ''::text);

select ifnull(t1,t2) from test_bccf;
select ifnull(t1,t3) from test_bccf;
select ifnull(t1,t4) from test_bccf;
select ifnull(t3,t4) from test_bccf;

----------------ISNULL expression------------------------

--null
SELECT isnull(null);
SELECT isnull('');
SELECT isnull(''::text);
SELECT isnull(''::char);
SELECT isnull(''::int2);
SELECT isnull(''::int4);
SELECT isnull(''::int8);
SELECT isnull(''::numeric);
SELECT isnull('j');
SELECT isnull('hello!'::text);
SELECT isnull('100'::int2);
SELECT isnull('100.001'::float8);
SELECT isnull('100.001'::numeric);


select * from test_bccf where isnull(t1) order by 1, 2;
select * from test_bccf where isnull(t2) order by 1, 2;
select * from test_bccf where isnull(t3) order by 1, 2;
select * from test_bccf where isnull(t4) order by 1, 2;
select * from test_bccf where isnull(t1) and t2 is not NULL order by 1, 2;
select * from test_bccf where isnull(t2) and t3 is not NULL order by 1, 2;
select * from test_bccf where isnull(t3) and t4 is not NULL order by 1, 2;
select * from test_bccf where isnull(t4) and t1 is not NULL order by 1, 2;



----------------STRCMP expression------------------------
select strcmp(null, null);
select strcmp(null, 'other');
select strcmp('other', null);
select strcmp('something','other');
select strcmp('something','something');
select strcmp(null, 321);
select strcmp(321, null);
select strcmp(321, 321);
select strcmp(321, 123);
select strcmp(null, 321::int2);
select strcmp(321::int2, null);
select strcmp(321::int2, 321::int2);
select strcmp(321::int2, 123::int2);
select strcmp(null, 321::int4);
select strcmp(321::int4, null);
select strcmp(321::int4, 321::int4);
select strcmp(321::int4, 123::int4);
select strcmp(null, 321::int8);
select strcmp(321::int8, null);
select strcmp(321::int8, 321::int8);
select strcmp(321::int8, 123::int8);
select strcmp(null, 321::float4);
select strcmp(321::float4, null);
select strcmp(321::float4, 321::float4);
select strcmp(321::float4, 123::float4);
select strcmp(null, 321::float8);
select strcmp(321::float8, null);
select strcmp(321::float8, 321::float8);
select strcmp(321::float8, 123::float8);

select strcmp(null,'something'::text);
select strcmp('something'::text,null);
select strcmp('something'::text,'something'::text);
select strcmp('something'::text,'other'::text);

select strcmp(null, '2001-01-01 01:01:01.3654'::date);
select strcmp('2001-01-01 01:01:01.3654'::date, null);
select strcmp('2001-01-01 01:01:01.3654'::date, '2001-01-01 01:01:01.3654'::date);
select strcmp('2001-01-01 01:01:01.3654'::date, '1999-01-01 01:01:01.3654'::date);

SELECT strcmp(t1, 12) from test_bccf;
SELECT strcmp(t2, 213.4) from test_bccf;
SELECT strcmp(t3, 'c') from test_bccf;
SELECT strcmp(t4, 'hello') from test_bccf;
SELECT strcmp(t1,t2) from test_bccf;
SELECT strcmp(t2,t1) from test_bccf;




----------------interval expression------------------------
select interval(null);
select interval(null, 123);
select interval(null, 123.45);
select interval(123,null);
select interval(123,null,17,43.123);
select interval(123.213,null,17,43.123);
select interval(100, '10e2');
select interval(100, '   10e1');
select interval(100, '   10e1', '      10Eeasd');
select interval(10e3, '   10e1', '      10E1easd    ', '2e10');
select interval(0, '   wa');
select interval(0, '   wa', '  e1212');
select interval(0, '   abiusdhiw');
select interval(0, '   abiusdhiw', '   kqiwjoiqh  ');
select interval(0, '   1e12abiusdhiw');
select interval(0, '   e13fsdsda');
select interval(0, '1e13fsdsda');
select interval(100, '1e1fsdsda');
select interval('+12', 121);
select interval('+12', 12);
select interval('+12', 0);
select interval('-12', 0);
select interval('-12.2', 0);
select interval('-.2', 0);
select interval('-.2', -100);
select interval('-.2E2', -100);
select interval('-.2E4', -100);
select interval('..', 21);
select interval('.21.', 21);
select interval('.21.', 0);
select interval('+.21.', 0);
select interval('-.21.', 0);
select interval('-.21.', -1);
select interval('-..21.', -1);
select interval('...', -1);
select interval('...', -1);


select interval(123::int2,null);
select interval(123::int4,null);
select interval(123::int8,null);
select interval(123,123.23);
select interval(123,123.23::int);

select interval(null, '2001-01-01 01:01:01.3654'::date);
select interval('2001-01-01 01:01:01.3654'::date, null);
select interval('2001-01-01 01:01:01.3654'::date, '2001-01-01 01:01:01.3654'::date);
select interval('2001-01-01 01:01:01.3654'::date, '1999-01-01 01:01:01.3654'::date);
select interval('2022-12-12'::timestamp,123);
select interval('2022-12-12'::timestamp,'2022-12-12'::timestamp);
select interval(123,'2022-12-12'::timestamp);
select interval(1,' ');
select interval(b'1',' ');
select interval('a',' ');
select interval(false,' ');
select interval(' ',true);
select interval(' ',b'1');
select interval(' ','a');
select interval(' ',12);

select interval(t1, t2) from test_bccf;
select interval(t2, t1) from test_bccf;

drop table test_bccf;


create table test_bccf (t1 int ,t2 float, t3 char, t4 text) WITH (ORIENTATION = COLUMN);
insert into test_bccf values(1,3,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(1,null,'b','asdf');
insert into test_bccf values(3,2.0,'b','sdf');
insert into test_bccf values(4,6.1,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(5,null,'b','asdf');
insert into test_bccf values(6,6.0,'b','sdf');
insert into test_bccf values(7,9,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(1,null,'b','asdf');
insert into test_bccf values(1,4.0,'b','sdf');
insert into test_bccf values(1,7.1,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(11,null,'b','asdf');
insert into test_bccf values(10,6.0,'b','sdf');
insert into test_bccf values(11,14.6,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(13,null,'b','asdf');
insert into test_bccf values(12,2.0,'b','sdf');
insert into test_bccf values(9,2.2,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(8,null,'b','asdf');
insert into test_bccf values(7,2.3,'b','sdf');
insert into test_bccf values(123,3,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(51,null,'b','asdf');
insert into test_bccf values(73,72.0,'b','sdf');
insert into test_bccf values(84,96.1,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(5,null,'b','asdf');
insert into test_bccf values(6,6.7,'b','sdf');
insert into test_bccf values(7,null,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(1,null,'b','asdf');
insert into test_bccf values(2,4.0,'b','sdf');
insert into test_bccf values(1,7.1,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(13,null,'b','asdf');
insert into test_bccf values(10,6.7,'b','sdf');
insert into test_bccf values(11,24.6,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(13,null,'b','asdf');
insert into test_bccf values(12,2.7,'b','sdf');
insert into test_bccf values(9,12.2,null,null);
insert into test_bccf values(null,null,'c',null);
insert into test_bccf values(8,null,'b','asdf');
insert into test_bccf values(7,42.3,'b','sdf');

----------------Coalesce function----------------------
select coalesce(null, null);
select coalesce(null, 'other');
select coalesce('something','other');
select coalesce(null,321);
select coalesce(null, null,123,'321');
select coalesce(null, null,123::int2,321::int2);
select coalesce(null, null,123::int4,321::int4);
select coalesce(null, null,123::int8,321::int8);
select coalesce(null, null,123::float4,321::float4);
select coalesce(null, null,null,123::float8,321::float8);
select coalesce(null, null,null,''::text,'hello!'::text);
select coalesce(null, null,null,'100.001'::numeric,134);
select coalesce(null, null,null,'2001-01-01 01:01:01.3654'::date,'2012-08-02 15:57:54.6365'::date);
select coalesce(null, null,null,'2001-01-01 01:01:01.3654'::timestamp,'2012-08-02 15:57:54.6365'::timestamp);
select coalesce(null, null);
select coalesce(null, 'other');
select coalesce('something','other');
select coalesce(null,321);
select coalesce(123,'321');
select coalesce(123::int2,null, null,321::int2);
select coalesce(123::int4,null, null,321::int4);
select coalesce(123::int8,null, null,321::int8);
select coalesce(123::float4,null, null,321::float4);
select coalesce(123::float8,null, null,321::float8);
select coalesce(null, null,'2001-01-01 01:01:01.3654'::date,'2012-08-02 15:57:54.6365'::date);
select coalesce('2001-01-01 01:01:01.3654'::timestamp,null, null,'2012-08-02 15:57:54.6365'::timestamp);
SELECT coalesce(null,1, 'k');
SELECT coalesce(null,1, '6');
SELECT coalesce(null,'j', '');
SELECT coalesce(null,'j', null);
SELECT coalesce(null,'', 'k');
SELECT coalesce(null, 'k');
SELECT coalesce(null,t1, 12) from test_bccf;
SELECT coalesce(null,t2, 213.4) from test_bccf;
SELECT coalesce(null,t3, 'c') from test_bccf;
SELECT coalesce(null,t4, 'hello') from test_bccf;
SELECT coalesce(null,t1,t2) from test_bccf;
SELECT coalesce(null,t2,t1) from test_bccf;

----------------IfNULL function------------------------
select ifnull(null, null);
select ifnull(null, 'other');
select ifnull('something','other');
select ifnull(null,321);
select ifnull(123,'321');
select ifnull(123::int2,321::int2);
select ifnull(123::int4,321::int4);
select ifnull(123::int8,321::int8);
select ifnull(123::float4,321::float4);
select ifnull(123::float8,321::float8);
select ifnull('2001-01-01 01:01:01.3654'::date,'2012-08-02 15:57:54.6365'::date);
select ifnull('2001-01-01 01:01:01.3654'::timestamp,'2012-08-02 15:57:54.6365'::timestamp);


--ifnull(unknown, int)
SELECT ifnull('j', 1);
--ifnull(int, unknown)
SELECT ifnull(1, 'k');
SELECT ifnull(1, '6');
--ifnull(char, int)
SELECT ifnull('o'::char, 1);
--ifnull(int, char)
SELECT ifnull(1, 'o'::char);
--ifnull(text, int2)
SELECT ifnull('hello!'::text, '100'::int2);
SELECT ifnull('100'::int2, 'hello!'::text);
--ifnull(text, int4)
SELECT ifnull('hello!'::text, '100'::int4);
SELECT ifnull('100'::int4, 'hello!'::text);
--ifnull(text, int8)
SELECT ifnull('hello!'::text, '100'::int8);
SELECT ifnull('100'::int8, 'hello!'::text);
--ifnull(text, float4)
SELECT ifnull('hello!'::text, '100.001'::float4);
SELECT ifnull('100.001'::float4, 'hello!'::text);
--ifnull(text, float8)
SELECT ifnull('hello!'::text, '100.001'::float8);
SELECT ifnull('100.001'::float8, 'hello!'::text);
--ifnull(text, number)
SELECT ifnull('hello!'::text, '100.001'::numeric);
SELECT ifnull('100.001'::numeric, 'hello!'::text);

--null
SELECT ifnull('j', '');
SELECT ifnull('j', null);
SELECT ifnull('', 'k');
SELECT ifnull(null, 'k');
SELECT ifnull(''::text, '100'::int2);
SELECT ifnull(''::int2, 'hello!'::text)L;
SELECT ifnull('hello!'::text, ''::int2);
SELECT ifnull('100'::int2, ''::text);
SELECT ifnull(''::text, '100.001'::float8);
SELECT ifnull('100.001'::float8, ''::text);
SELECT ifnull(''::text, '100.001'::numeric);
SELECT ifnull(''::numeric, 'hello!'::text);
SELECT ifnull('hello!'::text, ''::numeric);
SELECT ifnull('100.001'::numeric, ''::text);

select ifnull(t1,t2) from test_bccf;
select ifnull(t1,t3) from test_bccf;
select ifnull(t1,t4) from test_bccf;
select ifnull(t3,t4) from test_bccf;

----------------ISNULL expression------------------------

--null
SELECT isnull(null);
SELECT isnull('');
SELECT isnull(''::text);
SELECT isnull(''::char);
SELECT isnull(''::int2);
SELECT isnull(''::int4);
SELECT isnull(''::int8);
SELECT isnull(''::numeric);
SELECT isnull('j');
SELECT isnull('hello!'::text);
SELECT isnull('100'::int2);
SELECT isnull('100.001'::float8);
SELECT isnull('100.001'::numeric);


select * from test_bccf where isnull(t1) order by 1, 2;
select * from test_bccf where isnull(t2) order by 1, 2;
select * from test_bccf where isnull(t3) order by 1, 2;
select * from test_bccf where isnull(t4) order by 1, 2;
select * from test_bccf where isnull(t1) and t2 is not NULL order by 1, 2;
select * from test_bccf where isnull(t2) and t3 is not NULL order by 1, 2;
select * from test_bccf where isnull(t3) and t4 is not NULL order by 1, 2;
select * from test_bccf where isnull(t4) and t1 is not NULL order by 1, 2;



----------------STRCMP expression------------------------
select strcmp(null, null);
select strcmp(null, 'other');
select strcmp('other', null);
select strcmp('something','other');
select strcmp('something','something');
select strcmp(null, 321);
select strcmp(321, null);
select strcmp(321, 321);
select strcmp(321, 123);
select strcmp(null, 321::int2);
select strcmp(321::int2, null);
select strcmp(321::int2, 321::int2);
select strcmp(321::int2, 123::int2);
select strcmp(null, 321::int4);
select strcmp(321::int4, null);
select strcmp(321::int4, 321::int4);
select strcmp(321::int4, 123::int4);
select strcmp(null, 321::int8);
select strcmp(321::int8, null);
select strcmp(321::int8, 321::int8);
select strcmp(321::int8, 123::int8);
select strcmp(null, 321::float4);
select strcmp(321::float4, null);
select strcmp(321::float4, 321::float4);
select strcmp(321::float4, 123::float4);
select strcmp(null, 321::float8);
select strcmp(321::float8, null);
select strcmp(321::float8, 321::float8);
select strcmp(321::float8, 123::float8);

select strcmp(null,'something'::text);
select strcmp('something'::text,null);
select strcmp('something'::text,'something'::text);
select strcmp('something'::text,'other'::text);

select strcmp(null, '2001-01-01 01:01:01.3654'::date);
select strcmp('2001-01-01 01:01:01.3654'::date, null);
select strcmp('2001-01-01 01:01:01.3654'::date, '2001-01-01 01:01:01.3654'::date);
select strcmp('2001-01-01 01:01:01.3654'::date, '1999-01-01 01:01:01.3654'::date);

SELECT strcmp(t1, 12) from test_bccf;
SELECT strcmp(t2, 213.4) from test_bccf;
SELECT strcmp(t3, 'c') from test_bccf;
SELECT strcmp(t4, 'hello') from test_bccf;
SELECT strcmp(t1,t2) from test_bccf;
SELECT strcmp(t2,t1) from test_bccf;

----------------interval expression------------------------
select interval(null);
select interval(null, 123);
select interval(null, 123.45);
select interval(123,null);
select interval(123,null,17,43.123);
select interval(123.213,null,17,43.123);
select interval(100, '10e2');
select interval(100, '   10e1');
select interval(100, '   10e1', '      10Eeasd');
select interval(10e3, '   10e1', '      10E1easd    ', '2e10');
select interval(0, '   wa');
select interval(0, '   wa', '  e1212');
select interval(0, '   abiusdhiw');
select interval(0, '   abiusdhiw', '   kqiwjoiqh  ');
select interval(0, '   1e12abiusdhiw');
select interval(0, '   e13fsdsda');
select interval(0, '1e13fsdsda');
select interval(100, '1e1fsdsda');
select interval('+12', 121);
select interval('+12', 12);
select interval('+12', 0);
select interval('-12', 0);
select interval('-12.2', 0);
select interval('-.2', 0);
select interval('-.2', -100);
select interval('-.2E2', -100);
select interval('-.2E4', -100);
select interval('..', 21);
select interval('.21.', 21);
select interval('.21.', 0);
select interval('+.21.', 0);
select interval('-.21.', 0);
select interval('-.21.', -1);
select interval('-..21.', -1);
select interval('...', -1);
select interval('...', -1);

select interval(123::int2,null);
select interval(123::int4,null);
select interval(123::int8,null);
select interval(123,123.23);
select interval(123,123.23::int);

select interval(null, '2001-01-01 01:01:01.3654'::date);
select interval('2001-01-01 01:01:01.3654'::date, null);
select interval('2001-01-01 01:01:01.3654'::date, '2001-01-01 01:01:01.3654'::date);
select interval('2001-01-01 01:01:01.3654'::date, '1999-01-01 01:01:01.3654'::date);
select interval('2022-12-12'::timestamp,123);
select interval('2022-12-12'::timestamp,'2022-12-12'::timestamp);
select interval(123,'2022-12-12'::timestamp);
select interval(1,' ');
select interval(b'1',' ');
select interval('a',' ');
select interval(false,' ');
select interval(' ',true);
select interval(' ',b'1');
select interval(' ','a');
select interval(' ',12);
select interval(t1, t2) from test_bccf;
select interval(t2, t1) from test_bccf;

drop table test_bccf;


--Type Conversion Test--
drop table if exists typeset;
create table typeset (
	tyint TINYINT not null,
	smint SMALLINT not null,
	anint INT not null,
	bgint BIGINT not null,
	dcmal DECIMAL not null,
	nmric NUMERIC not null,
	flt   FLOAT not null,
	bt    BIT(8) not null,
	dt    DATE not null,
	tmstp TIMESTAMP not null,
	tm    TIME not null,
	ch    CHAR(30) not null,
	vch   VARCHAR(30) not null,
	blb   BLOB not null,
	txt   TEXT not null
);

insert into typeset (
tyint,smint,anint,bgint,dcmal,nmric,flt,
bt,dt,tmstp,tm,
ch,vch,blb,
txt 
) values (
127, 127, 127, 127, 127.234, 127.32, 127.213,
b'01111111', '2001-04-19','2001-04-19', '22:23:44',
'2001-04-19 22:23:44', '2001-04-19 22:23:44', '1233454212',
'2001-04-19 22:23:44'
);


select coalesce(tyint, smint) from typeset;
select coalesce(tyint, anint) from typeset;
select coalesce(tyint, bgint) from typeset;
select coalesce(tyint, dcmal) from typeset;
select coalesce(tyint, nmric) from typeset;
select coalesce(tyint, flt) from typeset;
select coalesce(tyint, bt) from typeset;
select coalesce(tyint, dt) from typeset;
select coalesce(tyint, tmstp) from typeset;
select coalesce(tyint, tm) from typeset;
select coalesce(tyint, ch) from typeset;
select coalesce(tyint, vch) from typeset;
select coalesce(tyint, blb) from typeset;
select coalesce(tyint, txt) from typeset;
select coalesce(smint, anint) from typeset;
select coalesce(smint, bgint) from typeset;
select coalesce(smint, dcmal) from typeset;
select coalesce(smint, nmric) from typeset;
select coalesce(smint, flt) from typeset;
select coalesce(smint, bt) from typeset;
select coalesce(smint, dt) from typeset;
select coalesce(smint, tmstp) from typeset;
select coalesce(smint, tm) from typeset;
select coalesce(smint, ch) from typeset;
select coalesce(smint, vch) from typeset;
select coalesce(smint, blb) from typeset;
select coalesce(smint, txt) from typeset;
select coalesce(anint, bgint) from typeset;
select coalesce(anint, dcmal) from typeset;
select coalesce(anint, nmric) from typeset;
select coalesce(anint, flt) from typeset;
select coalesce(anint, bt) from typeset;
select coalesce(anint, dt) from typeset;
select coalesce(anint, tmstp) from typeset;
select coalesce(anint, tm) from typeset;
select coalesce(anint, ch) from typeset;
select coalesce(anint, vch) from typeset;
select coalesce(anint, blb) from typeset;
select coalesce(anint, txt) from typeset;
select coalesce(bgint, dcmal) from typeset;
select coalesce(bgint, nmric) from typeset;
select coalesce(bgint, flt) from typeset;
select coalesce(bgint, bt) from typeset;
select coalesce(bgint, dt) from typeset;
select coalesce(bgint, tmstp) from typeset;
select coalesce(bgint, tm) from typeset;
select coalesce(bgint, ch) from typeset;
select coalesce(bgint, vch) from typeset;
select coalesce(bgint, blb) from typeset;
select coalesce(bgint, txt) from typeset;
select coalesce(dcmal, nmric) from typeset;
select coalesce(dcmal, flt) from typeset;
select coalesce(dcmal, bt) from typeset;
select coalesce(dcmal, dt) from typeset;
select coalesce(dcmal, tmstp) from typeset;
select coalesce(dcmal, tm) from typeset;
select coalesce(dcmal, ch) from typeset;
select coalesce(dcmal, vch) from typeset;
select coalesce(dcmal, blb) from typeset;
select coalesce(dcmal, txt) from typeset;
select coalesce(nmric, flt) from typeset;
select coalesce(nmric, bt) from typeset;
select coalesce(nmric, dt) from typeset;
select coalesce(nmric, tmstp) from typeset;
select coalesce(nmric, tm) from typeset;
select coalesce(nmric, ch) from typeset;
select coalesce(nmric, vch) from typeset;
select coalesce(nmric, blb) from typeset;
select coalesce(nmric, txt) from typeset;
select coalesce(flt, bt) from typeset;
select coalesce(flt, dt) from typeset;
select coalesce(flt, tmstp) from typeset;
select coalesce(flt, tm) from typeset;
select coalesce(flt, ch) from typeset;
select coalesce(flt, vch) from typeset;
select coalesce(flt, blb) from typeset;
select coalesce(flt, txt) from typeset;
select coalesce(bt, dt) from typeset;
select coalesce(bt, tmstp) from typeset;
select coalesce(bt, tm) from typeset;
select coalesce(bt, ch) from typeset;
select coalesce(bt, vch) from typeset;
select coalesce(bt, blb) from typeset;
select coalesce(bt, txt) from typeset;
select coalesce(dt, tmstp) from typeset;
select coalesce(dt, tm) from typeset;
select coalesce(dt, ch) from typeset;
select coalesce(dt, vch) from typeset;
select coalesce(dt, blb) from typeset;
select coalesce(dt, txt) from typeset;
select coalesce(tmstp, tm) from typeset;
select coalesce(tmstp, ch) from typeset;
select coalesce(tmstp, vch) from typeset;
select coalesce(tmstp, blb) from typeset;
select coalesce(tmstp, txt) from typeset;
select coalesce(tm, ch) from typeset;
select coalesce(tm, vch) from typeset;
select coalesce(tm, blb) from typeset;
select coalesce(tm, txt) from typeset;
select coalesce(ch, vch) from typeset;
select coalesce(ch, blb) from typeset;
select coalesce(ch, txt) from typeset;
select coalesce(vch, blb) from typeset;
select coalesce(vch, txt) from typeset;
select coalesce(blb, txt) from typeset;


select ifnull(tyint, smint) from typeset;
select ifnull(tyint, anint) from typeset;
select ifnull(tyint, bgint) from typeset;
select ifnull(tyint, dcmal) from typeset;
select ifnull(tyint, nmric) from typeset;
select ifnull(tyint, flt) from typeset;
select ifnull(tyint, bt) from typeset;
select ifnull(tyint, dt) from typeset;
select ifnull(tyint, tmstp) from typeset;
select ifnull(tyint, tm) from typeset;
select ifnull(tyint, ch) from typeset;
select ifnull(tyint, vch) from typeset;
select ifnull(tyint, blb) from typeset;
select ifnull(tyint, txt) from typeset;
select ifnull(smint, anint) from typeset;
select ifnull(smint, bgint) from typeset;
select ifnull(smint, dcmal) from typeset;
select ifnull(smint, nmric) from typeset;
select ifnull(smint, flt) from typeset;
select ifnull(smint, bt) from typeset;
select ifnull(smint, dt) from typeset;
select ifnull(smint, tmstp) from typeset;
select ifnull(smint, tm) from typeset;
select ifnull(smint, ch) from typeset;
select ifnull(smint, vch) from typeset;
select ifnull(smint, blb) from typeset;
select ifnull(smint, txt) from typeset;
select ifnull(anint, bgint) from typeset;
select ifnull(anint, dcmal) from typeset;
select ifnull(anint, nmric) from typeset;
select ifnull(anint, flt) from typeset;
select ifnull(anint, bt) from typeset;
select ifnull(anint, dt) from typeset;
select ifnull(anint, tmstp) from typeset;
select ifnull(anint, tm) from typeset;
select ifnull(anint, ch) from typeset;
select ifnull(anint, vch) from typeset;
select ifnull(anint, blb) from typeset;
select ifnull(anint, txt) from typeset;
select ifnull(bgint, dcmal) from typeset;
select ifnull(bgint, nmric) from typeset;
select ifnull(bgint, flt) from typeset;
select ifnull(bgint, bt) from typeset;
select ifnull(bgint, dt) from typeset;
select ifnull(bgint, tmstp) from typeset;
select ifnull(bgint, tm) from typeset;
select ifnull(bgint, ch) from typeset;
select ifnull(bgint, vch) from typeset;
select ifnull(bgint, blb) from typeset;
select ifnull(bgint, txt) from typeset;
select ifnull(dcmal, nmric) from typeset;
select ifnull(dcmal, flt) from typeset;
select ifnull(dcmal, bt) from typeset;
select ifnull(dcmal, dt) from typeset;
select ifnull(dcmal, tmstp) from typeset;
select ifnull(dcmal, tm) from typeset;
select ifnull(dcmal, ch) from typeset;
select ifnull(dcmal, vch) from typeset;
select ifnull(dcmal, blb) from typeset;
select ifnull(dcmal, txt) from typeset;
select ifnull(nmric, flt) from typeset;
select ifnull(nmric, bt) from typeset;
select ifnull(nmric, dt) from typeset;
select ifnull(nmric, tmstp) from typeset;
select ifnull(nmric, tm) from typeset;
select ifnull(nmric, ch) from typeset;
select ifnull(nmric, vch) from typeset;
select ifnull(nmric, blb) from typeset;
select ifnull(nmric, txt) from typeset;
select ifnull(flt, bt) from typeset;
select ifnull(flt, dt) from typeset;
select ifnull(flt, tmstp) from typeset;
select ifnull(flt, tm) from typeset;
select ifnull(flt, ch) from typeset;
select ifnull(flt, vch) from typeset;
select ifnull(flt, blb) from typeset;
select ifnull(flt, txt) from typeset;
select ifnull(bt, dt) from typeset;
select ifnull(bt, tmstp) from typeset;
select ifnull(bt, tm) from typeset;
select ifnull(bt, ch) from typeset;
select ifnull(bt, vch) from typeset;
select ifnull(bt, blb) from typeset;
select ifnull(bt, txt) from typeset;
select ifnull(dt, tmstp) from typeset;
select ifnull(dt, tm) from typeset;
select ifnull(dt, ch) from typeset;
select ifnull(dt, vch) from typeset;
select ifnull(dt, blb) from typeset;
select ifnull(dt, txt) from typeset;
select ifnull(tmstp, tm) from typeset;
select ifnull(tmstp, ch) from typeset;
select ifnull(tmstp, vch) from typeset;
select ifnull(tmstp, blb) from typeset;
select ifnull(tmstp, txt) from typeset;
select ifnull(tm, ch) from typeset;
select ifnull(tm, vch) from typeset;
select ifnull(tm, blb) from typeset;
select ifnull(tm, txt) from typeset;
select ifnull(ch, vch) from typeset;
select ifnull(ch, blb) from typeset;
select ifnull(ch, txt) from typeset;
select ifnull(vch, blb) from typeset;
select ifnull(vch, txt) from typeset;
select ifnull(blb, txt) from typeset;



select interval(tyint, smint) from typeset;
select interval(tyint, anint) from typeset;
select interval(tyint, bgint) from typeset;
select interval(tyint, dcmal) from typeset;
select interval(tyint, nmric) from typeset;
select interval(tyint, flt) from typeset;
select interval(tyint, bt) from typeset;
select interval(tyint, dt) from typeset;
select interval(tyint, tmstp) from typeset;
select interval(tyint, tm) from typeset;
select interval(tyint, ch) from typeset;
select interval(tyint, vch) from typeset;
select interval(tyint, blb) from typeset;
select interval(tyint, txt) from typeset;
select interval(smint, anint) from typeset;
select interval(smint, bgint) from typeset;
select interval(smint, dcmal) from typeset;
select interval(smint, nmric) from typeset;
select interval(smint, flt) from typeset;
select interval(smint, bt) from typeset;
select interval(smint, dt) from typeset;
select interval(smint, tmstp) from typeset;
select interval(smint, tm) from typeset;
select interval(smint, ch) from typeset;
select interval(smint, vch) from typeset;
select interval(smint, blb) from typeset;
select interval(smint, txt) from typeset;
select interval(anint, bgint) from typeset;
select interval(anint, dcmal) from typeset;
select interval(anint, nmric) from typeset;
select interval(anint, flt) from typeset;
select interval(anint, bt) from typeset;
select interval(anint, dt) from typeset;
select interval(anint, tmstp) from typeset;
select interval(anint, tm) from typeset;
select interval(anint, ch) from typeset;
select interval(anint, vch) from typeset;
select interval(anint, blb) from typeset;
select interval(anint, txt) from typeset;
select interval(bgint, dcmal) from typeset;
select interval(bgint, nmric) from typeset;
select interval(bgint, flt) from typeset;
select interval(bgint, bt) from typeset;
select interval(bgint, dt) from typeset;
select interval(bgint, tmstp) from typeset;
select interval(bgint, tm) from typeset;
select interval(bgint, ch) from typeset;
select interval(bgint, vch) from typeset;
select interval(bgint, blb) from typeset;
select interval(bgint, txt) from typeset;
select interval(dcmal, nmric) from typeset;
select interval(dcmal, flt) from typeset;
select interval(dcmal, bt) from typeset;
select interval(dcmal, dt) from typeset;
select interval(dcmal, tmstp) from typeset;
select interval(dcmal, tm) from typeset;
select interval(dcmal, ch) from typeset;
select interval(dcmal, vch) from typeset;
select interval(dcmal, blb) from typeset;
select interval(dcmal, txt) from typeset;
select interval(nmric, flt) from typeset;
select interval(nmric, bt) from typeset;
select interval(nmric, dt) from typeset;
select interval(nmric, tmstp) from typeset;
select interval(nmric, tm) from typeset;
select interval(nmric, ch) from typeset;
select interval(nmric, vch) from typeset;
select interval(nmric, blb) from typeset;
select interval(nmric, txt) from typeset;
select interval(flt, bt) from typeset;
select interval(flt, dt) from typeset;
select interval(flt, tmstp) from typeset;
select interval(flt, tm) from typeset;
select interval(flt, ch) from typeset;
select interval(flt, vch) from typeset;
select interval(flt, blb) from typeset;
select interval(flt, txt) from typeset;
select interval(bt, dt) from typeset;
select interval(bt, tmstp) from typeset;
select interval(bt, tm) from typeset;
select interval(bt, ch) from typeset;
select interval(bt, vch) from typeset;
select interval(bt, blb) from typeset;
select interval(bt, txt) from typeset;
select interval(dt, tmstp) from typeset;
select interval(dt, tm) from typeset;
select interval(dt, ch) from typeset;
select interval(dt, vch) from typeset;
select interval(dt, blb) from typeset;
select interval(dt, txt) from typeset;
select interval(tmstp, tm) from typeset;
select interval(tmstp, ch) from typeset;
select interval(tmstp, vch) from typeset;
select interval(tmstp, blb) from typeset;
select interval(tmstp, txt) from typeset;
select interval(tm, ch) from typeset;
select interval(tm, vch) from typeset;
select interval(tm, blb) from typeset;
select interval(tm, txt) from typeset;
select interval(ch, vch) from typeset;
select interval(ch, blb) from typeset;
select interval(ch, txt) from typeset;
select interval(vch, blb) from typeset;
select interval(vch, txt) from typeset;
select interval(blb, txt) from typeset;



select strcmp(tyint, smint) from typeset;
select strcmp(tyint, anint) from typeset;
select strcmp(tyint, bgint) from typeset;
select strcmp(tyint, dcmal) from typeset;
select strcmp(tyint, nmric) from typeset;
select strcmp(tyint, flt) from typeset;
select strcmp(tyint, bt) from typeset;
select strcmp(tyint, dt) from typeset;
select strcmp(tyint, tmstp) from typeset;
select strcmp(tyint, tm) from typeset;
select strcmp(tyint, ch) from typeset;
select strcmp(tyint, vch) from typeset;
select strcmp(tyint, blb) from typeset;
select strcmp(tyint, txt) from typeset;
select strcmp(smint, anint) from typeset;
select strcmp(smint, bgint) from typeset;
select strcmp(smint, dcmal) from typeset;
select strcmp(smint, nmric) from typeset;
select strcmp(smint, flt) from typeset;
select strcmp(smint, bt) from typeset;
select strcmp(smint, dt) from typeset;
select strcmp(smint, tmstp) from typeset;
select strcmp(smint, tm) from typeset;
select strcmp(smint, ch) from typeset;
select strcmp(smint, vch) from typeset;
select strcmp(smint, blb) from typeset;
select strcmp(smint, txt) from typeset;
select strcmp(anint, bgint) from typeset;
select strcmp(anint, dcmal) from typeset;
select strcmp(anint, nmric) from typeset;
select strcmp(anint, flt) from typeset;
select strcmp(anint, bt) from typeset;
select strcmp(anint, dt) from typeset;
select strcmp(anint, tmstp) from typeset;
select strcmp(anint, tm) from typeset;
select strcmp(anint, ch) from typeset;
select strcmp(anint, vch) from typeset;
select strcmp(anint, blb) from typeset;
select strcmp(anint, txt) from typeset;
select strcmp(bgint, dcmal) from typeset;
select strcmp(bgint, nmric) from typeset;
select strcmp(bgint, flt) from typeset;
select strcmp(bgint, bt) from typeset;
select strcmp(bgint, dt) from typeset;
select strcmp(bgint, tmstp) from typeset;
select strcmp(bgint, tm) from typeset;
select strcmp(bgint, ch) from typeset;
select strcmp(bgint, vch) from typeset;
select strcmp(bgint, blb) from typeset;
select strcmp(bgint, txt) from typeset;
select strcmp(dcmal, nmric) from typeset;
select strcmp(dcmal, flt) from typeset;
select strcmp(dcmal, bt) from typeset;
select strcmp(dcmal, dt) from typeset;
select strcmp(dcmal, tmstp) from typeset;
select strcmp(dcmal, tm) from typeset;
select strcmp(dcmal, ch) from typeset;
select strcmp(dcmal, vch) from typeset;
select strcmp(dcmal, blb) from typeset;
select strcmp(dcmal, txt) from typeset;
select strcmp(nmric, flt) from typeset;
select strcmp(nmric, bt) from typeset;
select strcmp(nmric, dt) from typeset;
select strcmp(nmric, tmstp) from typeset;
select strcmp(nmric, tm) from typeset;
select strcmp(nmric, ch) from typeset;
select strcmp(nmric, vch) from typeset;
select strcmp(nmric, blb) from typeset;
select strcmp(nmric, txt) from typeset;
select strcmp(flt, bt) from typeset;
select strcmp(flt, dt) from typeset;
select strcmp(flt, tmstp) from typeset;
select strcmp(flt, tm) from typeset;
select strcmp(flt, ch) from typeset;
select strcmp(flt, vch) from typeset;
select strcmp(flt, blb) from typeset;
select strcmp(flt, txt) from typeset;
select strcmp(bt, dt) from typeset;
select strcmp(bt, tmstp) from typeset;
select strcmp(bt, tm) from typeset;
select strcmp(bt, ch) from typeset;
select strcmp(bt, vch) from typeset;
select strcmp(bt, blb) from typeset;
select strcmp(bt, txt) from typeset;
select strcmp(dt, tmstp) from typeset;
select strcmp(dt, tm) from typeset;
select strcmp(dt, ch) from typeset;
select strcmp(dt, vch) from typeset;
select strcmp(dt, blb) from typeset;
select strcmp(dt, txt) from typeset;
select strcmp(tmstp, tm) from typeset;
select strcmp(tmstp, ch) from typeset;
select strcmp(tmstp, vch) from typeset;
select strcmp(tmstp, blb) from typeset;
select strcmp(tmstp, txt) from typeset;
select strcmp(tm, ch) from typeset;
select strcmp(tm, vch) from typeset;
select strcmp(tm, blb) from typeset;
select strcmp(tm, txt) from typeset;
select strcmp(ch, vch) from typeset;
select strcmp(ch, blb) from typeset;
select strcmp(ch, txt) from typeset;
select strcmp(vch, blb) from typeset;
select strcmp(vch, txt) from typeset;
select strcmp(blb, txt) from typeset;




\c postgres
drop database db_test_condition;
