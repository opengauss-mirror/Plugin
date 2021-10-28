create database db_b_parser_test_db dbcompatibility 'B';
\c db_b_parser_test_db

create extension db_b_parser;

-- test with parser disabled
set enable_custom_parser to off;

--text(n), tinytext, mediumtext, longtext, failed
create table t1(a text(10));
create table t2(a tinytext);
create table t3(a mediumtext);
create table t4(a longtext);

drop table if exists t1;
drop table if exists t2;
drop table if exists t3;
drop table if exists t4;

--default presicion of decimal/number/dec/numeric
create table t_default_decimal(a decimal, b number, c dec, d numeric);
\d t_default_decimal
drop table t_default_decimal;

--failed
create table t_default_float1(a float4(10));
create table t_default_float2(a double);

drop table if exists t_default_float1;
drop table if exists t_default_float2;

--real is single presicion, float is double presicion
create table t_default_float3(a real, b float);
\d t_default_float3
drop table if exists t_default_float3;

-- :=
-- only support 
--     update set clause
--     set stmt\alter system set
----------------------------
create table a(a1 int, a2 int);
insert into a select generate_series(1,5);
update a set a1 := 11;
select a1 from a;
update a set (a1, a2) := (12,13);
set io_limits := 100;
set io_limits := default;

drop table if exists a;

-- DIV\MOD\REGEXP\NOT REGEXP\RLIKE\XOR  gram test
Select 8div3;
Select 8 div3;
Select 8div 3, 8div 3div2, 8div 3div 2;
Select 8mod;
Select 8 div;
Select 8 as div;
Select xor1;
Select div 1;

create schema xor;
create table xor(a int);
create table opr_test2(regexp int);
create type regexp;
create type regexpp as (regexp int);
CREATE FUNCTION regexp(int) returns bigint as 'SELECT count(*) FROM  pg_class ;' LANGUAGE SQL;
CREATE FUNCTION regexp2(regexp int) returns bigint as 'SELECT count(*) FROM  pg_class ;' LANGUAGE SQL;
create procedure mod(int,int)
as
begin
        select $1 + $2;
end;
/
create procedure mod2(mod int, xxx int)
as
begin
        select $1 + $2;
end;
/

drop schema xor;
drop table xor;
drop table opr_test2;
drop type regexp;
drop type regexpp;
drop function regexp;
drop function regexp2;
drop procedure mod(int,int);
drop procedure mod2;

SELECT '-12.3abc' div NULL;
SELECT '-12.3abc' div -100.1;
SELECT '-12.3abc' div 0;
SELECT '-12.3abc' div 5;
SELECT '-12.3abc' div 158.3;
SELECT '-12.3abc' div -8.222e4;
SELECT '-12.3abc' div true;
SELECT '-12.3abc' div false;
SELECT '-12.3abc' div 'NULL';

select 123456 div 5 div 4;
select 8 div 1 where 100 div 3 div 4 = 0;
select 8 div 3 where 100 div 3 div 4 > 0;

SELECT '-12.3abc' mod NULL;
SELECT '-12.3abc' mod -100.1;
SELECT '-12.3abc' mod 0;
SELECT '-12.3abc' mod 5;
SELECT '-12.3abc' mod 158.3;
SELECT '-12.3abc' mod -8.222e4;
SELECT '-12.3abc' mod true;
SELECT '-12.3abc' mod false;
SELECT '-12.3abc' mod 'NULL';

select 123456 mod 5 mod 4;
select 8 mod 1 where 100 mod 3 mod 4 = 0;
select 8 mod 3 where 100 mod 3 mod 4 > 0;

SELECT '-12.3abc' regexp NULL;
SELECT '-12.3abc' regexp -100.1;
SELECT '-12.3abc' regexp 0;
SELECT '-12.3abc' regexp 5;
SELECT '-12.3abc' regexp 158.3;
SELECT '-12.3abc' regexp -8.222e4;
SELECT '-12.3abc' regexp true;
SELECT '-12.3abc' regexp false;
SELECT '-12.3abc' regexp 'NULL';

SELECT '-12.3abc' not regexp NULL;
SELECT '-12.3abc' not regexp -100.1;
SELECT '-12.3abc' not regexp 0;
SELECT '-12.3abc' not regexp 5;
SELECT '-12.3abc' not regexp 158.3;
SELECT '-12.3abc' not regexp -8.222e4;
SELECT '-12.3abc' not regexp true;
SELECT '-12.3abc' not regexp false;
SELECT '-12.3abc' not regexp 'NULL';

