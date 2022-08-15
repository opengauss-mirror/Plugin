drop database if exists db_conv;
create database db_conv dbcompatibility 'B';
\c db_conv

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
select conv('-1',-16,-16),conv(0,10,10);

select conv('null',10,2),conv('a57b',11,35),conv('null',30,20);
select conv(95446461559756285324284,18,10),conv(6.69488535e+25,26,17),conv(12345678965412314567,10,16);
select conv('张三',26,17),conv('pxy',16,10),conv(256,2,16),conv(973,8,15);
select conv('%',26,17),conv('$',31,19),conv('?',10,16);
select conv(10,1,27),conv('zzzz',0,5),conv('xy57',37,8),conv(10,-1,27),conv('xy57',-37,8);
select conv(10,1/2,23),conv(10,1.333,23),conv('xy57',pi(),8),conv(10,'Asc',27),conv(321,'',8),conv(321,'张三',8);
select conv(10,27,1/2),conv(10,16,'AscY'),conv(321,7,''),conv(321,10,'张三');
select conv(10,27,1),conv('zzzz',0,0),conv('xy57',-8,37);
select conv(558,8,8),conv(121,2,10),conv(678,7,10),conv(789,8,10),conv('22G',16,10);
select conv(818,8,8),conv(310,2,10),conv(999,7,10),conv(888,8,10),conv('GHT',16,10),conv(3,-3,8);
select conv(1.5,10,10),conv(1.4,10,10);
select conv(9544646155975628532428411,10,10);
select conv(9544646155975628532428411,10,-10);
select conv(9544646155975628532428411,-10,10);
select conv(9544646155975628532428411,-10,-10);
select conv(-9544646155975628532428411,10,10);
select conv(-9544646155975628532428411,10,-10);
select conv(-9544646155975628532428411,-10,10);
select conv(-9544646155975628532428411,-10,-10);

\c postgres
drop database if exists db_conv;
