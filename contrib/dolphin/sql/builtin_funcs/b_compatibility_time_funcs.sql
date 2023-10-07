create schema b_time_funcs;
set current_schema to 'b_time_funcs';
set datestyle to 'ISO,YMD';

create table func_test(functionName varchar(256),result varchar(256));
truncate table func_test;
-- makedate()
    -- 非指定类型的参数
insert into func_test(functionName, result) values('makedate(''2003'',''61'')', makedate('2003','61'));
insert into func_test(functionName, result) values('makedate(''12.4'',''12.5'')', makedate('12.4','12.5'));
insert into func_test(functionName, result) values('makedate(12.4,12.5)', makedate(12.4,12.5));
insert into func_test(functionName, result) values('makedate(''abcd'', ''61'')', makedate('abcd', '61'));
insert into func_test(functionName, result) values('makedate(false, true)', makedate(false, true));
insert into func_test(functionName, result) values('makedate(B''101'', B''101'')', makedate(B'101', B'101'));
insert into func_test(functionName, result) values('makedate(bytea''1'', bytea''1'')', makedate(bytea'1', bytea'1')); -- mysql中要将bytea转为binary

insert into func_test(functionName, result) values('makedate(null, 10)', makedate(null, 10));
insert into func_test(functionName, result) values('makedate(2000, null)', makedate(2000, null));
insert into func_test(functionName, result) values('makedate(-1, 20)', makedate(-1, 20));
insert into func_test(functionName, result) values('makedate(10000, 20)', makedate(10000, 20));
insert into func_test(functionName, result) values('makedate(2000, 0)', makedate(2000, 0));
insert into func_test(functionName, result) values('makedate(2000, 60)', makedate(2000, 60));
insert into func_test(functionName, result) values('makedate(2000, 380)', makedate(2000, 380));
insert into func_test(functionName, result) values('makedate(69, 32)',makedate(69, 32));
insert into func_test(functionName, result) values('makedate(70, 32)', makedate(70, 32));
insert into func_test(functionName, result) values('makedate(100,32)', makedate(100,32));
insert into func_test(functionName, result) values('makedate(9999,366)', makedate(9999,366));
insert into func_test(functionName, result) values('makedate(pow(2,62),366)', makedate(pow(2,62),366));
insert into func_test(functionName, result) values('makedate(2001, 9223372036854775807)', makedate(2001, 9223372036854775807));
-- MAKETIME
    -- 非指定类型的参数
insert into func_test(functionName, result) values ('maketime(''25.5'', ''30.4'', ''30'')', maketime('25.5', '30.4', '30'));
insert into func_test(functionName, result) values ('maketime(25.5, 30.4, 30)', maketime(25.5, 30.4, 30));
insert into func_test(functionName, result) values ('maketime(''ABCD'', ''30.4'', ''30.123'')', maketime('ABCD', '30.4', '30.123'));
insert into func_test(functionName, result) values ('maketime(false, true, true)', maketime(false, true, true));
insert into func_test(functionName, result) values ('maketime(B''111'', B''111'', B''111'')', maketime(B'111', B'111', B'111'));
insert into func_test(functionName, result) values ('maketime(bytea''1'', bytea''1'', bytea''1'')', maketime(bytea'1', bytea'1', bytea'1'));
    -- 特殊参数
insert into func_test(functionName, result) values ('maketime(null, 0, 0)', maketime(null, 0, 0));
insert into func_test(functionName, result) values ('maketime(0, null, 0)', maketime(0, null, 0));
insert into func_test(functionName, result) values ('maketime(0, 0, null)', maketime(0, 0, null));
insert into func_test(functionName, result) values ('maketime(0, -1, 0)', maketime(0, -1, 0)); -- minute < 0
insert into func_test(functionName, result) values ('maketime(0, 60, 0)', maketime(0, 60, 0));-- minute > 59
insert into func_test(functionName, result) values ('maketime(0, 60, 0)', maketime(0, 59.5, 0)); -- 小数按照四舍五入规则取整
insert into func_test(functionName, result) values ('maketime(0, 0, -1)', maketime(0, 0, -1)); -- second < 0
insert into func_test(functionName, result) values ('maketime(0, 0, 60)', maketime(0, 0, 60)); -- second >= 60
insert into func_test(functionName, result) values ('maketime(0, 0, pow(10,18))', maketime(0, 0, pow(10,18))); -- second >=10^18
insert into func_test(functionName, result) values ('maketime(4294967295, 1, 1)', maketime(4294967295, 1, 1));
insert into func_test(functionName, result) values ('maketime(-4294967295, 0, 1)', maketime(-4294967295, 0, 1));
insert into func_test(functionName, result) values ('maketime(2147483649, 1, 1)', maketime(2147483649, 1, 1));
insert into func_test(functionName, result) values ('maketime(-2147483649, 1, 1)', maketime(-2147483649, 1, 1));
insert into func_test(functionName, result) values ('maketime(-9223372036854775808, 0, 0)', maketime(-9223372036854775808, 0, 0));