SELECT '-12.3abc' xor NULL;
SELECT '-12.3abc' xor -100.1;
SELECT '-12.3abc' xor 0;
SELECT '-12.3abc' xor 5;
SELECT '-12.3abc' xor 158.3;
SELECT '-12.3abc' xor -8.222e4;
SELECT '-12.3abc' xor true;
SELECT '-12.3abc' xor false;
SELECT '-12.3abc' xor 'NULL';

SELECT '-12.3abc' rlike NULL;
SELECT '-12.3abc' rlike -100.1;
SELECT '-12.3abc' rlike 0;
SELECT '-12.3abc' rlike 5;
SELECT '-12.3abc' rlike 158.3;
SELECT '-12.3abc' rlike -8.222e4;
SELECT '-12.3abc' rlike true;
SELECT '-12.3abc' rlike false;
SELECT '-12.3abc' rlike 'NULL';

----------------------------------------
-- copy from regex.sql of fastcheck
----------------------------------------
set enable_bitmapscan = off;
set standard_conforming_strings = on;
select 'bbbbb' regexp '^([bc])\1*$' as t, 'bbbbb' not regexp '^([bc])\1*$' as t2, 'bbbbb' rlike '^([bc])\1*$' as t;
select 'ccc' regexp '^([bc])\1*$' as t, 'ccc' not regexp '^([bc])\1*$' as t2, 'ccc' rlike '^([bc])\1*$' as t;
select 'xxx' regexp '^([bc])\1*$' as f, 'xxx' not regexp '^([bc])\1*$' as f2, 'xxx' rlike '^([bc])\1*$' as f;
select 'bbc' regexp '^([bc])\1*$' as f, 'bbc' not regexp '^([bc])\1*$' as f2, 'bbc' rlike '^([bc])\1*$' as f;
select 'b' regexp '^([bc])\1*$' as t, 'b' not regexp '^([bc])\1*$' as t2, 'b' rlike '^([bc])\1*$' as t;
select 'abc abc abc' regexp '^(\w+)( \1)+$' as t, 'abc abc abc' not regexp '^(\w+)( \1)+$' as t, 'abc abc abc' rlike '^(\w+)( \1)+$' as t;
select 'abc abd abc' regexp '^(\w+)( \1)+$' as f, 'abc abd abc' not regexp '^(\w+)( \1)+$' as f, 'abc abd abc' rlike '^(\w+)( \1)+$' as f;
select 'abc abc abd' regexp '^(\w+)( \1)+$' as f, 'abc abc abd' not regexp '^(\w+)( \1)+$' as f, 'abc abc abd' rlike '^(\w+)( \1)+$' as f;
select 'abc abc abc' regexp '^(.+)( \1)+$' as t, 'abc abc abc' not regexp '^(.+)( \1)+$' as t, 'abc abc abc' rlike '^(.+)( \1)+$' as t;
select 'abc abd abc' regexp '^(.+)( \1)+$' as f, 'abc abd abc' not regexp '^(.+)( \1)+$' as f, 'abc abd abc' rlike '^(.+)( \1)+$' as f;
select 'abc abc abd' regexp '^(.+)( \1)+$' as f, 'abc abc abd' not regexp '^(.+)( \1)+$' as f, 'abc abc abd' rlike '^(.+)( \1)+$' as f;
select 'a' regexp '($|^)*', 'a' not regexp '($|^)*', 'a' rlike '($|^)*';
select 'a' regexp '(^)+^', 'a' not regexp '(^)+^', 'a' rlike '(^)+^';
select 'a' regexp '$($$)+', 'a' not regexp '$($$)+', 'a' rlike '$($$)+';
select 'a' regexp '($^)+', 'a' not regexp '($^)+', 'a' rlike '($^)+';
select 'a' regexp '(^$)*', 'a' not regexp '(^$)*', 'a' rlike '(^$)*';
select 'aa bb cc' regexp '(^(?!aa))+', 'aa bb cc' not regexp '(^(?!aa))+', 'aa bb cc' rlike '(^(?!aa))+';
select 'aa x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'aa x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'aa x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'bb x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'bb x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'bb x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'cc x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'cc x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'cc x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'dd x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'dd x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'dd x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'x' regexp 'abcd(\m)+xyz', 'x' not regexp 'abcd(\m)+xyz', 'x' rlike 'abcd(\m)+xyz';
select 'x' regexp 'a^(^)bcd*xy(((((($a+|)+|)+|)+$|)+|)+|)^$', 'x' not regexp 'a^(^)bcd*xy(((((($a+|)+|)+|)+$|)+|)+|)^$', 'x' rlike 'a^(^)bcd*xy(((((($a+|)+|)+|)+$|)+|)+|)^$';
select 'a' regexp '((((((a)*)*)*)*)*)*', 'a' not regexp '((((((a)*)*)*)*)*)*', 'a' rlike '((((((a)*)*)*)*)*)*';
select 'a' regexp '((((((a+|)+|)+|)+|)+|)+|)', 'a' not regexp '((((((a+|)+|)+|)+|)+|)+|)', 'a' rlike '((((((a+|)+|)+|)+|)+|)+|)';
select 'a' regexp '\x7fffffff', 'a' not regexp '\x7fffffff', 'a' rlike '\x7fffffff';
select 'a' regexp '$()|^\1', 'a' not regexp '$()|^\1', 'a' rlike '$()|^\1';
select 'a' regexp '.. ()|\1', 'a' not regexp '.. ()|\1', 'a' rlike '.. ()|\1';
select 'a' regexp '()*\1', 'a' not regexp '()*\1', 'a' rlike '()*\1';
select 'a' regexp '()+\1', 'a' not regexp '()+\1', 'a' rlike '()+\1';

