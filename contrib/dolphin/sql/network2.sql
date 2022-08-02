drop database if exists network2;
create database network2 dbcompatibility 'b';
\c network2
select inet_ntoa(inet_aton('255.255.255.255.255.255.255.255'));
select inet_ntoa(1099511627775),inet_ntoa(4294902271),inet_ntoa(511);
select inet_aton('255.255.255.255.255'),inet_aton('255.255.1.255'),inet_aton('0.1.255');
select hex(inet_aton('127'));
select hex(inet_aton('127.1'));
select hex(inet_aton('127.1.1'));
select inet_aton('');
select inet_aton('122.256');
select inet_aton('122.226.');
SELECT INET6_ATON('1.2.3') IS NULL;
SELECT INET6_ATON('1.2.3.') IS NULL;
SELECT INET6_ATON('1..3.4') IS NULL;
SELECT INET6_ATON('-1.2.3.4') IS NULL;
SELECT INET6_ATON('1.2.3.256') IS NULL;
SELECT INET6_ATON('1.2.3.4.5') IS NULL;
SELECT INET6_ATON('0001.2.3.4') IS NULL;
SELECT INET6_ATON('0x1.2.3.4') IS NULL;
SELECT INET6_ATON('a.2.3.4') IS NULL;
SELECT INET6_ATON('1.2.3.4:80') IS NULL;
SELECT INET6_ATON('1.2.3.4/32') IS NULL;
SELECT INET6_ATON('mysql.com') IS NULL;
SELECT INET6_ATON(':::') IS NULL;
SELECT INET6_ATON(':1:2:3') IS NULL;
SELECT INET6_ATON('1:2:3:') IS NULL;
SELECT INET6_ATON(':1::2:3') IS NULL;
SELECT INET6_ATON('1::2:3:') IS NULL;
SELECT INET6_ATON('::00001') IS NULL;
SELECT INET6_ATON('::00001:2') IS NULL;
SELECT INET6_ATON('::12345') IS NULL;
SELECT INET6_ATON('1020::3040::5060') IS NULL;
SELECT INET6_ATON('::abcz') IS NULL;
SELECT INET6_ATON('::0x1.2.3.4') IS NULL;
SELECT INET6_ATON('::1.0x2.3.4') IS NULL;
SELECT INET6_ATON('::a.b.c.d') IS NULL;
SELECT INET6_ATON('::ffff:0x1.2.3.4') IS NULL;
SELECT INET6_ATON('::ffff:1.0x2.3.4') IS NULL;
SELECT INET6_ATON('::ffff:a.b.c.d') IS NULL;
SELECT INET6_ATON('::1.2.3.4:abcd') IS NULL;
SELECT HEX(INET6_ATON('::abcd:1.2.3.4'));
SELECT HEX(INET6_ATON('0.0.0.0'));
SELECT HEX(INET6_ATON('00.00.00.00'));
SELECT HEX(INET6_ATON('000.000.000.000'));
SELECT HEX(INET6_ATON('1.2.3.4'));
SELECT HEX(INET6_ATON('01.02.03.04'));
SELECT HEX(INET6_ATON('001.002.003.004'));
SELECT HEX(INET6_ATON('255.255.255.255'));
SELECT HEX(INET6_ATON('::'));
SELECT HEX(INET6_ATON('0::0'));
SELECT HEX(INET6_ATON('1::2'));
SELECT HEX(INET6_ATON('0::'));
SELECT HEX(INET6_ATON('1::'));
SELECT HEX(INET6_ATON('::0'));
SELECT HEX(INET6_ATON('::1'));
SELECT HEX(INET6_ATON('1:2:3:4:5:6:7:8'));
SELECT HEX(INET6_ATON('::2:3:4:5:6:7:8'));
SELECT HEX(INET6_ATON('1::3:4:5:6:7:8'));
SELECT HEX(INET6_ATON('1:2::4:5:6:7:8'));
SELECT HEX(INET6_ATON('1:2:3::5:6:7:8'));
SELECT HEX(INET6_ATON('1:2:3:4::6:7:8'));
SELECT HEX(INET6_ATON('1:2:3:4:5::7:8'));
SELECT HEX(INET6_ATON('1:2:3:4:5:6::8'));
SELECT HEX(INET6_ATON('1:2:3:4:5:6:7::'));
SELECT HEX(INET6_ATON('0000:0000::0000:0001'));
SELECT HEX(INET6_ATON('1234:5678:9abc:def0:4321:8765:cba9:0fed'));
SELECT HEX(INET6_ATON('0000:0000:0000:0000:0000:0000:0000:0001'));
SELECT HEX(INET6_ATON('::c0a8:0102'));
SELECT HEX(INET6_ATON('::192.168.1.2'));
SELECT HEX(INET6_ATON('::ffff:c0a8:0102'));
SELECT HEX(INET6_ATON('::ffff:192.168.1.2'));
SELECT HEX(INET6_ATON('::01.2.3.4'));
SELECT HEX(INET6_ATON('::1.02.3.4'));
SELECT HEX(INET6_ATON('::1.2.03.4'));
SELECT HEX(INET6_ATON('::1.2.3.04'));
SELECT HEX(INET6_ATON('::1.2.3.00'));
SELECT HEX(INET6_ATON('::ffff:01.2.3.4'));
SELECT HEX(INET6_ATON('::ffff:1.02.3.4'));
SELECT HEX(INET6_ATON('::ffff:1.2.03.4'));
SELECT HEX(INET6_ATON('::ffff:1.2.3.04'));
SELECT HEX(INET6_ATON('::ffff:1.2.3.00'));
SELECT INET6_NTOA(NULL);
SELECT INET6_NTOA(123);
SELECT INET6_NTOA(123.456);
SELECT INET6_NTOA(INET6_ATON('::'));
SELECT INET6_NTOA(INET6_ATON('0::0'));
SELECT INET6_NTOA(INET6_ATON('1::2'));
SELECT INET6_NTOA(INET6_ATON('0::'));
SELECT INET6_NTOA(INET6_ATON('1::'));
SELECT INET6_NTOA(INET6_ATON('::0'));
SELECT INET6_NTOA(INET6_ATON('::1'));
SELECT INET6_NTOA(INET6_ATON('1:2:3:4:5:6:7:8'));
SELECT INET6_NTOA(INET6_ATON('::2:3:4:5:6:7:8'));
SELECT INET6_NTOA(INET6_ATON('1::3:4:5:6:7:8'));
SELECT INET6_NTOA(INET6_ATON('1:2::4:5:6:7:8'));
SELECT INET6_NTOA(INET6_ATON('1:2:3::5:6:7:8'));
SELECT INET6_NTOA(INET6_ATON('1:2:3:4::6:7:8'));
SELECT INET6_NTOA(INET6_ATON('1:2:3:4:5::7:8'));
SELECT INET6_NTOA(INET6_ATON('1:2:3:4:5:6::8'));
SELECT INET6_NTOA(INET6_ATON('1:2:3:4:5:6:7::'));
SELECT INET6_NTOA(INET6_ATON('0000:0000::0000:0001'));
SELECT INET6_NTOA(INET6_ATON('1234:5678:9abc:def0:4321:8765:cba9:0fed'));
SELECT INET6_NTOA(INET6_ATON('0000:0000:0000:0000:0000:0000:0000:0001'));
SELECT INET6_NTOA(INET6_ATON('::c0a8:0102'));
SELECT INET6_NTOA(INET6_ATON('::192.168.1.2'));
SELECT INET6_NTOA(INET6_ATON('::ffff:c0a8:0102'));
SELECT INET6_NTOA(INET6_ATON('::ffff:192.168.1.2'));
SELECT INET6_NTOA(INET6_ATON('::01.2.3.4'));
SELECT INET6_NTOA(INET6_ATON('::1.02.3.4'));
SELECT INET6_NTOA(INET6_ATON('::1.2.03.4'));
SELECT INET6_NTOA(INET6_ATON('::1.2.3.04'));
SELECT INET6_NTOA(INET6_ATON('::1.2.3.00'));
SELECT INET6_NTOA(INET6_ATON('::ffff:01.2.3.4'));
SELECT INET6_NTOA(INET6_ATON('::ffff:1.02.3.4'));
SELECT INET6_NTOA(INET6_ATON('::ffff:1.2.03.4'));
SELECT INET6_NTOA(INET6_ATON('::ffff:1.2.3.04'));
SELECT INET6_NTOA(INET6_ATON('::ffff:1.2.3.00'));
SELECT INET6_NTOA(INET6_ATON('192.168.1.2'));
SELECT HEX(INET_ATON('192.168.1.2'));
SELECT HEX(INET6_ATON('192.168.1.2'));
SELECT HEX(INET_ATON('255.255.255.255'));
SELECT HEX(INET6_ATON('255.255.255.255'));
SELECT HEX(INET_ATON('192.168.08.2'));
SELECT HEX(INET6_ATON('192.168.08.2'));
SELECT HEX(INET_ATON('192.168.0x8.2'));
SELECT HEX(INET6_ATON('192.168.0x8.2'));
SELECT HEX(INET_ATON('1.2.255'));
SELECT HEX(INET6_ATON('1.2.255'));
SELECT HEX(INET_ATON('1.2.256'));
SELECT HEX(INET6_ATON('1.2.256'));
SELECT HEX(INET_ATON('1.0002.3.4'));
SELECT HEX(INET6_ATON('1.0002.3.4'));
SELECT HEX(INET_ATON('1.2.3.4.5'));
SELECT HEX(INET6_ATON('1.2.3.4.5'));
SELECT HEX(INET6_ATON(INET_NTOA(INET_ATON('1.2.3.4')))) AS x;
\c postgres
drop database if exists network2;