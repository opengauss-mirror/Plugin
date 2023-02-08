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

drop schema b_datatype_test cascade;
reset current_schema;