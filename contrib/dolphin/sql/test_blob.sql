create schema test_blob;
set current_schema to 'test_blob';
create table test_template (t tinyblob, b blob, m mediumblob, l longblob);
insert into test_template values('aaaaaaaaa', 'aaaaaaaaa', 'aaaaaaaaa', 'aaaaaaaaa');
create table test_tiny (t tinyblob);
create table test_blob (b blob);
create table test_medium (m mediumblob);
create table test_long (l longblob);

insert into test_tiny select t from test_template;
insert into test_tiny select b from test_template;
insert into test_tiny select m from test_template;
insert into test_tiny select l from test_template;

insert into test_blob select t from test_template;
insert into test_blob select b from test_template;
insert into test_blob select m from test_template;
insert into test_blob select l from test_template;

insert into test_medium select t from test_template;
insert into test_medium select b from test_template;
insert into test_medium select m from test_template;
insert into test_medium select l from test_template;

insert into test_long select t from test_template;
insert into test_long select b from test_template;
insert into test_long select m from test_template;
insert into test_long select l from test_template;

\COPY test_template to './test_template.data';
create table test_template2 (t tinyblob, b blob, m mediumblob, l longblob);
\COPY test_template2 from './test_template.data';

truncate test_tiny;
truncate test_blob;
truncate test_medium;
truncate test_long;

insert into test_tiny values('ppp');
insert into test_blob values('ppp');
insert into test_medium values('ppp');
insert into test_long values('ppp');

set dolphin.b_compatibility_mode = true;
set bytea_output = 'escape';

insert into test_blob values('ppp');

select * from test_tiny;
select * from test_blob;
select * from test_medium;
select * from test_long;

set dolphin.b_compatibility_mode = false;
select * from test_blob;

drop table test_template;
drop table test_template2;
drop table test_tiny;
drop table test_blob;
drop table test_medium;
drop table test_long;

set dolphin.b_compatibility_mode to on;

SELECT 'ab'::blob = 'ab';
SELECT 'ab'::tinyblob = 'ab';
SELECT 'ab'::mediumblob = 'ab';
SELECT 'ab'::longblob = 'ab';

SELECT 'ab'::blob > 'ab';
SELECT 'ab'::tinyblob > 'ab';
SELECT 'ab'::mediumblob > 'ab';
SELECT 'ab'::longblob > 'ab';

SELECT 'ab'::blob >= 'ab';
SELECT 'ab'::tinyblob >= 'ab';
SELECT 'ab'::mediumblob >= 'ab';
SELECT 'ab'::longblob >= 'ab';

SELECT 'ab'::blob < 'ab';
SELECT 'ab'::tinyblob < 'ab';
SELECT 'ab'::mediumblob < 'ab';
SELECT 'ab'::longblob < 'ab';

SELECT 'ab'::blob <= 'ab';
SELECT 'ab'::tinyblob <= 'ab';
SELECT 'ab'::mediumblob <= 'ab';
SELECT 'ab'::longblob <= 'ab';

SELECT 'ab'::blob <> 'ab';
SELECT 'ab'::tinyblob <> 'ab';
SELECT 'ab'::mediumblob <> 'ab';
SELECT 'ab'::longblob <> 'ab';

SELECT 'ab' = 'ab'::blob;
SELECT 'ab' = 'ab'::tinyblob;
SELECT 'ab'= 'ab'::mediumblob ;
SELECT 'ab' = 'ab'::longblob;

SELECT 'ab' > 'ab'::blob;
SELECT 'ab' > 'ab'::tinyblob;
SELECT 'ab'> 'ab'::mediumblob ;
SELECT 'ab' > 'ab'::longblob;

SELECT 'ab' >= 'ab'::blob;
SELECT 'ab' >= 'ab'::tinyblob;
SELECT 'ab'>= 'ab'::mediumblob ;
SELECT 'ab' >= 'ab'::longblob;

SELECT 'ab' < 'ab'::blob;
SELECT 'ab' < 'ab'::tinyblob;
SELECT 'ab'< 'ab'::mediumblob ;
SELECT 'ab' < 'ab'::longblob;

