
set b_format_behavior_compat_options = 'default_collation,enable_multi_charset';
CREATE DATABASE test_b1 with dbcompatibility 'b' ENCODING='SQL_ASCII' LC_COLLATE='C' LC_CTYPE='C';
CREATE DATABASE test_b2 with dbcompatibility 'b' ENCODING='GBK' LC_COLLATE='zh_CN.gbk' LC_CTYPE='zh_CN.gbk';
create database test_b3 with dbcompatibility 'b';

\c test_b1
set bytea_output=escape;
set client_encoding = 'UTF8';
select cast('熊猫' as binary(10));
set b_format_behavior_compat_options = 'default_collation,enable_multi_charset';
select cast('熊猫' as binary(10));
reset b_format_behavior_compat_options;

\c test_b2
set client_encoding = 'UTF8';
set bytea_output=escape;
select cast('熊猫' as binary(10));
set b_format_behavior_compat_options = 'default_collation,enable_multi_charset';
select cast('熊猫' as binary(10));
reset b_format_behavior_compat_options;

\c test_b3
set client_encoding = 'UTF8';
set bytea_output=escape;
select cast('熊猫' as binary(10));
set b_format_behavior_compat_options = 'default_collation,enable_multi_charset';
select cast('熊猫' as binary(10));
reset b_format_behavior_compat_options;

set b_format_behavior_compat_options = 'enable_multi_charset';
-- test for core issue 1
drop table if exists t1;
create table t1(d blob, b blob);
insert into t1(d, b) values ('aaaa',''),('bnbb','');
select 1 from t1 where (select b from t1) in (select (d>=1) from t1);

drop table if exists t1;
create table t1(d tinyblob, b tinyblob);
insert into t1(d, b) values ('aaaa',''),('bnbb','');
select 1 from t1 where (select b from t1) in (select (d>=1) from t1);

drop table if exists t1;
create table t1(d mediumblob, b mediumblob);
insert into t1(d, b) values ('aaaa',''),('bnbb','');
select 1 from t1 where (select b from t1) in (select (d>=1) from t1);

drop table if exists t1;
create table t1(d longblob, b longblob);
insert into t1(d, b) values ('aaaa',''),('bnbb','');
select 1 from t1 where (select b from t1) in (select (d>=1) from t1);
-- test for core issue 2
drop table if exists t2;
create table t2(a blob not null, b date not null) engine=innodb;
select not exists 
(select 1 from t2 where (select a from t2) 
in (select b from t2)
) as rescol from t2;

drop table if exists t2;
create table t2(a tinyblob not null, b date not null) engine=innodb;
select not exists 
(select 1 from t2 where (select a from t2) 
in (select b from t2)
) as rescol from t2;

drop table if exists t2;
create table t2(a mediumblob not null, b date not null) engine=innodb;
select not exists 
(select 1 from t2 where (select a from t2) 
in (select b from t2)
) as rescol from t2;

drop table if exists t2;
create table t2(a longblob not null, b date not null) engine=innodb;
select not exists 
(select 1 from t2 where (select a from t2) 
in (select b from t2)
) as rescol from t2;
-- test for core issue 3
drop table if exists t3;
create table t3(a bit(64));
insert into t3 values(1);
create view v3(a) as select concat(a) from t3; -- core
show columns from v3;
select hex(a) from v3;
reset b_format_behavior_compat_options;
-- clean
drop table t3;
drop table t2;
drop table t1;

\c postgres
