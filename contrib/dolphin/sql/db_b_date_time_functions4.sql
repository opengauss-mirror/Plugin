---- b compatibility case
drop database if exists b_datetime_func_test;
create database b_datetime_func_test dbcompatibility 'b';
\c b_datetime_func_test
set datestyle = 'ISO,ymd';
set time zone "Asia/Shanghai";
-- test get_format
select get_format(DATE, 'EUR');
select get_format(DATE, 'USA');
select get_format(DATE, 'JIS');
select get_format(DATE, 'ISO');
select get_format(DATE, 'INTERNAL');
select get_format(DATETIME, 'EUR');
select get_format(DATETIME, 'USA');
select get_format(DATETIME, 'JIS');
select get_format(DATETIME, 'ISO');
select get_format(DATETIME, 'INTERNAL');
select get_format(TIME, 'EUR');
select get_format(TIME, 'USA');
select get_format(TIME, 'JIS');
select get_format(TIME, 'ISO');
select get_format(TIME, 'INTERNAL');
select get_format(DATE, 'abc');
select get_format(abc, 'EUR');

-- test extract
set b_compatibility_mode = true;
-- 严格模式
set sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat';
-- 单unit
select extract(YEAR FROM '2019-07-02');
select extract(MONTH FROM '2019-07-02');
select extract(WEEK FROM '2019-07-02');
select extract(QUARTER FROM '2019-07-02');
select extract(DAY FROM '2019-07-02');
select extract(SECOND FROM '2019-07-02 12:12:12');
select extract(MINUTE FROM '2019-07-02 12:12:12');
select extract(HOUR FROM '2019-07-02 12:12:12');
select extract(MICROSECOND FROM '2019-07-02 12:12:12.12121');
select extract(SECOND FROM '12:12:12');
select extract(MINUTE FROM '12:12:12');
select extract(HOUR FROM '12:12:12');
select extract(MICROSECOND FROM '12:12:12.12121');
select extract(hour from '838:59:59');
select extract(hour from '-838:59:59');
-- 混合unit
select extract(YEAR_MONTH FROM '2019-07-02');
select extract(DAY_MINUTE FROM '2019-07-02 12:12:12');
select extract(DAY_HOUR FROM '2019-07-02 12:12:12');
select extract(DAY_MINUTE FROM '2019-07-02 12:12:12');
select extract(DAY_MICROSECOND FROM '2019-07-02 12:12:12.12121');
select extract(SECOND_MICROSECOND FROM '2019-07-02 12:12:12.12121');
select extract(SECOND_MICROSECOND FROM '12:12:12.12121');
select extract(MINUTE_MICROSECOND FROM '2019-07-02 12:12:12.12121');
select extract(MINUTE_MICROSECOND FROM '12:12:12.12121');
select extract(MINUTE_SECOND FROM '2019-07-02 12:12:12.12121');
select extract(MINUTE_SECOND FROM '12:12:12.12121');
select extract(HOUR_SECOND FROM '2019-07-02 12:12:12.12121');
select extract(HOUR_SECOND FROM '12:12:12.12121');
select extract(HOUR_MINUTE FROM '2019-07-02 12:12:12.12121');
select extract(HOUR_MINUTE FROM '12:12:12.12121');
select extract(HOUR_MICROSECOND FROM '2019-07-02 12:12:12.12121');
select extract(HOUR_MICROSECOND FROM '12:12:12.12121');
-- 边界值，坏值
select extract(aaa FROM '2019-07-02');
select extract(DAY FROM '0000-00-00');
select extract(DAY FROM '2001-01-45');
select extract(YEAR FROM '9999-01-01');
select extract(YEAR FROM '10000-01-01');
select extract(hour from '838:59:59.1');
select extract(hour from '839:00:00');
select extract(hour from '-838:59:59.1');
select extract(hour from '-839:00:00');
select extract(hour from '40 1:1:0');
select extract(hour from '-40 1:1:0');

-- 非严格模式
set sql_mode = 'sql_mode_full_group,pipes_as_concat';
-- 单unit
select extract(hour from '838:59:59');
select extract(hour from '-838:59:59');
-- 边界值，坏值
select extract(aaa FROM '2019-07-02');
select extract(DAY FROM '0000-00-00');
select extract(DAY FROM '2001-01-45');
select extract(YEAR FROM '9999-01-01');
select extract(YEAR FROM '10000-01-01');
select extract(hour from '838:59:59.1');
select extract(hour from '839:00:00');
select extract(hour from '-838:59:59.1');
select extract(hour from '-839:00:00');
select extract(hour from '40 1:1:0');
select extract(hour from '-40 1:1:0');
-- extract 结束
set b_compatibility_mode = false;

