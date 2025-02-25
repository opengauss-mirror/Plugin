create schema test_enum_collation;
set current_schema = 'test_enum_collation';
-- test check duplicate enum scenes
-- 1. column charset/collate
drop table if exists test_enum_key1;
create table test_enum_key1(a enum('', '  '));
create table test_enum_key1(a enum('', '  ') collate utf8_bin);
create table test_enum_key1(a enum('a', 'a  '));
create table test_enum_key1(a enum('a', 'a  ') collate utf8_bin);
create table test_enum_key1(a enum('aaa', 'AAA  ') collate utf8_general_ci);
create table test_enum_key1(a enum('高斯sS', '高斯ŠŠ  ') collate utf8_general_ci);
create table test_enum_key1(a enum('aaa', 'aaA   ') charset 'utf8');
create table test_enum_key1(a enum('aaa', 'aaA   ') charset 'gbk');
create table test_enum_key1(a enum('aaa', 'aaA   ') charset 'gb18030');

--succeed scenes
create table t_column_collation1(a enum('a', 'b', 'A','B') collate utf8_bin);
create table t_column_collation2(a enum('a', 'b', 'A','B') collate utf8_bin);
create table t_column_collation3(a enum('aaa', 'bbb') charset 'utf8');
create table t_column_collation4(a enum('a', 'b', 'A','B') collate utf8_bin);
drop table t_column_collation1;
drop table t_column_collation2;
drop table t_column_collation3;
drop table t_column_collation4;

-- 2. table charset/collate
create table test_enum_key1(a enum('aaa', 'aaA   ')) charset 'utf8';
create table test_enum_key1(a enum('aaa', 'aaA   ')) collate 'utf8_general_ci';
create table test_enum_key1(a enum('aaa', 'aaA   ')) charset 'gbk';
create table test_enum_key1(a enum('aaa', 'aaa   ')) charset 'gbk' collate gbk_bin;
create table test_enum_key1(a enum('aaa', 'aaA   ')) charset 'gb18030';
create table test_enum_key1(a enum('aaa', 'aaa   ')) charset 'gb18030' collate gb18030_bin;

--succeed scenes
create table t_table_collation1(a enum('aaa', 'bbb')) charset 'utf8';
create table t_table_collation2(a enum('aaa', 'aaA   ') collate 'utf8_bin')charset 'utf8';
drop table t_table_collation1;
drop table t_table_collation2;

-- 3. schema charset/collate
create schema enum_test_schema1 charset = utf8;
set current_schema='enum_test_schema1';

create table t(a enum('a', 'A'));  --fail
create table t(a enum('a', 'b'));  -- succeed
alter table t add column b enum('a', 'A', 'b'); -- fail
alter table t add column b enum('a', 'A', 'b') collate utf8_bin; --succeed
insert into t values('a  ', 'a  ');
insert into t values('a  ', 'A  ');
insert into t values('B  ', 'A  ');
insert into t values('a  ', 'B  ');  --fail
drop table t;

create schema enum_test_schema2  charset = utf8mb4 collate = utf8mb4_bin;
set current_schema='enum_test_schema2';
create table t(a enum('a', 'A'));  -- succeed
alter table t add column b enum('a', 'A', 'b') collate utf8_general_ci; --fail
alter table t add column b enum('a', 'A', 'b'); -- succeed
insert into t values ('a  ', 'b  ');
insert into t values ('a  ', 'B  '); -- fail
drop table t;

set current_schema = 'test_enum_collation';
drop schema enum_test_schema1;
drop schema enum_test_schema2;
set b_format_behavior_compat_options = 'enable_multi_charset';

-- 1. insert value while collate like _bin
create table test_collation1(a enum('aaa', 'AAA','高斯SS', '高斯ss') collate utf8_bin);
insert into test_collation1 values('aaa  ');
insert into test_collation1 values('aaa');
insert into test_collation1 values('AAA');
insert into test_collation1 values('AAA  ');
insert into test_collation1 values('高斯ss   ');
insert into test_collation1 values('高斯SS   ');
insert into test_collation1 values('高斯sS   '); --failed
insert into test_collation1 values('高斯ss,高斯ss   '); --failed