-- 精度一致
insert into func_test(functionName, result) values ('maketime(0, 0, 59.9999)', maketime(0, 0, 59.9999));  -- 结果精度与参数相同
insert into func_test(functionName, result) values ('maketime(0, 0, 59.999999001)', maketime(0, 0, 59.999999001)); -- 最高保留精度为小数点后6位
-- 函数主体
insert into func_test(functionName, result) values ('maketime(838, 59, 58.999999)', maketime(838, 59, 58.999999));
insert into func_test(functionName, result) values ('maketime(-838, 59, 58.999999)', maketime(-838, 59, 58.999999));
insert into func_test(functionName, result) values ('maketime(25, 30, 30)', maketime(25, 30, 30));
insert into func_test(functionName, result) values ('maketime(-25, 30, 30)', maketime(-25, 30, 30));
insert into func_test(functionName, result) values ('maketime(pow(2, 32),0,0)', maketime(pow(2, 32),0,0));
insert into func_test(functionName, result) values ('maketime(-pow(2, 32),0,0)', maketime(-pow(2, 32),0,0));
insert into func_test(functionName, result) values ('maketime(0, 59, 59.9999994999)', maketime(0, 59, 59.9999994999));
insert into func_test(functionName, result) values ('maketime(0, 10, 59.999999500)', maketime(0, 10, 59.999999500));
insert into func_test(functionName, result) values ('maketime(0, 59, 59.999999500)', maketime(0, 59, 59.999999500));
insert into func_test(functionName, result) values ('maketime(0, 59, 59.999998500)', maketime(0, 59, 59.999998500));
-- 返回值越界
insert into func_test(functionName, result) values ('maketime(839, 0, 0)', maketime(839, 0, 0)); 
insert into func_test(functionName, result) values ('maketime(-839, 0, 0)', maketime(-839, 0, 0));
insert into func_test(functionName, result) values ('maketime(838, 59, 59.000001)', maketime(838, 59, 59.000001)); -- 小数导致越界
insert into func_test(functionName, result) values ('maketime(838, 59, 59.000000600)', maketime(838, 59, 59.000000600)); -- 进位导致越界

-- PERIOD_ADD
    -- 非指定类型的参数
insert into func_test(functionName, result) values ('period_add(''202101'', ''10'')', period_add('202101', '10'));
insert into func_test(functionName, result) values ('period_add(''202101.4'', ''10.5'')', period_add('202101.4', '10.5'));
insert into func_test(functionName, result) values ('period_add(202101.4, 10.5)', period_add(202101.4, 10.5));
insert into func_test(functionName, result) values ('period_add(''abcd'', 1)', period_add('abcd', 1));
insert into func_test(functionName, result) values ('period_add(true, false)', period_add(true, false));
insert into func_test(functionName, result) values ('period_add(B''1111'', B''11'')', period_add(B'1111', B'11'));
insert into func_test(functionName, result) values ('period_add(bytea''1111'', bytea''11'')', period_add(bytea'1111', bytea'11'));
    -- 特殊/非法参数测试
insert into func_test(functionName, result) values ('period_add(NULL, 1)', period_add(NULL, 1));
insert into func_test(functionName, result) values ('period_add(202205, NULL)', period_add(202205, NULL));
insert into func_test(functionName, result) values ('period_add(0, 1)', period_add(0, 1)); -- p == 0
insert into func_test(functionName, result) values ('period_add(200000, 0)', period_add(200000, 0)); -- p的月份为0
insert into func_test(functionName, result) values ('period_add(-202205, 1)', period_add(-202205, 1)); -- p < 0
insert into func_test(functionName, result) values ('period_add(202205, pow(2,31))', period_add(202205, pow(2,31))); -- n处于int边界值
    -- 函数主体
