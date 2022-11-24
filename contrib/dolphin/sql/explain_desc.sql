create database db_explain_desc with dbcompatibility 'B';
\c db_explain_desc

create table ed_t(c1 int, c2 varchar(100), c3 int default 10);
insert into ed_t values(generate_series(1, 10), 'hello', 100);

-- 1.use explain to query table's info
explain ed_t;
explain public.ed_t;

-- 2.use desc to query plan info
desc select c1, c2, c3 from ed_t;
desc insert into ed_t values(100, 'hello', 100);
desc update ed_t set c2 = 'world' where c1 < 5;
desc extended delete from ed_t where c1 < 5;
describe select c1, c2, c3 from ed_t;
describe insert into ed_t values(100, 'hello', 100);
describe extended update ed_t set c2 = 'world' where c1 < 5;
describe extended delete from ed_t where c1 < 5;
desc format=json select c1, c2, c3 from ed_t;
describe format=traditional insert into ed_t values(100, 'hello', 100);

-- 3.explain with key world: extended
explain extended select c1, c2, c3 from ed_t;
explain extended insert into ed_t values(100, 'hello', 100);
explain extended update ed_t set c2 = 'world' where c1 < 5;
explain extended delete from ed_t where c1 < 5;

-- 4.explain with specifying format(json or traditional)
explain format=json select c1, c2, c3 from ed_t;
explain format=json insert into ed_t values(100, 'hello', 100);
explain format=JSON update ed_t set c2 = 'world' where c1 < 5;
explain format=json delete from ed_t where c1 < 5;
explain format=Json delete from ed_t where c1 < 5;
explain format=JsOn delete from ed_t where c1 < 5;
explain format='JsOn' delete from ed_t where c1 < 5;
explain format='json' delete from ed_t where c1 < 5;
explain format=TRADITIONAL select c1, c2, c3 from ed_t;
explain format=TRADITIONAL insert into ed_t values(100, 'hello', 100);
explain format=TRaDITiONAL update ed_t set c2 = 'world' where c1 < 5;
explain format=traditional delete from ed_t where c1 < 5;
explain format='traditional' delete from ed_t where c1 < 5;
explain format='TraDitional' delete from ed_t where c1 < 5;

drop table ed_t;

\c postgres

drop database db_explain_desc;
