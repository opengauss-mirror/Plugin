create schema test_binary_like;
set search_path = test_binary_like;

set enable_indexscan = off;

create table test_binary(a binary(10));
create index on test_binary(a);
insert into test_binary values('aaa');
explain(costs off)select * from test_binary where a like 'a%';
select * from test_binary where a like 'a%';

create table test_varbinary(a varbinary(10));
create index on test_varbinary(a);
insert into test_varbinary values('aaa');
explain(costs off) select * from test_varbinary where a like 'a%';
select * from test_varbinary where a like 'a%';

reset search_path;
drop schema test_binary_like cascade;