insert into func_test(functionName, result) values ('period_add(202211, 100)', period_add(202211, 100)); -- 跨年计算，包含闰年
insert into func_test(functionName, result) values ('period_add(197005, 7)', period_add(197005, 7)); -- 年内运算
insert into func_test(functionName, result) values ('period_add(202205, -12)', period_add(202205, -12)); -- N < 0 
insert into func_test(functionName, result) values ('period_add(1,12)', period_add(1,12)); -- P的年份=0
insert into func_test(functionName, result) values ('period_add(6901,12)', period_add(6901,12)); -- P的年份<70
insert into func_test(functionName, result) values ('period_add(7001,12)', period_add(7001,12)); -- 70 <= P的年份 < 100
insert into func_test(functionName, result) values ('period_add(10001,12)', period_add(10001,12)); -- P的年份>=100
insert into func_test(functionName, result) values ('period_add(10001, -12)', period_add(10001, -12)); -- 70 <= 返回结果年份 < 100
insert into func_test(functionName, result) values ('period_add(10001, -12*30-1)', period_add(10001, -361)); -- 返回结果年份 < 70
insert into func_test(functionName, result) values ('period_add(pow(2,60), 20)', period_add(pow(2,60), 20));

--PERIOD_DIFF
    -- 非指定类型的参数
insert into func_test(functionName, result) values ('period_diff(''202101'', ''202102'')', period_diff('202101', '202102')); 
insert into func_test(functionName, result) values ('period_diff(''202101.4'', ''202102.5'')', period_diff('202101.4', '202102.5')); 
insert into func_test(functionName, result) values ('period_diff(202101.4, 202102.5)', period_diff(202101.4, 202102.5)); 
insert into func_test(functionName, result) values ('period_diff(''abcd'', ''abcd'')', period_diff('abcd', 'abcd')); 
insert into func_test(functionName, result) values ('period_diff(true, false)', period_diff(true, false)); 
insert into func_test(functionName, result) values ('period_diff(B''1111'', B''11111'')); ', period_diff(B'1111', B'11111')); 
insert into func_test(functionName, result) values ('period_diff(bytea''1111'', bytea''11111'')); ', period_diff(bytea'1111', bytea'11111')); 
    -- 特殊/非法参数测试
insert into func_test(functionName, result) values ('period_diff(NULL, 200001)', period_diff(NULL, 200001));
insert into func_test(functionName, result) values ('period_diff(202011, NULL)', period_diff(202011, NULL));
insert into func_test(functionName, result) values ('period_diff(-202011, 200001)', period_diff(-202011, 200001)); -- P1<0
insert into func_test(functionName, result) values ('period_diff(202011, -200001)', period_diff(202011, -200001)); -- P2<0
insert into func_test(functionName, result) values ('period_diff(707712, 202201)', period_diff(707712, 202201));
    -- P1、P2年份处于70和100的边界处
insert into func_test(functionName, result) values ('period_diff(6912, 10001)', period_diff(6912, 10001));
insert into func_test(functionName, result) values ('period_diff(7001, 10001)', period_diff(7001, 10001));
insert into func_test(functionName, result) values ('period_diff(10002, 6901)', period_diff(10002, 6901));
insert into func_test(functionName, result) values ('period_diff(10002, 7001)', period_diff(10002, 7001));
    -- 0值
insert into func_test(functionName, result) values ('period_diff(0, 30)', period_diff(0, 30)); -- P1 == 0, P2的年份为0
insert into func_test(functionName, result) values ('period_diff(30, 0)', period_diff(30, 0)); -- P2 == 0, P1的年份为0
    -- 超大数计算
insert into func_test(functionName, result) values ('period_diff(pow(2,62), pow(2,60))', period_diff(pow(2,62), pow(2,60)));

--SEC_TO_TIME
    -- 非指定类型的参数
