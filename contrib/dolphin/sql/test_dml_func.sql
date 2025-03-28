create schema test_dml_func;
set current_schema to 'test_dml_func';

create table test_type_table
(
   `int1` tinyint,
   `uint1` tinyint unsigned,
   `int2` smallint,
   `uint2` smallint unsigned,
   `int4` integer,
   `uint4` integer unsigned,
   `int8` bigint,
   `uint8` bigint unsigned,
   `float4` float4,
   `float8` float8,
   `numeric` decimal(20, 6),
   `bit1` bit(1),
   `bit64` bit(64),
   `boolean` boolean,
   `date` date,
   `time` time,
   `time(4)` time(4),
   `datetime` datetime,
   `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
   `timestamp` timestamp,
   `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
   `year` year,
   `char` char(100),
   `varchar` varchar(100), 
   `binary` binary(100),
   `varbinary` varbinary(100),
   `tinyblob` tinyblob,
   `blob` blob,
   `mediumblob` mediumblob,
   `longblob` longblob,
   `text` text,
   `enum_t` enum('a', 'b', 'c'),
   `set_t` set('a', 'b', 'c'),
   `json` json   
);


create table test_type_table_ustore
(
   `int1` tinyint,
   `uint1` tinyint unsigned,
   `int2` smallint,
   `uint2` smallint unsigned,
   `int4` integer,
   `uint4` integer unsigned,
   `int8` bigint,
   `uint8` bigint unsigned,
   `float4` float4,
   `float8` float8,
   `numeric` decimal(20, 6),
   `bit1` bit(1),
   `bit64` bit(64),
   `boolean` boolean,
   `date` date,
   `time` time,
   `time(4)` time(4),
   `datetime` datetime,
   `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
   `timestamp` timestamp,
   `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
   `year` year,
   `char` char(100),
   `varchar` varchar(100), 
   `binary` binary(100),
   `varbinary` varbinary(100),
   `tinyblob` tinyblob,
   `blob` blob,
   `mediumblob` mediumblob,
   `longblob` longblob,
   `text` text,
   `enum_t` enum('a', 'b', 'c'),
   `set_t` set('a', 'b', 'c'),
   `json` json   
) with (storage_type = ustore);


create table test_type_table_partition
(
   `int1` tinyint,
   `uint1` tinyint unsigned,
   `int2` smallint,
   `uint2` smallint unsigned,
   `int4` integer,
   `uint4` integer unsigned,
   `int8` bigint,
   `uint8` bigint unsigned,
   `float4` float4,
   `float8` float8,
   `numeric` decimal(20, 6),
   `bit1` bit(1),
   `bit64` bit(64),
   `boolean` boolean,
   `date` date,
   `time` time,
   `time(4)` time(4),
   `datetime` datetime,
   `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
   `timestamp` timestamp,
   `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
   `year` year,
   `char` char(100),
   `varchar` varchar(100), 
   `binary` binary(100),
   `varbinary` varbinary(100),
   `tinyblob` tinyblob,
   `blob` blob,
   `mediumblob` mediumblob,
   `longblob` longblob,
   `text` text,
   `enum_t` enum('a', 'b', 'c'),
   `set_t` set('a', 'b', 'c'),
   `json` json   
) PARTITION BY RANGE(`int4`)
(
    PARTITION P1 VALUES LESS THAN(1),
    PARTITION P2 VALUES LESS THAN(2),
    PARTITION P3 VALUES LESS THAN(3),
    PARTITION P4 VALUES LESS THAN(MAXVALUE)
);


create table test_type_table_second_partition
(
   `int1` tinyint,
   `uint1` tinyint unsigned,
   `int2` smallint,
   `uint2` smallint unsigned,
   `int4` integer,
   `uint4` integer unsigned,
   `int8` bigint,
   `uint8` bigint unsigned,
   `float4` float4,
   `float8` float8,
   `numeric` decimal(20, 6),
   `bit1` bit(1),
   `bit64` bit(64),
   `boolean` boolean,
   `date` date,
   `time` time,
   `time(4)` time(4),
   `datetime` datetime,
   `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
   `timestamp` timestamp,
   `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
   `year` year,
   `char` char(100),
   `varchar` varchar(100), 
   `binary` binary(100),
   `varbinary` varbinary(100),
   `tinyblob` tinyblob,
   `blob` blob,
   `mediumblob` mediumblob,
   `longblob` longblob,
   `text` text,
   `enum_t` enum('a', 'b', 'c'),
   `set_t` set('a', 'b', 'c'),
   `json` json   
) PARTITION BY RANGE(`int4`) SUBPARTITION BY hash(`int1`)
(
    PARTITION P1 VALUES LESS THAN(1)
    (
        SUBPARTITION p0_0,
        SUBPARTITION p0_1,
        SUBPARTITION p0_2
    ),
    PARTITION P2 VALUES LESS THAN(2)
    (
        SUBPARTITION p1_0,
        SUBPARTITION p1_1,
        SUBPARTITION p1_2
    ),
    PARTITION P3 VALUES LESS THAN(3)
    (
        SUBPARTITION p2_0,
        SUBPARTITION p2_1,
        SUBPARTITION p2_2
    ),
    PARTITION P4 VALUES LESS THAN(MAXVALUE)
    (
        SUBPARTITION p3_0,
        SUBPARTITION p3_1,
        SUBPARTITION p3_2
    )
);


