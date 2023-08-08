create schema issue_online;
set current_schema to 'issue_online';

set dolphin.sql_mode='';
drop table if exists tt01;
create table tt01(a int, b int, c int);

insert into tt01 values(1, 1, 1);
insert into tt01 values(1, 2, 2);
insert into tt01 values(1, 3, 3);
insert into tt01 values(1, 4, 4);
insert into tt01 values(1, 5, 5);

insert into tt01 values(2, 6, 6);
insert into tt01 values(2, 7, 7);
insert into tt01 values(2, 8, 8);
insert into tt01 values(2, 9, 9);
insert into tt01 values(2, 10, 10);

--包含having子句，不包含grouping by子句。
explain (costs off)select * from tt01 having a = 1;
select * from tt01 having a = 1;

--包含having子句和where子句，不包含grouping by子句。
explain (costs off)select * from tt01 where a = 1 having b < 3;
select * from tt01 where a = 1 having b < 3;

--包含having子句，不包含grouping by子句, having子句中包含agg函数
explain (costs off)select * from tt01 where a = 1 having avg(b) > 1;
select * from tt01 where a = 1 having avg(b) > 1;
explain (costs off)select sum(b), avg(b) from tt01 where a = 1 having avg(b) > 1;
select sum(b), avg(b) from tt01 where a = 1 having avg(b) > 1;

--包含having子句，包含grouping by子句
explain (costs off)select * from tt01 group by a having b < 3;
select * from tt01 group by a having b < 3;
explain (costs off)select * from tt01 group by a having avg(b) > 1;
select * from tt01 group by a having avg(b) > 1;
explain (costs off)select sum(b), avg(b) from tt01 group by a having avg(b) > 1;
select sum(b), avg(b) from tt01 group by a having avg(b) > 1;

--包含grouping sets
explain (costs off)select * from tt01 group by grouping sets((a), ());
select * from tt01 group by grouping sets((a), ());

--sql_mode_full_group 模式下，直接报错
set dolphin.sql_mode='sql_mode_full_group';
select * from tt01 having a = 1;  --error

set dolphin.sql_mode='sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length';
drop schema issue_online cascade;
reset current_schema;