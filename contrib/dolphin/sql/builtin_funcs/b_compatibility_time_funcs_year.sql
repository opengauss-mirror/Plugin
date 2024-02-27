create schema b_compatibility_time_funcs_year;
set current_schema to 'b_compatibility_time_funcs_year';

set dolphin.b_compatibility_mode=on;
SET dolphin.sql_mode TO 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length';

drop table if exists t_year_0002;
create table t_year_0002(
c1 int not null,
c2 year default 1991,
c3 year(4),
c4 year);
insert into t_year_0002 values (1, 1901, 1901, 1901);

select c1, extract(year from c2), extract(year from c3),
extract(year from c4) from t_year_0002 order by c1;
select c1, yearweek(c2), yearweek(c3), yearweek(c4)
from t_year_0002 order by c1;
select c1, makedate(c2, 10), makedate(c3, 100), makedate(c4, 1000)
from t_year_0002 order by c1;
select c1, timestampadd(year, 1, c2), timestampadd(year, 1, c3),
timestampadd(year, 1, c4) from t_year_0002 order by c1;
select c1, timestampadd(day, timestamptz'2000-1-1 1:1:1+05', c2), timestampadd(day, timestamptz'2000-1-1 1:1:1+05', c3),
timestampadd(day, timestamptz'2000-1-1 1:1:1+05', c4) from t_year_0002 order by c1;
select c1, timestampadd(year, '1', c2), timestampadd(year, '1', c3),
timestampadd(year, '1', c4) from t_year_0002 order by c1;
select c1, timestampdiff(year, '2024-01-01', c2),
timestampdiff(year, '2024-01-01', c3),
timestampdiff(year, '2024-01-01', c4)
from t_year_0002 order by c1;
select c1, date_add(c2, interval '5' year), date_add(c3, interval '5' year),
date_add(c4, interval '5' year) from t_year_0002 order by c1;
select c1, date_sub(c2, interval '5' year), date_sub(c3, interval '5' year),
date_sub(c4, interval '5' year) from t_year_0002 order by c1;

drop table if exists func_test;
create table func_test(functionName varchar(256), result varchar(256));
insert ignore into func_test(functionName, result) values('extract(year from cast(1901 as year))', extract(year from cast(1901 as year)));
insert ignore into func_test(functionName, result) values('yearweek(cast(1901 as year))', yearweek(cast(1901 as year)));
insert ignore into func_test(functionName, result) values('makedate(cast(1901 as year), 10)', makedate(cast(1901 as year), 10));
insert ignore into func_test(functionName, result) values('timestampadd(year, 1, cast(1901 as year))', timestampadd(year, 1, cast(1901 as year)));
insert ignore into func_test(functionName, result) values('timestampadd(day, timestamptz''2000-1-1 1:1:1+05'', cast(1901 as year))', timestampadd(day, timestamptz'2000-1-1 1:1:1+05', cast(1901 as year)));
insert ignore into func_test(functionName, result) values('timestampadd(year, ''1'', cast(1901 as year))', timestampadd(year, '1', cast(1901 as year)));
insert ignore into func_test(functionName, result) values('timestampdiff(year, ''2024-01-01'', cast(1901 as year))', timestampdiff(year, '2024-01-01', cast(1901 as year)));
insert ignore into func_test(functionName, result) values('timestampdiff(year, cast(1901 as year), ''2024-01-01'')', timestampdiff(year, cast(1901 as year), '2024-01-01'));
insert ignore into func_test(functionName, result) values('date_add(cast(1901 as year), interval ''5'' year)', date_add(cast(1901 as year), interval '5' year));
insert ignore into func_test(functionName, result) values('date_sub(cast(1901 as year), interval ''5'' year)', date_sub(cast(1901 as year), interval '5' year));
select * from func_test;
truncate table func_test;

