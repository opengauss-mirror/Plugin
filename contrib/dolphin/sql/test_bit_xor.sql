create schema test_bit_xor;
set current_schema to 'test_bit_xor';
set dolphin.b_compatibility_mode to on;
-- test datetime
create table test_datetime (t datetime);
select bit_xor(t) from test_datetime;
insert into test_datetime values ('1000-01-01 00:00:00');
select bit_xor(t) from test_datetime;
insert into test_datetime values ('9999-12-31 23:59:59');
select bit_xor(t) from test_datetime;
insert into test_datetime values ('2000-01-30 -01:-10:-59');
select bit_xor(t) from test_datetime;
insert into test_datetime values (null);
select bit_xor(t) from test_datetime;
create table test_timestamp (t timestamp);
select bit_xor(t) from test_timestamp;
insert into test_timestamp values ('1979-01-01 00:00:00');
select bit_xor(t) from test_timestamp;
insert into test_timestamp values ('2038-01-19 03:14:08');
select bit_xor(t) from test_timestamp;
insert into test_timestamp values ('2000-01-30 -23:-59:-59');
select bit_xor(t) from test_timestamp;
insert into test_timestamp values (null);
select bit_xor(t) from test_timestamp;
drop table test_datetime;
drop table test_timestamp;

--test int
CREATE TABLE bit_xor_int1(a TINYINT);
INSERT INTO bit_xor_int1(a) values(-1.11);
SELECT BIT_XOR(a) from bit_xor_int1;
INSERT INTO bit_xor_int1(a) values(1.11);
SELECT BIT_XOR(a) from bit_xor_int1;
INSERT INTO bit_xor_int1(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_int1;
DROP TABLE if exists bit_xor_int1;

-- test smallint
CREATE TABLE bit_xor_int2(a SMALLINT);
INSERT INTO bit_xor_int2(a) values(-1.11);
SELECT BIT_XOR(a) from bit_xor_int2;
INSERT INTO bit_xor_int2(a) values(1.11);
SELECT BIT_XOR(a) from bit_xor_int2;
INSERT INTO bit_xor_int2(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_int2;
DROP TABLE if exists bit_xor_int2;

--test mediumint
CREATE TABLE bit_xor_int3(a MEDIUMINT);
INSERT INTO bit_xor_int3(a) values(-1.11);
SELECT BIT_XOR(a) from bit_xor_int3;
INSERT INTO bit_xor_int3(a) values(1.11);
SELECT BIT_XOR(a) from bit_xor_int3;
INSERT INTO bit_xor_int3(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_int3;
DROP TABLE if exists bit_xor_int3;

-- test int
CREATE TABLE bit_xor_int4(a INT);
INSERT INTO bit_xor_int4(a) values(-1.11);
SELECT BIT_XOR(a) from bit_xor_int4;
INSERT INTO bit_xor_int4(a) values(1.11);
SELECT BIT_XOR(a) from bit_xor_int4;
INSERT INTO bit_xor_int4(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_int4;
DROP TABLE if exists bit_xor_int4;

-- test bigint
CREATE TABLE bit_xor_int8(a BIGINT);
INSERT INTO bit_xor_int8(a) values(-1.11);
SELECT BIT_XOR(a) from bit_xor_int8;
INSERT INTO bit_xor_int8(a) values(1.11);
SELECT BIT_XOR(a) from bit_xor_int8;
INSERT INTO bit_xor_int8(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_int8;
DROP TABLE if exists bit_xor_int8;

-- test float
CREATE TABLE bit_xor_float(a FLOAT);
INSERT INTO bit_xor_float(a) values(-1.11);
SELECT BIT_XOR(a) from bit_xor_float;
INSERT INTO bit_xor_float(a) values(1.11);
SELECT BIT_XOR(a) from bit_xor_float;
INSERT INTO bit_xor_float(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_float;
DROP TABLE if exists bit_xor_float;

-- test double
CREATE TABLE bit_xor_double(a DOUBLE);
INSERT INTO bit_xor_double(a) values(-1.11);
SELECT BIT_XOR(a) from bit_xor_double;
INSERT INTO bit_xor_double(a) values(1.11);
SELECT BIT_XOR(a) from bit_xor_double;
INSERT INTO bit_xor_double(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_double;
DROP TABLE if exists bit_xor_double;

-- test decimal
CREATE TABLE bit_xor_dec(a DECIMAL(5,2));
INSERT INTO bit_xor_dec(a) values(-1.11);
INSERT INTO bit_xor_dec(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_dec;
DROP TABLE if exists bit_xor_dec;

-- test char
CREATE TABLE bit_xor_char(a CHAR(5));
INSERT INTO bit_xor_char(a) values('-1.11');
SELECT BIT_XOR(a) from bit_xor_char;
INSERT INTO bit_xor_char(a) values('1.11');
SELECT BIT_XOR(a) from bit_xor_char;
INSERT INTO bit_xor_char(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_char;
DROP TABLE if exists bit_xor_char;

--test varchar
CREATE TABLE bit_xor_varchar(a VARCHAR(5));
INSERT INTO bit_xor_varchar(a) values('-1.11');
SELECT BIT_XOR(a) from bit_xor_varchar;
INSERT INTO bit_xor_varchar(a) values('1.11');
SELECT BIT_XOR(a) from bit_xor_varchar;
INSERT INTO bit_xor_varchar(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_varchar;
DROP TABLE if exists bit_xor_varchar;

-- test tinyblob
CREATE TABLE bit_xor_tinyblob(a TINYBLOB);
INSERT INTO bit_xor_tinyblob(a) values('111');
SELECT BIT_XOR(a) from bit_xor_tinyblob;
INSERT INTO bit_xor_tinyblob(a) values('101');
SELECT BIT_XOR(a) from bit_xor_tinyblob;
INSERT INTO bit_xor_tinyblob(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_tinyblob;
DROP TABLE if exists bit_xor_tinyblob;

-- test tinytext
CREATE TABLE bit_xor_tinytext(a TINYTEXT);
INSERT INTO bit_xor_tinytext(a) values('-1.11');
SELECT BIT_XOR(a) from bit_xor_tinytext;
INSERT INTO bit_xor_tinytext(a) values('1.11');
SELECT BIT_XOR(a) from bit_xor_tinytext;
INSERT INTO bit_xor_tinytext(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_tinytext;
DROP TABLE if exists bit_xor_tinytext;

-- test text
CREATE TABLE bit_xor_text(a TEXT);
INSERT INTO bit_xor_text(a) values('-1.11');
SELECT BIT_XOR(a) from bit_xor_text;
INSERT INTO bit_xor_text(a) values('1.11');
SELECT BIT_XOR(a) from bit_xor_text;
delete from bit_xor_text;
INSERT INTO bit_xor_text(a) values('1.999');
SELECT BIT_XOR(a) from bit_xor_text;
INSERT INTO bit_xor_text(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_text;
DROP TABLE if exists bit_xor_text;

CREATE TABLE bit_xor_text(a TEXT);
INSERT INTO bit_xor_text(a) values('111111111111111111111111111111111111111111111111');
SELECT BIT_XOR(a) from bit_xor_text;
delete from bit_xor_text;
INSERT INTO bit_xor_text(a) values('-111111111111111111111111111111111111111111111111');
SELECT BIT_XOR(a) from bit_xor_text;
INSERT INTO bit_xor_text(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_text;
DROP TABLE if exists bit_xor_text;


CREATE TABLE bit_xor_mediumtext(a MEDIUMTEXT);
INSERT INTO bit_xor_mediumtext(a) values('-1.11');
SELECT BIT_XOR(a) from bit_xor_mediumtext;
INSERT INTO bit_xor_mediumtext(a) values('1.11');
SELECT BIT_XOR(a) from bit_xor_mediumtext;
INSERT INTO bit_xor_mediumtext(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_mediumtext;
DROP TABLE if exists bit_xor_mediumtext;

CREATE TABLE bit_xor_longtext(a LONGTEXT);
INSERT INTO bit_xor_longtext(a) values('-1.11');
SELECT BIT_XOR(a) from bit_xor_longtext;
INSERT INTO bit_xor_longtext(a) values('1.11');
SELECT BIT_XOR(a) from bit_xor_longtext;
INSERT INTO bit_xor_longtext(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_longtext;
DROP TABLE if exists bit_xor_longtext;


-- test blob
CREATE TABLE bit_xor_blob(a BLOB);
INSERT INTO bit_xor_blob(a) values('111');
SELECT BIT_XOR(a) from bit_xor_blob;
INSERT INTO bit_xor_blob(a) values('101');
SELECT BIT_XOR(a) from bit_xor_blob;
INSERT INTO bit_xor_blob(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_blob;
DROP TABLE if exists bit_xor_blob;

CREATE TABLE bit_xor_mediumblob(a MEDIUMBLOB);
INSERT INTO bit_xor_mediumblob(a) values('111');
SELECT BIT_XOR(a) from bit_xor_mediumblob;
INSERT INTO bit_xor_mediumblob(a) values('101');
SELECT BIT_XOR(a) from bit_xor_mediumblob;
INSERT INTO bit_xor_mediumblob(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_mediumblob;
DROP TABLE if exists bit_xor_mediumblob;

CREATE TABLE bit_xor_longblob(a LONGBLOB);
INSERT INTO bit_xor_longblob(a) values('111');
SELECT BIT_XOR(a) from bit_xor_longblob;
INSERT INTO bit_xor_longblob(a) values('101');
SELECT BIT_XOR(a) from bit_xor_longblob;
INSERT INTO bit_xor_longblob(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_longblob;
DROP TABLE if exists bit_xor_longblob;

-- test raw
CREATE TABLE bit_xor_raw(a raw);
INSERT INTO bit_xor_raw(a) values('111');
SELECT BIT_XOR(a) from bit_xor_raw;
INSERT INTO bit_xor_raw(a) values('101');
SELECT BIT_XOR(a) from bit_xor_raw;
INSERT INTO bit_xor_raw(a) values(NULL);
SELECT BIT_XOR(a) from bit_xor_raw;
DROP TABLE if exists bit_xor_raw;

create table test_time (t time);
insert into test_time values('-00:00:01');
insert into test_time values('-01:00:00');
insert into test_time values('-01:00:01');
select bit_xor(t) from test_time;
insert into test_time values('12:59:59.999');
insert into test_time values('00:00:01');
insert into test_time values('838:59:59');
-- 表中数据为
select * from test_time;
-- 测试不含时区的time类型
select bit_xor(t) from test_time;

create table test_time_with_zone (t time with time zone);
insert into test_time_with_zone values('00:00:01 UTC');
insert into test_time_with_zone values('00:00:02 PST');
insert into test_time_with_zone values('00:10:00 CST');
insert into test_time_with_zone values('10:00:01 JST');
insert into test_time_with_zone values('23:59:59 CCT');
-- 表中数据为
select * from test_time_with_zone;
-- 测试含时区的time类型
select bit_xor(t) from test_time_with_zone;

-- 测试空值
create table test_time_with_null (t time);
insert into test_time_with_null values(NULL);
insert into test_time_with_null values('12:34:56');
insert into test_time_with_null values(NULL);
select * from test_time_with_null;
select bit_xor(t) from test_time_with_null;

create table test_time_with_zone_with_null (t time with time zone);
insert into test_time_with_zone_with_null values(NULL);
insert into test_time_with_zone_with_null values('12:34:56 UTC');
insert into test_time_with_zone_with_null values(NULL);
select * from test_time_with_zone_with_null;
select bit_xor(t) from test_time_with_zone_with_null;

-- test date

create table test_date(col date);

insert into test_date values('2022-01-01');
select bit_xor(col) from test_date;

insert into test_date values('2022-01-02');
select bit_xor(col) from test_date;

insert into test_date values(null);
select bit_xor(col) from test_date;

drop table test_date;

create table test_date(col date);

insert into test_date values('22-01-01');
select bit_xor(col) from test_date;

insert into test_date values('22-01-02');
select bit_xor(col) from test_date;

-- test year

create table test_year(col year);

insert into test_year values('2022');
select bit_xor(col) from test_year;

insert into test_year values('2021');
select bit_xor(col) from test_year;

insert into test_year values(null);
select bit_xor(col) from test_year;

drop table test_year;
create table test_year(col year);

insert into test_year values('22');
select bit_xor(col) from test_year;

insert into test_year values('21');
select bit_xor(col) from test_year;

drop table test_year;

-- test bit

create table test_bit(col bit);

insert into test_bit values(b'1');
select bit_xor(col) from test_bit;

insert into test_bit values(b'0');
select bit_xor(col) from test_bit;

insert into test_bit values(null);
select bit_xor(col) from test_bit;

drop table test_bit;

create table test_bit(col bit(8));

insert into test_bit values(b'11');
select bit_xor(col) from test_bit;

insert into test_bit values(b'101');
select bit_xor(col) from test_bit;

-- test varbit

create table test_varbit(col varbit);

insert into test_varbit values(b'1');
select bit_xor(col) from test_varbit;

insert into test_varbit values(b'0');
select bit_xor(col) from test_varbit;

insert into test_varbit values(null);
select bit_xor(col) from test_varbit;

drop table test_varbit;

create table test_varbit(col bit(8));

insert into test_varbit values(b'11');
select bit_xor(col) from test_varbit;

insert into test_varbit values(b'101');
select bit_xor(col) from test_varbit;
drop table test_varbit;

drop table if exists test_type_table cascade;
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
    `nullvalue` bit(1),

    `bignumber` bigint,
    `bigstring` text
);
drop table if exists test_type_table_2 cascade;
create table test_type_table_2
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
    `nullvalue` bit(1),

    `bignumber` bigint,
    `bigstring` text
);
insert into test_type_table values (
	1, 1, 1, 1, 
	1, 1, 1, 1, 
	1, 1, 1, b'1', 
	b'111', true, '2023-02-05', '19:10:50', 
	'19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', 
	'2023-02-05 19:10:50.456', '2023', '123a', '123a', 
	'123a', '123a', '123a', '123a',
	'123a', '123a', '123a', null, 
    9223372036854775807, 'openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。'
);
insert into test_type_table_2 values (
	2, 2, 2, 2,
	2, 2, 2, 2,
	2, 2, 2, b'0',
	b'100', false, '2023-02-06', '20:11:51',
	'19:11:51.3457', '2023-02-06 19:10:51', '2023-02-06 19:10:50.457', '2023-02-06 19:10:51',
	'2023-02-06 19:10:50.457', '2024', '124b', '124b',
	'124b', '124b', '124b', '124b',
	'124b', '124b', '124b', null, 
    -9223372036854775808, 'openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。'
);

drop view if exists res_view;
create view res_view as select
 a.`int1` ^ b.`int1` as 'a.`int1` ^ b.`int1`',
 a.`uint1` ^ b.`uint1` as 'a.`uint1` ^ b.`uint1`',
 a.`int2` ^ b.`int2` as 'a.`int2` ^ b.`int2`',
 a.`uint2` ^ b.`uint2` as 'a.`uint2` ^ b.`uint2`',
 a.`int4` ^ b.`int4` as 'a.`int4` ^ b.`int4`',
 a.`uint4` ^ b.`uint4` as 'a.`uint4` ^ b.`uint4`',
 a.`int8` ^ b.`int8` as 'a.`int8` ^ b.`int8`',
 a.`uint8` ^ b.`uint8` as 'a.`uint8` ^ b.`uint8`',
 a.`float4` ^ b.`float4` as 'a.`float4` ^ b.`float4`',
 a.`float8` ^ b.`float8` as 'a.`float8` ^ b.`float8`',
 a.`numeric` ^ b.`numeric` as 'a.`numeric` ^ b.`numeric`',
 a.`bit1` ^ b.`bit1` as 'a.`bit1` ^ b.`bit1`',
 a.`bit64` ^ b.`bit64` as 'a.`bit64` ^ b.`bit64`',
 a.`boolean` ^ b.`boolean` as 'a.`boolean` ^ b.`boolean`',
 a.`date` ^ b.`date` as 'a.`date` ^ b.`date`',
 a.`time` ^ b.`time` as 'a.`time` ^ b.`time`',
 a.`time(4)` ^ b.`time(4)` as 'a.`time(4)` ^ b.`time(4)`',
 a.`datetime` ^ b.`datetime` as 'a.`datetime` ^ b.`datetime`',
 a.`datetime(4)` ^ b.`datetime(4)` as 'a.`datetime(4)` ^ b.`datetime(4)`',
 a.`timestamp` ^ b.`timestamp` as 'a.`timestamp` ^ b.`timestamp`',
 a.`timestamp(4)` ^ b.`timestamp(4)` as 'a.`timestamp(4)` ^ b.`timestamp(4)`',
 a.`year` ^ b.`year` as 'a.`year` ^ b.`year`',
 a.`char` ^ b.`char` as 'a.`char` ^ b.`char`',
 a.`varchar` ^ b.`varchar` as 'a.`varchar` ^ b.`varchar`',
 a.`binary` ^ b.`binary` as 'a.`binary` ^ b.`binary`',
 a.`varbinary` ^ b.`varbinary` as 'a.`varbinary` ^ b.`varbinary`',
 a.`tinyblob` ^ b.`tinyblob` as 'a.`tinyblob` ^ b.`tinyblob`',
 a.`blob` ^ b.`blob` as 'a.`blob` ^ b.`blob`',
 a.`mediumblob` ^ b.`mediumblob` as 'a.`mediumblob` ^ b.`mediumblob`',
 a.`longblob` ^ b.`longblob` as 'a.`longblob` ^ b.`longblob`',
 a.`text` ^ b.`text` as 'a.`text` ^ b.`text`'
 from test_type_table as a, test_type_table_2 as b;
\x
select * from res_view;
\x
desc res_view;
drop view if exists res_view;

-- strict mode test
set dolphin.sql_mode='sql_mode_strict,pad_char_to_full_length,auto_recompile_function';

-- Abnormal arguments (null、large number、long string、different input types)
select `nullvalue` ^ `bit1` from test_type_table;
select `bit1` ^ `nullvalue` from test_type_table;
select `nullvalue` ^ `nullvalue` from test_type_table;
select `bignumber` ^ `bignumber` from test_type_table;
select `bigstring` ^ `bigstring` from test_type_table;
select `bit1` ^ `bit64` from test_type_table;
select `int1` ^ `bit64` from test_type_table;
select `uint1` ^ `bit64` from test_type_table;
select `int2` ^ `bit64` from test_type_table;
select `uint2` ^ `bit64` from test_type_table;
select `int4` ^ `bit64` from test_type_table;
select `uint4` ^ `bit64` from test_type_table;
select `int8` ^ `bit64` from test_type_table;
select `uint8` ^ `bit64` from test_type_table;
select `float4` ^ `bit64` from test_type_table;
select `float8` ^ `bit64` from test_type_table;
select `numeric` ^ `bit64` from test_type_table;
select `date` ^ `bit64` from test_type_table;
select `time` ^ `bit64` from test_type_table;
select `time(4)` ^ `bit64` from test_type_table;
select `datetime` ^ `bit64` from test_type_table;
select `datetime(4)` ^ `bit64` from test_type_table;
select `timestamp` ^ `bit64` from test_type_table;
select `timestamp(4)` ^ `bit64` from test_type_table;
select `year` ^ `bit64` from test_type_table;
select `char` ^ `bit64` from test_type_table;
select `varchar` ^ `bit64` from test_type_table;
select `binary` ^ `bit64` from test_type_table;
select `varbinary` ^ `bit64` from test_type_table;
select `tinyblob` ^ `bit64` from test_type_table;
select `blob` ^ `bit64` from test_type_table;
select `mediumblob` ^ `bit64` from test_type_table;
select `longblob` ^ `bit64` from test_type_table;
select `text` ^ `bit64` from test_type_table;
select `bit64` ^ `bit1` from test_type_table;
select `bit64` ^ `int1` from test_type_table;
select `bit64` ^ `uint1` from test_type_table;
select `bit64` ^ `int2` from test_type_table;
select `bit64` ^ `uint2` from test_type_table;
select `bit64` ^ `int4` from test_type_table;
select `bit64` ^ `uint4` from test_type_table;
select `bit64` ^ `int8` from test_type_table;
select `bit64` ^ `uint8` from test_type_table;
select `bit64` ^ `float4` from test_type_table;
select `bit64` ^ `float8` from test_type_table;
select `bit64` ^ `numeric` from test_type_table;
select `bit64` ^ `date` from test_type_table;
select `bit64` ^ `time` from test_type_table;
select `bit64` ^ `time(4)` from test_type_table;
select `bit64` ^ `datetime` from test_type_table;
select `bit64` ^ `datetime(4)` from test_type_table;
select `bit64` ^ `timestamp` from test_type_table;
select `bit64` ^ `timestamp(4)` from test_type_table;
select `bit64` ^ `year` from test_type_table;
select `bit64` ^ `char` from test_type_table;
select `bit64` ^ `varchar` from test_type_table;
select `bit64` ^ `binary` from test_type_table;
select `bit64` ^ `varbinary` from test_type_table;
select `bit64` ^ `tinyblob` from test_type_table;
select `bit64` ^ `blob` from test_type_table;
select `bit64` ^ `mediumblob` from test_type_table;
select `bit64` ^ `longblob` from test_type_table;
select `bit64` ^ `text` from test_type_table;

-- IUD
insert into test_type_table(`int1`) values(b'1' ^ b'0');
insert ignore into test_type_table(`int1`) values(b'1' ^ b'0');
insert into test_type_table(`uint1`) values(b'1' ^ b'0');
insert ignore into test_type_table(`uint1`) values(b'1' ^ b'0');
insert into test_type_table(`int2`) values(b'1' ^ b'0');
insert ignore into test_type_table(`int2`) values(b'1' ^ b'0');
insert into test_type_table(`uint2`) values(b'1' ^ b'0');
insert ignore into test_type_table(`uint2`) values(b'1' ^ b'0');
insert into test_type_table(`int4`) values(b'1' ^ b'0');
insert ignore into test_type_table(`int4`) values(b'1' ^ b'0');
insert into test_type_table(`uint4`) values(b'1' ^ b'0');
insert ignore into test_type_table(`uint4`) values(b'1' ^ b'0');
insert into test_type_table(`int8`) values(b'1' ^ b'0');
insert ignore into test_type_table(`int8`) values(b'1' ^ b'0');
insert into test_type_table(`uint8`) values(b'1' ^ b'0');
insert ignore into test_type_table(`uint8`) values(b'1' ^ b'0');
insert into test_type_table(`float4`) values(b'1' ^ b'0');
insert ignore into test_type_table(`float4`) values(b'1' ^ b'0');
insert into test_type_table(`float8`) values(b'1' ^ b'0');
insert ignore into test_type_table(`float8`) values(b'1' ^ b'0');
insert into test_type_table(`numeric`) values(b'1' ^ b'0');
insert ignore into test_type_table(`numeric`) values(b'1' ^ b'0');
insert into test_type_table(`bit1`) values(b'1' ^ b'0');
insert ignore into test_type_table(`bit1`) values(b'1' ^ b'0');
insert into test_type_table(`bit64`) values(b'1' ^ b'0');
insert ignore into test_type_table(`bit64`) values(b'1' ^ b'0');
insert into test_type_table(`boolean`) values(b'1' ^ b'0');
insert ignore into test_type_table(`boolean`) values(b'1' ^ b'0');
insert into test_type_table(`date`) values(b'1' ^ b'0');
insert ignore into test_type_table(`date`) values(b'1' ^ b'0');
insert into test_type_table(`time`) values(b'1' ^ b'0');
insert ignore into test_type_table(`time`) values(b'1' ^ b'0');
insert into test_type_table(`time(4)`) values(b'1' ^ b'0');
insert ignore into test_type_table(`time(4)`) values(b'1' ^ b'0');
insert into test_type_table(`datetime`) values(b'1' ^ b'0');
insert ignore into test_type_table(`datetime`) values(b'1' ^ b'0');
insert into test_type_table(`datetime(4)`) values(b'1' ^ b'0');
insert ignore into test_type_table(`datetime(4)`) values(b'1' ^ b'0');
insert into test_type_table(`timestamp`) values(b'1' ^ b'0');
insert ignore into test_type_table(`timestamp`) values(b'1' ^ b'0');
insert into test_type_table(`timestamp(4)`) values(b'1' ^ b'0');
insert ignore into test_type_table(`timestamp(4)`) values(b'1' ^ b'0');
insert into test_type_table(`year`) values(b'1' ^ b'0');
insert ignore into test_type_table(`year`) values(b'1' ^ b'0');
insert into test_type_table(`char`) values(b'1' ^ b'0');
insert ignore into test_type_table(`char`) values(b'1' ^ b'0');
insert into test_type_table(`varchar`) values(b'1' ^ b'0');
insert ignore into test_type_table(`varchar`) values(b'1' ^ b'0');
insert into test_type_table(`binary`) values(b'1' ^ b'0');
insert ignore into test_type_table(`binary`) values(b'1' ^ b'0');
insert into test_type_table(`varbinary`) values(b'1' ^ b'0');
insert ignore into test_type_table(`varbinary`) values(b'1' ^ b'0');
insert into test_type_table(`tinyblob`) values(b'1' ^ b'0');
insert ignore into test_type_table(`tinyblob`) values(b'1' ^ b'0');
insert into test_type_table(`blob`) values(b'1' ^ b'0');
insert ignore into test_type_table(`blob`) values(b'1' ^ b'0');
insert into test_type_table(`mediumblob`) values(b'1' ^ b'0');
insert ignore into test_type_table(`mediumblob`) values(b'1' ^ b'0');
insert into test_type_table(`longblob`) values(b'1' ^ b'0');
insert ignore into test_type_table(`longblob`) values(b'1' ^ b'0');
insert into test_type_table(`text`) values(b'1' ^ b'0');
insert ignore into test_type_table(`text`) values(b'1' ^ b'0');
insert into test_type_table(`nullvalue`) values(b'1' ^ b'0');
insert ignore into test_type_table(`nullvalue`) values(b'1' ^ b'0');
insert into test_type_table(`bignumber`) values(b'1' ^ b'0');
insert ignore into test_type_table(`bignumber`) values(b'1' ^ b'0');
insert into test_type_table(`bigstring`) values(b'1' ^ b'0');
insert ignore into test_type_table(`bigstring`) values(b'1' ^ b'0');

-- default value in create table
drop table if exists test_type_table_3 cascade;
create table test_type_table_3(a int default 1^0);
show create table test_type_table_3;
insert into test_type_table_3 default values;
select * from test_type_table_3;

reset dolphin.sql_mode;

-- none strict mode
set dolphin.sql_mode='pad_char_to_full_length,auto_recompile_function';
truncate table test_type_table cascade;
insert into test_type_table values (
	1, 1, 1, 1, 
	1, 1, 1, 1, 
	1, 1, 1, b'1', 
	b'111', true, '2023-02-05', '19:10:50', 
	'19:10:50.3456', '2023-02-05 19:10:50', '2023-02-05 19:10:50.456', '2023-02-05 19:10:50', 
	'2023-02-05 19:10:50.456', '2023', '123a', '123a', 
	'123a', '123a', '123a', '123a',
	'123a', '123a', '123a', null, 
    9223372036854775807, 'openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。openGauss是一款提供面向多核的极致性能、全链路的业务和数据安全，基于AI的调优和高效运维的能力，全面友好开放，携手伙伴共同打造全球领先的企业级开源关系型数据库，采用木兰宽松许可证v2发行。openGauss深度融合华为在数据库领域多年的研发经验，结合企业级场景需求，持续构建竞争力特性。'
);

-- Abnormal arguments (null、large number、long string、different input types)
select `nullvalue` ^ `bit1` from test_type_table;
select `bit1` ^ `nullvalue` from test_type_table;
select `nullvalue` ^ `nullvalue` from test_type_table;
select `bignumber` ^ `bignumber` from test_type_table;
select `bigstring` ^ `bigstring` from test_type_table;
select `bit1` ^ `bit64` from test_type_table;
select `int1` ^ `bit64` from test_type_table;
select `uint1` ^ `bit64` from test_type_table;
select `int2` ^ `bit64` from test_type_table;
select `uint2` ^ `bit64` from test_type_table;
select `int4` ^ `bit64` from test_type_table;
select `uint4` ^ `bit64` from test_type_table;
select `int8` ^ `bit64` from test_type_table;
select `uint8` ^ `bit64` from test_type_table;
select `float4` ^ `bit64` from test_type_table;
select `float8` ^ `bit64` from test_type_table;
select `numeric` ^ `bit64` from test_type_table;
select `date` ^ `bit64` from test_type_table;
select `time` ^ `bit64` from test_type_table;
select `time(4)` ^ `bit64` from test_type_table;
select `datetime` ^ `bit64` from test_type_table;
select `datetime(4)` ^ `bit64` from test_type_table;
select `timestamp` ^ `bit64` from test_type_table;
select `timestamp(4)` ^ `bit64` from test_type_table;
select `year` ^ `bit64` from test_type_table;
select `char` ^ `bit64` from test_type_table;
select `varchar` ^ `bit64` from test_type_table;
select `binary` ^ `bit64` from test_type_table;
select `varbinary` ^ `bit64` from test_type_table;
select `tinyblob` ^ `bit64` from test_type_table;
select `blob` ^ `bit64` from test_type_table;
select `mediumblob` ^ `bit64` from test_type_table;
select `longblob` ^ `bit64` from test_type_table;
select `text` ^ `bit64` from test_type_table;
select `bit64` ^ `bit1` from test_type_table;
select `bit64` ^ `int1` from test_type_table;
select `bit64` ^ `uint1` from test_type_table;
select `bit64` ^ `int2` from test_type_table;
select `bit64` ^ `uint2` from test_type_table;
select `bit64` ^ `int4` from test_type_table;
select `bit64` ^ `uint4` from test_type_table;
select `bit64` ^ `int8` from test_type_table;
select `bit64` ^ `uint8` from test_type_table;
select `bit64` ^ `float4` from test_type_table;
select `bit64` ^ `float8` from test_type_table;
select `bit64` ^ `numeric` from test_type_table;
select `bit64` ^ `date` from test_type_table;
select `bit64` ^ `time` from test_type_table;
select `bit64` ^ `time(4)` from test_type_table;
select `bit64` ^ `datetime` from test_type_table;
select `bit64` ^ `datetime(4)` from test_type_table;
select `bit64` ^ `timestamp` from test_type_table;
select `bit64` ^ `timestamp(4)` from test_type_table;
select `bit64` ^ `year` from test_type_table;
select `bit64` ^ `char` from test_type_table;
select `bit64` ^ `varchar` from test_type_table;
select `bit64` ^ `binary` from test_type_table;
select `bit64` ^ `varbinary` from test_type_table;
select `bit64` ^ `tinyblob` from test_type_table;
select `bit64` ^ `blob` from test_type_table;
select `bit64` ^ `mediumblob` from test_type_table;
select `bit64` ^ `longblob` from test_type_table;
select `bit64` ^ `text` from test_type_table;

-- IUD
insert into test_type_table(`int1`) values(b'1' ^ b'0');
insert ignore into test_type_table(`int1`) values(b'1' ^ b'0');
insert into test_type_table(`uint1`) values(b'1' ^ b'0');
insert ignore into test_type_table(`uint1`) values(b'1' ^ b'0');
insert into test_type_table(`int2`) values(b'1' ^ b'0');
insert ignore into test_type_table(`int2`) values(b'1' ^ b'0');
insert into test_type_table(`uint2`) values(b'1' ^ b'0');
insert ignore into test_type_table(`uint2`) values(b'1' ^ b'0');
insert into test_type_table(`int4`) values(b'1' ^ b'0');
insert ignore into test_type_table(`int4`) values(b'1' ^ b'0');
insert into test_type_table(`uint4`) values(b'1' ^ b'0');
insert ignore into test_type_table(`uint4`) values(b'1' ^ b'0');
insert into test_type_table(`int8`) values(b'1' ^ b'0');
insert ignore into test_type_table(`int8`) values(b'1' ^ b'0');
insert into test_type_table(`uint8`) values(b'1' ^ b'0');
insert ignore into test_type_table(`uint8`) values(b'1' ^ b'0');
insert into test_type_table(`float4`) values(b'1' ^ b'0');
insert ignore into test_type_table(`float4`) values(b'1' ^ b'0');
insert into test_type_table(`float8`) values(b'1' ^ b'0');
insert ignore into test_type_table(`float8`) values(b'1' ^ b'0');
insert into test_type_table(`numeric`) values(b'1' ^ b'0');
insert ignore into test_type_table(`numeric`) values(b'1' ^ b'0');
insert into test_type_table(`bit1`) values(b'1' ^ b'0');
insert ignore into test_type_table(`bit1`) values(b'1' ^ b'0');
insert into test_type_table(`bit64`) values(b'1' ^ b'0');
insert ignore into test_type_table(`bit64`) values(b'1' ^ b'0');
insert into test_type_table(`boolean`) values(b'1' ^ b'0');
insert ignore into test_type_table(`boolean`) values(b'1' ^ b'0');
insert into test_type_table(`date`) values(b'1' ^ b'0');
insert ignore into test_type_table(`date`) values(b'1' ^ b'0');
insert into test_type_table(`time`) values(b'1' ^ b'0');
insert ignore into test_type_table(`time`) values(b'1' ^ b'0');
insert into test_type_table(`time(4)`) values(b'1' ^ b'0');
insert ignore into test_type_table(`time(4)`) values(b'1' ^ b'0');
insert into test_type_table(`datetime`) values(b'1' ^ b'0');
insert ignore into test_type_table(`datetime`) values(b'1' ^ b'0');
insert into test_type_table(`datetime(4)`) values(b'1' ^ b'0');
insert ignore into test_type_table(`datetime(4)`) values(b'1' ^ b'0');
insert into test_type_table(`timestamp`) values(b'1' ^ b'0');
insert ignore into test_type_table(`timestamp`) values(b'1' ^ b'0');
insert into test_type_table(`timestamp(4)`) values(b'1' ^ b'0');
insert ignore into test_type_table(`timestamp(4)`) values(b'1' ^ b'0');
insert into test_type_table(`year`) values(b'1' ^ b'0');
insert ignore into test_type_table(`year`) values(b'1' ^ b'0');
insert into test_type_table(`char`) values(b'1' ^ b'0');
insert ignore into test_type_table(`char`) values(b'1' ^ b'0');
insert into test_type_table(`varchar`) values(b'1' ^ b'0');
insert ignore into test_type_table(`varchar`) values(b'1' ^ b'0');
insert into test_type_table(`binary`) values(b'1' ^ b'0');
insert ignore into test_type_table(`binary`) values(b'1' ^ b'0');
insert into test_type_table(`varbinary`) values(b'1' ^ b'0');
insert ignore into test_type_table(`varbinary`) values(b'1' ^ b'0');
insert into test_type_table(`tinyblob`) values(b'1' ^ b'0');
insert ignore into test_type_table(`tinyblob`) values(b'1' ^ b'0');
insert into test_type_table(`blob`) values(b'1' ^ b'0');
insert ignore into test_type_table(`blob`) values(b'1' ^ b'0');
insert into test_type_table(`mediumblob`) values(b'1' ^ b'0');
insert ignore into test_type_table(`mediumblob`) values(b'1' ^ b'0');
insert into test_type_table(`longblob`) values(b'1' ^ b'0');
insert ignore into test_type_table(`longblob`) values(b'1' ^ b'0');
insert into test_type_table(`text`) values(b'1' ^ b'0');
insert ignore into test_type_table(`text`) values(b'1' ^ b'0');
insert into test_type_table(`nullvalue`) values(b'1' ^ b'0');
insert ignore into test_type_table(`nullvalue`) values(b'1' ^ b'0');
insert into test_type_table(`bignumber`) values(b'1' ^ b'0');
insert ignore into test_type_table(`bignumber`) values(b'1' ^ b'0');
insert into test_type_table(`bigstring`) values(b'1' ^ b'0');
insert ignore into test_type_table(`bigstring`) values(b'1' ^ b'0');

-- default value in create table
drop table if exists test_type_table_3 cascade;
create table test_type_table_3(a int default 1^0);
show create table test_type_table_3;
insert into test_type_table_3 default values;
select * from test_type_table_3;

-- bit ^ bit (different length)
set dolphin.sql_mode='sql_mode_strict,pad_char_to_full_length,auto_recompile_function';
create table bit_t(`bit1` bit(1), `bit18` bit(18), `bit45` bit(45), `bit64` bit(64));
insert into bit_t values(1, 244520, 4564685412, 1565646212555);
select `bit1` ^ `bit18`, `bit1` ^ `bit45`, `bit1` ^ `bit64` from bit_t;
select `bit18` ^ `bit1`, `bit18` ^ `bit45`, `bit18` ^ `bit64` from bit_t;
select `bit45` ^ `bit1`, `bit45` ^ `bit18`, `bit45` ^ `bit64` from bit_t;
select `bit64` ^ `bit1`, `bit64` ^ `bit18`, `bit64` ^ `bit45` from bit_t;
drop table bit_t;

reset dolphin.sql_mode;
drop table if exists test_type_table cascade;
drop table if exists test_type_table_2 cascade;
drop table if exists test_type_table_3 cascade;
drop schema test_bit_xor cascade;
reset current_schema;