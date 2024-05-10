create schema partition_no_key;
set current_schema to 'partition_no_key';

-- primary key
create table t1 (a int, b int, primary key (b)) partition by key () partitions 2;
create table t2 (a int, b int) partition by key () partitions 2;
create table t3 (a int, b int, primary key (b)) partition by key (b) partitions 2;
create table t4 (a int, b int) partition by key (b) partitions 2;

-- composite primary key
create table t5 (a int, b int, primary key (a, b)) partition by key () partitions 2;
create table t6 (a int, b int, primary key (a, b)) partition by key (a, b) partitions 2;