-- 4. insert value while collate like _ci
create table test_collation2(a enum('高斯sS', '汉字sS', 'aaa', 'bbb')) charset utf8;
select pg_get_tabledef('test_collation2');

insert into test_collation2 values('高斯sS   ');
insert into test_collation2 values('高斯ss,高斯ss   ');
insert into test_collation2 values('高斯ŠŠ');
insert into test_collation2 values('汉字sS   ');
insert into test_collation2 values('汉字ss,汉字ss   ');
insert into test_collation2 values('汉字ŠŠ');
insert into test_collation2 values('aaa  ');
insert into test_collation2 values('aaa');
insert into test_collation2 values('AAA  ');

-- test update with colalte
create table test_update(a enum('a', 'A', 'b'))collate utf8_bin;
insert into test_update values('a'),('A');
update test_update enum a = 'b' where a = 'a';
update test_update enum a = 'b' where a = 'a' collate utf8_general_ci;
select * from test_update;
update test_update enum a = 'a' where a = 'B' collate utf8_general_ci;
select * from test_update;
drop table test_update;

-- test operator
create table test_collation_op(a enum('a', 'A', 'b', 'B'), b enum('a', 'A', 'b', 'B')) collate utf8_bin;
insert into test_collation_op values('a', 'A'),('A','a'),('b','B'),('B','b');

-- test '='
select a from test_collation1 where a = 'aaa';
select a from test_collation1 where a = '高斯ss' collate utf8_bin;
select a from test_collation1 where a = 'aaa' collate "utf8_general_ci"; -- enum = text
select a from test_collation1 where a = '高斯ss' collate utf8_general_ci;

select a from test_collation2 where a = '高斯ss';
select a from test_collation2 where a = '高斯ss' collate utf8_bin;
select a from test_collation2 where a = 'aaa' collate "utf8_general_ci";

select a from test_collation1 where 'aaa' = a collate "utf8_general_ci"; -- text = enum

select
  distinct t1.a
from
  test_collation_op as t1
where t1.a = t1.b  collate "utf8_general_ci"; -- enum = enum

select
  distinct t1.a
from
  test_collation_op as t1
where t1.a = t1.b;

-- '<>'
select a, b from test_collation_op where a <> b; -- enum <> other enum
select a, b from test_collation_op where a <> b collate 'utf8_general_ci';
select a, b from test_collation_op where a <> 'a'; -- enum <> text
select a, b from test_collation_op where a <> 'a' collate 'utf8_general_ci';
select a, b from test_collation_op where 'a' <> a;
select a, b from test_collation_op where 'a' <> a collate 'utf8_general_ci'; -- text <> enum

-- '<'
select a, b from test_collation_op where a < b; -- enum < other enum
select a from test_collation_op where a < b collate 'utf8_general_ci';
select a from test_collation_op where a < 'C'; -- enum < text
select a from test_collation_op where a < 'C' collate 'utf8_general_ci';
select a from test_collation_op where 'B' < a;
select a from test_collation_op where 'B' < a collate 'utf8_general_ci';

-- '<='
select a, b from test_collation_op where a <= b; -- enum <= other enum
select a from test_collation_op where a <= b collate 'utf8_general_ci';
select a from test_collation_op where a <= 'A'; -- enum <= text
select a from test_collation_op where a <= 'A' collate 'utf8_general_ci';
select a from test_collation_op where 'B' <= a; -- text <= enum
select a from test_collation_op where 'B' <= a collate 'utf8_general_ci';

-- '>'
select a, b from test_collation_op where a > b; -- enum > other enum
select a from test_collation_op where a > b collate 'utf8_general_ci';
select a from test_collation_op where a > 'B'; -- enum > text
select a from test_collation_op where a > 'B' collate 'utf8_general_ci';
select a from test_collation_op where 'C' > a;
select a from test_collation_op where 'C' > a collate 'utf8_general_ci';

