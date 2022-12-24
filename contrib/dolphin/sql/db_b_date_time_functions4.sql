---- b compatibility case
drop database if exists b_datetime_func_test;
create database b_datetime_func_test dbcompatibility 'b';
\c b_datetime_func_test
set datestyle = 'ISO,ymd';
set time zone "Asia/Shanghai";
create table test(funcname text, result text);
-- test get_format
-- 功能
insert into test values('get_format(DATE, ''EUR'')', get_format(DATE, 'EUR'));
insert into test values('get_format(DATE, ''USA'')', get_format(DATE, 'USA'));
insert into test values('get_format(DATE, ''JIS'')', get_format(DATE, 'JIS'));
insert into test values('get_format(DATE, ''ISO'')', get_format(DATE, 'ISO'));
insert into test values('get_format(DATE, ''INTERNAL'')', get_format(DATE, 'INTERNAL'));
insert into test values('get_format(DATETIME, ''EUR'')', get_format(DATETIME, 'EUR'));
insert into test values('get_format(DATETIME, ''USA'')', get_format(DATETIME, 'USA'));
insert into test values('get_format(DATETIME, ''JIS'')', get_format(DATETIME, 'JIS'));
insert into test values('get_format(DATETIME, ''ISO'')', get_format(DATETIME, 'ISO'));
insert into test values('get_format(DATETIME, ''INTERNAL'')', get_format(DATETIME, 'INTERNAL'));
insert into test values('get_format(TIME, ''EUR'')', get_format(TIME, 'EUR'));
insert into test values('get_format(TIME, ''USA'')', get_format(TIME, 'USA'));
insert into test values('get_format(TIME, ''JIS'')', get_format(TIME, 'JIS'));
insert into test values('get_format(TIME, ''ISO'')', get_format(TIME, 'ISO'));
insert into test values('get_format(TIME, ''INTERNAL'')', get_format(TIME, 'INTERNAL'));
insert into test values('get_format(TIMESTAMP, ''EUR'')', get_format(TIMESTAMP, 'EUR'));
insert into test values('get_format(TIMESTAMP, ''USA'')', get_format(TIMESTAMP, 'USA'));
insert into test values('get_format(TIMESTAMP, ''JIS'')', get_format(TIMESTAMP, 'JIS'));
insert into test values('get_format(TIMESTAMP, ''ISO'')', get_format(TIMESTAMP, 'ISO'));
insert into test values('get_format(TIMESTAMP, ''INTERNAL'')', get_format(TIMESTAMP, 'INTERNAL'));
-- 特异
insert into test values('get_format(DATE, ''abc'')', get_format(DATE, 'abc'));
insert into test values('get_format(DATE, null)', get_format(DATE, null));
-- 错误情况
insert into test values('get_format(abc, ''EUR'')', get_format(abc, 'EUR'));

