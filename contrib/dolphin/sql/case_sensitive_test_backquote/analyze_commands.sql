\c table_name_test_db;
SET lower_case_table_names TO 0;
create schema `analyze_commands`;
set search_path to analyze_commands;

drop table if exists `T1`;
create table `T1`(`a` int, `b` int, `c` int);
insert into `T1` values(generate_series(1,10),generate_series(1,2),generate_series(1,2));
set default_statistics_target=100;
analyze `T1`;
analyze `T1`((`b`,`c`));
select * from `pg_stats` where `tablename` = 'T1' order by `attname`;
select * from `pg_catalog`.`pg_ext_stats`;

drop table if exists `T1`;
create table `T1`(`a` int, `b` int, `c` int);
insert into `T1` values(generate_series(1,10),generate_series(1,2),generate_series(1,2));
set default_statistics_target=100;
select * from `pg_stats` where `tablename` = 'T1' order by `attname`;
select * from `pg_catalog`.`pg_ext_stats`;

drop table if exists `T1`;
create table `T1`(`a` int, `b` int, `c` int);
insert into `T1` values(generate_series(1,10),generate_series(1,2),generate_series(1,2));
set default_statistics_target=-2;
analyze `T1`;
analyze `T1`((`b`,`c`));
select * from `pg_stats` where `tablename` = 'T1' order by `attname`;
select * from `pg_catalog`.`pg_ext_stats`;

insert into `T1` values(generate_series(1,10),generate_series(3,4),generate_series(3,4));
select * from `pg_stats` where `tablename` = 'T1' order by `attname`;
select * from `pg_catalog`.`pg_ext_stats`;

reset search_path;
drop schema `analyze_commands` cascade;