-- '>='
select a, b from test_collation_op where a >= b; -- enum >= other enum
select a from test_collation_op where a >= b collate 'utf8_general_ci';
select a from test_collation_op where a >= 'B'; -- enum >= text
select a from test_collation_op where a >= 'B' collate 'utf8_general_ci';
select a from test_collation_op where 'C' >= a;
select a from test_collation_op where 'C' >= a collate 'utf8_general_ci';

-- test order by
select a from test_collation1 order by a;
select a from test_collation1 order by a collate 'utf8_general_ci';
select a from test_collation1 order by a collate 'utf8_bin';

select a from test_collation2 order by a;
select a from test_collation2 order by a collate 'utf8_general_ci';
select a from test_collation2 order by a collate 'utf8_bin';

-- test distinct
select distinct a from test_collation1;
select distinct a collate 'utf8_bin' from test_collation1;
select distinct a collate 'utf8_general_ci' from test_collation1;

select distinct a from test_collation2;
select distinct a collate 'utf8_bin' from test_collation2;
select distinct a collate 'utf8_general_ci' from test_collation2;

-- test alter table modify/add
alter table test_collation2 modify a char(10) collate utf8_bin;
select a from test_collation2 where a = '高斯ss';
select a from test_collation2 where a = '高斯sS' collate utf8_bin;
alter table test_collation2 modify a enum('高斯sS', '汉字sS', 'aaa', 'bbb');
select pg_get_tabledef('test_collation2');
insert into test_collation2 values('高斯ŠŠ  ');
alter table test_collation2 add b enum('a','b', 'c');
insert into test_collation2(b) values('A'), ('b  '), (3);

-- test unique/primary key
create table t_collation_enum5(a enum('aaa', 'bbb') collate utf8_general_ci unique);
insert into t_collation_enum5 values('aaa');
insert into t_collation_enum5 values('aaa  ');
insert into t_collation_enum5 values('AAA  ');

create table t_collation_enum6(a enum('aaa', 'bbb') collate utf8_general_ci primary key);
insert into t_collation_enum6 values('aaa  ');
insert into t_collation_enum6 values('aaa');
insert into t_collation_enum6 values('AAA  ');

drop table t_collation_enum5;
drop table t_collation_enum6;

-- test join
create table test_join1(
  c_char char(10),
  c_varchar varchar(10),
  c_nvarchar2 nvarchar2(10),
  c_text text,
  c_enum enum('a')
) collate utf8_bin;

create table test_join2(
  c_enum enum('A')
) collate utf8_bin;

insert into test_join1 values('a  ', 'a  ', 'a  ', 'a  ','a  ');
insert into test_join2 values('A');

select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_char;
select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_char collate 'utf8_general_ci';
select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_char > t2.c_enum;
select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_char > t2.c_enum collate 'utf8_general_ci';
select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_char;
select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_char collate 'utf8_general_ci';
select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_char;
select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_char collate 'utf8_general_ci';
select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_char;
select t1.c_char, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_char collate 'utf8_general_ci';

select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_varchar;
select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_varchar collate 'utf8_general_ci';
select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_varchar > t2.c_enum;
select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_varchar > t2.c_enum collate 'utf8_general_ci';
select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_varchar;
select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_varchar collate 'utf8_general_ci';
select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_varchar;
select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_varchar collate 'utf8_general_ci';
select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_varchar;
select t1.c_varchar, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_varchar collate 'utf8_general_ci';

select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_nvarchar2;
select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_nvarchar2 collate 'utf8_general_ci';
select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_nvarchar2 > t2.c_enum;
select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_nvarchar2 > t2.c_enum collate 'utf8_general_ci';
select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_nvarchar2;
select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_nvarchar2 collate 'utf8_general_ci';
select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_nvarchar2;
select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_nvarchar2 collate 'utf8_general_ci';
select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_nvarchar2;
select t1.c_nvarchar2, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_nvarchar2 collate 'utf8_general_ci';

