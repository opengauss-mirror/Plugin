create schema b_datatype_test;
set current_schema to 'b_datatype_test';

-- bit(n), when insert into bit, support the length less than n, which must be equal to n in normal case
create table bit_test(a bit);
create table bit_test2(a bit(5));
create table bit_test3(a bit(35));

insert into bit_test values(b'');
update bit_test set a = b'1' where a = b'';
insert into bit_test values(b'0');
update bit_test set a = b'' where a = b'0';
select * from bit_test order by 1;
--error, too long
update bit_test set a = b'00' where a = b'0';

delete from bit_test where a=b'0';
delete from bit_test where a=b'1';
delete from bit_test where a=b'';
delete from bit_test where a=b'1111';
select * from bit_test order by 1;

--error, too long
insert into bit_test values(b'11');

insert into bit_test2 values(b'');
update bit_test2 set a = b'0' where a = b'';
insert into bit_test2 values(b'1');
update bit_test2 set a = b'0' where a = b'1';
insert into bit_test2 values(b'11');
update bit_test2 set a = b'00' where a = b'11';
insert into bit_test2 values(b'111');
update bit_test2 set a = b'000' where a = b'111';
insert into bit_test2 values(b'1111');
update bit_test2 set a = b'0000' where a = b'1111';
insert into bit_test2 values(b'11111');
update bit_test2 set a = b'00000' where a = b'11111';
select * from bit_test2 order by 1;

--error, too long
insert into bit_test2 values(b'111111');
update bit_test2 set a = b'111111' where a = b'00000';
delete from bit_test2 where a=b'0000000000';
select * from bit_test2 order by 1;

insert into bit_test3 values(b'');
insert into bit_test3 values(b'1');
select * from bit_test3 where a=b'' order by 1;
select * from bit_test3 where a=b'0' order by 1;
select * from bit_test3 where a=b'00' order by 1;
select * from bit_test3 where a=b'000' order by 1;
select * from bit_test3 where a=b'01' order by 1;
select * from bit_test3 where a=b'000000000000000000000000000000000000000000000000000000000000000000000001' order by 1;
select * from bit_test3 where a=b'10' order by 1;
select * from bit_test3 where a>b'';
select * from bit_test3 where a<b'';
select * from bit_test3 where a>=b'';
select * from bit_test3 where a<=b'';
select * from bit_test3 where a>b'1';
select * from bit_test3 where a<b'1';
select * from bit_test3 where a>=b'1';
select * from bit_test3 where a<=b'1';

insert into bit_test2 values(b'');
insert into bit_test2 values(b'001');
select * from bit_test2 order by 1;

explain(costs off) select * from bit_test2 a left join bit_test3 b on a.a=b.a;
select * from bit_test2 a left join bit_test3 b on a.a=b.a order by 1;

set enable_mergejoin to off;
explain(costs off) select * from bit_test2 a left join bit_test3 b on a.a=b.a;
select * from bit_test2 a left join bit_test3 b on a.a=b.a order by 1;

drop table bit_test;
drop table bit_test2;
drop table bit_test3;

select b'11'::bit(4);
select b'11'::bit(8);
select b'11'::bit(9);
select b'11'::bit(15);
select b'11'::bit(16);
select b'11'::bit(17);
select b'11'::bit(32);
select b'11'::bit(33);

--tinyint(n),smallint(n),mediumint,mediumint(n),int(n),bigint(n)
create table all_int_test(a tinyint(9999999999), b smallint(9999999999), c mediumint, d mediumint(9999999999), e int(9999999999), f bigint(9999999999));
\d all_int_test

drop table all_int_test;

CREATE TABLE t0(c0 VARCHAR, c1 VARCHAR);
INSERT INTO t0 VALUES (0,0);
UPDATE t0 SET c0=0;
select * from t0 order by 1,2;
UPDATE t0 SET c0=true,c1='true';
select * from t0 order by 1,2;

