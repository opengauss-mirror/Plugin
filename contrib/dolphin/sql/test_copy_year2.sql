drop schema if exists test_copy_year2;
create schema test_copy_year2;
set current_schema = test_copy_year2;
create table t_year2(c year(2));
copy t_year2(c) from stdin;
55
69
70
2055
2155
1970
\.
select * from t_year2 order by c;
drop table t_year2;
reset current_schema;
drop schema test_copy_year2 cascade;
