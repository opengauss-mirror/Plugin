create schema db_alter_table;
set current_schema to 'db_alter_table';

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

set dolphin.sql_mode='pipes_as_concat,pad_char_to_full_length';

create table table_ddl_0030_01(col1 int primary key,col2 varchar(20));
create table table_ddl_0030_02(col1 int,col2 int);
ALTER TABLE table_ddl_0030_02 ADD CONSTRAINT FOREIGN KEY idx_ddl_0030(col1) REFERENCES table_ddl_0030_01(col1);
show dolphin.sql_mode;

create table foreign_key_table_001(
COL_1 smallint primary key,
COL_2 char(30),
COL_3 int,
COL_4 TIMESTAMP WITH TIME ZONE ,
COL_5 boolean,
COL_6 nchar(30),
COL_7 float
);

create table foreign_key_table_002(
COL_1 smallint default 3,
COL_2 char(30),
COL_3 int,
COL_4 TIMESTAMP WITH TIME ZONE ,
COL_5 boolean,
COL_6 nchar(30),
COL_7 float,
foreign key(COL_1) REFERENCES foreign_key_table_001 on delete set default
);

insert into foreign_key_table_001 values(1,'aaaa',1,'2020-07-20',true,'aaaa',1.1);
insert into foreign_key_table_001 values(2,'bbbb',2,'2020-07-20',true,'bbbb',1.1);
insert into foreign_key_table_001 values(3,'cccc',3,'2020-07-20',true,'cccc',1.1);

insert into foreign_key_table_002 values(1,'dddd',1,'2020-07-20',true,'dddd',2.2);
insert into foreign_key_table_002 values(2,'eeee',2,'2020-07-20',true,'eeee',2.2);

delete from foreign_key_table_001 where COL_1=1;
select * from foreign_key_table_002 order by 1,2,3;
drop table foreign_key_table_001,foreign_key_table_002;
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

--new add column grammar
create table add_col_test(a int);
ALTER TABLE add_col_test ADD KEY f2k(f2), ADD COLUMN(f1 INT, f3 int), ADD COLUMN (f2 INT);
ALTER TABLE if EXISTS add_col_test ADD COLUMN (f11 INT, f31 int), ADD COLUMN (f21 INT), ADD KEY f1k(f1);
ALTER TABLE if EXISTS add_col_test ADD (f12 INT, f32 int), ADD COLUMN (f22 INT);
ALTER TABLE if EXISTS add_col_test ADD COLUMN (f13 INT, f33 int), ADD COLUMN (f23 INT);
ALTER TABLE if EXISTS does_not_exists_table_test ADD COLUMN (f1 INT, f3 int), ADD COLUMN (f2 INT), ADD KEY f2k(f2);

--original grammar
ALTER TABLE add_col_test ADD (f14 INT, f34 int);
ALTER TABLE if EXISTS add_col_test ADD (f15 INT, f35 int);
ALTER TABLE if EXISTS does_not_exists_table_test ADD (f13 INT, f33 int);
\d+ add_col_test

drop table add_col_test;
drop schema db_alter_table cascade;
reset current_schema;