insert into func_test(functionName, result) values ('sec_to_time(''1000'')', sec_to_time('1000'));
insert into func_test(functionName, result) values ('sec_to_time(''1000.5'')', sec_to_time('1000.5'));
insert into func_test(functionName, result) values ('sec_to_time(''-5231'')', sec_to_time('-5231'));
insert into func_test(functionName, result) values ('sec_to_time(1000.5)', sec_to_time(1000.5));
insert into func_test(functionName, result) values ('sec_to_time(''abcd'')', sec_to_time('abcd'));
insert into func_test(functionName, result) values ('sec_to_time(true)', sec_to_time(true));
insert into func_test(functionName, result) values ('sec_to_time(B''1111'')', sec_to_time(B'1111'));
insert into func_test(functionName, result) values ('sec_to_time(bytea''1111'')', sec_to_time(bytea'1111'));   
    -- 
insert into func_test(functionName, result) values ('sec_to_time(NULL)', sec_to_time(NULL));   
insert into func_test(functionName, result) values ('sec_to_time(pow(10,18))', sec_to_time(pow(10,18))); -- second ≥ 10 ^18
insert into func_test(functionName, result) values ('sec_to_time(pow(10,18)-1)', sec_to_time(pow(10,18)-1));
insert into func_test(functionName, result) values ('sec_to_time(1000)', sec_to_time(1000)); 
insert into func_test(functionName, result) values ('sec_to_time(0)', sec_to_time(0));
insert into func_test(functionName, result) values ('sec_to_time(-1000000)', sec_to_time(-1000000));
insert into func_test(functionName, result) values ('sec_to_time(''2022-12-31 14:25:30'')', sec_to_time('2022-12-31 14:25:30'));
insert into func_test(functionName, result) values ('sec_to_time(''2012-2-25'')', sec_to_time('2012-2-25'));
insert into func_test(functionName, result) values ('sec_to_time(''14:25:56'')', sec_to_time('14:25:56'));
    -- 小数精度
insert into func_test(functionName, result) values ('sec_to_time(1000000.499)', sec_to_time(1000000.499));
insert into func_test(functionName, result) values ('sec_to_time(1000000.4990000)', sec_to_time(1000000.4990000));
    -- 纳秒位进位
insert into func_test(functionName, result) values ('sec_to_time(1000000.4999994999)', sec_to_time(1000000.4999994999));
insert into func_test(functionName, result) values ('sec_to_time(1000000.499999500)', sec_to_time(1000000.499999500));
insert into func_test(functionName, result) values ('sec_to_time(1000000.499998500)', sec_to_time(1000000.499998500));
insert into func_test(functionName, result) values ('sec_to_time(0.999999500)', sec_to_time(0.999999500));
insert into func_test(functionName, result) values ('sec_to_time(59.999999600)', sec_to_time(59.999999600));
insert into func_test(functionName, result) values ('sec_to_time(3599.999999700)', sec_to_time(3599.999999700));
    -- 返回值越界TIME类型
insert into func_test(functionName, result) values ('sec_to_time(3020399)', sec_to_time(3020399)); -- 未超出边界
insert into func_test(functionName, result) values ('sec_to_time(3020399 + 1)', sec_to_time(3020399 + 1)); -- 超出边界
insert into func_test(functionName, result) values ('sec_to_time(3020399.000000500)', sec_to_time(3020399.000000500)); -- 进位导致超出边界
insert into func_test(functionName, result) values ('sec_to_time(-100000000)', sec_to_time(-100000000));
insert into func_test(functionName, result) values ('sec_to_time(-999999999)', sec_to_time(-999999999));
insert into func_test(functionName, result) values ('sec_to_time(10000000000000000000)', sec_to_time(10000000000000000000));
insert into func_test(functionName, result) values ('sec_to_time(-10000000000000000000)', sec_to_time(-10000000000000000000));
insert into func_test(functionName, result) values ('sec_to_time(-9223372036854775808)', sec_to_time(-9223372036854775808));

-- subdate
    -- 非指定类型的参数
