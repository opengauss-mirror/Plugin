drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select get_lock('slock',0);
select get_lock('slock',1);
select get_lock('slock',-1);
select get_lock('slock',-32768);
select get_lock('slock',32767);
select get_lock('slock',-2147483648);
select get_lock('slock',2147483647);
select get_lock('slock',-9223372036854775808);
select get_lock('slock',-9223372036854775807);
select get_lock('slock',9223372036854775807);
select get_lock('slock',9223372036854775806);
--failed
select get_lock('slock',-9223372036854775809);
--failed
select get_lock('slock',9223372036854775808);
\c postgres
drop database if exists mysqllock;
