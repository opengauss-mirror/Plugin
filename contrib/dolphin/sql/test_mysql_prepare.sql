drop database if exists test_mysql_prepare;
create database test_mysql_prepare dbcompatibility 'b';
\c test_mysql_prepare
create table test(name text, age int);
insert into test values('a',18);
prepare s1 as select * from test;
execute s1;
DEALLOCATE s1;
prepare s2(varchar(10)) as select * from test where name=$1;
execute s2('a');
execute s2;
DEALLOCATE s2;
prepare s3 as select * from test where name=$1;
execute s3('a');
execute s3;
DEALLOCATE s3;
prepare s1_mysql from select * from test;
execute s1_mysql;
execute s1_mysql('a');
DEALLOCATE s1_mysql;
prepare s2_mysql from select * from test where name=$1;
execute s2_mysql('a');
execute s2_mysql;
DEALLOCATE s2_mysql;
prepare s3_mysql(varchar(10)) as select * from test where name=$1;
execute s3_mysql('a');
execute s3_mysql;
DEALLOCATE s3_mysql;
drop table test;

prepare s_error from 'create table t1(a int)';
prepare s_error from 'select * from test; select * from test';
prepare s_error from 'prepare s_error2 from '' prepare s_error3 from select * from t1''';

create table prepare_table_01(a int, b numeric) ;
create table prepare_table_02(a int, b int) ;
insert into prepare_table_01 values(1, 1),(1, 1),(1, 1);
insert into prepare_table_02 values(1, 1),(1, 1),(1, 1);
analyze prepare_table_01;
analyze prepare_table_02;

set dolphin.b_compatibility_mode to on;
prepare p1 from 'select * from prepare_table_01, prepare_table_02 where prepare_table_01.a = prepare_table_02.a';
prepare p2(int) from 'select * from prepare_table_01, prepare_table_02 where prepare_table_01.a = prepare_table_02.a and prepare_table_01.b = ?';
prepare s_error from 'select * from prepare_table_01, prepare_table_02 where prepare_table_01.a = prepare_table_02.a and prepare_table_01.b = ? and prepare_table_01.a = $1';

--stream plan and stream exec.
explain (costs off, verbose on) execute p1;
execute p1;

explain (costs off, verbose on) execute p2(1);
execute p2(1);
execute p2 using 1;

set enable_set_variable_b_format to on;
set @a = 1;
set @b = 2;
set @c = 3;
execute p2 using @a;
execute p2 using @b;
execute p2 using @a, @b;

prepare p5 as 'select a from prepare_table_01 INTERSECT select a from prepare_table_02 order by 1 limit 1';
explain (costs off, verbose on) execute p5;
execute p5;

prepare p6 as '(select * from prepare_table_01) union (select * from prepare_table_02)';
explain (costs off, verbose on) execute p6;
execute p6;

SELECT jsonb '{"a":null, "b":"qq"}' ? 'a';
SELECT jsonb_exists('{"a":null, "b":"qq"}', 'a');
SELECT jsonb '{"a":null, "b":"qq"}' ?| ARRAY['a','b'];
SELECT jsonb '{"a":null, "b":"qq"}' ?& ARRAY['a','b'];

DEALLOCATE p1;
DEALLOCATE p2;
DEALLOCATE p5;
DEALLOCATE p6;

insert into prepare_table_02 values(5, 5);
prepare p1 from 'select * from prepare_table_02 where a=?+?+?+?+?';
execute p1 using @a,@a,@a,@b,0;
DEALLOCATE p1;

prepare p1 from 'select * from prepare_table_02 where a=? +? +?+ ?+?';
execute p1 using @a,@a,@a,@b,0;
DEALLOCATE p1;

prepare p1 from 'select * from prepare_table_02 where a= ?* ?*?*? +?';
execute p1 using @a,@a,@a,@b,@c;
DEALLOCATE p1;

prepare p1 from 'select * from prepare_table_02 where a= ?* ?*? *?+ ?';
execute p1 using @a,@a,@a,@b,@c;
DEALLOCATE p1;