INSERT INTO t0 VALUES (1,'1'),(true,'true'),(false,'false');
SELECT * FROM t0 WHERE t0.c0 = true order by 1,2;
SELECT * FROM t0 WHERE t0.c0 is true order by 1,2;
SELECT * FROM t0 WHERE t0.c0 is false order by 1,2;

SELECT * FROM t0 WHERE t0.c1 = true order by 1,2;
SELECT * FROM t0 WHERE t0.c1 is true order by 1,2;
SELECT * FROM t0 WHERE t0.c1 is false order by 1,2;
drop table t0;

--test for set
drop table if exists set_tab;
create table set_tab (
  c1 set('1','2','3','4','5'),
  c2 set('a', 'b', 'c', 'd', 'e')
);
insert into set_tab values('1,2','a,b');
insert into set_tab values('3,4','c,d');

create or replace function gettypeid(tname text) returns INT4 as
$$
begin
    return oid from pg_type where typname = :tname;
end;
$$ language plpgsql;

select varlenatoset(c1,gettypeid('set_tab_c1_set')) from set_tab order by 1;
select varlenatoset(c2,gettypeid('set_tab_c2_set')) from set_tab order by 1;

drop function gettypeid;
drop table set_tab;

----
set dolphin.b_compatibility_mode = true;
drop table if exists t1;
create table t1 (a char(16), b date, c datetime);
insert into t1 SET a='test 2000-01-01', b='2000-01-01', c='2000-01-01';
select * from t1 where c = '2000-01-01';
select * from t1 where b = '2000-01-01';
select * from t1 where c = '2000-01-01'::datetime;
select * from t1 where b = '2000-01-01'::date;
set dolphin.b_compatibility_mode = off;
select * from t1 where c = '2000-01-01';
select * from t1 where b = '2000-01-01';
select * from t1 where c = '2000-01-01'::datetime;
select * from t1 where b = '2000-01-01'::date;

drop table t1;

---
select pg_catalog.delete(cast('test=>NULL' as hstore), cast(pg_catalog.dolphin_types() as _text));



-- test bit output 
create table test_bit64(c1 bit(64));
insert into test_bit64 values (b'1001000110100010101111000100110101011110011011111000100100100');
insert into test_bit64 values (b'10101011110011011111000100100100');
insert into test_bit64 values (b'1111011');

create table t_bit(c_bit1 bit(1), c_bit10 bit(10));

insert into t_bit values (null, 12);
insert into t_bit values (1, 123);
insert into t_bit values (null, 123.4);
insert into t_bit values (null, 12.345);
insert into t_bit values (null, 123.45);
insert into t_bit values (null, 123.45678);
insert into t_bit values (1, b'0101111');

-- dec format
set dolphin.bit_output = dec;
select * from test_bit64;
select * from t_bit;

-- hex format
set dolphin.bit_output = hex;
select * from test_bit64;
select * from t_bit;

-- binary format
set dolphin.bit_output = bin;
select * from test_bit64;
select * from t_bit;

drop table test_bit64;
drop table t_bit;


-- test treat_float_with_precision_as_float_type
create table test_double(c1 double, c2 double(10, 0), c3 double(15, 3), c4 float, c5 float4, c6 float4(10, 0), c7 float4(7, 5), c8 float(10, 0), c9 float(7, 5), c10 float4(5));
\d+ test_double;
drop table test_double;
set dolphin.treat_float_with_precision_as_float_type = true;
create table test_double(c1 double, c2 double(10, 0), c3 double(15, 3), c4 float, c5 float4, c6 float4(10, 0), c7 float4(7, 5), c8 float(10, 0), c9 float(7, 5), c10 float4(5));
\d+ test_double;
insert into test_double values (12.3456, 12.3456, 12.3456, 12.3456, 12.3456, 12.3456, 12.3456, 12.3456, 12.3456, 12.3456);
select * from test_double;
drop table test_double;

reset dolphin.treat_float_with_precision_as_float_type;

drop schema b_datatype_test cascade;
reset current_schema;