SELECT 'ab' <= 'ab'::blob;
SELECT 'ab' <= 'ab'::tinyblob;
SELECT 'ab'<= 'ab'::mediumblob ;
SELECT 'ab' <= 'ab'::longblob;

SELECT 'ab' <> 'ab'::blob;
SELECT 'ab' <> 'ab'::tinyblob;
SELECT 'ab'<> 'ab'::mediumblob ;
SELECT 'ab' <> 'ab'::longblob;

SELECT 'ab'::blob = 'ab'::raw;
SELECT 'ab'::tinyblob = 'ab'::raw;
SELECT 'ab'::mediumblob = 'ab'::raw;
SELECT 'ab'::longblob = 'ab'::raw;

SELECT 'ab'::raw = 'ab'::blob;
SELECT 'ab'::raw = 'ab'::tinyblob;
SELECT 'ab'::raw = 'ab'::mediumblob;
SELECT 'ab'::raw = 'ab'::longblob;

SELECT 'ab'::blob = 'ab'::blob;
SELECT 'ab'::tinyblob = 'ab'::blob;
SELECT 'ab'::mediumblob = 'ab'::blob;
SELECT 'ab'::longblob = 'ab'::blob;

SELECT 'ab'::blob = 'ab'::tinyblob;
SELECT 'ab'::tinyblob = 'ab'::tinyblob;
SELECT 'ab'::mediumblob = 'ab'::tinyblob;
SELECT 'ab'::longblob = 'ab'::tinyblob;

SELECT 'ab'::blob = 'ab'::mediumblob;
SELECT 'ab'::tinyblob = 'ab'::mediumblob;
SELECT 'ab'::mediumblob = 'ab'::mediumblob;
SELECT 'ab'::longblob = 'ab'::mediumblob;

SELECT 'ab'::blob = 'ab'::longblob;
SELECT 'ab'::tinyblob = 'ab'::longblob;
SELECT 'ab'::mediumblob = 'ab'::longblob;
SELECT 'ab'::longblob = 'ab'::longblob;

SELECT 'ab'::blob < 'ab'::raw;
SELECT 'ab'::tinyblob < 'ab'::raw;
SELECT 'ab'::mediumblob < 'ab'::raw;
SELECT 'ab'::longblob < 'ab'::raw;

SELECT 'ab'::raw < 'ab'::blob;
SELECT 'ab'::raw < 'ab'::tinyblob;
SELECT 'ab'::raw < 'ab'::mediumblob;
SELECT 'ab'::raw < 'ab'::longblob;

SELECT 'ab'::blob < 'ab'::blob;
SELECT 'ab'::tinyblob < 'ab'::blob;
SELECT 'ab'::mediumblob < 'ab'::blob;
SELECT 'ab'::longblob < 'ab'::blob;

SELECT 'ab'::blob < 'ab'::tinyblob;
SELECT 'ab'::tinyblob < 'ab'::tinyblob;
SELECT 'ab'::mediumblob < 'ab'::tinyblob;
SELECT 'ab'::longblob < 'ab'::tinyblob;

SELECT 'ab'::blob < 'ab'::mediumblob;
SELECT 'ab'::tinyblob < 'ab'::mediumblob;
SELECT 'ab'::mediumblob < 'ab'::mediumblob;
SELECT 'ab'::longblob < 'ab'::mediumblob;

SELECT 'ab'::blob < 'ab'::longblob;
SELECT 'ab'::tinyblob < 'ab'::longblob;
SELECT 'ab'::mediumblob < 'ab'::longblob;
SELECT 'ab'::longblob < 'ab'::longblob;

SELECT 'ab'::blob <= 'ab'::raw;
SELECT 'ab'::tinyblob <= 'ab'::raw;
SELECT 'ab'::mediumblob <= 'ab'::raw;
SELECT 'ab'::longblob <= 'ab'::raw;

SELECT 'ab'::raw <= 'ab'::blob;
SELECT 'ab'::raw <= 'ab'::tinyblob;
SELECT 'ab'::raw <= 'ab'::mediumblob;
SELECT 'ab'::raw <= 'ab'::longblob;

SELECT 'ab'::blob <= 'ab'::blob;
SELECT 'ab'::tinyblob <= 'ab'::blob;
SELECT 'ab'::mediumblob <= 'ab'::blob;
SELECT 'ab'::longblob <= 'ab'::blob;