explain (costs off) select * from pg_proc where proname regexp 'abc';
explain (costs off) select * from pg_proc where proname regexp '^abc';
explain (costs off) select * from pg_proc where proname regexp '^abc$';
explain (costs off) select * from pg_proc where proname regexp '^abcd*e';
explain (costs off) select * from pg_proc where proname regexp '^abc+d';
explain (costs off) select * from pg_proc where proname regexp '^(abc)(def)';
explain (costs off) select * from pg_proc where proname regexp '^(abc)$';
explain (costs off) select * from pg_proc where proname regexp '^(abc)?d';

explain (costs off) select * from pg_proc where proname not regexp 'abc';
explain (costs off) select * from pg_proc where proname not regexp '^abc';
explain (costs off) select * from pg_proc where proname not regexp '^abc$';
explain (costs off) select * from pg_proc where proname not regexp '^abcd*e';
explain (costs off) select * from pg_proc where proname not regexp '^abc+d';
explain (costs off) select * from pg_proc where proname not regexp '^(abc)(def)';
explain (costs off) select * from pg_proc where proname not regexp '^(abc)$';
explain (costs off) select * from pg_proc where proname not regexp '^(abc)?d';

explain (costs off) select * from pg_proc where proname rlike 'abc';
explain (costs off) select * from pg_proc where proname rlike '^abc';
explain (costs off) select * from pg_proc where proname rlike '^abc$';
explain (costs off) select * from pg_proc where proname rlike '^abcd*e';
explain (costs off) select * from pg_proc where proname rlike '^abc+d';
explain (costs off) select * from pg_proc where proname rlike '^(abc)(def)';
explain (costs off) select * from pg_proc where proname rlike '^(abc)$';
explain (costs off) select * from pg_proc where proname rlike '^(abc)?d';

set enable_bitmapscan = default;
set standard_conforming_strings = default;

------DAYOFMONTH() DAYOFWEEK() DAYOFYEAR() HOUR() MICROSECOND() MINUTE() QUARTER() SECOND() WEEKDAY() WEEKOFYEAR() YEAR() 
select DAYOFMONTH(timestamp '2021-05-26 16:30:44.341191');
select DAYOFWEEK(timestamp '2021-05-26 16:30:44.341191');
select DAYOFYEAR(timestamp '2021-05-26 16:30:44.341191');
select HOUR(timestamp '2021-05-26 16:30:44.341191');
select MICROSECOND(timestamp '2021-05-26 16:30:44.341191');
select MINUTE(timestamp '2021-05-26 16:30:44.341191');
select QUARTER(timestamp '2021-05-26 16:30:44.341191');
select SECOND(timestamp '2021-05-26 16:30:44.341191');
select WEEKDAY(timestamp '2021-05-26 16:30:44.341191');
select WEEKOFYEAR(timestamp '2021-05-26 16:30:44.341191');
select YEAR(timestamp '2021-05-26 16:30:44.341191');

