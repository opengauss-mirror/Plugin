drop DATABASE if exists any_value_test;
CREATE DATABASE any_value_test dbcompatibility 'B';
\c any_value_test;

--test int type
create table test_int1(a tinyint, b int);
create table test_int2(a smallint, b int);
create table test_int4(a int, b int);
create table test_int8(a bigint, b int);
insert into test_int1 values(1,1),(2,1),(3,2),(4,2);
insert into test_int2 values(1,1),(2,1),(3,2),(4,2);
insert into test_int4 values(1,1),(2,1),(3,2),(4,2);
insert into test_int8 values(1,1),(2,1),(3,2),(4,2);
select any_value(a),b from test_int1 group by b;
select any_value(a),b from test_int2 group by b;
select any_value(a),b from test_int4 group by b;
select any_value(a),b from test_int8 group by b;


--test unsigned int type
create table test_uint1(a uint1, b int);
create table test_uint2(a uint2, b int);
create table test_uint4(a uint4, b int);
create table test_uint8(a uint8, b int);
insert into test_uint1 values(1,1),(2,1),(3,2),(4,2);
insert into test_uint2 values(1,1),(2,1),(3,2),(4,2);
insert into test_uint4 values(1,1),(2,1),(3,2),(4,2);
insert into test_uint8 values(1,1),(2,1),(3,2),(4,2);
select any_value(a),b from test_uint1 group by b;
select any_value(a),b from test_uint2 group by b;
select any_value(a),b from test_uint4 group by b;
select any_value(a),b from test_uint8 group by b;


--test float type
create table test_float(a float, b int);
create table test_double(a double, b int);
create table test_numeric(a numeric(2,1), b int);
insert into test_float values(1.1,1),(2.7,1),(3.6,2),(4.5,2);
insert into test_double values(1.1,1),(2.7,1),(3.6,2),(4.5,2);
insert into test_numeric values(1.1,1),(2.7,1),(3.6,2),(4.5,2);
select any_value(a),b from test_float group by b;
select any_value(a),b from test_double group by b;
select any_value(a),b from test_numeric group by b;


--test varchar type
create table test_char(a char, b int);
create table test_varchar(a varchar(5), b int);
create table test_text(a text, b int);
insert into test_char values('c',1),('v',1),('r',2),('a',2);
insert into test_varchar values('varch',1),('char',1),('text',2),('test',2);
insert into test_text values('varch',1),('char',1),('text',2),('test',2);
select any_value(a),b from test_char group by b;
select any_value(a),b from test_varchar group by b;
select any_value(a),b from test_text group by b;


--test date type
create table test_date(a date, b int);
insert into test_date values('2022-11-08',1),('2022-11-09',1),('2022-11-10',2),('2022-11-11',2);
select any_value(a),b from test_date group by b;


--test bool type
create table test_bool(a bool, b int);
insert into test_bool values('true',1),('false',1),('false',2),('true',2);
select any_value(a),b from test_bool group by b;


--test year and set type
create table test_year(a year, b int);
insert into test_year values('2022',1),('2021',1),('2020',2),('2019',2);
select any_value(a),b from test_year group by b;
create table test_set(a set('set1','set2','set3','set4'), b int);
insert into test_set values('set1',1),('set2',1),('set3',2),('set4',2);
select any_value(a),b from test_set group by b;

--test blob and bytea type
create table test_blob_bytea(a int, b blob, c bytea);
insert into test_blob_bytea values(1,'9abc',E'\\xeabc');
insert into test_blob_bytea values(1,'abcd',E'\\xdabc');
insert into test_blob_bytea values(2,'9abc',E'\\xdabc');
insert into test_blob_bytea values(2,'abcd',E'\\xeabc');
select any_value(b) from test_blob_bytea group by a;
select any_value(c) from test_blob_bytea group by a;

\c postgres;
drop DATABASE if exists any_value_test;