drop database if exists test_network;
create database test_network dbcompatibility 'b';
\c test_network
create table test (ip1 varchar(20),ip2 char(20),ip3 nvarchar2(20),ip4 text,ip5 clob);
insert into test (ip1,ip2,ip3,ip4,ip5) values ('192.168.1.1','127.0.0.1','10.0.0.10','172.0.0.1','0.0.0.0'),('fe80::1','a::f','a::c','a::d','a::e'),('192.168.1.256','192.168.1','256.168.1.1','192.256.1.1','192.168.1.-1');
select is_ipv4(ip1),is_ipv4(ip2),is_ipv4(ip3),is_ipv4(ip4),is_ipv4(ip5) from test;
select is_ipv6(ip1),is_ipv6(ip2),is_ipv6(ip3),is_ipv6(ip4),is_ipv6(ip5) from test;
select is_ipv4('127.0.0.1'),is_ipv4('127.0.0.1'::inet),is_ipv4('127.0.0.1'::cidr),is_ipv4('127.0.0.1'::cstring),is_ipv4('127.0.0.1'::char(20));
select is_ipv6('fe80::1'),is_ipv6('fe80::1'::inet),is_ipv6('fe80::1'::cidr),is_ipv6('fe80::1'::cstring),is_ipv6('fe80::1'::char(20));
select is_ipv4('0.0.0.0');
select is_ipv4('0.0.0.256');
select is_ipv4('255.255.255.255');
select is_ipv4('255.255.255.-255');
select is_ipv4(10);
select is_ipv4(3.5);
select is_ipv4(NULL);
select is_ipv6('0000:0000:0000:0000:0000:0000:0000:0000');
select is_ipv6('0000:0000:0000:0000:0000:0000:0000');
select is_ipv6('ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff');
select is_ipv6('ffff:ffff:ffff:ffff:ffff:ffff:ffff');
select is_ipv6('2403:A200:A200:0:AFFF::3');
select is_ipv6('2403:A200:A200:1100::F00:3');
select is_ipv6('2403::AFFF::3');
select is_ipv6('::3');
select is_ipv6(10);
select is_ipv6(3.5);
select is_ipv6(NULL);
\c postgres
drop database if exists test_network;