insert into func_test(functionName, result) values('extract(year from cast(1901 as year))', extract(year from cast(1901 as year)));
insert into func_test(functionName, result) values('yearweek(cast(1901 as year))', yearweek(cast(1901 as year)));
insert into func_test(functionName, result) values('makedate(cast(1901 as year), 10)', makedate(cast(1901 as year), 10));
insert into func_test(functionName, result) values('timestampadd(year, 1, cast(1901 as year))', timestampadd(year, 1, cast(1901 as year)));
insert into func_test(functionName, result) values('timestampadd(day, timestamptz''2000-1-1 1:1:1+05'', cast(1901 as year))', timestampadd(day, timestamptz'2000-1-1 1:1:1+05', cast(1901 as year)));
insert into func_test(functionName, result) values('timestampadd(year, ''1'', cast(1901 as year))', timestampadd(year, '1', cast(1901 as year)));
insert into func_test(functionName, result) values('timestampdiff(year, ''2024-01-01'', cast(1901 as year))', timestampdiff(year, '2024-01-01', cast(1901 as year)));
insert into func_test(functionName, result) values('timestampdiff(year, cast(1901 as year), ''2024-01-01'')', timestampdiff(year, cast(1901 as year), '2024-01-01'));
insert into func_test(functionName, result) values('date_add(cast(1901 as year), interval ''5'' year)', date_add(cast(1901 as year), interval '5' year));
insert into func_test(functionName, result) values('date_sub(cast(1901 as year), interval ''5'' year)', date_sub(cast(1901 as year), interval '5' year));
select * from func_test;
truncate table func_test;


-- 非严格模式下的写测试
SET dolphin.sql_mode TO 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert ignore into func_test(functionName, result) values('extract(year from cast(1901 as year))', extract(year from cast(1901 as year)));
insert ignore into func_test(functionName, result) values('yearweek(cast(1901 as year))', yearweek(cast(1901 as year)));
insert ignore into func_test(functionName, result) values('makedate(cast(1901 as year), 10)', makedate(cast(1901 as year), 10));
insert ignore into func_test(functionName, result) values('timestampadd(year, 1, cast(1901 as year))', timestampadd(year, 1, cast(1901 as year)));
insert ignore into func_test(functionName, result) values('timestampadd(day, timestamptz''2000-1-1 1:1:1+05'', cast(1901 as year))', timestampadd(day, timestamptz'2000-1-1 1:1:1+05', cast(1901 as year)));
insert ignore into func_test(functionName, result) values('timestampadd(year, ''1'', cast(1901 as year))', timestampadd(year, '1', cast(1901 as year)));
insert ignore into func_test(functionName, result) values('timestampdiff(year, ''2024-01-01'', cast(1901 as year))', timestampdiff(year, '2024-01-01', cast(1901 as year)));
insert ignore into func_test(functionName, result) values('timestampdiff(year, cast(1901 as year), ''2024-01-01'')', timestampdiff(year, cast(1901 as year), '2024-01-01'));
insert ignore into func_test(functionName, result) values('date_add(cast(1901 as year), interval ''5'' year)', date_add(cast(1901 as year), interval '5' year));
insert ignore into func_test(functionName, result) values('date_sub(cast(1901 as year), interval ''5'' year)', date_sub(cast(1901 as year), interval '5' year));
select * from func_test;
truncate table func_test;

insert into func_test(functionName, result) values('extract(year from cast(1901 as year))', extract(year from cast(1901 as year)));
insert into func_test(functionName, result) values('yearweek(cast(1901 as year))', yearweek(cast(1901 as year)));
insert into func_test(functionName, result) values('makedate(cast(1901 as year), 10)', makedate(cast(1901 as year), 10));
insert into func_test(functionName, result) values('timestampadd(year, 1, cast(1901 as year))', timestampadd(year, 1, cast(1901 as year)));
insert into func_test(functionName, result) values('timestampadd(day, timestamptz''2000-1-1 1:1:1+05'', cast(1901 as year))', timestampadd(day, timestamptz'2000-1-1 1:1:1+05', cast(1901 as year)));
insert into func_test(functionName, result) values('timestampadd(year, ''1'', cast(1901 as year))', timestampadd(year, '1', cast(1901 as year)));
insert into func_test(functionName, result) values('timestampdiff(year, ''2024-01-01'', cast(1901 as year))', timestampdiff(year, '2024-01-01', cast(1901 as year)));
insert into func_test(functionName, result) values('timestampdiff(year, cast(1901 as year), ''2024-01-01'')', timestampdiff(year, cast(1901 as year), '2024-01-01'));
insert into func_test(functionName, result) values('date_add(cast(1901 as year), interval ''5'' year)', date_add(cast(1901 as year), interval '5' year));
insert into func_test(functionName, result) values('date_sub(cast(1901 as year), interval ''5'' year)', date_sub(cast(1901 as year), interval '5' year));
select * from func_test;

drop table if exists func_test cascade;
drop table if exists t_year_0002 cascade;
drop schema b_compatibility_time_funcs_year cascade;
reset current_schema;