-- test date_format
-- 严格模式
set sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat';
-- format的各种情况
select date_format('2001-01-01 12:12:12','%a');
select date_format('2001-01-01 12:12:12','%b');
select date_format('2001-01-01 12:12:12','%c');
select date_format('2001-01-01 12:12:12','%D');
select date_format('2001-01-01 12:12:12','%d');
select date_format('2001-01-01 12:12:12','%e');
select date_format('2001-01-01 12:12:12','%f');
select date_format('2001-01-01 12:12:12','%H');
select date_format('2001-01-01 12:12:12','%h');
select date_format('2001-01-01 12:12:12','%I');
select date_format('2001-01-01 12:12:12','%i');
select date_format('2001-01-01 12:12:12','%j');
select date_format('2001-01-01 12:12:12','%k');
select date_format('2001-01-01 12:12:12','%l');
select date_format('2001-01-01 12:12:12','%M');
select date_format('2001-01-01 12:12:12','%m');
select date_format('2001-01-01 12:12:12','%p');
select date_format('2001-01-01 12:12:12','%r');
select date_format('2001-01-01 12:12:12','%S');
select date_format('2001-01-01 12:12:12','%s');
select date_format('2001-01-01 12:12:12','%T');
select date_format('2001-01-01 12:12:12','%U');
select date_format('2001-01-01 12:12:12','%u');
select date_format('2001-01-01 12:12:12','%V');
select date_format('2001-01-01 12:12:12','%v');
select date_format('2001-01-01 12:12:12','%W');
select date_format('2001-01-01 12:12:12','%w');
select date_format('2001-01-01 12:12:12','%X');
select date_format('2001-01-01 12:12:12','%x');
select date_format('2001-01-01 12:12:12','%Y');
select date_format('2001-01-01 12:12:12','%y');
select date_format('2001-01-01 12:12:12','aaaa');
select date_format('2001-01-01 12:12:12','%dffff');
select date_format('2001-01-01 12:12:12.34567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890','%b');
select date_format('2001-01-01 12:12:12','%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b');
-- format参数存在组合
select date_format('2001-01-01 12:12:12','%Y %M %D %U');
select date_format('2001-01-01 12:12:12','%H %k %I %r %T %S %w');
-- 边界值，坏值
select date_format('0000-00-00','%Y');
select date_format('2001-01-9999','%Y');
select date_format('2001-00-09','%Y');
select date_format('2001-15-09','%Y');
select date_format('9999-00-09','%Y');
select date_format('10000-00-09','%Y');
-- 不同类型输入
select date_format(date'2001-01-09','%Y');
select date_format(datetime'2001-01-09 12:12:12','%Y');
select date_format(20010101,'%Y');

-- 非严格模式
set sql_mode = 'sql_mode_full_group,pipes_as_concat';
-- 边界值，坏值
select date_format('0000-00-00','%Y');
select date_format('2001-01-9999','%Y');
select date_format('2001-00-09','%Y');
select date_format('2001-15-09','%Y');
select date_format('9999-00-09','%Y');
select date_format('10000-00-09','%Y');
-- date_format 结束

-- test from_unixtime
-- 单参数
select from_unixtime(0);
select from_unixtime(1);
select from_unixtime(1.999999);
select from_unixtime(1.9999995);
select from_unixtime(1454545444);
-- 双参数
select from_unixtime(0,'%Y-%m-%d %T');
select from_unixtime(1.999999,'%Y-%m-%d %T.%f');
select from_unixtime(1454545444,'%Y %M %D');
-- 边界值，坏值
select from_unixtime(-1);
select from_unixtime(-1454545444);
select from_unixtime(-1,'%Y');
select from_unixtime(-1454545444,'%Y %M %D');
select from_unixtime(11111111111111111,'%Y %M %D');
select from_unixtime(11111111111111111);
select from_unixtime(2147483647);
select from_unixtime(2147483647,'%Y');
select from_unixtime(2147483648);

