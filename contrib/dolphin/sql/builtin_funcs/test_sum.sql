create schema db_test_sum;
set current_schema to 'db_test_sum';

set dolphin.sql_mode = '';

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

insert into test_type_table values (NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

insert into test_type_table values (NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));


insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));


insert into test_type_table values (NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

insert into test_type_table values (NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

create table test_sum as select  sum(`int1`) as sum1, sum(`uint1`) as sum2, sum(`int2`) as sum3,  sum(`uint2`) as sum4,  sum(`int4`) as sum5, sum(`uint4`) as sum6,  sum(`int8`) as sum7,  sum(`uint8`) as sum8, sum(`float4`) as sum9, sum(`float8`) as sum10,  sum(`numeric`) as sum11, sum(`bit1`) as sum12,  sum(`bit64`) as sum13,  sum(`boolean`) as sum14, sum(`date`) as sum15, sum(`time`) as sum16, sum(`time(4)`) as sum17, sum(`datetime`) as sum18, sum(`datetime(4)`) as sum19, sum(`timestamp`) as sum20, sum(`timestamp(4)`) as sum21, sum(`year`) as sum22, sum(`char`) as sum23,  sum(`varchar`) as sum24,  sum(`binary`) as sum25, sum(`varbinary`) as sum26, sum(`tinyblob`) as sum27, sum(`blob`) as sum28, sum(`mediumblob`) as sum29, sum(`longblob`) as sum30, sum(`text`) as sum31, sum(`enum_t`) as sum32, sum(`set_t`) as sum33 from test_type_table;

select * from test_sum;

\d test_sum

create table test_text(c1 text);
insert into test_text values ('9999999999999999999999999999999999999999999999999999999.99');
insert into test_text values ('9999999999999999999999999999999999999999999999999999999.99');
insert into test_text values ('9999999999999999999999999999999999999999999999999999999.99');
select sum(c1) from test_text;
insert into test_text values ('-9999999999999999999999999999999999999999999999999999999.99');
insert into test_text values ('-9999999999999999999999999999999999999999999999999999999.99');
insert into test_text values ('-9999999999999999999999999999999999999999999999999999999.99');
insert into test_text values ('-9999999999999999999999999999999999999999999999999999999.99');
insert into test_text values ('-9999999999999999999999999999999999999999999999999999999.99');
insert into test_text values ('-9999999999999999999999999999999999999999999999999999999.99');
select sum(c1) from test_text;

drop table test_text;
drop table test_type_table;
drop table test_sum;

drop schema db_test_sum cascade;
reset current_schema;