SELECT 'ab'::blob <= 'ab'::tinyblob;
SELECT 'ab'::tinyblob <= 'ab'::tinyblob;
SELECT 'ab'::mediumblob <= 'ab'::tinyblob;
SELECT 'ab'::longblob <= 'ab'::tinyblob;

SELECT 'ab'::blob <= 'ab'::mediumblob;
SELECT 'ab'::tinyblob <= 'ab'::mediumblob;
SELECT 'ab'::mediumblob <= 'ab'::mediumblob;
SELECT 'ab'::longblob <= 'ab'::mediumblob;

SELECT 'ab'::blob <= 'ab'::longblob;
SELECT 'ab'::tinyblob <= 'ab'::longblob;
SELECT 'ab'::mediumblob <= 'ab'::longblob;
SELECT 'ab'::longblob <= 'ab'::longblob;

SELECT 'ab'::blob >= 'ab'::raw;
SELECT 'ab'::tinyblob >= 'ab'::raw;
SELECT 'ab'::mediumblob >= 'ab'::raw;
SELECT 'ab'::longblob >= 'ab'::raw;

SELECT 'ab'::raw >= 'ab'::blob;
SELECT 'ab'::raw >= 'ab'::tinyblob;
SELECT 'ab'::raw >= 'ab'::mediumblob;
SELECT 'ab'::raw >= 'ab'::longblob;

SELECT 'ab'::blob >= 'ab'::blob;
SELECT 'ab'::tinyblob >= 'ab'::blob;
SELECT 'ab'::mediumblob >= 'ab'::blob;
SELECT 'ab'::longblob >= 'ab'::blob;

SELECT 'ab'::blob >= 'ab'::tinyblob;
SELECT 'ab'::tinyblob >= 'ab'::tinyblob;
SELECT 'ab'::mediumblob >= 'ab'::tinyblob;
SELECT 'ab'::longblob >= 'ab'::tinyblob;

SELECT 'ab'::blob >= 'ab'::mediumblob;
SELECT 'ab'::tinyblob >= 'ab'::mediumblob;
SELECT 'ab'::mediumblob >= 'ab'::mediumblob;
SELECT 'ab'::longblob >= 'ab'::mediumblob;

SELECT 'ab'::blob >= 'ab'::longblob;
SELECT 'ab'::tinyblob >= 'ab'::longblob;
SELECT 'ab'::mediumblob >= 'ab'::longblob;
SELECT 'ab'::longblob >= 'ab'::longblob;

SELECT 'ab'::blob > 'ab'::raw;
SELECT 'ab'::tinyblob > 'ab'::raw;
SELECT 'ab'::mediumblob > 'ab'::raw;
SELECT 'ab'::longblob > 'ab'::raw;

SELECT 'ab'::raw > 'ab'::blob;
SELECT 'ab'::raw > 'ab'::tinyblob;
SELECT 'ab'::raw > 'ab'::mediumblob;
SELECT 'ab'::raw > 'ab'::longblob;

SELECT 'ab'::blob > 'ab'::blob;
SELECT 'ab'::tinyblob > 'ab'::blob;
SELECT 'ab'::mediumblob > 'ab'::blob;
SELECT 'ab'::longblob > 'ab'::blob;

SELECT 'ab'::blob > 'ab'::tinyblob;
SELECT 'ab'::tinyblob > 'ab'::tinyblob;
SELECT 'ab'::mediumblob > 'ab'::tinyblob;
SELECT 'ab'::longblob > 'ab'::tinyblob;

SELECT 'ab'::blob > 'ab'::mediumblob;
SELECT 'ab'::tinyblob > 'ab'::mediumblob;
SELECT 'ab'::mediumblob > 'ab'::mediumblob;
SELECT 'ab'::longblob > 'ab'::mediumblob;

SELECT 'ab'::blob > 'ab'::longblob;
SELECT 'ab'::tinyblob > 'ab'::longblob;
SELECT 'ab'::mediumblob > 'ab'::longblob;
SELECT 'ab'::longblob > 'ab'::longblob;

