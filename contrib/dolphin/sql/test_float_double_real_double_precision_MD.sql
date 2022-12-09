drop database if exists float_double_real_double_precision_MD;
create database float_double_real_double_precision_MD dbcompatibility 'b';
\c float_double_real_double_precision_MD;

create table test(a float(20, 2), b double(20, 2), c real(20, 2), d double precision(20, 2));
\d test;
insert into test values(0.014, 0.014, 0.014, 0.014), (0.015, 0.015, 0.015, 0.015), (0.0151, 0.0151, 0.0151, 0.0151),(0.016, 0.016, 0.016, 0.016),
(0.024, 0.024, 0.024, 0.024), (0.025, 0.025, 0.025, 0.025), (0.0251, 0.0251, 0.0251, 0.0251), (0.026, 0.026, 0.026, 0.026);
select a, b, c, d from test;

create table test1(a float(0, 0));
create table test1(a double(0, 0));
create table test1(a real(0, 0));
create table test1(a double precision(0, 0));

create table test1(a float(1001, 0));
create table test1(a double(1001, 0));
create table test1(a real(1001, 0));
create table test1(a double precision(1001, 0));

create table test1(a float(1, 2));
create table test1(a double(1, 2));
create table test1(a real(1, 2));
create table test1(a double precision(1, 2));

create table test1(a float(255, 30), b double(255, 30), c real(255, 30), d double precision(255, 30));
insert into test1 values(9007199254740991, 9007199254740991, 9007199254740991, 9007199254740991),
(9007199254740992, 9007199254740992, 9007199254740992, 9007199254740992),
(9007199254740993, 9007199254740993, 9007199254740993, 9007199254740993),
(9007199254740994, 9007199254740994, 9007199254740994, 9007199254740994),
(9007199254740995, 9007199254740995, 9007199254740995, 9007199254740995),
(9007199254740996, 9007199254740996, 9007199254740996, 9007199254740996);
select a, b, c, d from test1;

create table test2(a float(100, 50), b double(100, 50),c real(100, 50), d double precision(100, 50));
insert into test2 values(99999999999999999999999999999999999999999999999999.99999999999999999999999999999999999999999999999999,
99999999999999999999999999999999999999999999999999.99999999999999999999999999999999999999999999999999,
99999999999999999999999999999999999999999999999999.99999999999999999999999999999999999999999999999999,
99999999999999999999999999999999999999999999999999.99999999999999999999999999999999999999999999999999);
select a, b, c, d from test2;

create table test3(a float(3.6, 1.6));
create table test3(a real(3.6, 1.6));
create table test3(a double precision(3.6, 1.6));
create table test3(a double(3.6, 1.6));
\d test3;