--- LOCATE
select locate('test','testgood');
select locate('test','11testgood',5);
select locate('test','11testgood',3);

-- IFNULL
SELECT IFNULL(NULL, 'test');

-- IF
SELECT IF(TRUE,'A','B');
SELECT IF(FALSE,'A','B');

------------------------------------------------------------------------------------------------------------------------
----------------------------
-- bit_length\octet_length\length
----------------------------
select * from pg_proc where proname in ('db_b_parser_bit_length');

select bit_length(b'1'), bit_length(b'101'), bit_length(b'1011111111');
select bit_length( '1'), bit_length( '101'), bit_length( '1011111111');
select bit_length( '哈1哈\n'),bit_length( '\n'),bit_length( '\\n');

select octet_length(b'1'), octet_length(b'101'), octet_length(b'1011111111');
select octet_length( '1'), octet_length( '101'), octet_length( '1011111111');
select octet_length( '哈1哈\n'),octet_length( '\n'),octet_length( '\\n');

select length(b'1'), length(b'101'), length(b'1011111111');
select length( '1'), length( '101'), length( '1011111111');
select length( '哈1哈\n'),length( '\n'),length( '\\n');

----------------------------
-- \lcase\ucase\rand\truncate\current_date()
----------------------------
select * from pg_proc where proname in ('db_b_parser_bit_length', 'lower', 'upper', 'random', 'trunc') order by oid;

select lcase('ABc'), lcase('哈哈'), lcase('123456');
select lower('ABc'), lower('哈哈'), lower('123456');
select ucase('ABc'), ucase('哈哈'), ucase('123456');
select upper('ABc'), upper('哈哈'), upper('123456');

select rand(), random();

select truncate(111.28), truncate(111.28,1), truncate(111.28,5), truncate(111.28,500), truncate(111.28,-1), truncate(111.28,-4);
select    trunc(111.28),    trunc(111.28,1),    trunc(111.28,5),    trunc(111.28,500),    trunc(111.28,-1),    trunc(111.28,-4);
truncate a;

select current_date(), current_date;

----------------------------------------------
-- test with parser enabled, same testcases --
----------------------------------------------
set enable_custom_parser to on;

--text(n), tinytext, mediumtext, longtext, failed
create table t1(a text(10));
create table t2(a tinytext);
create table t3(a mediumtext);
create table t4(a longtext);

drop table if exists t1;
drop table if exists t2;
drop table if exists t3;
drop table if exists t4;

--default presicion of decimal/number/dec/numeric
create table t_default_decimal(a decimal, b number, c dec, d numeric);
\d t_default_decimal
drop table t_default_decimal;

--failed
create table t_default_float1(a float4(10));
create table t_default_float2(a double);

drop table if exists t_default_float1;
drop table if exists t_default_float2;

--real is single presicion, float is double presicion
create table t_default_float3(a real, b float);
\d t_default_float3
drop table t_default_float3;

-- :=
-- only support 
--     update set clause
--     set stmt\alter system set
----------------------------
create table a(a1 int, a2 int);
insert into a select generate_series(1,5);
update a set a1 := 11;
select a1 from a;
update a set (a1, a2) := (12,13);
set io_limits := 100;
set io_limits := default;

drop table if exists a;

-- DIV\MOD\REGEXP\NOT REGEXP\RLIKE\XOR  gram test
Select 8div3;
Select 8 div3;
Select 8div 3, 8div 3div2, 8div 3div 2;
Select 8mod;
Select 8 div;
Select 8 as div;
Select xor1;
Select div 1;

create schema xor;
create table xor(a int);
create table opr_test2(regexp int);
create type regexp;
create type regexpp as (regexp int);
CREATE FUNCTION regexp(int) returns bigint as 'SELECT count(*) FROM  pg_class ;' LANGUAGE SQL;
CREATE FUNCTION regexp2(regexp int) returns bigint as 'SELECT count(*) FROM  pg_class ;' LANGUAGE SQL;
create procedure mod(int,int)
as
begin
        select $1 + $2;
end;
/
create procedure mod2(mod int, xxx int)
as
begin
        select $1 + $2;
end;
/