set dolphin.b_compatibility_mode to off;
prepare p2(int) from 'select * from prepare_table_01, prepare_table_02 where prepare_table_01.a = prepare_table_02.a and prepare_table_01.b = ?';
SELECT jsonb '{"a":null, "b":"qq"}' ? 'a';
SELECT jsonb '{"a":null, "b":"qq"}' ?| ARRAY['a','b'];
SELECT jsonb '{"a":null, "b":"qq"}' ?& ARRAY['a','b'];

drop table prepare_table_01;
drop table prepare_table_02;

set dolphin.b_compatibility_mode to on;
-- Testset 1 dynamic datanode reduction for single table
create table t1_xc_fqs(id1 int, id2 int, num int);

-- only params
prepare s as 'select * from t1_xc_fqs where id1=? and id2=?';
prepare i as insert into t1_xc_fqs values (?, ?, ?);
prepare u as 'update t1_xc_fqs set num=0 where id1=? and id2=?';
prepare d as delete from t1_xc_fqs where id1=? and id2=?;
insert into t1_xc_fqs values (1,1,1), (2,2,2), (3,3,3), (4,4,4), (5,5,5);

explain (costs off, verbose on) execute s using 1,@a;
explain (costs off, verbose on) execute i using 6,6,6;
explain (costs off, verbose on) execute u using 2,@b;
explain (costs off, verbose on) execute d using @c,@c;
execute s using @a,1;
execute i using 6,6,6;
select * from t1_xc_fqs order by id1;
execute u using @b,2;
select * from t1_xc_fqs order by id1;
execute d using @c,@c;
select * from t1_xc_fqs order by id1;
deallocate s;
deallocate i;
deallocate u;
deallocate d;

prepare s as select * from t1_xc_fqs where id1=? and id2=2;
prepare i as 'insert into t1_xc_fqs values (?, 2, 3)';
prepare u as update t1_xc_fqs set num=1 where id1=? and id2=2;
prepare d as 'delete from t1_xc_fqs where id1=? and id2=2';
truncate t1_xc_fqs;
insert into t1_xc_fqs values (1,1,1), (2,2,2), (3,3,3), (4,4,4), (5,5,5);
explain (costs off, verbose on) execute s using @a;
explain (costs off, verbose on) execute i using 6;
explain (costs off, verbose on) execute u using 2;
explain (costs off, verbose on) execute d using @c;
execute s using @a;
execute i using 6;
select * from t1_xc_fqs order by id1;
execute u using 2;
select * from t1_xc_fqs order by id1;
execute d using @c;
select * from t1_xc_fqs order by id1, id2;

deallocate s;
deallocate i;
deallocate u;
deallocate d;

-- Testset 2 dynamic datanode reduction for multi-table join
truncate t1_xc_fqs;
insert into t1_xc_fqs values (1,1,11), (2,2,21), (3,3,31), (4,4,41), (5,5,51);
create table t2_xc_fqs(id1 int, id2 int, num int);
insert into t2_xc_fqs values (1,2,12), (2,3,22), (3,4,32), (4,5,42), (5,6,52);
create table t3_xc_fqs(id11 int, id22 int, num int);
insert into t3_xc_fqs values (1,13,13), (2,23,23), (3,33,33), (4,43,43), (5,53,53);

-- implicit join
prepare s0 as select * from t1_xc_fqs t1,t2_xc_fqs t2 where t1.id1=? and t2.id1=?;
prepare s1 as 'select id11 from t1_xc_fqs t1,t2_xc_fqs t2,t3_xc_fqs t3 where t1.id1=? and t2.id1=? and t3.id11=?';
prepare s2 as select * from t1_xc_fqs t1,t2_xc_fqs t2 where t1.id1=t2.id1 and t1.id1=?;
prepare s3 as 'select id11 from t1_xc_fqs t1,t2_xc_fqs t2,t3_xc_fqs t3 where t1.id1=t2.id1 and t3.id11=?';
prepare s4 as select id11 from t1_xc_fqs t1,t2_xc_fqs t2,t3_xc_fqs t3 where t1.id1=t2.id1 and t1.id1=t3.id11 and t3.id11=?;
prepare s5 as 'select * from t1_xc_fqs t1,t2_xc_fqs t2 where t1.id1=t2.id2 and t2.id1=? and t2.id2=?';

