drop database if exists b_char_blank_test;
create database b_char_blank_test encoding 'UTF-8' lc_collate 'en_US.UTF-8' lc_ctype 'en_US.UTF-8' dbcompatibility 'B';
\c b_char_blank_test

---------------------------
----- row table
--------------------------
create table t_char1(a char(10), b varchar(10), c text);
insert into t_char1 values('1','1','1');
insert into t_char1 values(' 1 ',' 1 ',' 1 ');
insert into t_char1 values('1 ','1 ','1 ');
insert into t_char1 values('1  ','1  ','1  ');
insert into t_char1 values(' 1',' 1',' 1');
create table t_char2(a char(10), b varchar(10), c text);
insert into t_char2 values('1','1','1');
insert into t_char2 values(' 1 ',' 1 ',' 1 ');
insert into t_char2 values('1 ','1 ','1 ');
insert into t_char2 values('1  ','1  ','1  ');
insert into t_char2 values(' 1',' 1',' 1');

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

select length(text_larger('1', '1 ')), length(text_larger('1', '1   ')), length(text_larger('1', ' 1 ')), length(text_larger('1', ' 1'));
select length(text_smaller('1', '1 ')), length(text_smaller('1', '1   ')), length(text_smaller('1', ' 1 ')), length(text_smaller('1', ' 1'));
select text_pattern_lt('1', '1 '), text_pattern_lt('1', '1   '), text_pattern_lt('1', ' 1 '), text_pattern_lt('1', ' 1');
select text_pattern_le('1', '1 '), text_pattern_le('1', '1   '), text_pattern_le('1', ' 1 '), text_pattern_le('1', ' 1');
select text_pattern_ge('1', '1 '), text_pattern_ge('1', '1   '), text_pattern_ge('1', ' 1 '), text_pattern_ge('1', ' 1');
select text_pattern_gt('1', '1 '), text_pattern_gt('1', '1   '), text_pattern_gt('1', ' 1 '), text_pattern_gt('1', ' 1');
select bttext_pattern_cmp('1', '1 '), bttext_pattern_cmp('1', '1   '), bttext_pattern_cmp('1', ' 1 '), bttext_pattern_cmp('1', ' 1');
select bttextcmp('1', '1 '), bttextcmp('1', '1   '), bttextcmp('1', ' 1 '), bttextcmp('1', ' 1');

--- row table + vec engine
set try_vector_engine_strategy to force;
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

select length(text_larger('1', '1 ')), length(text_larger('1', '1   ')), length(text_larger('1', ' 1 ')), length(text_larger('1', ' 1'));
select length(text_smaller('1', '1 ')), length(text_smaller('1', '1   ')), length(text_smaller('1', ' 1 ')), length(text_smaller('1', ' 1'));
select text_pattern_lt('1', '1 '), text_pattern_lt('1', '1   '), text_pattern_lt('1', ' 1 '), text_pattern_lt('1', ' 1');
select text_pattern_le('1', '1 '), text_pattern_le('1', '1   '), text_pattern_le('1', ' 1 '), text_pattern_le('1', ' 1');
select text_pattern_ge('1', '1 '), text_pattern_ge('1', '1   '), text_pattern_ge('1', ' 1 '), text_pattern_ge('1', ' 1');
select text_pattern_gt('1', '1 '), text_pattern_gt('1', '1   '), text_pattern_gt('1', ' 1 '), text_pattern_gt('1', ' 1');
select bttext_pattern_cmp('1', '1 '), bttext_pattern_cmp('1', '1   '), bttext_pattern_cmp('1', ' 1 '), bttext_pattern_cmp('1', ' 1');
select bttextcmp('1', '1 '), bttextcmp('1', '1   '), bttextcmp('1', ' 1 '), bttextcmp('1', ' 1');

drop table t_char1;
drop table t_char2;

\c postgres
drop database b_char_blank_test;