insert into func_test(functionName, result) values ('subdate(''2022-1-1'', 20)',subdate('2022-1-1', '20'));
insert into func_test(functionName, result) values ('subdate(''2022-1-1'', ''20.5'')',subdate('2022-1-1', '20.5'));
insert into func_test(functionName, result) values ('subdate(''2022-1-1'', 20.5)',subdate('2022-1-1', 20.5));
insert into func_test(functionName, result) values ('subdate(''abcd'', ''abcd'')',subdate('abcd', 'abcd'));
insert into func_test(functionName, result) values ('subdate(''2022-1-1'', true)',subdate('2022-1-1', true));
insert into func_test(functionName, result) values ('subdate(''2022-1-1'', B''1111'')',subdate('2022-1-1', B'1111'));
insert into func_test(functionName, result) values ('subdate(''2022-1-1'', bytea''1'')',subdate('2022-1-1', bytea'1'));
insert into func_test(functionName, result) values ('subdate(date''2022-1-1'', 1)',subdate(date'2022-1-1', 1));
insert into func_test(functionName, result) values ('subdate(cast(''2022-1-1 1:1:1'' as datetime), 1)',subdate(cast('2022-1-1 1:1:1' as datetime), 1));
    -- 特殊/非法参数测试
insert into func_test(functionName, result) values ('subdate(NULL, 20)',subdate(NULL, 20));
insert into func_test(functionName, result) values ('subdate(''2022-1-1'', NULL)',subdate('2022-1-1', NULL));
insert into func_test(functionName, result) values ('subdate(''10000-1-1'', INTERVAL ''6'' SECOND)', subdate('10000-1-1', INTERVAL '6' SECOND));
insert into func_test(functionName, result) values ('subdate(''-0001-1-1'', INTERVAL ''6'' SECOND)', subdate('-0001-1-1', INTERVAL '6' SECOND)); -- date < 0
insert into func_test(functionName, result) values ('subdate(''2022-11-30 25:00:00'', 20)',subdate('2022-11-30 25:00:00', 20)); -- 小时数超出24
insert into func_test(functionName, result) values ('subdate(''2022-11-30 00:60:00", 20)',subdate('2022-11-30 00:60:00', 20)); -- 分钟数超出59
insert into func_test(functionName, result) values ('subdate(''2022-11-30 00:00:60'', 20)',subdate('2022-11-30 00:00:60', 20)); -- 秒数数超出59
insert into func_test(functionName, result) values ('subdate(''2022-2-29'', INTERVAL ''6'' SECOND)',subdate('2022-2-29', INTERVAL '6' SECOND)); -- 不存在的日期，非闰年2-29日
insert into func_test(functionName, result) values ('subdate(''2022-4-31'', 20)',subdate('2022-4-31', 20)); -- 不存在的日期，4月31日
insert into func_test(functionName, result) values ('subdate("2022-0-1 6:05:05", 20)',subdate('2022-0-1 6:05:05', 20)); 
insert into func_test(functionName, result) values ('subdate("2022-1-0 6:05:05", INTERVAL ''6'' SECOND)',subdate('2022-1-0 6:05:05', INTERVAL '6' SECOND));-- 天数为0
insert into func_test(functionName, result) values ('subdate(''2022-1-1 6:05:05'', INTERVAL ''10 20:20'' DAY TO SECOND)',subdate('2022-1-1 6:05:05', INTERVAL '10 20:20' DAY TO SECOND));
    -- 不同格式的date/datetime输入
insert into func_test(functionName, result) values ('subdate(20220101010101.555, 1)',subdate(20220101010101.555, 1));
insert into func_test(functionName, result) values ('subdate(20220101, 1)',subdate(20220101, 1));
    -- 时分秒相关INTREVAL