-- test str_to_date
-- 严格模式
set sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat';
-- 正常值
select str_to_date('01,5,2013','%d,%m,%Y');
select str_to_date('May 1, 2013','%M %d,%Y');
select str_to_date('a09:30:17','a%h:%i:%s');
select str_to_date('a09:30:17','%h:%i:%s');
select str_to_date('09:30:17','%h:%i:%s');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %H:%i:%s');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %h:%i:%s');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %I:%i:%s');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %k:%i:%S');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %l:%i:%s');
select str_to_date('abc','abc');
select str_to_date('9','%m');
select str_to_date('9','%s');
select str_to_date('00/00/0000', '%m/%d/%Y');
select str_to_date('04/31/2004', '%m/%d/%Y');
select str_to_date('2022,12,12','%Y,%m,%D');
select str_to_date('2022,January,12','%Y,%M,%d');
select str_to_date('22,January,12','%y,%M,%d');
select str_to_date('2022,Jan,12','%Y,%M,%d');
select str_to_date('2022,1,12','%Y,%c,%d');
select str_to_date('2022,1,12','%Y,%c,%e');
select str_to_date('2022,1,12 12112','%Y,%c,%e %f');
select str_to_date('2022,345', '%Y,%j');
select str_to_date('2001-01-01 12:1:12 PM','%Y-%m-%D %r');
select str_to_date('2001-01-01 12:1:12 PM','%Y-%m-%D %T');
select str_to_date('2001,00,Sun 12:1:12 PM','%Y,%U,%a %T');
select str_to_date('2001,00,Sun 12:1:12 PM','%Y,%u,%a %T');
select str_to_date('2001,05,Sat 12:1:12 PM','%Y,%V,%a %T');
select str_to_date('2001,05,Sat 12:1:12 PM','%Y,%v,%a %T');
select str_to_date('2001,01,Sunday 12:1:12 PM','%Y,%U,%W %T');
select str_to_date('2001,01,1 12:1:12 PM','%Y,%U,%w %T');
select str_to_date('2001,01,01','%Y,%X,%V');
select str_to_date('2001,01,01','%Y,%x,%v');
SELECT str_to_date('200442 Monday', '%X%V %W');
-- 边界值，坏值
select str_to_date(null, '%Y');
select str_to_date('2021-11-12', null);
select str_to_date('01,5,9999','%d,%m,%Y');
select str_to_date('22,January,Sun','%y,%M,%a');
select str_to_date('01,5,10000','%d,%m,%Y');
select str_to_date('01,5,9999','%d,%m%Y');
select str_to_date('015,9999','%d,%m,%Y');
select str_to_date('00,5,9999','%d,%m,%Y');
select str_to_date('01,0,9999','%d,%m,%Y');
select str_to_date('01,111,9999','%d,%m,%Y');
SELECT STR_TO_DATE('200454 Monday', '%X%V %W');
-- 非严格模式
set sql_mode = 'sql_mode_full_group,pipes_as_concat';
-- 正常值
select str_to_date('01,5,2013','%d,%m,%Y');
select str_to_date('May 1, 2013','%M %d,%Y');
select str_to_date('a09:30:17','a%h:%i:%s');
select str_to_date('a09:30:17','%h:%i:%s');
select str_to_date('09:30:17','%h:%i:%s');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %H:%i:%s');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %h:%i:%s');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %I:%i:%s');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %k:%i:%S');
select str_to_date(' 2001,01,01 09:30:17','%Y,%m,%D %l:%i:%s');
select str_to_date('abc','abc');
select str_to_date('9','%m');
select str_to_date('9','%s');
select str_to_date('00/00/0000', '%m/%d/%Y');
select str_to_date('04/31/2004', '%m/%d/%Y');
select str_to_date('2022,12,12','%Y,%m,%D');
select str_to_date('2022,January,12','%Y,%M,%d');
select str_to_date('22,January,12','%y,%M,%d');
select str_to_date('2022,Jan,12','%Y,%M,%d');
select str_to_date('2022,1,12','%Y,%c,%d');
select str_to_date('2022,1,12','%Y,%c,%e');
select str_to_date('2022,1,12 12112','%Y,%c,%e %f');
select str_to_date('2022,345', '%Y,%j');
select str_to_date('2001-01-01 12:1:12 PM','%Y-%m-%D %r');
select str_to_date('2001-01-01 12:1:12 PM','%Y-%m-%D %T');
select str_to_date('2001,00,Sun 12:1:12 PM','%Y,%U,%a %T');
select str_to_date('2001,00,Sun 12:1:12 PM','%Y,%u,%a %T');
select str_to_date('2001,05,Sat 12:1:12 PM','%Y,%V,%a %T');
select str_to_date('2001,05,Sat 12:1:12 PM','%Y,%v,%a %T');
select str_to_date('2001,01,Sunday 12:1:12 PM','%Y,%U,%W %T');
select str_to_date('2001,01,1 12:1:12 PM','%Y,%U,%w %T');
select str_to_date('2001,01,01','%Y,%X,%V');
select str_to_date('2001,01,01','%Y,%x,%v');
SELECT STR_TO_DATE('200442 Monday', '%X%V %W');
SELECT STR_TO_DATE('200442 Monday', '%x%v %W');
-- 边界值，坏值
select str_to_date(null, '%Y');
select str_to_date('2021-11-12', null);
select str_to_date('01,5,9999','%d,%m,%Y');
select str_to_date('22,January,Sun','%y,%M,%a');
select str_to_date('01,5,10000','%d,%m,%Y');
select str_to_date('01,5,9999','%d,%m%Y');
select str_to_date('015,9999','%d,%m,%Y');
select str_to_date('00,5,9999','%d,%m,%Y');
select str_to_date('01,0,9999','%d,%m,%Y');
select str_to_date('01,111,9999','%d,%m,%Y');
SELECT STR_TO_DATE('200454 Monday', '%X%V %W');
SELECT STR_TO_DATE('200454 Monday', '%x%v %W');
-- str_to_date结束
set sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat';
\c contrib_regression
DROP DATABASE b_datetime_func_test;