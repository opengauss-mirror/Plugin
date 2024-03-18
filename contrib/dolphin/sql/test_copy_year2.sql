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

\copy t_year2(c) to './test_year_binary.data' with binary;
\copy t_year2(c) from './test_year_binary.data' with binary;
select * from t_year2 order by c;

create table t_year4(c year(4));
copy t_year4(c) from stdin;
55
69
70
2055
2155
1970
\.
select * from t_year4 order by c;

\copy t_year4(c) to './test_year_binary.data' with binary;
\copy t_year4(c) from './test_year_binary.data' with binary;
select * from t_year4 order by c;

select pg_catalog.year_recv(pg_catalog.int4send(9));

drop table t_year2;
drop table t_year4;
reset current_schema;
drop schema test_copy_year2 cascade;