-- test extract
set dolphin.b_compatibility_mode = true;
-- 严格模式或者非严格模式都有值
-- 功能-单unit
insert into test values('extract(YEAR FROM ''2019-07-02'')',extract(YEAR FROM '2019-07-02'));
insert into test values('extract(MONTH FROM ''2019-07-02'')',extract(MONTH FROM '2019-07-02'));
insert into test values('extract(WEEK FROM ''2019-07-02'')',extract(WEEK FROM '2019-07-02'));
insert into test values('extract(QUARTER FROM ''2019-07-02'')',extract(QUARTER FROM '2019-07-02'));
insert into test values('extract(DAY FROM ''2019-07-02'')',extract(DAY FROM '2019-07-02'));
insert into test values('extract(SECOND FROM ''2019-07-02 12:12:12'')',extract(SECOND FROM '2019-07-02 12:12:12'));
insert into test values('extract(MINUTE FROM ''2019-07-02 12:12:12'')',extract(MINUTE FROM '2019-07-02 12:12:12'));
insert into test values('extract(HOUR FROM ''2019-07-02 12:12:12'')',extract(HOUR FROM '2019-07-02 12:12:12'));
insert into test values('extract(MICROSECOND FROM ''2019-07-02 12:12:12.12121'')',extract(MICROSECOND FROM '2019-07-02 12:12:12.12121'));
insert into test values('extract(SECOND FROM ''12:12:12'')',extract(SECOND FROM '12:12:12'));
insert into test values('extract(MINUTE FROM ''12:12:12'')',extract(MINUTE FROM '12:12:12'));
insert into test values('extract(HOUR FROM ''12:12:12'')',extract(HOUR FROM '12:12:12'));
insert into test values('extract(MICROSECOND FROM ''12:12:12.12121'')',extract(MICROSECOND FROM '12:12:12.12121'));
-- 功能-混合unit
insert into test values('extract(YEAR_MONTH FROM ''2019-07-02'')',extract(YEAR_MONTH FROM '2019-07-02'));
insert into test values('extract(DAY_HOUR FROM ''2019-07-02 12:12:12'')', extract(DAY_HOUR FROM '2019-07-02 12:12:12'));
insert into test values('extract(DAY_MINUTE FROM ''2019-07-02 12:12:12'')', extract(DAY_MINUTE FROM '2019-07-02 12:12:12'));
insert into test values('extract(DAY_MICROSECOND FROM ''2019-07-02 12:12:12.12121'')', extract(DAY_MICROSECOND FROM '2019-07-02 12:12:12.12121'));
insert into test values('extract(SECOND_MICROSECOND FROM ''2019-07-02 12:12:12.12121'')', extract(SECOND_MICROSECOND FROM '2019-07-02 12:12:12.12121'));
insert into test values('extract(SECOND_MICROSECOND FROM ''12:12:12.12121'')', extract(SECOND_MICROSECOND FROM '12:12:12.12121'));
insert into test values('extract(MINUTE_MICROSECOND FROM ''2019-07-02 12:12:12.12121'')', extract(MINUTE_MICROSECOND FROM '2019-07-02 12:12:12.12121'));
insert into test values('extract(MINUTE_MICROSECOND FROM ''12:12:12.12121'')', extract(MINUTE_MICROSECOND FROM '12:12:12.12121'));
insert into test values('extract(MINUTE_SECOND FROM ''2019-07-02 12:12:12.12121'')', extract(MINUTE_SECOND FROM '2019-07-02 12:12:12.12121'));
insert into test values('extract(MINUTE_SECOND FROM ''12:12:12.12121'')', extract(MINUTE_SECOND FROM '12:12:12.12121'));
insert into test values('extract(HOUR_SECOND FROM ''2019-07-02 12:12:12.12121'')', extract(HOUR_SECOND FROM '2019-07-02 12:12:12.12121'));
insert into test values('extract(HOUR_SECOND FROM ''12:12:12.12121'')', extract(HOUR_SECOND FROM '12:12:12.12121'));
insert into test values('extract(HOUR_MINUTE FROM ''2019-07-02 12:12:12.12121'')', extract(HOUR_MINUTE FROM '2019-07-02 12:12:12.12121'));
insert into test values('extract(HOUR_MINUTE FROM ''12:12:12.12121'')', extract(HOUR_MINUTE FROM '12:12:12.12121'));
insert into test values('extract(HOUR_MICROSECOND FROM ''2019-07-02 12:12:12.12121'')', extract(HOUR_MICROSECOND FROM '2019-07-02 12:12:12.12121'));
insert into test values('extract(HOUR_MICROSECOND FROM ''12:12:12.12121'')', extract(HOUR_MICROSECOND FROM '12:12:12.12121'));
insert into test values('extract(day_hour from ''838:59:59'')',extract(day_hour from '838:59:59'));
insert into test values('extract(day_minute from ''838:59:59'')',extract(day_minute from '838:59:59'));
insert into test values('extract(day_second from ''838:59:59'')',extract(day_second from '838:59:59'));
insert into test values('extract(day_microsecond from ''838:59:59'')',extract(day_microsecond from '838:59:59'));
-- 边界
insert into test values('extract(YEAR FROM ''9999-12-31'')', extract(YEAR FROM '9999-12-31'));
insert into test values('extract(YEAR FROM ''0000-1-1'')', extract(YEAR FROM '0000-1-1'));
insert into test values('extract(hour from ''838:59:59'')', extract(hour from '838:59:59'));
insert into test values('extract(hour from ''-838:59:59'')', extract(hour from '-838:59:59'));
-- 类型
insert into test values('extract(hour from time''-838:59:59'')', extract(hour from time'-838:59:59'));
insert into test values('extract(MONTH FROM date''2019-07-02'')', extract(MONTH FROM date'2019-07-02'));
insert into test values('extract(YEAR from cast(''2019-07-02'' as datetime)', extract(YEAR from cast('2019-07-02' as datetime)));
insert into test values('extract(hour from 8385959)', extract(hour from 8385959));
insert into test values('extract(MONTH FROM 20190702)', extract(MONTH FROM 20190702));
insert into test values('extract(year FROM 101)', extract(year FROM 101));
insert into test values('extract(month FROM 101)', extract(month FROM 101));
insert into test values('extract(quarter FROM 101)', extract(quarter FROM 101));
insert into test values('extract(day FROM 101)', extract(day FROM 101));
insert into test values('extract(day_hour FROM 101)', extract(day_hour FROM 101));
insert into test values('extract(day_microsecond FROM 101)', extract(day_microsecond FROM 101));
insert into test values('extract(hour FROM 101)', extract(hour FROM 101));
insert into test values('extract(minute FROM 101)', extract(minute FROM 101));
insert into test values('extract(second FROM 101)', extract(second FROM 101));
insert into test values('extract(microsecond FROM 101)', extract(microsecond FROM 101));
insert into test values('extract(year FROM 0)', extract(year FROM 0));
insert into test values('extract(month FROM 0)', extract(month FROM 0));
insert into test values('extract(quarter FROM 0)', extract(quarter FROM 0));
insert into test values('extract(day FROM 0)', extract(day FROM 0));
insert into test values('extract(day_hour FROM 0)', extract(day_hour FROM 0));
insert into test values('extract(day_microsecond FROM 0)', extract(day_microsecond FROM 0));
insert into test values('extract(hour FROM 0)', extract(hour FROM 0));
insert into test values('extract(minute FROM 0)', extract(minute FROM 0));
insert into test values('extract(second FROM 0)', extract(second FROM 0));
insert into test values('extract(microsecond FROM 0)', extract(microsecond FROM 0));
insert into test values('extract(microsecond from 99991231235959.999999)', extract(microsecond from 99991231235959.999999));
-- 特异
insert into test values('extract(hour from null)', extract(hour from null));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('extract(DAY FROM ''10000-01-01'')', extract(DAY FROM '10000-01-01'));
insert into test values('extract(year from 100000101)', extract(year from 100000101));
insert into test values('extract(hour from ''838:59:59.1'')', extract(hour from '838:59:59.1'));
insert into test values('extract(hour from ''839:00:00'')', extract(hour from '839:00:00'));
insert into test values('extract(hour from ''-838:59:59.1'')', extract(hour from '-838:59:59.1'));
insert into test values('extract(hour from ''-839:00:00'')', extract(hour from '-839:00:00'));
insert into test values('extract(hour from ''40 1:1:0'')', extract(hour from '40 1:1:0'));
insert into test values('extract(hour from ''-40 1:1:0'')', extract(hour from '-40 1:1:0'));
insert into test values('extract(day_hour from ''2022-01-01'')', extract(day_hour from '2022-01-01'));
insert into test values('extract(day_minute from ''2022-01-01'')', extract(day_minute from '2022-01-01'));
insert into test values('extract(day_second from ''2022-01-01'')', extract(day_second from '2022-01-01'));
insert into test values('extract(day_microsecond from ''2022-01-01'')', extract(day_microsecond from '2022-01-01'));
-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('extract(DAY FROM ''10000-01-01'')', extract(DAY FROM '10000-01-01'));
insert into test values('extract(year from 100000101)', extract(year from 100000101));
insert into test values('extract(hour from ''838:59:59.1'')', extract(hour from '838:59:59.1'));
insert into test values('extract(hour from ''839:00:00'')', extract(hour from '839:00:00'));
insert into test values('extract(hour from ''-838:59:59.1'')', extract(hour from '-838:59:59.1'));
insert into test values('extract(hour from ''-839:00:00'')', extract(hour from '-839:00:00'));
insert into test values('extract(hour from ''40 1:1:0'')', extract(hour from '40 1:1:0'));
insert into test values('extract(hour from ''-40 1:1:0'')', extract(hour from '-40 1:1:0'));
insert into test values('extract(day_hour from ''2022-01-01'')', extract(day_hour from '2022-01-01'));
insert into test values('extract(day_minute from ''2022-01-01'')', extract(day_minute from '2022-01-01'));
insert into test values('extract(day_second from ''2022-01-01'')', extract(day_second from '2022-01-01'));
insert into test values('extract(day_microsecond from ''2022-01-01'')', extract(day_microsecond from '2022-01-01'));
set dolphin.b_compatibility_mode = false;