create temporary table test_type_table_temporary
(
   `int1` tinyint,
   `uint1` tinyint unsigned,
   `int2` smallint,
   `uint2` smallint unsigned,
   `int4` integer,
   `uint4` integer unsigned,
   `int8` bigint,
   `uint8` bigint unsigned,
   `float4` float4,
   `float8` float8,
   `numeric` decimal(20, 6),
   `bit1` bit(1),
   `bit64` bit(64),
   `boolean` boolean,
   `date` date,
   `time` time,
   `time(4)` time(4),
   `datetime` datetime,
   `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
   `timestamp` timestamp,
   `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
   `year` year,
   `char` char(100),
   `varchar` varchar(100), 
   `binary` binary(100),
   `varbinary` varbinary(100),
   `tinyblob` tinyblob,
   `blob` blob,
   `mediumblob` mediumblob,
   `longblob` longblob,
   `text` text,
   `enum_t` enum('a', 'b', 'c'),
   `set_t` set('a', 'b', 'c'),
   `json` json   
);

create UNLOGGED  table test_type_table_unlogged
(
   `int1` tinyint,
   `uint1` tinyint unsigned,
   `int2` smallint,
   `uint2` smallint unsigned,
   `int4` integer,
   `uint4` integer unsigned,
   `int8` bigint,
   `uint8` bigint unsigned,
   `float4` float4,
   `float8` float8,
   `numeric` decimal(20, 6),
   `bit1` bit(1),
   `bit64` bit(64),
   `boolean` boolean,
   `date` date,
   `time` time,
   `time(4)` time(4),
   `datetime` datetime,
   `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
   `timestamp` timestamp,
   `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
   `year` year,
   `char` char(100),
   `varchar` varchar(100), 
   `binary` binary(100),
   `varbinary` varbinary(100),
   `tinyblob` tinyblob,
   `blob` blob,
   `mediumblob` mediumblob,
   `longblob` longblob,
   `text` text,
   `enum_t` enum('a', 'b', 'c'),
   `set_t` set('a', 'b', 'c'),
   `json` json   
);

create table test_type_table_segment
(
   `int1` tinyint,
   `uint1` tinyint unsigned,
   `int2` smallint,
   `uint2` smallint unsigned,
   `int4` integer,
   `uint4` integer unsigned,
   `int8` bigint,
   `uint8` bigint unsigned,
   `float4` float4,
   `float8` float8,
   `numeric` decimal(20, 6),
   `bit1` bit(1),
   `bit64` bit(64),
   `boolean` boolean,
   `date` date,
   `time` time,
   `time(4)` time(4),
   `datetime` datetime,
   `datetime(4)` datetime(4) default '2022-11-11 11:11:11',
   `timestamp` timestamp,
   `timestamp(4)` timestamp(4) default '2022-11-11 11:11:11',
   `year` year,
   `char` char(100),
   `varchar` varchar(100), 
   `binary` binary(100),
   `varbinary` varbinary(100),
   `tinyblob` tinyblob,
   `blob` blob,
   `mediumblob` mediumblob,
   `longblob` longblob,
   `text` text,
   `enum_t` enum('a', 'b', 'c'),
   `set_t` set('a', 'b', 'c'),
   `json` json   
) with(segment = on);

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

insert into test_type_table_ustore values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

insert into test_type_table_partition values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

insert into test_type_table_second_partition values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

insert into test_type_table_temporary values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

insert into test_type_table_unlogged values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));


insert into test_type_table_segment values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));