insert into func_test(functionName, result) values ('subdate(''2022-1-1 6:05:05'', INTERVAL ''6'' SECOND)',subdate('2022-1-1 6:05:05', INTERVAL '-6' SECOND)); -- 负interval
insert into func_test(functionName, result) values ('subdate(''2022-6-1 6:05:05.5555555'', INTERVAL 6.444444 SECOND)',subdate('2022-6-1 6:05:05.5555555', INTERVAL 6.444444 SECOND)); -- 正interval 且 datetime微妙位>interval微妙位
insert into func_test(functionName, result) values ('subdate(''2022-6-1 6:05:05.444444'', INTERVAL ''6.555555'' HOUR TO SECOND)',subdate('2022-6-1 6:05:05.444444', INTERVAL '1:1:6.555555' HOUR TO SECOND)); -- datetime微妙位>interval微妙位
insert into func_test(functionName, result) values ('subdate(''0001-1-1 00:00:01.555555'', INTERVAL ''1.555556'' SECOND)',subdate('0001-1-1 00:00:01.555555', INTERVAL '1.555556' SECOND)); -- 返回结果年份=0
insert into func_test(functionName, result) values ('subdate(''0001-1-1 00:00:01'', INTERVAL 1 SECOND)', subdate('0001-1-1 00:00:01', INTERVAL 1 SECOND)); -- 返回结果年份>0
insert into func_test(functionName, result) values ('subdate(''2020-2-28 23:59:59'', INTERVAL ''-1:1'' MINUTE TO SECOND)',subdate('2020-2-28 23:59:59', INTERVAL '-1:1' MINUTE TO SECOND)); -- 闰年计算
insert into func_test(functionName, result) values ('subdate(''9999-12-1 22:01:01.111111'', INTERVAL ''-30 1:58:58.888889'' DAY TO SECOND)',subdate('9999-12-1 22:01:01.111111', INTERVAL '-30 1:58:58.888889' DAY TO SECOND)); -- 越界9999-12-31
insert into func_test(functionName, result) values ('subdate(''0001-1-1 22:01:01'', INTERVAL ''366 22:01:02'' DAY TO SECOND)',subdate('0001-1-1 22:01:01', INTERVAL '366 22:01:02' DAY TO SECOND)); -- 越界0
    -- 天数相关INTREVAL
insert into func_test(functionName, result) values ('subdate(''2020-2-29'', INTERVAL ''366'' DAY)',subdate('2020-2-29', INTERVAL '366' DAY)); -- 正interval，闰年计算
insert into func_test(functionName, result) values ('subdate(''2020-2-29'', INTERVAL ''-366'' DAY)',subdate('2020-2-29', INTERVAL '-366' DAY));
insert into func_test(functionName, result) values ('subdate(''0001-1-1 1:1:1'', INTERVAL ''1'' DAY)',subdate('0001-1-1 1:1:1', INTERVAL '1' DAY)); -- 返回结果的年份 == 0
insert into func_test(functionName, result) values ('subdate(''0001-1-2 1:1:1'', INTERVAL ''1'' DAY)',subdate('0001-1-2 1:1:1', INTERVAL '1' DAY)); -- 返回结果的年份 > 0
insert into func_test(functionName, result) values ('subdate(''9999-12-1'', -31)',subdate('9999-12-1', -31)); -- 越界9999-12-31
insert into func_test(functionName, result) values ('subdate(''0001-1-1'', 367)',subdate('0001-1-1', 367)); -- 越界0
    -- 月份相关INTREVAL
insert into func_test(functionName, result) values ('subdate(''2020-2-29'', INTERVAL ''-365'' MONTH)',subdate('2020-2-29', INTERVAL '-365' MONTH)); -- 负interval, 闰年计算
insert into func_test(functionName, result) values ('subdate(''0001-1-31'', INTERVAL ''1'' MONTH)',subdate('0001-1-31', INTERVAL '1' MONTH)); -- 正interval 返回结果的年份 == 0
insert into func_test(functionName, result) values ('subdate(''0001-2-1'', INTERVAL ''1'' MONTH)',subdate('0001-2-1', INTERVAL '1' MONTH)); -- 返回结果的年份 > 0
insert into func_test(functionName, result) values ('subdate(''9999-12-1'', INTERVAL ''-1'' MONTH)',subdate('9999-12-1', INTERVAL '-1' MONTH)); -- 越界9999-12-31
insert into func_test(functionName, result) values ('subdate(''0001-1-31'', INTERVAL ''13'' MONTH)',subdate('0001-1-31', INTERVAL '13' MONTH)); -- 越界0
    -- YEAR相关INTREVAL
