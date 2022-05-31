drop database if exists b_datatype_test;
create database b_datatype_test dbcompatibility 'B';
\c b_datatype_test

-- bit(n), when insert into bit, support the length less than n, which must be equal to n in normal case
create table bit_test(a bit);
create table bit_test2(a bit(5));

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

delete from bit_test2 where a=b'0';
delete from bit_test2 where a=b'00';
delete from bit_test2 where a=b'000';
delete from bit_test2 where a=b'0000';
delete from bit_test2 where a=b'00000';
delete from bit_test2 where a=b'0000000000';
select * from bit_test2 order by 1;

drop table bit_test;
drop table bit_test2;

--tinyint(n),smallint(n),mediumint,mediumint(n),int(n),bigint(n)
create table all_int_test(a tinyint(9999999999), b smallint(9999999999), c mediumint, d mediumint(9999999999), e int(9999999999), f bigint(9999999999));
\d all_int_test

drop table all_int_test;

\c postgres
drop database b_datatype_test;