drop schema xor;
drop table xor;
drop table opr_test2;
drop type regexp;
drop type regexpp;
drop function regexp;
drop function regexp2;
drop procedure mod(int,int);
drop procedure mod2;

SELECT '-12.3abc' div NULL;
SELECT '-12.3abc' div -100.1;
SELECT '-12.3abc' div 0;
SELECT '-12.3abc' div 5;
SELECT '-12.3abc' div 158.3;
SELECT '-12.3abc' div -8.222e4;
SELECT '-12.3abc' div true;
SELECT '-12.3abc' div false;
SELECT '-12.3abc' div 'NULL';

select 123456 div 5 div 4;
select 8 div 1 where 100 div 3 div 4 = 0;
select 8 div 3 where 100 div 3 div 4 > 0;

SELECT '-12.3abc' mod NULL;
SELECT '-12.3abc' mod -100.1;
SELECT '-12.3abc' mod 0;
SELECT '-12.3abc' mod 5;
SELECT '-12.3abc' mod 158.3;
SELECT '-12.3abc' mod -8.222e4;
SELECT '-12.3abc' mod true;
SELECT '-12.3abc' mod false;
SELECT '-12.3abc' mod 'NULL';

select 123456 mod 5 mod 4;
select 8 mod 1 where 100 mod 3 mod 4 = 0;
select 8 mod 3 where 100 mod 3 mod 4 > 0;

SELECT '-12.3abc' regexp NULL;
SELECT '-12.3abc' regexp -100.1;
SELECT '-12.3abc' regexp 0;
SELECT '-12.3abc' regexp 5;
SELECT '-12.3abc' regexp 158.3;
SELECT '-12.3abc' regexp -8.222e4;
SELECT '-12.3abc' regexp true;
SELECT '-12.3abc' regexp false;
SELECT '-12.3abc' regexp 'NULL';

SELECT '-12.3abc' not regexp NULL;
SELECT '-12.3abc' not regexp -100.1;
SELECT '-12.3abc' not regexp 0;
SELECT '-12.3abc' not regexp 5;
SELECT '-12.3abc' not regexp 158.3;
SELECT '-12.3abc' not regexp -8.222e4;
SELECT '-12.3abc' not regexp true;
SELECT '-12.3abc' not regexp false;
SELECT '-12.3abc' not regexp 'NULL';

SELECT '-12.3abc' xor NULL;
SELECT '-12.3abc' xor -100.1;
SELECT '-12.3abc' xor 0;
SELECT '-12.3abc' xor 5;
SELECT '-12.3abc' xor 158.3;
SELECT '-12.3abc' xor -8.222e4;
SELECT '-12.3abc' xor true;
SELECT '-12.3abc' xor false;
SELECT '-12.3abc' xor 'NULL';

SELECT '-12.3abc' rlike NULL;
SELECT '-12.3abc' rlike -100.1;
SELECT '-12.3abc' rlike 0;
SELECT '-12.3abc' rlike 5;
SELECT '-12.3abc' rlike 158.3;
SELECT '-12.3abc' rlike -8.222e4;
SELECT '-12.3abc' rlike true;
SELECT '-12.3abc' rlike false;
SELECT '-12.3abc' rlike 'NULL';