-- select (subquery) from tbl_name
select (select t1.`int4`) from test_type_table t1;
select (select sum(`int1` + `int4`)) from test_type_table;
select (select `int4` from test_type_table t1 where t1.`int4` = t2.`int4`) from test_type_table t2;
select (select `int4` from test_type_table t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table t2;
select (select `int4` from (select * from test_type_table) t1 where t1.`int4` = t2.`int4`) from test_type_table t2;
select (select `int4` from test_type_table t1 where t1.`int4` in (select t2.`int4`)) from test_type_table t2;
create view v1 as select (select `int4` from test_type_table t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table t2;
select * from v1;


select (select t1.`int4`) from test_type_table_ustore t1;
select (select sum(`int1` + `int4`)) from test_type_table_ustore;
select (select `int4` from test_type_table_ustore t1 where t1.`int4` = t2.`int4`) from test_type_table_ustore t2;
select (select `int4` from test_type_table_ustore t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table_ustore t2;
select (select `int4` from (select * from test_type_table_ustore) t1 where t1.`int4` = t2.`int4`) from test_type_table_ustore t2;
select (select `int4` from test_type_table_ustore t1 where t1.`int4` in (select t2.`int4`)) from test_type_table_ustore t2;
create view v2 as select (select `int4` from test_type_table_ustore t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table_ustore t2;
select * from v2;

select (select t1.`int4`) from test_type_table_partition t1;
select (select sum(`int1` + `int4`)) from test_type_table_second_partition;
select (select `int4` from test_type_table_second_partition t1 where t1.`int4` = t2.`int4`) from test_type_table_partition t2;
select (select `int4` from test_type_table_second_partition t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table_partition t2;
select (select `int4` from (select * from test_type_table_partition) t1 where t1.`int4` = t2.`int4`) from test_type_table t2;
select (select `int4` from test_type_table_partition t1 where t1.`int4` in (select t2.`int4`)) from test_type_table_second_partition t2;
create view v3 as select (select `int4` from test_type_table_partition t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table_second_partition t2;
select * from v3;

select (select t1.`int4`) from test_type_table_temporary t1;
select (select sum(`int1` + `int4`)) from test_type_table_unlogged;
select (select `int4` from test_type_table_unlogged t1 where t1.`int4` = t2.`int4`) from test_type_table_temporary t2;
select (select `int4` from test_type_table_temporary t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table_unlogged t2;
select (select `int4` from (select * from test_type_table_unlogged) t1 where t1.`int4` = t2.`int4`) from test_type_table_temporary t2;
select (select `int4` from test_type_table_temporary t1 where t1.`int4` in (select t2.`int4`)) from test_type_table_unlogged t2;
create view v4 as select (select `int4` from test_type_table_temporary t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table_unlogged t2;
select * from v4;

select (select t1.`int4`) from test_type_table_segment t1;
select (select sum(`int1` + `int4`)) from test_type_table_segment;
select (select `int4` from test_type_table_segment t1 where t1.`int4` = t2.`int4`) from test_type_table_segment t2;
select (select `int4` from test_type_table_segment t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table_segment t2;
select (select `int4` from (select * from test_type_table_segment) t1 where t1.`int4` = t2.`int4`) from test_type_table_segment t2;
select (select `int4` from test_type_table_segment t1 where t1.`int4` in (select t2.`int4`)) from test_type_table_segment t2;
create view v5 as select (select `int4` from test_type_table_segment t1 where t1.`int4` = t2.`int4` order by t2.`int4`) from test_type_table_segment t2;
select * from v5;

set enable_opfusion=on;
explain select (select t1.`int4`) from test_type_table t1;
reset enable_opfusion;


-- non_subquery_operand LIKE (subquery)
select * from test_type_table where `text` like (select '1%') union all select * from test_type_table where `int4` like (select '1%'); 
select * from test_type_table where `text` like (select `text` from test_type_table) union all select * from test_type_table where `int4` like (select `int4` from test_type_table); 
select * from test_type_table where `text` like (select `int1` from test_type_table) union all select * from test_type_table where `text` like (select `int4` from test_type_table); 
select * from test_type_table where `date` like (select '2023-%');
select * from test_type_table where `text` like (select upper(`text`) from test_type_table);
select * from test_type_table where `text` like (select concat(`text`, '%') from test_type_table);
select * from test_type_table where `text` like (select concat(`text`, 'a%') from test_type_table);
select * from test_type_table where `timestamp` like (select '2023-%');
select * from test_type_table where `date` like (select '2023-%' union all select '2023-%');


select * from test_type_table_ustore where `text` like (select '1%') union all select * from test_type_table_ustore where `int4` like (select '1%'); 
select * from test_type_table_ustore where `text` like (select `text` from test_type_table_ustore) union all select * from test_type_table_ustore where `int4` like (select `int4` from test_type_table_ustore); 
select * from test_type_table_ustore where `text` like (select `int1` from test_type_table_ustore) union all select * from test_type_table_ustore where `text` like (select `int4` from test_type_table_ustore); 
select * from test_type_table_ustore where `date` like (select '2023-%');
select * from test_type_table_ustore where `text` like (select upper(`text`) from test_type_table_ustore);
select * from test_type_table_ustore where `text` like (select concat(`text`, '%') from test_type_table_ustore);
select * from test_type_table_ustore where `text` like (select concat(`text`, 'a%') from test_type_table_ustore);
select * from test_type_table_ustore where `timestamp` like (select '2023-%');
select * from test_type_table_ustore where `date` like (select '2023-%' union all select '2023-%');


select * from test_type_table_partition where `text` like (select '1%') union all select * from test_type_table_second_partition where `int4` like (select '1%'); 
select * from test_type_table_second_partition where `text` like (select `text` from test_type_table_partition) union all select * from test_type_table_second_partition where `int4` like (select `int4` from test_type_table); 
select * from test_type_table where `text` like (select `int1` from test_type_table_second_partition) union all select * from test_type_table where `text` like (select `int4` from test_type_table); 
select * from test_type_table_second_partition where `date` like (select '2023-%');
select * from test_type_table_partition where `text` like (select upper(`text`) from test_type_table);
select * from test_type_table_second_partition where `text` like (select concat(`text`, '%') from test_type_table_partition);
select * from test_type_table_second_partition where `text` like (select concat(`text`, 'a%') from test_type_table);
select * from test_type_table_partition where `timestamp` like (select '2023-%');
select * from test_type_table_second_partition where `date` like (select '2023-%' union all select '2023-%');


select * from test_type_table_unlogged where `text` like (select '1%') union all select * from test_type_table_unlogged where `int4` like (select '1%'); 
select * from test_type_table_temporary where `text` like (select `text` from test_type_table_unlogged) union all select * from test_type_table_segment where `int4` like (select `int4` from test_type_table_unlogged); 
select * from test_type_table_unlogged where `text` like (select `int1` from test_type_table_temporary) union all select * from test_type_table_segment where `text` like (select `int4` from test_type_table_temporary); 
select * from test_type_table_temporary where `date` like (select '2023-%');
select * from test_type_table_temporary where `text` like (select upper(`text`) from test_type_table_unlogged);
select * from test_type_table_temporary where `text` like (select concat(`text`, '%') from test_type_table_temporary);
select * from test_type_table_unlogged where `text` like (select concat(`text`, 'a%') from test_type_table_segment);
select * from test_type_table_unlogged where `timestamp` like (select '2023-%');
select * from test_type_table_temporary where `date` like (select '2023-%' union all select '2023-%');


-- ANY
select * from test_type_table t1 where `text` = any(select `text` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `text` = any(select NULL);
select * from test_type_table t1 where `int4` > any(select `int4` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `int1` < any(select `int1` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `time` >= any(select `time` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `datetime` <= any(select `datetime` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `timestamp` <> any(select `timestamp` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `blob` != any(select `blob` from test_type_table t2 where t1.`int4` = t2.`int4`);


select * from test_type_table_ustore t1 where `text` != any(select `text` from test_type_table_ustore t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_ustore t1 where `text` > any(select NULL);
select * from test_type_table_ustore t1 where `int4` >= any(select `int4` from test_type_table_ustore t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_ustore t1 where `int1` <= any(select `int1` from test_type_table_ustore t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_ustore t1 where `time` < any(select `time` from test_type_table_ustore t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_ustore t1 where `datetime` = any(select `datetime` from test_type_table_ustore t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_ustore t1 where `timestamp` <= any(select `timestamp` from test_type_table_ustore t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_ustore t1 where `blob` <> any(select `blob` from test_type_table_ustore t2 where t1.`int4` = t2.`int4`);

-- some
select * from test_type_table t1 where `text` = SOME(select `text` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `text` = SOME(select NULL);
select * from test_type_table t1 where `int4` > SOME(select `int4` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `int1` < SOME(select `int1` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `time` >= SOME(select `time` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `datetime` <= SOME(select `datetime` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `timestamp` <> SOME(select `timestamp` from test_type_table t2 where t1.`int4` = t2.`int4`);
select * from test_type_table t1 where `blob` != SOME(select `blob` from test_type_table t2 where t1.`int4` = t2.`int4`);


select * from test_type_table_partition t1 where `text` != SOME(select `text` from test_type_table_second_partition t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_partition t1 where `text` > SOME(select NULL);
select * from test_type_table_second_partition t1 where `int4` >= SOME(select `int4` from test_type_table_partition t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_second_partition t1 where `int1` <= SOME(select `int1` from test_type_table_partition t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_second_partition t1 where `time` < SOME(select `time` from test_type_table_partition t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_partition t1 where `datetime` = SOME(select `datetime` from test_type_table_second_partition t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_partition t1 where `timestamp` <= SOME(select `timestamp` from test_type_table_second_partition t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_second_partition t1 where `blob` <> SOME(select `blob` from test_type_table_partition t2 where t1.`int4` = t2.`int4`);


-- all
select * from test_type_table_unlogged t1 where `text` = ALL(select `text` from test_type_table_temporary t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_temporary t1 where `text` = ALL(select NULL);
select * from test_type_table_temporary t1 where `int4` > ALL(select `int4` from test_type_table_unlogged t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_unlogged t1 where `int1` < ALL(select `int1` from test_type_table_temporary t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_temporary t1 where `time` >= ALL(select `time` from test_type_table_unlogged t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_temporary t1 where `datetime` <= ALL(select `datetime` from test_type_table_unlogged t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_unlogged t1 where `timestamp` <> ALL(select `timestamp` from test_type_table_temporary t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_unlogged t1 where `blob` != ALL(select `blob` from test_type_table_temporary t2 where t1.`int4` = t2.`int4`);


select * from test_type_table_segment t1 where `text` != ALL(select `text` from test_type_table_segment t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_segment t1 where `text` > ALL(select NULL);
select * from test_type_table_segment t1 where `int4` >= ALL(select `int4` from test_type_table_segment t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_segment t1 where `int1` <= ALL(select `int1` from test_type_table_segment t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_segment t1 where `time` < ALL(select `time` from test_type_table_segment t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_segment t1 where `datetime` = ALL(select `datetime` from test_type_table_segment t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_segment t1 where `timestamp` <= ALL(select `timestamp` from test_type_table_segment t2 where t1.`int4` = t2.`int4`);
select * from test_type_table_segment t1 where `blob` <> ALL(select `blob` from test_type_table_segment t2 where t1.`int4` = t2.`int4`);

-- in
select * from test_type_table t1 where `text` in (select `text` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_ustore t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where `text` in (select NULL);
select * from test_type_table t1 where `int4` in (select `int4` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_partition t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where `int1` in (select `int1` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_second_partition t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where `time` in (select `time` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_temporary t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where `datetime` in (select `datetime` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_unlogged t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where `timestamp` in (select `timestamp` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_segment t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where `blob` in (select `blob` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_ustore t3 where t3.`int4` = t1.`int4`);

-- in index column
create index index11 on test_type_table(`text`);
create index index12 on test_type_table(`int4`);
create index index13 on test_type_table(`int1`);
create index index21 on test_type_table_ustore(`text`);
create index index22 on test_type_table_ustore(`int4`);
create index index23 on test_type_table_ustore(`int1`);
create index index31 on test_type_table_partition(`text`);
create index index32 on test_type_table_partition(`int4`);
create index index33 on test_type_table_partition(`int1`);
create index index41 on test_type_table_second_partition(`text`);
create index index42 on test_type_table_second_partition(`int4`);
create index index43 on test_type_table_second_partition(`int1`);
create index index51 on test_type_table_temporary(`text`);
create index index52 on test_type_table_temporary(`int4`);
create index index53 on test_type_table_temporary(`int1`);
create index index61 on test_type_table_unlogged(`text`);
create index index62 on test_type_table_unlogged(`int4`);
create index index63 on test_type_table_unlogged(`int1`);
create index index71 on test_type_table_segment(`text`);
create index index72 on test_type_table_segment(`int4`);
create index index73 on test_type_table_segment(`int1`);
create index index81 on test_type_table_partition(`time`);
create index index82 on test_type_table_second_partition(`datetime`);
create index index83 on test_type_table(`timestamp`);
create index index84 on test_type_table(`blob`);

select * from test_type_table t1 where `int4` in (select `int4` from test_type_table_ustore);
select * from test_type_table t1 where `int4` not in (select `int4` from test_type_table_ustore);
select * from test_type_table t1 where `int4` in (select 1);
select * from test_type_table t1 where `int4` not in (select 1 from test_type_table_ustore);
select * from test_type_table t1 where `int4` in (select '1');
select * from test_type_table t1 where `int4` not in (select '1');
select * from test_type_table t1 where `int4` in (select `int4` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_ustore t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where `text` in (select NULL);
select * from test_type_table_segment t1 where `int4` in (select `int4` from test_type_table_ustore t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_partition t3 where t3.`int4` = t1.`int4`);
select * from test_type_table_ustore t1 where `int1` in (select `int1` from test_type_table_partition t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_segment t3 where t3.`int4` = t1.`int4`);
select * from test_type_table_partition t1 where `time` in (select `time` from test_type_table_partition t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_temporary t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where `datetime` in (select `datetime` from test_type_table_second_partition t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_temporary t3 where t3.`int4` = t1.`int4`);
select * from test_type_table_second_partition t1 where `timestamp` in (select `timestamp` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_segment t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where `blob` in (select `blob` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_ustore t3 where t3.`int4` = t1.`int4`);

-- exists
select * from test_type_table t1 where EXISTS (select `int4` from test_type_table_ustore);
select * from test_type_table t1 where not EXISTS (select `int4` from test_type_table_ustore);
select * from test_type_table t1 where EXISTS (select 1,2,3);
select * from test_type_table t1 where not EXISTS (select 1,2,3 from test_type_table_ustore);
select * from test_type_table t1 where EXISTS (select '1', '2', '3');
select * from test_type_table t1 where not EXISTS (select '1', '2', '3');
select * from test_type_table t1 where EXISTS (select `int4` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_ustore t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where EXISTS (select NULL);
select * from test_type_table_segment t1 where EXISTS (select `int4` from test_type_table_ustore t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_partition t3 where t3.`int4` = t1.`int4`);
select * from test_type_table_ustore t1 where EXISTS (select `int1` from test_type_table_partition t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_segment t3 where t3.`int4` = t1.`int4`);
select * from test_type_table_partition t1 where not EXISTS (select `time` from test_type_table_partition t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_temporary t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where EXISTS (select `datetime` from test_type_table_second_partition t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_temporary t3 where t3.`int4` = t1.`int4`);
select * from test_type_table_second_partition t1 where EXISTS (select `timestamp` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_segment t3 where t3.`int4` = t1.`int4`);
select * from test_type_table t1 where EXISTS (select `blob` from test_type_table t2 where t1.`int4` = t2.`int4` union all select `text` from test_type_table_ustore t3 where t3.`int4` = t1.`int4`);

-- [AS] tbl_name
select * from (select * from test_type_table) t1 join (select * from test_type_table_ustore t2) t2 on t1.`int4` = t2.`int4`;
select * from (select * from test_type_table) as t1 join (select * from test_type_table_ustore t2) t2 on t1.`int4` = t2.`int4`;
select * from (select t1.`int4` from test_type_table as t1 join test_type_table_partition on 1 = 1) as t1 join (select * from test_type_table_ustore t2) t2 where t1.`int4` = t2.`int4`;
select * from (select * from test_type_table_partition) t1 join (select * from test_type_table_second_partition t2) t2 on t1.`int4` = t2.`int4`;
select * from (select * from test_type_table_temporary) t1 join (select * from test_type_table_unlogged t2) t2 on t1.`int4` = t2.`int4`;
select * from (select * from test_type_table) t1 join (select * from test_type_table_segment t2) t2 on t1.`int4` = t2.`int4`;
select * from (select * from test_type_table) int4 join (select * from test_type_table_ustore t2) t2 on int4.`int4` = t2.`int4`;
select * from ((select * from test_type_table) t1 join (select `text` from test_type_table_ustore t2) t2 on t1.`text` = t2.`text`) t3 where t3.`int4` = 1;
select * from (select * from (select * from ((select * from test_type_table) t1 join (select `text` from test_type_table t2) t2 on t1.`text` = t2.`text`)t3 where t3.`int4` = 1) as t4)t5;

drop view v1;
drop view v2;
drop view v3;
drop view v4;
drop view v5;

drop table test_type_table;
drop table test_type_table_ustore;
drop table test_type_table_partition;
drop table test_type_table_second_partition;
drop table test_type_table_temporary;
drop table test_type_table_unlogged;
drop table test_type_table_segment;

drop schema test_dml_func cascade;
reset current_schema;