select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_text;
select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_text collate 'utf8_general_ci';
select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_text > t2.c_enum;
select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_text > t2.c_enum collate 'utf8_general_ci';
select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_text;
select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_text collate 'utf8_general_ci';
select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_text;
select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_text collate 'utf8_general_ci';
select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_text;
select t1.c_text, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_text collate 'utf8_general_ci';

select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum = t1.c_enum;
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum = t1.c_enum collate 'utf8_general_ci';
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_enum;
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <> t1.c_enum collate 'utf8_general_ci';
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_enum > t2.c_enum;
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t1.c_enum > t2.c_enum collate 'utf8_general_ci';
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_enum;
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum >= t1.c_enum collate 'utf8_general_ci';
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_enum;
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum < t1.c_enum collate 'utf8_general_ci';
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_enum;
select t1.c_enum, t2.c_enum from test_join1 t1 join test_join2 t2 on t2.c_enum <= t1.c_enum collate 'utf8_general_ci';

drop table test_join1;
drop table test_join2;
-- test partition table with collate
create table test_part_tab1(c1 int, c2 enum('a', 'b', 'A', 'B'))charset utf8 partition by range(c1)  --fail
(
    partition test_enum_tab_p1 values less than (5),
    partition test_enum_tab_p2 values less than (10)
);

create table test_part_tab1(c1 int, c2 enum('a', 'b', 'A', 'B'))charset utf8 collate utf8_bin partition by range(c1)  -- success
(
    partition test_enum_tab_p1 values less than (5),
    partition test_enum_tab_p2 values less than (10)
);
select pg_get_tabledef('test_part_tab1');
insert into test_part_tab1 values(1, 'a  ');
insert into test_part_tab1 values(6, 'A  ');
insert into test_part_tab1 values(2, 'b  ');
insert into test_part_tab1 values(7, 'B  ');

create table test_part_tab2(c1 int, c2 enum('a', 'b', 'c', 'd'))charset utf8 partition by range(c1) -- success
(
    partition test_enum_tab_p1 values less than (5),
    partition test_enum_tab_p2 values less than (10)
);
insert into test_part_tab2 values(1, 'a  ');
insert into test_part_tab2 values(6, 'A  ');
insert into test_part_tab2 values(2, 'b  ');
insert into test_part_tab2 values(7, 'B  ');

drop table test_part_tab1;
drop table test_part_tab2;

-- test local/global temp table with collate
create local temp table test_temp_tab1 (c1 enum('a', 'b')) charset utf8;
insert into test_temp_tab1 values('a  ');
insert into test_temp_tab1 values('A  ');

create global temp table test_temp_tab2 (c1 enum('a', 'b')) charset utf8 collate utf8_bin;
insert into test_temp_tab2 values('a  ');
insert into test_temp_tab2 values('A  '); --fail

drop table test_temp_tab1;
drop table test_temp_tab2;
-- test ustore
create table test_ustore1(
  c1 enum('a', 'b', 'c', 'd'),
  c2 enum('a', 'b','A', 'B') collate 'gbk_bin'
) charset utf8 with (STORAGE_TYPE = ustore);

select pg_get_tabledef('test_ustore1');
insert into test_ustore1 values('a  ', 'a  ');
insert into test_ustore1 values('A  ', 'A  ');
select c2 from test_ustore1 where c2 = 'a';
select c2 from test_ustore1 where c2 = 'a' collate 'utf8_general_ci';
select c2 from test_ustore1 where c2 = c1;
drop table test_ustore1;

-- test cstore(not supported)
create table test_cstore1(c1 enum('a', 'b', 'c', 'd') collate 'utf8_bin') with (ORIENTATION = column);

-- test check constraint with collate
create table test_check1(a enum('a', 'b', 'c') check(a >= 'b'))charset utf8;
insert into test_check1 values('a');
insert into test_check1 values('B  ');
insert into test_check1 values('C');
insert into test_check1 values('c');
drop table test_check1;

drop schema test_enum_collation cascade;
reset current_schema;