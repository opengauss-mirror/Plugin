drop database if exists insert_set;
create database insert_set dbcompatibility 'B';
\c insert_set

create table test_figure(tinyint tinyint, smallint smallint, integer integer, binary_integer binary_integer, bigint bigint);
insert into test_figure set bigint = 7234134, binary_integer = 1011101, integer = 10000, smallint = 1, tinyint = 3;
select * from test_figure;

create table test_money(money money);
insert into test_money set money = 43241;
select * from test_money;

create table test_boolean(boolean boolean);
insert into test_boolean set boolean = true;
select * from test_boolean;

create table test_char(char char, varchar varchar, varchar2 varchar2, nvarchar2 nvarchar2, text text, clob clob);
insert into test_char set clob = 'asfdasdfa', text = 'adfafasdfwrq', nvarchar2 = 'aaaaa', varchar2 = 'test', varchar = 'toowtest', char = 'A';
select * from test_char;

create table test_binary(blob blob, raw raw, bytea bytea);
insert into test_binary set bytea = '01101', raw = 'AF0189DBE', blob = '1001111101';
select * from test_binary;

create table test_time(date date, timestamp timestamp, timestamptz timestamptz, smalldatetime smalldatetime, interval interval, reltime reltime);
insert into test_time set reltime = '15:38:32', interval = now() - current_timestamp, smalldatetime = '2022-08-15 14:32:43', timestamptz = now(), timestamp = current_timestamp, date=current_date;
select * from test_time;

create table test_netid(cidr cidr, inet inet, macaddr macaddr);
insert into test_netid set macaddr = '08002b:010203', inet = '192.168.12.234', cidr = '10';
select * from test_netid;
insert into test_netid set macaddr = 18023b:2323, inet = 192.168.122.1;
select * from test_netid;

create table test_error(id int, name varchar);
insert into test_error set name = 'test', id=1, name='ttttt';
select * from test_error;
insert into test_error set name = 1, id='@';
select * from test_error;
insert into test_error set name = 23;
select * from test_error;

\c postgres
drop database insert_set;