insert into func_test(functionName, result) values ('subdate(''2020-2-29'', INTERVAL 3 YEAR)',subdate('2020-2-29', INTERVAL '3' YEAR)); -- 正interval，闰年到非闰年计算
insert into func_test(functionName, result) values ('subdate(''2020-2-29'', INTERVAL -3 YEAR)',subdate('2020-2-29', INTERVAL '-3' YEAR)); -- 负interval
insert into func_test(functionName, result) values ('subdate(''0001-12-31'', INTERVAL ''1'' YEAR)',subdate('0001-12-31', INTERVAL '1' YEAR));  -- 返回结果的年份 == 0
insert into func_test(functionName, result) values ('subdate(''9999-1-1'', INTERVAL ''-1'' YEAR)',subdate('9999-1-1', INTERVAL '-1' YEAR)); -- 越界9999-12-31
insert into func_test(functionName, result) values ('subdate(''2020-1-1'', INTERVAL ''2021'' YEAR)',subdate('2020-1-1', INTERVAL '2021' YEAR));
-- subdate(time: Time, interval/days)
insert into func_test(functionName, result) values ('subdate(time''22:11:11'', INTERVAL ''1'' YEAR)',subdate(time'22:11:11', INTERVAL '1' YEAR)); -- interval 包含 year
insert into func_test(functionName, result) values ('subdate(time''22:11:11'', INTERVAL ''-1'' MONTH)',subdate(time'22:11:11', INTERVAL '-1' MONTH)); -- interval 包含 month
insert into func_test(functionName, result) values ('subdate(time''100:59:59'', INTERVAL ''1 20:50:50.888888'' DAY To SECOND)',subdate(time'100:59:59', INTERVAL '1 20:50:50.888888' DAY TO SECOND)); -- 正interval 不包含 year与month
insert into func_test(functionName, result) values ('subdate(time''100:59:59'', INTERVAL ''-1 20:50:50.888888'' HOUR To SECOND)',subdate(time'100:59:59', INTERVAL '-20:50:50.888888' HOUR TO SECOND)); -- 负interval
insert into func_test(functionName, result) values ('subdate(time(6)''100:59:59.555'', 2',subdate(time(6)'100:59:59.555', 2)); -- time包含小数
insert into func_test(functionName, result) values ('subdate(time''815:0:0'', -1)',subdate(time'815:0:0', -1)); -- 越界838:59:59
insert into func_test(functionName, result) values ('subdate(time''-838:59:59'', INTERVAL ''1'' SECOND)',subdate(time'-838:59:59', INTERVAL '1' SECOND)); -- 越界-838:59:59
-- 浮点数格式的interval值
insert into func_test(functionName, result) values('subdate(''2022-01-01'', interval 21.9 year)', subdate('2022-01-01', interval 21.9 year));
insert into func_test(functionName, result) values('subdate(''2022-01-01 01:01:01'', interval 1.5 month)', subdate('2022-01-01 01:01:01', interval 1.5 month));
insert into func_test(functionName, result) values('subdate(time''838:59:59'', interval 1.999 hour)', subdate(time'838:59:59', interval 1.999 hour));
insert into func_test(functionName, result) values('subdate(''2022-01-01 01:01:01'', interval 1.999 minute)', subdate('2022-01-01 01:01:01', interval 1.999 minute));
insert into func_test(functionName, result) values('subdate(''2022-01-01 01:01:01'', interval 1.999 second)', subdate('2022-01-01 01:01:01', interval 1.999 second));
-- insert test
create table insert_subdate(date_col date, datetime_col datetime);
insert into insert_subdate(date_col, datetime_col) values (subdate('2021-1-1', 1), subdate('2021-1-1 01:01:01', 1));
drop table insert_subdate;

select * from func_test;

-- hour函数测试
select hour('-838:59:59');
select hour('838:59:59');
select hour('-840:59:59');
select hour('840:59:59');
select hour('');
select hour('abc');
select hour('1234abc');

set dolphin.sql_mode = '';

select hour('-838:59:59');
select hour('838:59:59');
select hour('-840:59:59');
select hour('840:59:59');
select hour('');
select hour('abc');
select hour('1234abc');

-- date_bool&time_bool
select date_bool('0000-00-00');
select time_bool('00:00:00');
select date_bool('0000-00-01');
select time_bool('00:00:01');
select date_bool('0000-01-00');
select time_bool('00:01:00');
select date_bool('0000-01-01');
select time_bool('00:01:01');
select date_bool('0001-00-00');
select time_bool('01:00:00');
select date_bool('0001-00-01');
select time_bool('01:00:01');
select date_bool('0001-01-00');
select time_bool('01:01:00');
select date_bool('2020-12-31');
select date_bool('2020-12-31 BC');
select time_bool('838:59:59');
select time_bool('-838:59:59');

reset dolphin.sql_mode;

drop schema b_time_funcs cascade;
reset current_schema;