SELECT 'ab'::blob <> 'ab'::raw;
SELECT 'ab'::tinyblob <> 'ab'::raw;
SELECT 'ab'::mediumblob <> 'ab'::raw;
SELECT 'ab'::longblob <> 'ab'::raw;

SELECT 'ab'::raw <> 'ab'::blob;
SELECT 'ab'::raw <> 'ab'::tinyblob;
SELECT 'ab'::raw <> 'ab'::mediumblob;
SELECT 'ab'::raw <> 'ab'::longblob;

SELECT 'ab'::blob <> 'ab'::blob;
SELECT 'ab'::tinyblob <> 'ab'::blob;
SELECT 'ab'::mediumblob <> 'ab'::blob;
SELECT 'ab'::longblob <> 'ab'::blob;

SELECT 'ab'::blob <> 'ab'::tinyblob;
SELECT 'ab'::tinyblob <> 'ab'::tinyblob;
SELECT 'ab'::mediumblob <> 'ab'::tinyblob;
SELECT 'ab'::longblob <> 'ab'::tinyblob;

SELECT 'ab'::blob <> 'ab'::mediumblob;
SELECT 'ab'::tinyblob <> 'ab'::mediumblob;
SELECT 'ab'::mediumblob <> 'ab'::mediumblob;
SELECT 'ab'::longblob <> 'ab'::mediumblob;

SELECT 'ab'::blob <> 'ab'::longblob;
SELECT 'ab'::tinyblob <> 'ab'::longblob;
SELECT 'ab'::mediumblob <> 'ab'::longblob;
SELECT 'ab'::longblob <> 'ab'::longblob;

create table t_dezebium_0007_02(c1 tinyblob,
c2 mediumblob,
c3 longblob,
c4 blob,
c5 binary(5),
c23 varbinary(50))default charset=utf8;

insert t_dezebium_0007_02 values('1010101','1010101','1010101','1010101','ab','ab');
select * from t_dezebium_0007_02;
set bytea_output=escape;
select * from t_dezebium_0007_02;
update t_dezebium_0007_02 set c2='101',c5='FG';
delete from t_dezebium_0007_02 where hex(c23)=6162;

DROP TABLE IF EXISTS t_index_test;
create table t_index_test(a blob, b tinyblob, c mediumblob, d longblob);
insert into t_index_test select i,i,i,i from generate_series(1,10000) i;
create index i_a on t_index_test(a);
create index i_b on t_index_test(b);
create index i_c on t_index_test(c);
create index i_d on t_index_test(d);
analyze t_index_test;
explain (costs off) select * from t_index_test where a='1';
explain (costs off) select * from t_index_test where a>='a1';
explain (costs off) select * from t_index_test where a>'a1';
explain (costs off) select * from t_index_test where a<='0';
explain (costs off) select * from t_index_test where a<'1';

explain (costs off) select * from t_index_test where b='1';
explain (costs off) select * from t_index_test where b>='a1';
explain (costs off) select * from t_index_test where b>'a1';
explain (costs off) select * from t_index_test where b<='0';
explain (costs off) select * from t_index_test where b<'1';

explain (costs off) select * from t_index_test where c='1';
explain (costs off) select * from t_index_test where c>='a1';
explain (costs off) select * from t_index_test where c>'a1';
explain (costs off) select * from t_index_test where c<='0';
explain (costs off) select * from t_index_test where c<'1';

explain (costs off) select * from t_index_test where d='1';
explain (costs off) select * from t_index_test where d>='a1';
explain (costs off) select * from t_index_test where d>'a1';
explain (costs off) select * from t_index_test where d<='0';
explain (costs off) select * from t_index_test where d<'1';

drop index i_a;
drop index i_b;
drop index i_c;
drop index i_d;
create index i_a on t_index_test(a) using hash;
create index i_b on t_index_test(b) using hash;
create index i_c on t_index_test(c) using hash;
create index i_d on t_index_test(d) using hash;
analyze t_index_test;
explain (costs off) select * from t_index_test where a='1';
explain (costs off) select * from t_index_test where b='1';
explain (costs off) select * from t_index_test where c='1';
explain (costs off) select * from t_index_test where d='1';

DROP TABLE t_index_test;

drop schema test_blob cascade;
reset current_schema;