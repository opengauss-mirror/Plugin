drop database if exists db_alter_table;
create database db_alter_table dbcompatibility 'b';
\c db_alter_table

create table alter_table_tbl1 (a int primary key, b int);
create table alter_table_tbl2 (c int primary key, d int);
alter table alter_table_tbl2 add constraint alter_table_tbl_fk foreign key (d) references alter_table_tbl1 (a);

create index alter_table_tbl_b_ind on alter_table_tbl1(b);

-- disbale/enable keys
alter table alter_table_tbl1 disable keys;
alter table alter_table_tbl1 enable keys;

-- drop index/key index_name
alter table alter_table_tbl1 drop index alter_table_tbl_b_ind;

create index alter_table_tbl_b_ind on alter_table_tbl1(b);
alter table alter_table_tbl1 drop key alter_table_tbl_b_ind;

-- drop primary key
alter table alter_table_tbl2 drop primary key;

-- drop foreign key fk_name
alter table alter_table_tbl2 drop foreign key alter_table_tbl_fk;

-- force
alter table alter_table_tbl1 force;

-- rename index/key old_index_name to new_index_name
create index alter_table_tbl_b_ind on alter_table_tbl1(b);
alter table alter_table_tbl1 rename index alter_table_tbl_b_ind to new_alter_table_tbl_b_ind;

-- test multi-cmd
alter table alter_table_tbl1 add column key int, rename index new_alter_table_tbl_b_ind to alter_table_tbl_b_ind;
alter table alter_table_tbl1 drop column key, drop key alter_table_tbl_b_ind;

drop table alter_table_tbl1, alter_table_tbl2;

set dolphin.sql_mode='pipes_as_concat';

create table table_ddl_0030_01(col1 int primary key,col2 varchar(20));
create table table_ddl_0030_02(col1 int,col2 int);
ALTER TABLE table_ddl_0030_02 ADD CONSTRAINT FOREIGN KEY idx_ddl_0030(col1) REFERENCES table_ddl_0030_01(col1);
show dolphin.sql_mode;

reset dolphin.sql_mode;
\d+ table_ddl_0030_02
drop table table_ddl_0030_01,table_ddl_0030_02;

create table test_primary(f11 int, f12 varchar(20), f13 bool);
alter table test_primary add primary key using btree(f11 desc, f12 asc) comment 'primary key' using btree;
\d+ test_primary
drop table test_primary;

create table test_primary(f11 int, f12 varchar(20), f13 bool);
alter table test_primary add primary key (f11 desc, f12 asc) comment 'primary key' using btree;
\d+ test_primary
drop table test_primary;

create table test_primary(f11 int, f12 varchar(20), f13 bool);
alter table test_primary add primary key using btree(f11 desc, f12 asc) comment 'primary key' using btree using btree;
\d+ test_primary
drop table test_primary;

create table test_unique(f31 int, f32 varchar(20));
alter table test_unique add unique using btree(f31) comment 'unique index' using btree;
\d+ test_unique
drop table test_unique;

create table test_unique(f31 int, f32 varchar(20));
alter table test_unique add unique (f31) comment 'unique index' using btree;
\d+ test_unique
drop table test_unique;

create table test_unique(f31 int, f32 varchar(20));
alter table test_unique add unique using btree(f31) comment 'unique index' using btree using btree;
\d+ test_unique
drop table test_unique;

create table test_unique(f31 int, f32 varchar(20), constraint con_t_unique unique using btree(f31, f32) comment 'unique index' using btree);
\d+ test_unique
drop table test_unique;

create table test_unique(f31 int, f32 varchar(20), constraint con_t_unique unique (f31, f32) comment 'unique index' using btree);
\d+ test_unique
drop table test_unique;

create table test_unique(f31 int, f32 varchar(20), constraint con_t_unique unique (f31, f32) comment 'unique index' using btree using btree);
\d+ test_unique
drop table test_unique;

create table test_primary(f11 int, f12 varchar(20), f13 bool, constraint con_t_pri primary key using btree(f11 desc, f12 asc) comment 'primary key' using btree);
\d+ test_primary
drop table test_primary;

create table test_primary(f11 int, f12 varchar(20), f13 bool, constraint con_t_pri primary key (f11 desc, f12 asc) comment 'primary key' using btree);
\d+ test_primary
drop table test_primary;

create table test_primary(f11 int, f12 varchar(20), f13 bool, constraint con_t_pri primary key using btree(f11 desc, f12 asc) comment 'primary key' using btree using btree);
\d+ test_primary
drop table test_primary;

\c postgres
drop database if exists db_alter_table;