----------------------------------------
-- copy from regex.sql of fastcheck
----------------------------------------
set enable_bitmapscan = off;
set standard_conforming_strings = on;
select 'bbbbb' regexp '^([bc])\1*$' as t, 'bbbbb' not regexp '^([bc])\1*$' as t2, 'bbbbb' rlike '^([bc])\1*$' as t;
select 'ccc' regexp '^([bc])\1*$' as t, 'ccc' not regexp '^([bc])\1*$' as t2, 'ccc' rlike '^([bc])\1*$' as t;
select 'xxx' regexp '^([bc])\1*$' as f, 'xxx' not regexp '^([bc])\1*$' as f2, 'xxx' rlike '^([bc])\1*$' as f;
select 'bbc' regexp '^([bc])\1*$' as f, 'bbc' not regexp '^([bc])\1*$' as f2, 'bbc' rlike '^([bc])\1*$' as f;
select 'b' regexp '^([bc])\1*$' as t, 'b' not regexp '^([bc])\1*$' as t2, 'b' rlike '^([bc])\1*$' as t;
select 'abc abc abc' regexp '^(\w+)( \1)+$' as t, 'abc abc abc' not regexp '^(\w+)( \1)+$' as t, 'abc abc abc' rlike '^(\w+)( \1)+$' as t;
select 'abc abd abc' regexp '^(\w+)( \1)+$' as f, 'abc abd abc' not regexp '^(\w+)( \1)+$' as f, 'abc abd abc' rlike '^(\w+)( \1)+$' as f;
select 'abc abc abd' regexp '^(\w+)( \1)+$' as f, 'abc abc abd' not regexp '^(\w+)( \1)+$' as f, 'abc abc abd' rlike '^(\w+)( \1)+$' as f;
select 'abc abc abc' regexp '^(.+)( \1)+$' as t, 'abc abc abc' not regexp '^(.+)( \1)+$' as t, 'abc abc abc' rlike '^(.+)( \1)+$' as t;
select 'abc abd abc' regexp '^(.+)( \1)+$' as f, 'abc abd abc' not regexp '^(.+)( \1)+$' as f, 'abc abd abc' rlike '^(.+)( \1)+$' as f;
select 'abc abc abd' regexp '^(.+)( \1)+$' as f, 'abc abc abd' not regexp '^(.+)( \1)+$' as f, 'abc abc abd' rlike '^(.+)( \1)+$' as f;
select 'a' regexp '($|^)*', 'a' not regexp '($|^)*', 'a' rlike '($|^)*';
select 'a' regexp '(^)+^', 'a' not regexp '(^)+^', 'a' rlike '(^)+^';
select 'a' regexp '$($$)+', 'a' not regexp '$($$)+', 'a' rlike '$($$)+';
select 'a' regexp '($^)+', 'a' not regexp '($^)+', 'a' rlike '($^)+';
select 'a' regexp '(^$)*', 'a' not regexp '(^$)*', 'a' rlike '(^$)*';
select 'aa bb cc' regexp '(^(?!aa))+', 'aa bb cc' not regexp '(^(?!aa))+', 'aa bb cc' rlike '(^(?!aa))+';
select 'aa x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'aa x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'aa x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'bb x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'bb x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'bb x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'cc x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'cc x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'cc x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'dd x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'dd x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'dd x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'x' regexp 'abcd(\m)+xyz', 'x' not regexp 'abcd(\m)+xyz', 'x' rlike 'abcd(\m)+xyz';
select 'x' regexp 'a^(^)bcd*xy(((((($a+|)+|)+|)+$|)+|)+|)^$', 'x' not regexp 'a^(^)bcd*xy(((((($a+|)+|)+|)+$|)+|)+|)^$', 'x' rlike 'a^(^)bcd*xy(((((($a+|)+|)+|)+$|)+|)+|)^$';
select 'a' regexp '((((((a)*)*)*)*)*)*', 'a' not regexp '((((((a)*)*)*)*)*)*', 'a' rlike '((((((a)*)*)*)*)*)*';
select 'a' regexp '((((((a+|)+|)+|)+|)+|)+|)', 'a' not regexp '((((((a+|)+|)+|)+|)+|)+|)', 'a' rlike '((((((a+|)+|)+|)+|)+|)+|)';
select 'a' regexp '\x7fffffff', 'a' not regexp '\x7fffffff', 'a' rlike '\x7fffffff';
select 'a' regexp '$()|^\1', 'a' not regexp '$()|^\1', 'a' rlike '$()|^\1';
select 'a' regexp '.. ()|\1', 'a' not regexp '.. ()|\1', 'a' rlike '.. ()|\1';
select 'a' regexp '()*\1', 'a' not regexp '()*\1', 'a' rlike '()*\1';
select 'a' regexp '()+\1', 'a' not regexp '()+\1', 'a' rlike '()+\1';

explain (costs off) select * from pg_proc where proname regexp 'abc';
explain (costs off) select * from pg_proc where proname regexp '^abc';
explain (costs off) select * from pg_proc where proname regexp '^abc$';
explain (costs off) select * from pg_proc where proname regexp '^abcd*e';
explain (costs off) select * from pg_proc where proname regexp '^abc+d';
explain (costs off) select * from pg_proc where proname regexp '^(abc)(def)';
explain (costs off) select * from pg_proc where proname regexp '^(abc)$';
explain (costs off) select * from pg_proc where proname regexp '^(abc)?d';