explain (costs off, verbose on) execute s0 using @a,@a;
explain (costs off, verbose on) execute s0 using @a,3;
explain (costs off, verbose on) execute s1 using 2,@b,2;
explain (costs off, verbose on) execute s2 using @c;
explain (costs off, verbose on) execute s3 using 4;
explain (costs off, verbose on) execute s4 using 5;
explain (costs off, verbose on) execute s5 using 4,5;
execute s0 using @a,@a;
execute s0 using @a,3;
execute s1 using 2,@b,2;
execute s2 using @c;
execute s3 (4);
execute s4 (5);
execute s5 (4,5);

deallocate s0;
deallocate s1;
deallocate s2;
deallocate s3;
deallocate s4;
deallocate s5;

-- explicit join
prepare s0 as 'select * from t1_xc_fqs t1 join t2_xc_fqs t2 on t1.id1=$1 and t2.id1=$2';
prepare s1 as select id11 from t1_xc_fqs t1 join t2_xc_fqs t2 on t1.id1=$1 and t2.id1=$2 join t3_xc_fqs t3 on t3.id11=$3;
prepare s2 as 'select * from t1_xc_fqs t1 join t2_xc_fqs t2 on t1.id1=t2.id1 and t2.id1=$1';
prepare s3 as select id11 from t1_xc_fqs t1 join t2_xc_fqs t2 on t1.id1=t2.id1 join t3_xc_fqs t3 on t3.id11=$1;
prepare s4 as 'select id11 from t1_xc_fqs t1 join t2_xc_fqs t2 on t1.id1=t2.id1 join t3_xc_fqs t3 on t1.id1=t3.id11 and t3.id11=$1';
prepare s5 as select * from t1_xc_fqs t1 join t2_xc_fqs t2 on t1.id1=t2.id2 where t2.id1=$1 and t2.id2=$2;

explain (costs off, verbose on) execute s0 using @a,@a;
explain (costs off, verbose on) execute s0 using @a,@c;
explain (costs off, verbose on) execute s1 using 2,@b,2;
explain (costs off, verbose on) execute s2 using @c;
explain (costs off, verbose on) execute s3 using 4;
explain (costs off, verbose on) execute s4 using 5;
explain (costs off, verbose on) execute s5 using 4,5;
execute s0 using @a,@a;
execute s0 using @a,3;
execute s1 using 2,@b,2;
execute s2 using @c;
execute s3 (4);
execute s4 (5);
execute s5 (4,5);

deallocate s0;
deallocate s1;
deallocate s2;
deallocate s3;
deallocate s4;
deallocate s5;

prepare s0 as select * from t1_xc_fqs t1 left join t2_xc_fqs t2 on t1.id1=t2.id1 and t2.id1=? order by t1.id1;
prepare s1 as 'select * from t1_xc_fqs t1 right join t2_xc_fqs t2 on t1.id1=t2.id1 and t2.id1=? order by t2.id1';
prepare s2 as select * from t1_xc_fqs t1 full join t2_xc_fqs t2 on t1.id1=t2.id1 and t2.id1=? order by t1.id1, t2.id1;

explain (costs off, verbose on) execute s0 using @a;
explain (costs off, verbose on) execute s1 using 1;
explain (costs off, verbose on) execute s2 using 1;
execute s0 using 1;
execute s1 using @a;
execute s2 using 1;

deallocate s0;
deallocate s1;
deallocate s2;

reset dolphin.b_compatibility_mode;
reset enable_set_variable_b_format;
\c postgres
drop database test_mysql_prepare;