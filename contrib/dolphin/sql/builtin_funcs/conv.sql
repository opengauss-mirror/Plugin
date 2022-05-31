drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select conv('a',16,2);
select conv('6e',18,8);
select conv(-17,10,-18);
select conv('10'+10,16,10);
select conv('ffffffffffffffff',16,10);
select conv('ffffffffffffffff',16,-10);
select conv('-ffffffffffffffff',16,10);
select conv('-ffffffffffffffff',16,-10);
select conv('ffffffffffffffff1',16,10);
select conv('ffffffffffffffff1',16,-10);
select conv('-ffffffffffffffff1',16,10);
select conv('-ffffffffffffffff1',16,-10);
select conv('',16,16);
select conv(5,4,10);
select conv(1234,5,10);
select conv(-9223372036854775809,-10,16);
select conv('8000000000000000',16,16);
select conv(9223372036854775807,10,16);
select conv(-9223372036854775808,10,16);
select conv(9223372036854775807,-10,16);
select conv(-9223372036854775808,-10,16);
select conv(9223372036854775807,-10,-16);
select conv(-9223372036854775808,-10,-16);
select conv(9223372036854775807,10,-16);
select conv(-9223372036854775808,10,-16);

select conv(15632,16,36);
select conv(-15632,16,32);
select conv(15632,-16,32);
select conv(-15632,-16,32);
select conv(15632,-16,-32);
select conv(-15632,-16,-32);
select conv(15632,16,-32);
select conv(-15632,16,-32);

select conv('ffffffffffffffff',16,16);
select conv('ffffffffffffffff',16,-16);
select conv('ffffffffffffffff',-16,16);
select conv('ffffffffffffffff',-16,-16);

select conv('-ffffffffffffffff',16,16);
select conv('-ffffffffffffffff',16,-16);
select conv('-ffffffffffffffff',-16,16);
select conv('-ffffffffffffffff',-16,-16);

select conv('1ffffffffffffffff',16,16);
select conv('1ffffffffffffffff',16,-16);
select conv('1ffffffffffffffff',-16,16);
select conv('1ffffffffffffffff',-16,-16);

select conv('-1ffffffffffffffff',16,16);
select conv('-1ffffffffffffffff',16,-16);
select conv('-1ffffffffffffffff',-16,16);
select conv('-1ffffffffffffffff',-16,-16);

select conv('8000000000000000',16,16);
select conv('8000000000000000',16,-16);
select conv('8000000000000000',-16,16);
select conv('8000000000000000',-16,-16);

select conv('-8000000000000000',16,16);
select conv('-8000000000000000',16,-16);
select conv('-8000000000000000',-16,16);
select conv('-8000000000000000',-16,-16);

select conv('1',16,16);
select conv('1',16,-16);
select conv('1',-16,16);
select conv('1',-16,-16);

select conv('-1',16,16);
select conv('-1',16,-16);
select conv('-1',-16,16);
select conv('-1',-16,-16);

\c postgres
drop database if exists format_test;