explain (costs off) select * from pg_proc where proname not regexp 'abc';
explain (costs off) select * from pg_proc where proname not regexp '^abc';
explain (costs off) select * from pg_proc where proname not regexp '^abc$';
explain (costs off) select * from pg_proc where proname not regexp '^abcd*e';
explain (costs off) select * from pg_proc where proname not regexp '^abc+d';
explain (costs off) select * from pg_proc where proname not regexp '^(abc)(def)';
explain (costs off) select * from pg_proc where proname not regexp '^(abc)$';
explain (costs off) select * from pg_proc where proname not regexp '^(abc)?d';

explain (costs off) select * from pg_proc where proname rlike 'abc';
explain (costs off) select * from pg_proc where proname rlike '^abc';
explain (costs off) select * from pg_proc where proname rlike '^abc$';
explain (costs off) select * from pg_proc where proname rlike '^abcd*e';
explain (costs off) select * from pg_proc where proname rlike '^abc+d';
explain (costs off) select * from pg_proc where proname rlike '^(abc)(def)';
explain (costs off) select * from pg_proc where proname rlike '^(abc)$';
explain (costs off) select * from pg_proc where proname rlike '^(abc)?d';

set enable_bitmapscan = default;
set standard_conforming_strings = default;

------DAYOFMONTH() DAYOFWEEK() DAYOFYEAR() HOUR() MICROSECOND() MINUTE() QUARTER() SECOND() WEEKDAY() WEEKOFYEAR() YEAR() 
select DAYOFMONTH(timestamp '2021-05-26 16:30:44.341191');
select DAYOFWEEK(timestamp '2021-05-26 16:30:44.341191');
select DAYOFYEAR(timestamp '2021-05-26 16:30:44.341191');
select HOUR(timestamp '2021-05-26 16:30:44.341191');
select MICROSECOND(timestamp '2021-05-26 16:30:44.341191');
select MINUTE(timestamp '2021-05-26 16:30:44.341191');
select QUARTER(timestamp '2021-05-26 16:30:44.341191');
select SECOND(timestamp '2021-05-26 16:30:44.341191');
select WEEKDAY(timestamp '2021-05-26 16:30:44.341191');
select WEEKOFYEAR(timestamp '2021-05-26 16:30:44.341191');
select YEAR(timestamp '2021-05-26 16:30:44.341191');

--- LOCATE
select locate('test','testgood');
select locate('test','11testgood',5);
select locate('test','11testgood',3);

-- IFNULL
SELECT IFNULL(NULL, 'test');

-- IF
SELECT IF(TRUE,'A','B');
SELECT IF(FALSE,'A','B');

------------------------------------------------------------------------------------------------------------------------
----------------------------
-- bit_length\octet_length\length
----------------------------
select * from pg_proc where proname in ('db_b_parser_bit_length');

select bit_length(b'1'), bit_length(b'101'), bit_length(b'1011111111');
select bit_length( '1'), bit_length( '101'), bit_length( '1011111111');
select bit_length( '哈1哈\n'),bit_length( '\n'),bit_length( '\\n');

select octet_length(b'1'), octet_length(b'101'), octet_length(b'1011111111');
select octet_length( '1'), octet_length( '101'), octet_length( '1011111111');
select octet_length( '哈1哈\n'),octet_length( '\n'),octet_length( '\\n');

select length(b'1'), length(b'101'), length(b'1011111111');
select length( '1'), length( '101'), length( '1011111111');
select length( '哈1哈\n'),length( '\n'),length( '\\n');

----------------------------
-- \lcase\ucase\rand\truncate\current_date()
----------------------------
select * from pg_proc where proname in ('db_b_parser_bit_length', 'lower', 'upper', 'random', 'trunc') order by oid;

select lcase('ABc'), lcase('哈哈'), lcase('123456');
select lower('ABc'), lower('哈哈'), lower('123456');
select ucase('ABc'), ucase('哈哈'), ucase('123456');
select upper('ABc'), upper('哈哈'), upper('123456');

select rand(), random();

select truncate(111.28), truncate(111.28,1), truncate(111.28,5), truncate(111.28,500), truncate(111.28,-1), truncate(111.28,-4);
select    trunc(111.28),    trunc(111.28,1),    trunc(111.28,5),    trunc(111.28,500),    trunc(111.28,-1),    trunc(111.28,-4);
truncate a;

select current_date(), current_date;

------------------------------------------------------------------------------------------------------------------------
\c contrib_regression
drop database db_b_parser_test_db;