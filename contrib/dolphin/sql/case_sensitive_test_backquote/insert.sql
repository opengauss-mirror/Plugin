\c table_name_test_db;te_test
SET dolphin.lower_case_table_names TO 0;
--
-- insert with DEFAULT in the target_list
--
create table `Inserttest` (`col1` int4, `col2` int4 NOT NULL, `col3` text default 'testing');
insert into `Inserttest` (`col1`, `col2`, `col3`) values (DEFAULT, DEFAULT, DEFAULT);
insert into `Inserttest` (`col2`, `col3`) values (3, DEFAULT);
insert into `Inserttest` (`col1`, `col2`, `col3`) values (DEFAULT, 5, DEFAULT);
insert into `Inserttest` values (DEFAULT, 5, 'test');
insert into `Inserttest` values (DEFAULT, 7);

select * from `Inserttest`;

--
-- insert with similar expression / target_list values (all fail)
--
insert into `Inserttest` (`col1`, `col2`, `col3`) values (DEFAULT, DEFAULT);
insert into `Inserttest` (`col1`, `col2`, `col3`) values (1, 2);
insert into `Inserttest` (`col1`) values (1, 2);
insert into `Inserttest` (`col1`) values (DEFAULT, DEFAULT);

select * from `Inserttest`;

--
-- VALUES test
--
insert into `Inserttest` values(10, 20, '40'), (-1, 2, DEFAULT),
    ((select 2), (select `i` from (values(3)) as foo (`i`)), 'values are fun!');

select * from `Inserttest` order by 1,2,3;

--
-- TOASTed value test
--
insert into `Inserttest` values(30, 50, repeat('x', 10000));

select `col1`, `col2`, char_length(`col3`) from `Inserttest` order by 1,2,3;

drop table `Inserttest`;

--
---- I tried to use generate_series() function to load a bunch of test data in a
---- table. It fails with the following error:
---- set-valued function called in context that cannot accept a set
--
-- distribute by hash
create table `Test_hash`(`a` int, `b` char(10)); 
insert into `Test_hash` values (generate_series(1,1000), 'foo');
drop table `Test_hash`;

-- distribute by modulo
create table `Test_modulo`(`a` int, `b` char(10)); 
insert into `Test_modulo` values (generate_series(1,1000), 'foo');
drop table `Test_modulo`;

-- distribute by replication
create table `Test_replication`(`a` int, `b` char(10));
insert into `Test_replication` values (generate_series(1,1000), 'foo');
drop table `Test_replication`;

-- distribute by roundrobin
create table `Test_roundrobin`(`a` int, `b` char(10));
insert into `Test_roundrobin` values (generate_series(1,1000), 'foo');
drop table `Test_roundrobin`;

--insert into pg_auth_history
insert into `pg_auth_history` values(10, '2015-110-10 08:00:00.57603+08', 'sha256232f8630ce6af1095f6db3ed4c05a48747038936d42176e1103594d43c7d1adc4aca54361a23e51c6cd9371ccc95776450219376e45bcca01e27a7f06bf8088a8b1a9e280cdcc315c8134879818442bc3e92064a70e27b2ea83fcf6990a607d0');