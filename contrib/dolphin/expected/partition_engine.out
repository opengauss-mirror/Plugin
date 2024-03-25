create schema partition_engine;
set current_schema = partition_engine;
create table t1 (int_column int, char_column char(5))
PARTITION BY RANGE (int_column) subpartition by key (char_column) subpartitions 2
(PARTITION p1 VALUES LESS THAN (5) ENGINE = InnoDB);
create table t2 (int_column int, char_column char(5))
PARTITION BY RANGE (int_column) subpartition by key (char_column) subpartitions 1
(PARTITION p1 VALUES LESS THAN (5) ENGINE = InnoDB (SUBPARTITION p11 ENGINE = InnoDB));
create table t3 (int_column int, char_column char(5))
PARTITION BY KEY (int_column)
(PARTITION p1 ENGINE = InnoDB);
create table t4 (int_column int, char_column char(5))
PARTITION BY LIST (int_column) subpartition by key (char_column)
(PARTITION p1 VALUES IN(1, 2, 3) ENGINE = InnoDB (SUBPARTITION p11 ENGINE = InnoDB));
drop table t1, t2, t3, t4;
reset current_schema;
drop schema partition_engine;
