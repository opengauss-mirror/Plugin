create schema test_bit_xor;
set current_schema to 'test_bit_xor';

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

drop schema test_bit_xor cascade;
reset current_schema;