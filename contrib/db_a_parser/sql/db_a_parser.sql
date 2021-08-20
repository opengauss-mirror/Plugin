CREATE EXTENSION db_a_parser;

-- test without db_a_parser
set enable_custom_parser = off;

-- constant
select systimestamp;

-- data type
create table test_long (a long);
drop table test_long;

create table test_raw (a long raw);
drop table test_raw;

create table test_nclob (a nclob);
drop table test_nclob;

create table test_bfile (a bfile);
drop table test_bfile;

create table test_urowid (a urowid);
drop table test_urowid;

create table test_rowid (a rowid);
drop table test_rowid;

create table test_binary_float (a binary_float);
drop table test_binary_float;

create table test_xmltype (a xmltype);
drop table test_xmltype;

create table test_pls_integer (a pls_integer);
drop table test_pls_integer;

create table test_timestamp_with_local_timezone (a timestamp with local time zone);
drop table test_timestamp_with_local_timezone;

create table test_number (a number(10, 0));
drop table test_number;

-- ddl
create type test_type as object (
    attr varchar2(60));

-- nocache
create sequence test_seq minvalue 0 maxvalue 20 start with 0 increment by 1 cycle nocache;

create tablespace ts1 relative location 'ts1';
create table test_ts1 (id int primary key);

-- rebuild tablespace
alter index test_ts1_pkey rebuild tablespace ts1;
drop table test_ts1;
drop tablespace ts1;

-- operator
select 7 mod 5;

create or replace function mod_func (a int, b int) return int is
begin
    return a mod b;
    exception when others then null;
end;
/

select 7 < = 5;

select 7 > = 5;

select 7 < > 5;

-- plpgsql
create or replace function test_end_label(
    a int,
    b int)
return int
is
begin
    b = a;
    exception when others then null;
end test_end_label;
/

create or replace procedure add_record(
    a int,
    b out int)
is
begin
    b := a;
    exception when others then null;
end add_record;
/

create table test_reverse (id int);
begin
  for j in reverse 1..4 loop
   insert into test_reverse values (j);
  end loop;
  exception when others then null;
end;
/
select * from test_reverse;
drop table test_reverse;

-- test db_a_parser
set enable_custom_parser = on;

-- constant
select systimestamp;

-- data type
create table test_long (a long);
drop table test_long;

create table test_raw (a long raw);
drop table test_raw;

create table test_nclob (a nclob);
drop table test_nclob;

create table test_bfile (a bfile);
drop table test_bfile;

create table test_urowid (a urowid);
drop table test_urowid;

create table test_rowid (a rowid);
drop table test_rowid;

create table test_binary_float (a binary_float);
drop table test_binary_float;

create table test_xmltype (a xmltype);
drop table test_xmltype;

create table test_pls_integer (a pls_integer);
drop table test_pls_integer;

create table test_timestamp_with_local_timezone (a timestamp with local time zone);
drop table test_timestamp_with_local_timezone;

create table test_number (a number(10, 0));
drop table test_number;

-- ddl
create type test_type as object (
    attr varchar2(60));

-- nocache
create sequence test_seq minvalue 0 maxvalue 20 start with 0 increment by 1 cycle nocache;

create tablespace ts1 relative location 'ts1';
create table test_ts1 (id int primary key);

-- rebuild tablespace
alter index test_ts1_pkey rebuild tablespace ts1;
drop table test_ts1;
drop tablespace ts1;

-- operator
select 7 mod 5;

create or replace function mod_func (a int, b int) return int is
begin
    return a mod b;
    exception when others then null;
end;
/

select 7 < = 5;

select 7 > = 5;

select 7 < > 5;

-- plpgsql
create or replace function test_end_label(
    a int,
    b int)
return int
is
begin
    b = a;
    exception when others then null;
end test_end_label;
/

create or replace procedure add_record(
    a int,
    b out int)
is
begin
    b := a;
    exception when others then null;
end add_record;
/

create table test_reverse (id int);
begin
  for j in reverse 1..4 loop
   insert into test_reverse values (j);
  end loop;
  exception when others then null;
end;
/
select * from test_reverse;
drop table test_reverse;