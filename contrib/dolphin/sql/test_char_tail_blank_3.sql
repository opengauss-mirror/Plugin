set dolphin.b_compatibility_mode to off;
drop database if exists b_char_blank_test_3;
create database b_char_blank_test_3 encoding 'UTF-8' lc_collate 'C' lc_ctype 'C' dbcompatibility 'B';
\c b_char_blank_test_3

---------------------------
----- column table
--------------------------
create table t_char1(a char(10), b varchar(10), c text) with (orientation=column);
insert into t_char1 values('1','1','1');
insert into t_char1 values(' 1 ',' 1 ',' 1 ');
insert into t_char1 values('1 ','1 ','1 ');
insert into t_char1 values('1  ','1  ','1  ');
insert into t_char1 values(' 1',' 1',' 1');
create table t_char2(a char(10), b varchar(10), c text) with (orientation=column);
insert into t_char2 values('1','1','1');
insert into t_char2 values(' 1 ',' 1 ',' 1 ');
insert into t_char2 values('1 ','1 ','1 ');
insert into t_char2 values('1  ','1  ','1  ');
insert into t_char2 values(' 1',' 1',' 1');

--- column table + codegen
set enable_codegen to on;
set codegen_cost_threshold to 0;

select * from t_char1 where a='1 ';
select * from t_char1 where a='1                                        ';
select * from t_char1 where a=' 1 ';
select * from t_char1 where a=' 1';
select * from t_char1 where a='1';
select * from t_char1 where b='1 ';
select * from t_char1 where b='1                                       ';
select * from t_char1 where b=' 1 ';
select * from t_char1 where b=' 1';
select * from t_char1 where b='1';
select * from t_char1 where c='1 ';
select * from t_char1 where c='1                                       ';
select * from t_char1 where c=' 1 ';
select * from t_char1 where c=' 1';
select * from t_char1 where c='1';

select * from t_char1 where a!='1 ';
select * from t_char1 where a!='1                                        ';
select * from t_char1 where a!=' 1 ';
select * from t_char1 where a!=' 1';
select * from t_char1 where a!='1';
select * from t_char1 where b!='1 ';
select * from t_char1 where b!='1                                       ';
select * from t_char1 where b!=' 1 ';
select * from t_char1 where b!=' 1';
select * from t_char1 where b!='1';
select * from t_char1 where c!='1 ';
select * from t_char1 where c!='1                                       ';
select * from t_char1 where c!=' 1 ';
select * from t_char1 where c!=' 1';
select * from t_char1 where c!='1';

select * from t_char1 where a>='1 ';
select * from t_char1 where a>='1                                        ';
select * from t_char1 where a>=' 1 ';
select * from t_char1 where a>=' 1';
select * from t_char1 where a>='1';
select * from t_char1 where b>='1 ';
select * from t_char1 where b>='1                                       ';
select * from t_char1 where b>=' 1 ';
select * from t_char1 where b>=' 1';
select * from t_char1 where b>='1';
select * from t_char1 where c>='1 ';
select * from t_char1 where c>='1                                       ';
select * from t_char1 where c>=' 1 ';
select * from t_char1 where c>=' 1';
select * from t_char1 where c>='1';

select * from t_char1 where a<='1 ';
select * from t_char1 where a<='1                                        ';
select * from t_char1 where a<=' 1 ';
select * from t_char1 where a<=' 1';
select * from t_char1 where a<='1';
select * from t_char1 where b<='1 ';
select * from t_char1 where b<='1                                       ';
select * from t_char1 where b<=' 1 ';
select * from t_char1 where b<=' 1';
select * from t_char1 where b<='1';
select * from t_char1 where c<='1 ';
select * from t_char1 where c<='1                                       ';
select * from t_char1 where c<=' 1 ';
select * from t_char1 where c<=' 1';
select * from t_char1 where c<='1';

select * from t_char1 where a<'1 ';
select * from t_char1 where a<'1                                        ';
select * from t_char1 where a<' 1 ';
select * from t_char1 where a<' 1';
select * from t_char1 where a<'1';
select * from t_char1 where b<'1 ';
select * from t_char1 where b<'1                                       ';
select * from t_char1 where b<' 1 ';
select * from t_char1 where b<' 1';
select * from t_char1 where b<'1';
select * from t_char1 where c<'1 ';
select * from t_char1 where c<'1                                       ';
select * from t_char1 where c<' 1 ';
select * from t_char1 where c<' 1';
select * from t_char1 where c<'1';

select * from t_char1 where a>'1 ';
select * from t_char1 where a>'1                                        ';
select * from t_char1 where a>' 1 ';
select * from t_char1 where a>' 1';
select * from t_char1 where a>'1';
select * from t_char1 where b>'1 ';
select * from t_char1 where b>'1                                       ';
select * from t_char1 where b>' 1 ';
select * from t_char1 where b>' 1';
select * from t_char1 where b>'1';
select * from t_char1 where c>'1 ';
select * from t_char1 where c>'1                                       ';
select * from t_char1 where c>' 1 ';
select * from t_char1 where c>' 1';
select * from t_char1 where c>'1';

--only when string_hash_compatible is on, then hash join will ignore tail blank
show string_hash_compatible;
set enable_hashjoin to on;
set enable_mergejoin to off;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;

explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;

explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;

set enable_hashjoin to off;
set enable_mergejoin to on;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;

explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;

explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;

set enable_mergejoin to off;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.a=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;

explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.b=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;

explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.a order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.b order by 1,2,3,4,5,6,7,8,9,10,11,12;
explain(costs off) select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;
select a.a,length(a.a),a.b,length(a.b),a.c,length(a.c),b.a,length(b.a),b.b,length(b.b),b.c,length(b.c) from t_char1 a left join t_char2 b on a.c=b.c order by 1,2,3,4,5,6,7,8,9,10,11,12;

drop table t_char1;
drop table t_char2;
\c postgres
drop database b_char_blank_test_3;