-- test date_format
-- 严格模式或者非严格模式都有值
-- 功能
insert into test values('date_format(''0000-01-01'',''%U %u %V %v %X %x %j'')', date_format('0000-01-01', '%U %u %V %v %X %x %j'));
insert into test values('date_format(''2021-11-12'',''%U %u %V %v %X %x %j'')', date_format('2021-11-12', '%U %u %V %v %X %x %j'));
insert into test values('date_format(''9999-12-31'',''%U %u %V %v %X %x %j'')', date_format('9999-12-31', '%U %u %V %v %X %x %j'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%a'')', date_format('2001-01-01 12:12:12','%a'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%b'')', date_format('2001-01-01 12:12:12','%b'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%c'')', date_format('2001-01-01 12:12:12','%c'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%D'')', date_format('2001-01-01 12:12:12','%D'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%d'')', date_format('2001-01-01 12:12:12','%d'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%e'')', date_format('2001-01-01 12:12:12','%e'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%f'')', date_format('2001-01-01 12:12:12','%f'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%H'')', date_format('2001-01-01 12:12:12','%H'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%h'')', date_format('2001-01-01 12:12:12','%h'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%I'')', date_format('2001-01-01 12:12:12','%I'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%i'')', date_format('2001-01-01 12:12:12','%i'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%j'')', date_format('2001-01-01 12:12:12','%j'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%k'')', date_format('2001-01-01 12:12:12','%k'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%l'')', date_format('2001-01-01 12:12:12','%l'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%M'')', date_format('2001-01-01 12:12:12','%M'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%m'')', date_format('2001-01-01 12:12:12','%m'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%p'')', date_format('2001-01-01 12:12:12','%p'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%r'')', date_format('2001-01-01 12:12:12','%r'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%S'')', date_format('2001-01-01 12:12:12','%S'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%s'')', date_format('2001-01-01 12:12:12','%s'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%T'')', date_format('2001-01-01 12:12:12','%T'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%U'')', date_format('2001-01-01 12:12:12','%U'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%u'')', date_format('2001-01-01 12:12:12','%u'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%V'')', date_format('2001-01-01 12:12:12','%V'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%v'')', date_format('2001-01-01 12:12:12','%v'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%W'')', date_format('2001-01-01 12:12:12','%W'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%w'')', date_format('2001-01-01 12:12:12','%w'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%X'')', date_format('2001-01-01 12:12:12','%X'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%x'')', date_format('2001-01-01 12:12:12','%x'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%Y'')', date_format('2001-01-01 12:12:12','%Y'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%y'')', date_format('2001-01-01 12:12:12','%y'));
insert into test values('date_format(''2001-01-01 12:12:12'',''aaaa'')', date_format('2001-01-01 12:12:12','aaaa'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%dffff'')', date_format('2001-01-01 12:12:12','%dffff'));
insert into test values('date_format(''2001-01-01 12:12:12.34567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890'',''%b'')', date_format('2001-01-01 12:12:12.34567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890','%b'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b'')', date_format('2001-01-01 12:12:12','%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%Y %M %D %U'')', date_format('2001-01-01 12:12:12','%Y %M %D %U'));
insert into test values('date_format(''2001-01-01 12:12:12'',''%H %k %I %r %T %S %w'')', date_format('2001-01-01 12:12:12','%H %k %I %r %T %S %w'));
-- 边界
insert into test values('date_format(''0000-01-01'',''%Y'')', date_format('0000-01-01','%Y'));
insert into test values('date_format(''9999-12-31'',''%Y'')', date_format('9999-12-31','%Y'));
insert into test values('date_format(''0000-01-01 00:00:00'',''%Y %m %d %T'')', date_format('0000-01-01 00:00:00','%Y %m %d %T'));
insert into test values('date_format(''9999-12-31 23:59:59.999999'',''%Y %m %d %T %f'')', date_format('9999-12-31 23:59:59.999999','%Y %m %d %T %f'));
-- 类型
insert into test values('date_format(date''2001-01-09'',''%Y'')', date_format(date'2001-01-09','%Y'));
insert into test values('date_format(cast(''2001-01-09 12:12:12'' as datetime),''%Y %m %d %T'')', date_format(cast('2001-01-09 12:12:12' as datetime),'%Y %m %d %T'));
insert into test values('date_format(20010101,''%Y'')', date_format(20010101,'%Y'));
insert into test values('date_format(20010101121212,''%Y %m %d %T'')', date_format(20010101121212,'%Y %m %d %T'));
insert into test values('date_format(20010101,''%Y'')', date_format(20010101,'%Y'));
insert into test values('date_format(101,''%Y %y %m %d %U %u %V %v %X %x'')', date_format(100,'%Y %y %m %d %U %u %V %v %X %x'));
-- 特异
insert into test values('date_format(null, ''%Y'')', date_format(null, '%Y'));
insert into test values('date_format(''2021-11-12'', null)', date_format('2021-11-12', null));
insert into test values('date_format(null, null)', date_format(null, null));
insert into test values('date_format(''2021-0-12 12:12:12'', ''%Y %m %d %T'')', date_format('2021-0-12 12:12:12', '%Y %m %d %T'));
insert into test values('date_format(''2021-1-0 12:12:12'', ''%Y %m %d %T'')', date_format('2021-1-0 12:12:12', '%Y %m %d %T'));
insert into test values('date_format(''2021-0-0 12:12:12'', ''%Y %m %d %T'')', date_format('2021-0-0 12:12:12', '%Y %m %d %T'));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('date_format(''10000-01-01 12:12:12'',''%Y %m %d %T'')', date_format('10000-01-01 12:12:12','%Y %m %d %T'));
insert into test values('date_format(''2001-01-32 12:12:12'',''%Y %m %d %T'')', date_format('2001-01-32 12:12:12','%Y %m %d %T'));
insert into test values('date_format(100000101, ''%b'')', date_format(100000101, '%b'));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('date_format(''10000-01-01 12:12:12'',''%Y %m %d %T'')', date_format('10000-01-01 12:12:12','%Y %m %d %T'));
insert into test values('date_format(''2001-01-32 12:12:12'',''%Y %m %d %T'')', date_format('2001-01-32 12:12:12','%Y %m %d %T'));
insert into test values('date_format(100000101, ''%b'')', date_format(100000101, '%b'));

-- test from_unixtime
-- 严格模式或者非严格模式都有值
-- 功能-单参数
insert into test values('from_unixtime(0)', from_unixtime(0));
insert into test values('from_unixtime(1)', from_unixtime(1));
insert into test values('from_unixtime(1.999999)', from_unixtime(1.999999));
insert into test values('from_unixtime(1.9999995)', from_unixtime(1.9999995));
insert into test values('from_unixtime(1454545444)', from_unixtime(1454545444));
-- 功能-双参数
insert into test values('from_unixtime(0,''%Y-%m-%d %T'')', from_unixtime(0,'%Y-%m-%d %T'));
insert into test values('from_unixtime(1.999999,''%Y-%m-%d %T.%f'')', from_unixtime(1.999999,'%Y-%m-%d %T.%f'));
insert into test values('from_unixtime(1454545444,''%Y %M %D'')', from_unixtime(1454545444,'%Y %M %D'));
-- 边界
insert into test values('from_unixtime(2147483647)', from_unixtime(2147483647));
insert into test values('from_unixtime(2147483647,''%Y'')', from_unixtime(2147483647,'%Y'));

-- 特异
insert into test values('from_unixtime(null)', from_unixtime(null));
insert into test values('from_unixtime(null,''%Y'')', from_unixtime(null,'%Y'));
insert into test values('from_unixtime(2147483647,null)', from_unixtime(2147483647,null));
insert into test values('from_unixtime(null,null)', from_unixtime(null,null));

-- 超范围，空值
insert into test values('from_unixtime(-1)', from_unixtime(-1));
insert into test values('from_unixtime(-1454545444)', from_unixtime(-1454545444));
insert into test values('from_unixtime(-1,''%Y'')', from_unixtime(-1,'%Y'));
insert into test values('from_unixtime(-1454545444,''%Y %M %D'')', from_unixtime(-1454545444,'%Y %M %D'));
insert into test values('from_unixtime(11111111111111111,''%Y %M %D'')', from_unixtime(11111111111111111,'%Y %M %D'));
insert into test values('from_unixtime(11111111111111111)', from_unixtime(11111111111111111));
insert into test values('from_unixtime(2147483648)', from_unixtime(2147483648));

-- test str_to_date
-- 严格模式或者非严格模式都有值
-- 功能-返回date
insert into test values('str_to_date(''2021-11-12'', ''%Y-%m-%d'')', str_to_date('2021-11-12', '%Y-%m-%d'));
insert into test values('str_to_date(''9999-12-31'', ''%Y-%m-%d'')', str_to_date('9999-12-31', '%Y-%m-%d'));
insert into test values('str_to_date(''12 12 12'', ''%y %m %d'')', str_to_date('12 12 12', '%y %m %d'));
insert into test values('str_to_date(''200442 Monday'', ''%X%V %W'')', str_to_date('200442 Monday', '%X%V %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%v %W'')', str_to_date('200442 Monday', '%x%v %W'));
insert into test values('str_to_date(''200442 Monday'', ''%Y%U %W'')', str_to_date('200442 Monday', '%Y%U %W'));
insert into test values('str_to_date(''200442 Monday'', ''%Y%u %W'')', str_to_date('200442 Monday', '%Y%u %W'));
insert into test values('str_to_date(''200442 Mon'', ''%Y%u %a'')', str_to_date('200442 Mon', '%Y%u %a'));
insert into test values('str_to_date(''200442 Tue'', ''%Y%u %a'')', str_to_date('200442 Tue', '%Y%u %a'));
insert into test values('str_to_date(''200442 Wed'', ''%Y%u %a'')', str_to_date('200442 Wed', '%Y%u %a'));
insert into test values('str_to_date(''200442 Thu'', ''%Y%u %a'')', str_to_date('200442 Thu', '%Y%u %a'));
insert into test values('str_to_date(''200442 Fri'', ''%Y%u %a'')', str_to_date('200442 Fri', '%Y%u %a'));
insert into test values('str_to_date(''200442 Sat'', ''%Y%u %a'')', str_to_date('200442 Sat', '%Y%u %a'));
insert into test values('str_to_date(''200442 Sun'', ''%Y%u %a'')', str_to_date('200442 Sun', '%Y%u %a'));
insert into test values('str_to_date(''200442 1'', ''%X%V %w'')', str_to_date('200442 1', '%X%V %w'));
insert into test values('str_to_date(''200442 1'', ''%x%v %w'')', str_to_date('200442 1', '%x%v %w'));
insert into test values('str_to_date(''200442 1'', ''%Y%U %w'')', str_to_date('200442 1', '%Y%U %w'));
insert into test values('str_to_date(''200442 1'', ''%Y%u %w'')', str_to_date('200442 1', '%Y%u %w'));
insert into test values('str_to_date(''200442 2'', ''%Y%u %w'')', str_to_date('200442 2', '%Y%u %w'));
insert into test values('str_to_date(''200442 3'', ''%Y%u %w'')', str_to_date('200442 3', '%Y%u %w'));
insert into test values('str_to_date(''200442 4'', ''%Y%u %w'')', str_to_date('200442 4', '%Y%u %w'));
insert into test values('str_to_date(''200442 5'', ''%Y%u %w'')', str_to_date('200442 5', '%Y%u %w'));
insert into test values('str_to_date(''200442 6'', ''%Y%u %w'')', str_to_date('200442 6', '%Y%u %w'));
insert into test values('str_to_date(''200442 0'', ''%Y%u %w'')', str_to_date('200442 0', '%Y%u %w'));
insert into test values('str_to_date(''2004 100'', ''%Y %j'')', str_to_date('2004 100', '%Y %j'));
insert into test values('str_to_date(''2004 January 12'', ''%Y %M %d'')', str_to_date('2004 January 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 February 12'', ''%Y %M %d'')', str_to_date('2004 February 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 March 12'', ''%Y %M %d'')', str_to_date('2004 March 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 April 12'', ''%Y %M %d'')', str_to_date('2004 April 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 May 12'', ''%Y %M %d'')', str_to_date('2004 May 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 June 12'', ''%Y %M %d'')', str_to_date('2004 June 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 July 12'', ''%Y %M %d'')', str_to_date('2004 July 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 August 12'', ''%Y %M %d'')', str_to_date('2004 August 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 September 12'', ''%Y %M %d'')', str_to_date('2004 September 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 October 12'', ''%Y %M %d'')', str_to_date('2004 October 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 November 12'', ''%Y %M %d'')', str_to_date('2004 November 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 December 12'', ''%Y %M %d'')', str_to_date('2004 December 12', '%Y %M %d'));
insert into test values('str_to_date(''2004 Jan 12'', ''%Y %b %d'')', str_to_date('2004 Jan 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Feb 12'', ''%Y %b %d'')', str_to_date('2004 Feb 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Mar 12'', ''%Y %b %d'')', str_to_date('2004 Mar 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Apr 12'', ''%Y %b %d'')', str_to_date('2004 Apr 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 May 12'', ''%Y %b %d'')', str_to_date('2004 May 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Jun 12'', ''%Y %b %d'')', str_to_date('2004 Jun 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Jul 12'', ''%Y %b %d'')', str_to_date('2004 Jul 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Aug 12'', ''%Y %b %d'')', str_to_date('2004 Aug 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Sep 12'', ''%Y %b %d'')', str_to_date('2004 Sep 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Oct 12'', ''%Y %b %d'')', str_to_date('2004 Oct 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Nov 12'', ''%Y %b %d'')', str_to_date('2004 Nov 12', '%Y %b %d'));
insert into test values('str_to_date(''2004 Dec 12'', ''%Y %b %d'')', str_to_date('2004 Dec 12', '%Y %b %d'));
-- 功能-返回datetime
insert into test values('str_to_date(''2021-11-12 12:12:12'', ''%Y-%m-%d %T'')', str_to_date('2021-11-12 12:12:12', '%Y-%m-%d %T'));
insert into test values('str_to_date(''9999-12-31 23:59:59'', ''%Y-%m-%d %T'')', str_to_date('9999-12-31 23:59:59', '%Y-%m-%d %T'));
insert into test values('str_to_date(''0000-1-1 00:00:00'', ''%Y-%m-%d %T'')', str_to_date('0000-1-1 00:00:00', '%Y-%m-%d %T'));
insert into test values('str_to_date(''2021-11-12 12:12:12.123456'', ''%Y-%m-%d %T.%f'')', str_to_date('2021-11-12 12:12:12.123456', '%Y-%m-%d %T.%f'));
insert into test values('str_to_date(''9999-12-31 23:59:59.999999'', ''%Y-%m-%d %T.%f'')', str_to_date('9999-12-31 23:59:59.999999', '%Y-%m-%d %T.%f'));
insert into test values('str_to_date(''0000-1-1 00:00:00.000000'', ''%Y-%m-%d %T.%f'')', str_to_date('0000-1-1 00:00:00.000000', '%Y-%m-%d %T.%f'));
-- 功能-返回time
insert into test values('str_to_date(''0'', ''%H'')', str_to_date('0', '%H'));
insert into test values('str_to_date(''23'', ''%H'')', str_to_date('23', '%H'));
insert into test values('str_to_date(''12'', ''%h'')', str_to_date('12', '%h'));
insert into test values('str_to_date(''10:11:12'', ''%T'')', str_to_date('10:11:12', '%T'));
insert into test values('str_to_date(''10:11:12.999999'', ''%T.%f'')', str_to_date('10:11:12.999999', '%T.%f'));
insert into test values('str_to_date(''10:11:12'', ''%H:%i:%S'')', str_to_date('10:11:12', '%H:%i:%S'));
insert into test values('str_to_date(''10:11:12.999999'', ''%H:%i:%S.%f'')', str_to_date('10:11:12.999999', '%H:%i:%S.%f'));
insert into test values('str_to_date(''1:1:1 pm'', ''%r'')', str_to_date('1:1:1 pm', '%r'));
insert into test values('str_to_date(''1:1:1 am'', ''%r'')', str_to_date('1:1:1 am', '%r'));
insert into test values('str_to_date(''1:1:1 pm'', ''%h:%i:%S %p'')', str_to_date('1:1:1 pm', '%h:%i:%S %p'));
insert into test values('str_to_date(''1:1:1 am'', ''%h:%i:%S %p'')', str_to_date('1:1:1 am', '%h:%i:%S %p'));
insert into test values('str_to_date(''1:1:1.999999 pm'', ''%h:%i:%S.%f %p'')', str_to_date('1:1:1.999999 pm', '%h:%i:%S.%f %p'));
insert into test values('str_to_date(''1:1:1.999999 am'', ''%h:%i:%S.%f %p'')', str_to_date('1:1:1.999999 am', '%h:%i:%S.%f %p'));
-- 边界
insert into test values('str_to_date(''0000 1 1'', ''%Y %m %d'')', str_to_date('0000 1 1', '%Y %m %d'));
insert into test values('str_to_date(''9999 12 31'', ''%Y %m %d'')', str_to_date('9999 12 31', '%Y %m %d'));
insert into test values('str_to_date(''0000 1 1 00:00:00'', ''%Y %m %d %T'')', str_to_date('0000 1 1 00:00:00', '%Y %m %d %T'));
insert into test values('str_to_date(''9999 12 31 23:59:59.999999'', ''%Y %m %d %T.%f'')', str_to_date('9999 12 31 23:59:59.999999', '%Y %m %d %T.%f'));
-- 特异
insert into test values('str_to_date(null, ''%Y %m %d'')', str_to_date(null, '%Y %m %d'));

-- 非严格模式，参数不合法，报warning，返回NULL或者对应值
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('str_to_date(''0'', ''%h'')', str_to_date('0', '%h'));
insert into test values('str_to_date(''10000-1-1'', ''%Y-%m-%d'')', str_to_date('10000-1-1', '%Y-%m-%d'));
insert into test values('str_to_date(''2021-13-1'', ''%Y-%m-%d'')', str_to_date('2021-13-1', '%Y-%m-%d'));
insert into test values('str_to_date(''2021-1-32'', ''%Y-%m-%d'')', str_to_date('2021-1-32', '%Y-%m-%d'));
insert into test values('str_to_date(''2021-1-1 24:00:00'', ''%Y-%m-%d %T'')', str_to_date('2021-1-1 24:00:00', '%Y-%m-%d %T'));
insert into test values('str_to_date(''2021-1-1 00:60:00'', ''%Y-%m-%d %T'')', str_to_date('2021-1-1 00:60:00', '%Y-%m-%d %T'));
insert into test values('str_to_date(''2021-1-1 00:00:60'', ''%Y-%m-%d %T'')', str_to_date('2021-1-1 00:00:60', '%Y-%m-%d %T'));
insert into test values('str_to_date(''2021-1-1 24:00:00'', ''%Y-%m-%d %H:%i:%S'')', str_to_date('2021-1-1 24:00:00', '%Y-%m-%d %H:%i:%S'));
insert into test values('str_to_date(''2021-1-1 00:60:00'', ''%Y-%m-%d %H:%i:%S'')', str_to_date('2021-1-1 00:60:00', '%Y-%m-%d %H:%i:%S'));
insert into test values('str_to_date(''2021-1-1 00:00:60'', ''%Y-%m-%d %H:%i:%S'')', str_to_date('2021-1-1 00:00:60', '%Y-%m-%d %H:%i:%S'));
insert into test values('str_to_date(''13:00:00 pm'', ''%r'')', str_to_date('13:00:00 pm', '%r'));
insert into test values('str_to_date(''13:00:00 am'', ''%r'')', str_to_date('13:00:00 am', '%r'));
insert into test values('str_to_date(''24:00:00'', ''%T'')', str_to_date('24:00:00', '%T'));
insert into test values('str_to_date(''13:00:00'', ''%h:%i:%S'')', str_to_date('13:00:00', '%h:%i:%S'));
insert into test values('str_to_date(''24:00:00'', ''%H:%i:%S'')', str_to_date('24:00:00', '%H:%i:%S'));
insert into test values('str_to_date(''13'',''%h'')', str_to_date('13','%h'));
insert into test values('str_to_date(''24'',''%H'')', str_to_date('24','%H'));
insert into test values('str_to_date(''2021-11-12'', ''%Y/a%m/%d'')', str_to_date('2021-11-12', '%Y/a%m/%d'));
insert into test values('str_to_date(''2021-a11-12'', ''%Y/%m/%d'')', str_to_date('2021-a11-12', '%Y/%m/%d'));
insert into test values('str_to_date(''2021/11/12abc'', ''%Y/%m/%d'')', str_to_date('2021/11/12abc', '%Y/%m/%d'));
insert into test values('str_to_date(''200442 Monday'', ''%X%v %W'')', str_to_date('200442 Monday', '%X%v %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%V %W'')', str_to_date('200442 Monday', '%x%V %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%U %W'')', str_to_date('200442 Monday', '%x%U %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%u %W'')', str_to_date('200442 Monday', '%x%u %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%U %W'')', str_to_date('200442 Monday', '%x%U %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%u %W'')', str_to_date('200442 Monday', '%x%u %W'));
insert into test values('str_to_date(''2021-11-12'', null)', str_to_date('2021-11-12', null));
insert into test values('str_to_date(''200454 Monday'', ''%X%V %W'')', str_to_date('200454 Monday', '%X%V %W'));

-- 严格模式，参数不合法，抛出错误
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into test values('str_to_date(''0'', ''%h'')', str_to_date('0', '%h'));
insert into test values('str_to_date(''10000-1-1'', ''%Y-%m-%d'')', str_to_date('10000-1-1', '%Y-%m-%d'));
insert into test values('str_to_date(''2021-13-1'', ''%Y-%m-%d'')', str_to_date('2021-13-1', '%Y-%m-%d'));
insert into test values('str_to_date(''2021-1-32'', ''%Y-%m-%d'')', str_to_date('2021-1-32', '%Y-%m-%d'));
insert into test values('str_to_date(''2021-1-1 24:00:00'', ''%Y-%m-%d %T'')', str_to_date('2021-1-1 24:00:00', '%Y-%m-%d %T'));
insert into test values('str_to_date(''2021-1-1 00:60:00'', ''%Y-%m-%d %T'')', str_to_date('2021-1-1 00:60:00', '%Y-%m-%d %T'));
insert into test values('str_to_date(''2021-1-1 00:00:60'', ''%Y-%m-%d %T'')', str_to_date('2021-1-1 00:00:60', '%Y-%m-%d %T'));
insert into test values('str_to_date(''2021-1-1 24:00:00'', ''%Y-%m-%d %H:%i:%S'')', str_to_date('2021-1-1 24:00:00', '%Y-%m-%d %H:%i:%S'));
insert into test values('str_to_date(''2021-1-1 00:60:00'', ''%Y-%m-%d %H:%i:%S'')', str_to_date('2021-1-1 00:60:00', '%Y-%m-%d %H:%i:%S'));
insert into test values('str_to_date(''2021-1-1 00:00:60'', ''%Y-%m-%d %H:%i:%S'')', str_to_date('2021-1-1 00:00:60', '%Y-%m-%d %H:%i:%S'));
insert into test values('str_to_date(''13:00:00 pm'', ''%r'')', str_to_date('13:00:00 pm', '%r'));
insert into test values('str_to_date(''13:00:00 am'', ''%r'')', str_to_date('13:00:00 am', '%r'));
insert into test values('str_to_date(''24:00:00'', ''%T'')', str_to_date('24:00:00', '%T'));
insert into test values('str_to_date(''13:00:00'', ''%h:%i:%S'')', str_to_date('13:00:00', '%h:%i:%S'));
insert into test values('str_to_date(''24:00:00'', ''%H:%i:%S'')', str_to_date('24:00:00', '%H:%i:%S'));
insert into test values('str_to_date(''13'',''%h'')', str_to_date('13','%h'));
insert into test values('str_to_date(''24'',''%H'')', str_to_date('24','%H'));
insert into test values('str_to_date(''2021-11-12'', ''%Y/a%m/%d'')', str_to_date('2021-11-12', '%Y/a%m/%d'));
insert into test values('str_to_date(''2021-a11-12'', ''%Y/%m/%d'')', str_to_date('2021-a11-12', '%Y/%m/%d'));
insert into test values('str_to_date(''2021/11/12abc'', ''%Y/%m/%d'')', str_to_date('2021/11/12abc', '%Y/%m/%d'));
insert into test values('str_to_date(''200442 Monday'', ''%X%v %W'')', str_to_date('200442 Monday', '%X%v %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%V %W'')', str_to_date('200442 Monday', '%x%V %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%U %W'')', str_to_date('200442 Monday', '%x%U %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%u %W'')', str_to_date('200442 Monday', '%x%u %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%U %W'')', str_to_date('200442 Monday', '%x%U %W'));
insert into test values('str_to_date(''200442 Monday'', ''%x%u %W'')', str_to_date('200442 Monday', '%x%u %W'));
insert into test values('str_to_date(''2021-11-12'', null)', str_to_date('2021-11-12', null));
insert into test values('str_to_date(''200454 Monday'', ''%X%V %W'')', str_to_date('200454 Monday', '%X%V %W'));

-- 结果
select * from test order by funcname;
drop table test;
\c contrib_regression
DROP DATABASE b_datetime_func_test;