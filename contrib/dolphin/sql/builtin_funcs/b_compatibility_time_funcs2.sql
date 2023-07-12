--
-- Test Time functions(Stage 2) under 'b' compatibility
-- Contains subtime()、timediff()、time()、time_format()、timestamp()、timestampadd()
--
create schema b_time_funcs2;
set current_schema to 'b_time_funcs2';
set datestyle to 'ISO,YMD';

create table func_test2(functionName varchar(256),result varchar(256));
truncate table func_test2;

-- SUBTIME()
-- SUBTIME ( datetime, time )
    -- 正常测试
insert into func_test2(functionName, result) values('subtime(''2000-02-28 20:59:59'', ''-25:00'')', subtime('2000-02-28 20:59:59', '-25:00') );
insert into func_test2(functionName, result) values('subtime(''2000-02-28 20:59:59'', ''25:00'')', subtime('2000-02-28 20:59:59', '25:00') );
insert into func_test2(functionName, result) values('SUBTIME(''2007-12-31 23:59:59.999999'',''1 1:1:1.000002'')', SUBTIME('2007-12-31 23:59:59.999999','1 1:1:1.000002'));
insert into func_test2(functionName, result) values('SUBTIME(''2007-12-31 23:59:59.999999'',''100 1:1:1.000002'')',SUBTIME('2007-12-31 23:59:59.999999','100 1:1:1.000002'));
insert into func_test2(functionName, result) values('SUBTIME(''2007-12-31 0:0:0.999999'',''1 1:1:1.000002'')', SUBTIME('2007-12-31 0:0:0.999999','1 1:1:1.000002'));
insert into func_test2(functionName, result) values('SUBTIME(''2007-12-31 23:59:59.999999'',''1 1:1:1.000002'')', SUBTIME('2007-12-31 23:59:59.999999','1 1:1:1.000002'));
insert into func_test2(functionName, result) values('SUBTIME(''2007-13-31 23:59:59.999999'',''1 1:1:1.000002'')', SUBTIME('2007-13-31 23:59:59.999999','1 1:1:1.000002'));
insert into func_test2(functionName, result) values('SUBTIME(''2007-12-40 23:59:59.999999'',''1 1:1:1.000002'')', SUBTIME('2007-12-40 23:59:59.999999','1 1:1:1.000002'));
insert into func_test2(functionName, result) values('SUBTIME(''2017-2-29 12:00:00'',''12:00:00'')', SUBTIME('2017-2-29 12:00:00','12:00:00'));
    -- 数值格式
insert into func_test2(functionName, result) values('subtime(''20000228205959'', ''250000'')', subtime('20000228205959', '250000') );
insert into func_test2(functionName, result) values('subtime(''20000228205959.000002'', ''250000.000001'')', subtime('20000228205959.000002', '250000.000001') );
insert into func_test2(functionName, result) values('subtime(20000228205959, 250000)', subtime(20000228205959, 250000) );
insert into func_test2(functionName, result) values('subtime(20000228205959.000002, 250000.000001)', subtime(20000228205959.000002, 250000.000001) );
    -- 特异参数测试
insert into func_test2(functionName, result) values('subtime(''2000-02-28 20:59:59'', NULL)', subtime('2000-02-28 20:59:59', NULL) );
insert into func_test2(functionName, result) values('subtime(NULL, ''-25:00'')', subtime(NULL, '-25:00') );
insert into func_test2(functionName, result) values('subtime(''abcd'', ''-25:00'')', subtime('abcd', '-25:00') );
insert into func_test2(functionName, result) values('subtime(''2000-02-28 20:59:59'', ''abcd'')', subtime('2000-02-28 20:59:59', 'abcd') );
    -- 边界测试
insert into func_test2(functionName, result) values('subtime(''0001-01-1 20:59:59'', ''21:00'');', subtime('0001-01-1 20:59:59', '21:00')   );
insert into func_test2(functionName, result) values('subtime(''9999-12-31 20:59:59'', ''-3:00:1'')', subtime('9999-12-31 20:59:59', '-3:00:1') );
insert into func_test2(functionName, result) values('subtime(''10000-1-1 20:59:59'', ''30:00:1'')', subtime('10000-1-1 20:59:59', '30:00:1') );
insert into func_test2(functionName, result) values('subtime(''9999-12-31 20:59:59'', ''839:59:59'')', subtime('9999-12-31 20:59:59', '839:59:59') );

-- SUBTIME ( time, time )
    -- 正常测试
insert into func_test2(functionName, result) values('subtime(''-37:59:59'', ''-39:59:59'')', subtime('-37:59:59', '-39:59:59') );
insert into func_test2(functionName, result) values('subtime(''-37:59:59'', ''39:59:59'')', subtime('-37:59:59', '39:59:59') );
insert into func_test2(functionName, result) values('subtime(''37:59:59'', ''-39:59:59'')', subtime('37:59:59', '-39:59:59') );
insert into func_test2(functionName, result) values('subtime(''375959'', ''395959'')', subtime('375959', '395959') );
insert into func_test2(functionName, result) values('SUBTIME(''010000.999999'', ''020000.999998'')', SUBTIME('010000.999999', '020000.999998'));
insert into func_test2(functionName, result) values('SUBTIME(''24:00:00.999999'', ''02:00:00.999998'')', SUBTIME('24:00:00.999999', '02:00:00.999998'));
insert into func_test2(functionName, result) values('SUBTIME(''01:00:00.000000'', ''02:00:00.999999'')', SUBTIME('01:00:00.000000', '02:00:00.999999'));
insert into func_test2(functionName, result) values('SUBTIME(''02:00:61.000000'', ''02:00:00.999999'')', SUBTIME('02:00:61.000000', '02:00:00.999999'));
insert into func_test2(functionName, result) values('SUBTIME(''02:61:00.000000'', ''02:00:00.999999'')', SUBTIME('02:61:00.000000', '02:00:00.999999'));
insert into func_test2(functionName, result) values('SUBTIME(''60:00:00.000000'', ''02:00:00.999999'')', SUBTIME('60:00:00.000000', '02:00:00.999999'));
    -- 数值格式
insert into func_test2(functionName, result) values('subtime(''-375959'', ''-395959'')', subtime('-375959', '395959') );
insert into func_test2(functionName, result) values('subtime(''-375959.000001'', ''-395959.000002'')', subtime('-375959.000001', '395959.000002') );
insert into func_test2(functionName, result) values('subtime(-375959, 395959)', subtime(-375959, 395959) );
insert into func_test2(functionName, result) values('subtime(-375959.000001, 395959.000002)', subtime(-375959.000001, 395959.000002) ); 
    -- 特异参数测试
insert into func_test2(functionName, result) values('subtime(NULL, ''-839:59:59'')', subtime(NULL, '-839:59:59') );
insert into func_test2(functionName, result) values('subtime(''-837:59:59'', NULL)', subtime('-837:59:59', NULL) );
insert into func_test2(functionName, result) values('subtime(''abcd'', ''-839:59:59'')', subtime('abcd', '-839:59:59') );
insert into func_test2(functionName, result) values('subtime(''-837:59:59'', ''abcd'')', subtime('-837:59:59', 'abcd') );
    -- 边界测试
insert into func_test2(functionName, result) values('subtime(''838:59:59'', ''-25:00'')', subtime('838:59:59', '-25:00') );
insert into func_test2(functionName, result) values('subtime(''-838:59:59'', ''25:00'')', subtime('-838:59:59', '25:00') );
insert into func_test2(functionName, result) values('subtime(''839:59:59'', ''837:59:59'')', subtime('839:59:59', '837:59:59') );
insert into func_test2(functionName, result) values('subtime(''-837:59:59'', ''-839:59:59'')', subtime('-837:59:59', '-839:59:59') );
-- date格式字符串
insert into func_test2(functionName, result) values('subtime(''2000-01-01'', ''2022-01-01'')', subtime('2000-01-01', '2022-01-01') ); 
-- 非字符串类型参数用例
insert into func_test2(functionName, result) values('subtime(false, true)', subtime(false, true) );
insert into func_test2(functionName, result) values('subtime(B''0'', B''1'')', subtime(B'0', B'1'));
insert into func_test2(functionName, result) values('SUBTIME(cast(''2000-02-28 20:59:59'' as datetime), time''-25:00'')', SUBTIME(cast('2000-02-28 20:59:59' as datetime), time'-25:00') );
insert into func_test2(functionName, result) values('SUBTIME(date''2000-02-28'', time''-25:00'')', SUBTIME(date'2000-02-28', time'-25:00') );
insert into func_test2(functionName, result) values('SUBTIME(time''20:59:59'', time''-25:00'')', SUBTIME(time'20:59:59', time'-25:00') );
insert into func_test2(functionName, result) values('SUBTIME(cast(''2000-02-28 20:59:59'' as datetime), date''2000-01-01'')', SUBTIME(cast('2000-02-28 20:59:59' as datetime), date'2000-01-01') );
insert into func_test2(functionName, result) values('SUBTIME(cast(''2000-02-28 20:59:59'' as datetime), cast(''2000-01-01 11:00:00'' as datetime))', SUBTIME(cast('2000-02-28 20:59:59' as datetime), cast('2000-01-01 11:00:00' as datetime)));

-- TIMEDIFF
-- TIMEDIFF ( datetime, datetime )
	-- 正常测试
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-02-28 20:59:59'', ''2001-02-28 23:00'')', TIMEDIFF('2000-02-28 20:59:59', '2001-02-28 23:00') );
insert into func_test2(functionName, result) values('TIMEDIFF(''2007-12-31 23:59:59.999999'',''2007-12-01 1:1:1.000002'')', TIMEDIFF('2007-12-31 23:59:59.999999','2007-12-01 1:1:1.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2007-12-31 23:59:59.999999'',''2007-12-01 1:1:1.000002'')',TIMEDIFF('2007-12-31 23:59:59.999999','2007-12-01 1:1:1.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2007-12-31 0:0:0.999999'',''2007-12-30 1:1:1.000002'')', TIMEDIFF('2007-12-31 0:0:0.999999','2007-12-30 1:1:1.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-01-01 00:00:00'',''2000-01-01 00:00:00.000001'')', TIMEDIFF('2000-01-01 00:00:00','2000-01-01 00:00:00.000001'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-01-01 00:00:00'',''2000-01-02 00:00:00.000001'')', TIMEDIFF('2000-01-01 00:00:00','2000-01-02 00:00:00.000001'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-01-01 00:00:00'',''2000-02-01 00:00:00.000001'')', TIMEDIFF('2000-01-01 00:00:00','2000-02-01 00:00:00.000001'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-01-01 00:00:01'',''2000-01-01 00:00:00.999999'')', TIMEDIFF('2000-01-01 00:00:01','2000-01-01 00:00:00.999999'));
insert into func_test2(functionName, result) values('TIMEDIFF(''01-01-01 00:00:00'',''01-01-01 00:00:00.000001'')', TIMEDIFF('01-01-01 00:00:00','01-01-01 00:00:00.000001'));
insert into func_test2(functionName, result) values('TIMEDIFF(''0-01-01 00:00:00'',''2000-01-01 00:00:00.000001'')', TIMEDIFF('0-01-01 00:00:00','2000-01-01 00:00:00.000001'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2008-12-31 23:59:59.000001'',''2008-12-30 01:01:01.000002'')', TIMEDIFF('2008-12-31 23:59:59.000001','2008-12-30 01:01:01.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2008-12-31 24:59:59.000001'',''2008-12-30 01:01:01.000002'')', TIMEDIFF('2008-12-31 24:59:59.000001','2008-12-30 01:01:01.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2008-12-31 23:59:59.0000015'',''2008-12-30 01:01:01.999999'')', TIMEDIFF('2008-12-31 23:59:59.0000015','2008-12-30 01:01:01.999999'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2008-12-31 23:59:59.0000014'',''2008-12-30 01:01:01.000002'')', TIMEDIFF('2008-12-31 23:59:59.0000014','2008-12-30 01:01:01.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2008-12-31 23:60:59.000001'',''2008-12-30 01:01:01.000002'')', TIMEDIFF('2008-12-31 23:60:59.000001','2008-12-30 01:01:01.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2008-12-31 23:59:59.000001'',''2008-12-30 01:60:01.000002'')', TIMEDIFF('2008-12-31 23:59:59.000001','2008-12-30 01:60:01.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2008-12-31 23:59:59.000001'',''2008-12-30 24:01:01.000002'')', TIMEDIFF('2008-12-31 23:59:59.000001','2008-12-30 24:01:01.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2008-12-31 23:59:59.000001'',''2008-12-30 01:01:01.000002'')', TIMEDIFF('2008-12-31 23:59:59.000001','2008-12-30 01:01:01.000002'));
	-- 数值格式
insert into func_test2(functionName, result) values('TIMEDIFF(''100000101151515'', ''100000101151514'')', TIMEDIFF('100000101151515', '100000101151514'));
insert into func_test2(functionName, result) values('TIMEDIFF(20000101010101, 20000101010101.123456)', TIMEDIFF(20000101010101, 20000101010101.123456));
insert into func_test2(functionName, result) values('TIMEDIFF(''20000101010101'', ''20000101010101.123456'')', TIMEDIFF('20000101010101', '20000101010101.123456'));
    -- 特异参数测试
insert into func_test2(functionName, result) values('TIMEDIFF(''2007-12-40 23:59:59.999999'',''2000-02-28 20:59:59'')', TIMEDIFF('2007-12-40 23:59:59.999999','2000-02-28 20:59:59'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2007-13-31 23:59:59.999999'',''2000-02-28 20:59:59'')', TIMEDIFF('2007-13-31 23:59:59.999999','2000-02-28 20:59:59'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-02-28 20:59:59'', NULL)', TIMEDIFF('2000-02-28 20:59:59', NULL) );
insert into func_test2(functionName, result) values('TIMEDIFF(NULL, ''2000-02-28 20:59:59'')', TIMEDIFF(NULL, '2000-02-28 20:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(''abcd'', ''-2000-02-28 20:59:59'')', TIMEDIFF('abcd', '-2000-02-28 20:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-02-28 20:59:59'', ''abcd'')', TIMEDIFF('2000-02-28 20:59:59', 'abcd') );
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-13-01 00:00:00'',''2000-01-01 00:00:00.000001'')', TIMEDIFF('2000-13-01 00:00:00','2000-01-01 00:00:00.000001'));
	-- 边界测试
insert into func_test2(functionName, result) values('TIMEDIFF(''9999-12-31 23:59:59.9999999'',''9999-12-31 23:59:59.9'')', TIMEDIFF('9999-12-31 23:59:59.9999999','9999-12-31 23:59:59.9'));
insert into func_test2(functionName, result) values('TIMEDIFF(''1-01-1 20:59:59'', ''2001-01-01 21:00'');', TIMEDIFF('1-01-1 20:59:59', '2001-01-01 21:00')   );
insert into func_test2(functionName, result) values('TIMEDIFF(''10000-1-1 20:59:59'', ''2001-01-01 21:00'')', TIMEDIFF('10000-1-1 20:59:59', '2001-01-01 21:00') );
insert into func_test2(functionName, result) values('TIMEDIFF(''1000-1-1 20:59:59'', ''1000-01-01 1:00'')', TIMEDIFF('1000-1-1 20:59:59', '1000-01-01 1:00') );
insert into func_test2(functionName, result) values('TIMEDIFF(''294277-1-9 4:00:54.775807'',''294277-1-9 4:00:54.775806'')', TIMEDIFF('294277-1-9 4:00:54.775807','294277-1-9 4:00:54.775806') );

-- TIMEDIFF ( time, time )
	-- 正常测试
insert into func_test2(functionName, result) values('TIMEDIFF(''-37:59:59'', ''-39:59:59'')', TIMEDIFF('-37:59:59', '-39:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(''-37:59:59'', ''39:59:59'')', TIMEDIFF('-37:59:59', '39:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(''37:59:59'', ''-39:59:59'')', TIMEDIFF('37:59:59', '-39:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(''37:59:59'', ''39:59:59'')', TIMEDIFF('37:59:59', '39:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(''23:59:59'', ''01:01:01'')', TIMEDIFF('23:59:59', '01:01:01'));
insert into func_test2(functionName, result) values('TIMEDIFF(''235959'', ''010101'')', TIMEDIFF('235959', '010101'));
insert into func_test2(functionName, result) values('TIMEDIFF(235959, 010101)', TIMEDIFF(235959, 010101));
insert into func_test2(functionName, result) values('TIMEDIFF(''01:00:00.999999'', ''02:00:00.999998'')', TIMEDIFF('01:00:00.999999', '02:00:00.999998'));
insert into func_test2(functionName, result) values('TIMEDIFF(''24:00:00.999999'', ''02:00:00.999998'')', TIMEDIFF('24:00:00.999999', '02:00:00.999998'));
insert into func_test2(functionName, result) values('TIMEDIFF(''01:00:00.000000'', ''02:00:00.999999'')', TIMEDIFF('01:00:00.000000', '02:00:00.999999'));
insert into func_test2(functionName, result) values('TIMEDIFF(''02:00:61.000000'', ''02:00:00.999999'')', TIMEDIFF('02:00:61.000000', '02:00:00.999999'));
insert into func_test2(functionName, result) values('TIMEDIFF(''02:61:00.000000'', ''02:00:00.999999'')', TIMEDIFF('02:61:00.000000', '02:00:00.999999'));
insert into func_test2(functionName, result) values('TIMEDIFF(''60:00:00.000000'', ''02:00:00.999999'')', TIMEDIFF('60:00:00.000000', '02:00:00.999999'));
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-01-01 00:00:00'',''2000-01-01 00:00:00.000001'')', TIMEDIFF('2000-01-01 00:00:00','2000-01-01 00:00:00.000001'));
insert into func_test2(functionName, result) values('TIMEDIFF(''00:00:00'',''00:00:00.000001'')', TIMEDIFF('00:00:00','00:00:00.000001'));
insert into func_test2(functionName, result) values('TIMEDIFF(''02:59:00.0000001578'', ''02:00:00.999999'')', TIMEDIFF('02:59:00.0000001578', '02:00:00.999999'));
	-- 数值格式
insert into func_test2(functionName, result) values('TIMEDIFF(''-375959'', ''-395959'')', TIMEDIFF('-375959', '395959') );
insert into func_test2(functionName, result) values('TIMEDIFF(''-375959.000002'', ''-395959.000001'')', TIMEDIFF('-375959.000002', '395959.000001') );
insert into func_test2(functionName, result) values('TIMEDIFF(-375959, 395959)', TIMEDIFF(-375959, 395959) );  
insert into func_test2(functionName, result) values('TIMEDIFF(-375959.000002, 395959.000001)', TIMEDIFF(-375959.000002, 395959.000001) );
    -- 特异参数测试
insert into func_test2(functionName, result) values('TIMEDIFF(NULL, ''-839:59:59'')', TIMEDIFF(NULL, '-839:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(''-837:59:59'', NULL)', TIMEDIFF('-837:59:59', NULL) );
insert into func_test2(functionName, result) values('TIMEDIFF(''abcd'', ''-839:59:59'')', TIMEDIFF('abcd', '-839:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(''-837:59:59'', ''abcd'')', TIMEDIFF('-837:59:59', 'abcd') );
insert into func_test2(functionName, result) values('TIMEDIFF(false, true)', TIMEDIFF(false, true) );
insert into func_test2(functionName, result) values('TIMEDIFF(B''101'', B''101'')', TIMEDIFF(B'101', B'101') );
	-- 边界测试
insert into func_test2(functionName, result) values('TIMEDIFF(''838:59:59'', ''-25:00'')', TIMEDIFF('838:59:59', '-25:00') );
insert into func_test2(functionName, result) values('TIMEDIFF(''-838:59:59'', ''25:00'')', TIMEDIFF('-838:59:59', '25:00') );
insert into func_test2(functionName, result) values('TIMEDIFF(''839:59:59'', ''837:59:59'')', TIMEDIFF('839:59:59', '837:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(''-837:59:59'', ''-839:59:59'')', TIMEDIFF('-837:59:59', '-839:59:59') );
-- TIMEDIFF ( time, datetime ), TIMEDIFF ( time, datetime )
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-02-28 20:59:59'', ''4:00'')', TIMEDIFF('2000-02-28 20:59:59', '4:00') );
insert into func_test2(functionName, result) values('TIMEDIFF(''20:59:59'', ''2000-02-28 23:00'')', TIMEDIFF('2000-02-28 20:59:59', '2000-02-28 23:00') );
-- date格式字符串
insert into func_test2(functionName, result) values('TIMEDIFF(''2000-01-01'', ''2022-01-01'')', TIMEDIFF('2000-01-01', '2022-01-01') ); 
-- 非字符串类型参数用例
insert into func_test2(functionName, result) values('TIMEDIFF(false, true)', TIMEDIFF(false, true) );
insert into func_test2(functionName, result) values('TIMEDIFF(B''101'', B''101'')', TIMEDIFF(B'101', B'101') );
insert into func_test2(functionName, result) values('TIMEDIFF(cast(''2000-02-28 20:59:59'' as datetime), cast(''2001-02-28 23:00'' as datetime))', TIMEDIFF(cast('2000-02-28 20:59:59' as datetime), cast('2001-02-28 23:00' as datetime)));
insert into func_test2(functionName, result) values('TIMEDIFF(time''23:59:59'',time''01:01:01'')', TIMEDIFF(time'23:59:59',time'01:01:01'));
insert into func_test2(functionName, result) values('TIMEDIFF(cast(235959 as time),cast(010101 as time))', TIMEDIFF(cast(235959 as time),cast(010101 as time)));
insert into func_test2(functionName, result) values('TIMEDIFF(235959::time, 010101::time)', TIMEDIFF(235959::time, 010101::time));
insert into func_test2(functionName, result) values('TIMEDIFF(cast(''2008-12-31 23:59:59.000001'' as datetime), time''22:01:01.000002'')', TIMEDIFF(cast('2008-12-31 23:59:59.000001' as datetime), time'22:01:01.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(cast(''2008-12-31 23:59:59.000001'' as datetime), date''2008-12-30'')', TIMEDIFF(cast('2008-12-31 23:59:59.000001' as datetime), date'2008-12-30'));
insert into func_test2(functionName, result) values('TIMEDIFF(time''23:59:59.000001'', cast(''2008-12-30 22:01:01.000002'' as datetime)))', TIMEDIFF(time'23:59:59.000001', cast('2008-12-30 22:01:01.000002' as datetime)));
insert into func_test2(functionName, result) values('TIMEDIFF(time''23:59:59.000001'', date''2008-12-30'')', TIMEDIFF(time'23:59:59.000001', date'2008-12-30'));
insert into func_test2(functionName, result) values('TIMEDIFF(time''-37:59:59'', time''-39:59:59'')', TIMEDIFF(time'-37:59:59', time'-39:59:59') );
insert into func_test2(functionName, result) values('TIMEDIFF(date''2008-12-31'', cast(''2008-12-30 01:01:01.000002'' as datetime)))', TIMEDIFF(date'2008-12-31', cast('2008-12-30 01:01:01.000002' as datetime)));
insert into func_test2(functionName, result) values('TIMEDIFF(date''2008-12-31'', date''2008-12-30'')', TIMEDIFF(date'2008-12-31', date'2008-12-30'));
insert into func_test2(functionName, result) values('TIMEDIFF(date''2008-12-31'', time''22:01:01.000002'')', TIMEDIFF(date'2008-12-31', time'22:01:01.000002'));
insert into func_test2(functionName, result) values('TIMEDIFF(cast(235956 as unsigned), cast(010101 as unsigned))', TIMEDIFF(cast(235956 as unsigned), cast(010101 as unsigned)));
insert into func_test2(functionName, result) values('TIMEDIFF(year''2021'', year''2022'')', TIMEDIFF(year'2021', year'2022'));

-- TIME (time格式)
insert into func_test2(functionName, result) values('TIME(''00:00:00'')',TIME('00:00:00'));
insert into func_test2(functionName, result) values('TIME(''240000'')',TIME('240000'));
insert into func_test2(functionName, result) values('TIME(240000)',TIME(240000));
insert into func_test2(functionName, result) values('TIME(240000.000001)',TIME(240000.000001));
insert into func_test2(functionName, result) values('TIME(''25:30:30'')',TIME('25:30:30'));
insert into func_test2(functionName, result) values('TIME(''-25:30:30'')',TIME('-25:30:30'));
insert into func_test2(functionName, result) values('TIME(''838:59:59'')',TIME('838:59:59'));
insert into func_test2(functionName, result) values('TIME(''-838:59:59'')',TIME('-838:59:59'));
-- 边界情况
insert into func_test2(functionName, result) values('TIME(''838:0:0'')',TIME('838:0:0'));
insert into func_test2(functionName, result) values('TIME(''839:0:0'')',TIME('839:0:0'));
insert into func_test2(functionName, result) values('TIME(''-839:0:0'')',TIME('-839:0:0'));
-- 异常格式
insert into func_test2(functionName, result) values('TIME(''0:-1:0'')',TIME('0:-1:0'));
insert into func_test2(functionName, result) values('TIME(''0:60:0'')',TIME('0:60:0'));
insert into func_test2(functionName, result) values('TIME(''0:59.5:0'')',TIME('0:59.5:0'));
insert into func_test2(functionName, result) values('TIME(''0:59.4:0'')',TIME('0:59.4:0'));
insert into func_test2(functionName, result) values('TIME(''0:0:-1'')',TIME('0:0:-1'));
insert into func_test2(functionName, result) values('TIME(''0:0:60'')',TIME('0:0:60'));
-- 进位
insert into func_test2(functionName, result) values('TIME(''00:00:59.9999'')',TIME('00:00:59.9999'));
insert into func_test2(functionName, result) values('TIME(''00:00:59.999999000'')',TIME('00:00:59.999999000'));

insert into func_test2(functionName, result) values('TIME(''83:59:59.0000000009'')',TIME('83:59:59.0000000009'));
insert into func_test2(functionName, result) values('TIME(''00:10:59.999999999'')',TIME('00:10:59.999999999'));
insert into func_test2(functionName, result) values('TIME(''00:59:59.999999999'')',TIME('00:59:59.999999999'));
insert into func_test2(functionName, result) values('TIME(''838:59:59.000000500'')',TIME('838:59:59.000000500'));
insert into func_test2(functionName, result) values('TIME(''83:59:59.0000000004'')',TIME('83:59:59.0000000004'));

-- TIME(datetime格式)
-- 正常测试
insert into func_test2(functionName, result) values('TIME(''2003-12-31 01:02:03'')', TIME('2003-12-31 01:02:03'));
insert into func_test2(functionName, result) values('TIME(''20031231010203'')', TIME('20031231010203'));
insert into func_test2(functionName, result) values('TIME(''2003-12-31 01:02:03.000123'')', TIME('2003-12-31 01:02:03.000123'));
insert into func_test2(functionName, result) values('TIME(''20031231010203.000123'')', TIME('20031231010203.000123'));
insert into func_test2(functionName, result) values('TIME(20031231010203)', TIME(20031231010203));
insert into func_test2(functionName, result) values('TIME(20031231010203.000123)', TIME(20031231010203.000123));
-- 进位
insert into func_test2(functionName, result) values('TIME(''2003-12-31 01:02:03.0001234'')',TIME('2003-12-31 01:02:03.0001234'));
insert into func_test2(functionName, result) values('TIME(''2003-12-31 01:02:03.0001235'')', TIME('2003-12-31 01:02:03.0001235'));
insert into func_test2(functionName, result) values('TIME(''2003-12-31 01:02:03.00012345'')', TIME('2003-12-31 01:02:03.00012345'));
insert into func_test2(functionName, result) values('TIME(''20031231010203.0001235'')', TIME('20031231010203.0001235'));
-- 边界情况
insert into func_test2(functionName, result) values('TIME(''9999-12-31 59:59:59'')', TIME('9999-12-31 59:59:59'));
insert into func_test2(functionName, result) values('TIME(''10000-01-01 00:00:00'')', TIME('10000-01-01 00:00:00'));
insert into func_test2(functionName, result) values('TIME(''0000-12-31 59:59:59'')', TIME('0000-12-31 59:59:59'));
-- 异常格式
insert into func_test2(functionName, result) values('TIME(''2003-12-40 01:02:03'')', TIME('2003-12-40 01:02:03'));
insert into func_test2(functionName, result) values('TIME(''2003-13-31 01:02:03.000123'')', TIME('2003-13-31 01:02:03'));

-- 非字符串类型
insert into func_test2(functionName, result) values('TIME(time''1:1:1'')', TIME(time'1:1:1'));
insert into func_test2(functionName, result) values('TIME(date''2000-01-01'')', TIME(date'2000-01-01'));
insert into func_test2(functionName, result) values('TIME(cast(''2001-12-10 23:59:59'' as datetime))', TIME(cast('2001-12-10 23:59:59' as datetime)));
insert into func_test2(functionName, result) values('TIME(false)', TIME(false));
insert into func_test2(functionName, result) values('TIME(B''1'')', TIME(B'1'));
insert into func_test2(functionName, result) values('TIME(date''5874897-12-31'')', TIME(date'5874897-12-31'));
insert into func_test2(functionName, result) values('TIME(date''10000-01-01'')', TIME(date'10000-01-01'));

-- TIME_FORMAT()
insert into func_test2(functionName, result) values(' TIME_FORMAT(''00:00:00'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('00:00:00', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''240000'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('240000', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(240000, ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT(240000, '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(240000.000001, ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT(240000.000001, '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''25:30:30'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('25:30:30', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''-25:30:30'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('-25:30:30', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''838:59:59'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('838:59:59', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''-838:59:59'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('-838:59:59', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values('TIME_FORMAT(''-838:59:59'',''%h'')', TIME_FORMAT('-838:59:59','%h'));
insert into func_test2(functionName, result) values('TIME_FORMAT(''-838:59:59'',''%h'')', TIME_FORMAT('-838:59:59','%h'));
insert into func_test2(functionName, result) values('TIME_FORMAT(''-838:59:59'',''%k'')', TIME_FORMAT('-838:59:59','%k'));
insert into func_test2(functionName, result) values('TIME_FORMAT(''-838:59:59'',''%r'')', TIME_FORMAT('-838:59:59','%r'));
insert into func_test2(functionName, result) values('TIME_FORMAT(''-838:59:59'',''%i'')', TIME_FORMAT('-838:59:59','%i'));
-- 边界情况
insert into func_test2(functionName, result) values(' TIME_FORMAT(''838:0:0'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('838:0:0', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''839:0:0'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('839:0:0', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''-839:0:0'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('-839:0:0', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
-- 分秒越界time格式
insert into func_test2(functionName, result) values(' TIME_FORMAT(''0:-1:0'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('0:-1:0', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''0:60:0'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('0:60:0', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''0:59.5:0'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('0:59.5:0', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''0:59.4:0'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('0:59.4:0', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''0:0:-1'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('0:0:-1', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''0:0:60'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('0:0:60', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
-- 进位
insert into func_test2(functionName, result) values(' TIME_FORMAT(''00:00:59.9999'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('00:00:59.9999', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''00:00:59.999999000'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('00:00:59.999999000', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));

insert into func_test2(functionName, result) values(' TIME_FORMAT(''83:59:59.0000000009'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('83:59:59.0000000009', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''00:10:59.999999999'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('00:10:59.999999999', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''00:59:59.999999999'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('00:59:59.999999999', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''838:59:59.000000500'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('838:59:59.000000500', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''83:59:59.0000000004'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('83:59:59.0000000004', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
-- 时分秒毫秒之外的fomrat格式 
insert into func_test2(functionName, result) values(' TIME_FORMAT(''100:59:59.0123'', ''%a|%b|%c|%D|%d|%e|%j|%M|%m'')    ', TIME_FORMAT('100:59:59.0123', '%a|%b|%D|%j|%M|%U|%u|%V|%v|%W|%w|%X|%x'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''100:59:59.0123'', ''%U|%u|%V|%v|%W|%w|%X|%x|%Y|%y'')    ', TIME_FORMAT('100:59:59.0123', '%c|%d|%e|%m|%Y|%y'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''100:59:59.0123'', ''%N|%n abcd'')    ', TIME_FORMAT('100:59:59.0123', '%N|%n abcd'));
-- TIME_FORMAT(非time格式)
insert into func_test2(functionName, result) values(' TIME_FORMAT(''2003-12-31 01:02:03.0123'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('2003-12-31 01:02:03.0123', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''20031231010203.0123'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('20031231010203.0123', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(20031231010203, ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT(20031231010203, '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(20031231010203.0123, ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT(20031231010203.0123, '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''10000-01-01 01:01:01'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('10000-01-01 01:01:01', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''0000-12-31 23:59:59'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('0000-12-31 23:59:59', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''2021-12-32 01:01:01'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('2021-12-32 01:01:01', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(''2021-12-31'', ''%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'')    ', TIME_FORMAT('2021-12-31', '%T|%r|%H|%h|%I|%i|%S|%f|%p|%k'));
-- 非字符串类型参数
insert into func_test2(functionName, result) values(' TIME_FORMAT(date''2021-12-31'', ''%T|%r||%f'')    ', TIME_FORMAT(date'2021-12-31', '%T|%r||%f'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(time''01:01:01'', ''%T|%r||%f'')    ', TIME_FORMAT(time'01:01:01', '%T|%r||%f'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(cast(''2001-12-10 23:59:59'' as datetime), ''%T|%r||%f'')    ', TIME_FORMAT(cast('2001-12-10 23:59:59' as datetime), '%T|%r||%f'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(B''1'', ''%T|%r||%f'')    ', TIME_FORMAT(B'1', '%T|%r||%f'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(false, ''%T|%r||%f'')    ', TIME_FORMAT(false, '%T|%r||%f'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(date''5874897-12-31'', ''%t|%r|%h|%h'')', TIME_FORMAT(date'5874897-12-31', '%t|%r|%h|%h'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(date''10000-1-1'', date''9999-12-31'')', TIME_FORMAT(date'10000-1-1', date'9999-12-31'));
insert into func_test2(functionName, result) values(' TIME_FORMAT(datetime''294277-1-9 4:00:54.775806'', ''%T|%r|%h|%h'')', TIME_FORMAT(datetime'294277-1-9 4:00:54.775806', '%T|%r|%h|%h'));

-- TIMESTAMP(expr)
-- 特异参数
insert into func_test2(functionName, result) values('TIMESTAMP(null)', TIMESTAMP(null)); 
insert into func_test2(functionName, result) values('TIMESTAMP(''asdasd'')', TIMESTAMP('asdasd'));
    -- 不存在的日期或时间
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-13-01'')', TIMESTAMP('2003-13-01'));
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-2-29'')', TIMESTAMP('2003-2-29'));
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31 24:00:00'')', TIMESTAMP('2003-12-31 24:00:00')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31 23:60:00'')', TIMESTAMP('2003-12-31 23:60:00')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31 23:59:60'')', TIMESTAMP('2003-12-31 23:59:60'));
-- 主体功能
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31'')', TIMESTAMP('2003-12-31'));
insert into func_test2(functionName, result) values('TIMESTAMP(''20031231'')', TIMESTAMP('20031231'));
insert into func_test2(functionName, result) values('TIMESTAMP(20031231)', TIMESTAMP(20031231));
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31 12:00:00.123456'')', TIMESTAMP('2003-12-31 12:00:00.123456'));
insert into func_test2(functionName, result) values('TIMESTAMP(''20000229120000.1234567'')', TIMESTAMP('20000229120000.1234567'));
insert into func_test2(functionName, result) values('TIMESTAMP(20000229120000.1234567)', TIMESTAMP(20000229120000.1234567));
insert into func_test2(functionName, result) values('TIMESTAMP(20000229120000)', TIMESTAMP(20000229120000));
-- 边界值
insert into func_test2(functionName, result) values('TIMESTAMP(''0000-01-01'')', TIMESTAMP('0000-01-01')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''9999-12-31'')', TIMESTAMP('9999-12-31')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''10000-01-01'')', TIMESTAMP('10000-01-01')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''0000-01-01 00:00:00.000001'')', TIMESTAMP('0000-01-01 00:00:00.000001')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''9999-12-31 23:59:59.999999'')', TIMESTAMP('9999-12-31 23:59:59.999999')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''10000-01-01 00:00:00'')', TIMESTAMP('10000-01-01 00:00:00')); 
-- 不同参数类型
insert into func_test2(functionName, result) values('TIMESTAMP(date''2003-01-01'')', TIMESTAMP(date'2003-01-01'));
insert into func_test2(functionName, result) values('TIMESTAMP(cast(''2007-12-10 23:59:59'' as datetime))', TIMESTAMP(cast('2007-12-10 23:59:59' as datetime))); 
insert into func_test2(functionName, result) values('TIMESTAMP(true)', TIMESTAMP(true));
insert into func_test2(functionName, result) values('TIMESTAMP(B''1'')', TIMESTAMP(B'1')); 

-- TIMESTAMP(expr1, expr2)
    -- 特异参数
insert into func_test2(functionName, result) values('TIMESTAMP(NULL, NULL)', TIMESTAMP(NULL, NULL)); 
insert into func_test2(functionName, result) values('TIMESTAMP(''20000321'', ''abcd'')', TIMESTAMP('20000321', 'abcd'));
insert into func_test2(functionName, result) values('TIMESTAMP(''abcd'', ''010101'')', TIMESTAMP('abcd', '010101'));
    -- 不存在的日期或时间
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-2-29'',''25:00:00'')', TIMESTAMP('2003-2-29', '25:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31 24:00:00'',''25:00:00'')', TIMESTAMP('2003-12-31 24:00:00', '25:00:00')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31 23:60:00'',''25:00:00'')', TIMESTAMP('2003-12-31 23:60:00', '25:00:00')); 
    -- 日期越界
insert into func_test2(functionName, result) values('TIMESTAMP(''10000-01-01'',''25:00:00'')', TIMESTAMP('10000-01-01', '25:00:00')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''10000-01-01 00:00:00'',''25:00:00'')', TIMESTAMP('10000-01-01 00:00:00', '25:00:00')); 
    -- 主体功能
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31'',''12:00:00.9999995'')', TIMESTAMP('2003-12-31','12:00:00.9999995'));
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31 12:00:00'',''32:00:00'')', TIMESTAMP('2003-12-31 12:00:00','32:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMP(''20000229'',''100:00:00.000001'')', TIMESTAMP('20000229','100:00:00.000001'));
insert into func_test2(functionName, result) values('TIMESTAMP(20000229,1000000)', TIMESTAMP(20000229, 1000000));
insert into func_test2(functionName, result) values('TIMESTAMP(20000229,1000000.000001)', TIMESTAMP(20000229, 1000000.000001));
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31 12:00:00'',''-12:00:00.000001'')', TIMESTAMP('2003-12-31 12:00:00','-12:00:00.000001'));
insert into func_test2(functionName, result) values('TIMESTAMP(''2003-12-31 12:00:00.000001'',''12:00:00.999999'')', TIMESTAMP('2003-12-31 12:00:00.000001','12:00:00.999999'));
insert into func_test2(functionName, result) values('TIMESTAMP(20031231120000,-120000)', TIMESTAMP(20031231120000,-120000));
insert into func_test2(functionName, result) values('TIMESTAMP(20031231120000.000002,-120000.000001)', TIMESTAMP(20031231120000.000002,-120000.000001));
    -- 边界值
insert into func_test2(functionName, result) values('TIMESTAMP(''1000-12-31'',''838:59:59'')', TIMESTAMP('1000-12-31', '838:59:59')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''1000-01-01 00:00:00'',''839:00:00'')', TIMESTAMP('1000-12-31 00:00:00', '839:00:00')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''9999-12-31'',''23:59:59.999999'')', TIMESTAMP('9999-12-31', '23:59:59.999999')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''9999-12-31 00:00:00'',''24:00:00'')', TIMESTAMP('9999-12-31 00:00:00', '24:00:00')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''0001-01-01'', ''00:00:00'')', TIMESTAMP('0001-01-01', '00:00:00')); 
insert into func_test2(functionName, result) values('TIMESTAMP(''0001-01-01 00:00:00'', ''-00:00:00.000001'')', TIMESTAMP('0001-01-01 00:00:00', '-00:00:00.000001')); 
    -- 不同参数类型
insert into func_test2(functionName, result) values('TIMESTAMP(date''2003-01-01'', date''2000-01-01'')', TIMESTAMP(date'2003-01-01', date'2000-01-01'));
insert into func_test2(functionName, result) values('TIMESTAMP(cast(''2007-12-10 23:59:59'' as datetime), cast(''2007-12-10 22:59:59'' as datetime))', TIMESTAMP(cast('2007-12-10 23:59:59' as datetime), cast('2007-12-10 22:59:59' as datetime)));
insert into func_test2(functionName, result) values('TIMESTAMP(true, false)', TIMESTAMP(true, false));
insert into func_test2(functionName, result) values('TIMESTAMP(B''1'', B''1'')', TIMESTAMP(B'1', B'1'));

-- TIMESTAMPADD()
-- 正常测试
-- date格式
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,1,''2022-07-27'')', TIMESTAMPADD(YEAR,1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,-1,''2022-07-27'')', TIMESTAMPADD(YEAR,-1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1,''2022-07-27'')', TIMESTAMPADD(MONTH,1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,-1,''2022-07-27'')', TIMESTAMPADD(MONTH,-1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(WEEK,1,''2022-07-27'')', TIMESTAMPADD(WEEK,1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(WEEK,-1,''2022-07-27'')', TIMESTAMPADD(WEEK,-1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1,''2022-07-27'')', TIMESTAMPADD(DAY,1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1,''2022-07-27'')', TIMESTAMPADD(DAY,-1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,1,''2022-07-27'')', TIMESTAMPADD(HOUR,1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,-1,''2022-07-27'')', TIMESTAMPADD(HOUR,-1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,1,''2022-07-27'')', TIMESTAMPADD(MINUTE,1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,-1,''2022-07-27'')', TIMESTAMPADD(MINUTE,-1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,1,''2022-07-27'')', TIMESTAMPADD(SECOND,1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-1,''2022-07-27'')', TIMESTAMPADD(SECOND,-1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-1, 2022-07-27)', TIMESTAMPADD(SECOND,-1, 20220727));
-- datetime格式
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,1,''2022-07-27 08:30:00'')', TIMESTAMPADD(YEAR,1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,-1,''2022-07-27 08:30:00'')', TIMESTAMPADD(YEAR,-1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1,''2022-07-27 08:30:00'')', TIMESTAMPADD(MONTH,1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,-1,''2022-07-27 08:30:00'')', TIMESTAMPADD(MONTH,-1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(WEEK,1,''2022-07-27 08:30:00'')', TIMESTAMPADD(WEEK,1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(WEEK,-1,''2022-07-27 08:30:00'')', TIMESTAMPADD(WEEK,-1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1,''2022-07-27 08:30:00'')', TIMESTAMPADD(DAY,1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1,''2022-07-27 08:30:00'')', TIMESTAMPADD(DAY,-1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,1,''2022-07-27 08:30:00'')', TIMESTAMPADD(HOUR,1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,-1,''2022-07-27 08:30:00'')', TIMESTAMPADD(HOUR,-1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,1,''2022-07-27 08:30:00'')', TIMESTAMPADD(MINUTE,1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,-1,''2022-07-27 08:30:00'')', TIMESTAMPADD(MINUTE,-1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,1,''2022-07-27 08:30:00'')', TIMESTAMPADD(SECOND,1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-1,''2022-07-27 08:30:00'')', TIMESTAMPADD(SECOND,-1,'2022-07-27 08:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-1, 20220727083000)', TIMESTAMPADD(SECOND,-1, 20220727083000));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-1, 20220727083000.000001)', TIMESTAMPADD(SECOND,-1, 20220727083000.000001));
-- 特异参数测试
-- interval的值为小数
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,1.1,''2022-07-27'')', TIMESTAMPADD(YEAR,1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,-1.1,''2022-07-27'')', TIMESTAMPADD(YEAR,-1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,1.6,''2022-07-27'')', TIMESTAMPADD(YEAR,1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,-1.6,''2022-07-27'')', TIMESTAMPADD(YEAR,-1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1.1,''2022-07-27'')', TIMESTAMPADD(MONTH,1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,-1.1,''2022-07-27'')', TIMESTAMPADD(MONTH,-1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1.6,''2022-07-27'')', TIMESTAMPADD(MONTH,1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,-1.6,''2022-07-27'')', TIMESTAMPADD(MONTH,-1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(WEEK,1.1,''2022-07-27'')', TIMESTAMPADD(WEEK,1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(WEEK,-1.1,''2022-07-27'')', TIMESTAMPADD(WEEK,-1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(WEEK,1.6,''2022-07-27'')', TIMESTAMPADD(WEEK,1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(WEEK,-1.6,''2022-07-27'')', TIMESTAMPADD(WEEK,-1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1.1,''2022-07-27'')', TIMESTAMPADD(DAY,1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1.1,''2022-07-27'')', TIMESTAMPADD(DAY,-1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1.6,''2022-07-27'')', TIMESTAMPADD(DAY,1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1.6,''2022-07-27'')', TIMESTAMPADD(DAY,-1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,1.1,''2022-07-27'')', TIMESTAMPADD(HOUR,1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,-1.1,''2022-07-27'')', TIMESTAMPADD(HOUR,-1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,1.6,''2022-07-27'')', TIMESTAMPADD(HOUR,1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,-1.6,''2022-07-27'')', TIMESTAMPADD(HOUR,-1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,1.1,''2022-07-27'')', TIMESTAMPADD(MINUTE,1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,-1.1,''2022-07-27'')', TIMESTAMPADD(MINUTE,-1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,1.6,''2022-07-27'')', TIMESTAMPADD(MINUTE,1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,-1.6,''2022-07-27'')', TIMESTAMPADD(MINUTE,-1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,1.1,''2022-07-27'')', TIMESTAMPADD(SECOND,1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-1.1,''2022-07-27'')', TIMESTAMPADD(SECOND,-1.1,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,1.6,''2022-07-27'')', TIMESTAMPADD(SECOND,1.6,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-1.6,''2022-07-27'')', TIMESTAMPADD(SECOND,-1.6,'2022-07-27'));
-- 不存在的日期或时间
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,1,''2022-07-36'')', TIMESTAMPADD(YEAR,1,'2022-07-36'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,-1,''2022-14-27'')', TIMESTAMPADD(YEAR,-1,'2022-14-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1,''2022-12-32'')', TIMESTAMPADD(MONTH,1,'2022-12-32'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,-1,''2022-01-00'')', TIMESTAMPADD(MONTH,-1,'2022-01-00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1,''2022-02-29'')', TIMESTAMPADD(DAY,1,'2022-02-29'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1,''2022-14-32'')', TIMESTAMPADD(DAY,-1,'2022-14-32'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,1,''2022-14-32'')', TIMESTAMPADD(HOUR,1,'2022-14-32'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,-1,''2022-02-29'')', TIMESTAMPADD(HOUR,-1,'2022-02-29'));

insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,1,''2022-07-27 24:30:00'')', TIMESTAMPADD(YEAR,1,'2022-07-27 24:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,-1,''2022-07-27 -01:-30:00'')', TIMESTAMPADD(YEAR,-1,'2022-07-27 -01:-30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1,''2022-07-27 26:61:60'')', TIMESTAMPADD(MONTH,1,'2022-07-27 26:61:60'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,-1,''2022-07-27 08:60:60'')', TIMESTAMPADD(MONTH,-1,'2022-07-27 08:60:60'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1,''2022-07-27 -01:30:00'')', TIMESTAMPADD(DAY,1,'2022-07-27 -01:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1,''2022-07-27 25:30:00'')', TIMESTAMPADD(DAY,-1,'2022-07-27 25:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,1,''2022-07-27 -12:30:00'')', TIMESTAMPADD(HOUR,1,'2022-07-27 -12:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,-1,''2022-07-27 25:30:00'')', TIMESTAMPADD(HOUR,-1,'2022-07-27 25:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,1,''2022-07-27 08:61:00'')', TIMESTAMPADD(MINUTE,1,'2022-07-27 08:61:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,-1,''2022-07-27 08:-59:00'')', TIMESTAMPADD(MINUTE,-1,'2022-07-27 08:-59:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,1,''2022-07-27 08:30:60'')', TIMESTAMPADD(SECOND,1,'2022-07-27 08:30:60'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-1,''2022-07-27 08:30:-01'')', TIMESTAMPADD(SECOND,-1,'2022-07-27 08:30:-01'));
-- 超大参数
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,10000000000,''9999999999-12-31'')', TIMESTAMPADD(YEAR,10000000000,'9999999999-12-31'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,-1000000000,''999999999-99999-999999'')', TIMESTAMPADD(YEAR,-1000000000,'999999999-99999-999999'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1000000000000000000000000000000000,''2022-12-28'')', TIMESTAMPADD(MONTH,1000000000000000000000000000000000,'2022-12-28'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,-10000000000000000000000000000000,''2022-01-01'')', TIMESTAMPADD(MONTH,-10000000000000000000000000000000,'2022-01-01'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,99999999999999999,''2022-02-06'')', TIMESTAMPADD(DAY,99999999999999999,'2022-02-06'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1000000000000000000000000000000,''2022-06-07'')', TIMESTAMPADD(DAY,-1000000000000000000000000000000,'2022-06-07'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,1000000000000000000000000000000,''2022-11-21'')', TIMESTAMPADD(HOUR,1000000000000000000000000000000,'2022-11-21'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,-10000000000000000000000000000,''2022-04-05'')', TIMESTAMPADD(HOUR,-10000000000000000000000000000,'2022-04-05'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,timestamptz''2000-1-1 1:1:1+05'',''2022-07-01'')', TIMESTAMPADD(DAY,timestamptz'2000-1-1 1:1:1+05','2022-07-01'));
-- 布尔类型参数
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,true,''2022-04-05'')', TIMESTAMPADD(HOUR,true,'2022-04-05'));
-- 比特类型参数
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,B''11'',''2022-04-05'')', TIMESTAMPADD(HOUR,B'11','2022-04-05'));
-- date类型参数
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,1,date''2022-04-05'')', TIMESTAMPADD(HOUR,1,date'2022-04-05'));
-- datetime类型参数
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1,cast(''2022-04-05'' as datetime))', TIMESTAMPADD(day,1,cast('2022-04-05' as datetime)));
-- time类型参数
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1,time''14:30:00'')', TIMESTAMPADD(day,1,time'14:30:00'));
-- 字符串增量
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,''abc'',''0001-01-01'')', TIMESTAMPADD(year,'abc','0001-01-01'));

-- 边界测试
-- 最大值
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1,''9999-12-31'')', TIMESTAMPADD(DAY,1,'9999-12-31'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1,''9999-12-31 23:59:59'')', TIMESTAMPADD(DAY,1,'9999-12-31 23:59:59'));
-- 最小值
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1,''0001-01-01'')', TIMESTAMPADD(DAY,-1,'0001-01-01'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1,''0001-01-01 00:00:00'')', TIMESTAMPADD(DAY,-1,'0001-01-01 00:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,1,''0000-01-01 00:00:00'')', TIMESTAMPADD(YEAR, 1,'0000-01-01 00:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(qtr,1,''0000-01-01 00:00:00'')', TIMESTAMPADD(qtr, 1,'0000-01-01 00:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1,''0000-01-01 14:25:59'')', TIMESTAMPADD(MONTH, 1,'0000-01-01 14:25:59'));
-- 大月的最后一天跨至小月
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,2,''2022-07-31'')', TIMESTAMPADD(MONTH,2,'2022-07-31'));
-- 闰年到非闰年
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,1,''2020-02-29'')', TIMESTAMPADD(YEAR,1,'2020-02-29'));
-- 非闰年到闰年
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,3,''2019-12-31'')', TIMESTAMPADD(MONTH,3,'2019-12-31'));
-- date格式隐式转换成datetime格式
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,3,''2022-07-27'')', TIMESTAMPADD(HOUR,3,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,0.3,''2022-07-27'')', TIMESTAMPADD(HOUR,0.3,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,3,''2022-07-27'')', TIMESTAMPADD(MINUTE,3,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,0.3,''2022-07-27'')', TIMESTAMPADD(MINUTE,0.3,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,3,''2022-07-27'')', TIMESTAMPADD(SECOND,3,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,0.03,''2022-07-27'')', TIMESTAMPADD(SECOND,0.03,'2022-07-27'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,0.000000003,''2022-07-27'')', TIMESTAMPADD(SECOND,0.000000003,'2022-07-27'));
-- 运算后进位或借位
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,1,''2020-02-29'')', TIMESTAMPADD(YEAR,1,'2020-02-29'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(YEAR,-1,''2020-02-29'')', TIMESTAMPADD(YEAR,-1,'2020-02-29'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1,''2022-08-31'')', TIMESTAMPADD(MONTH,1,'2022-08-31'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,-1,''2022-07-31'')', TIMESTAMPADD(MONTH,-1,'2022-07-31'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,1,''2022-12-31'')', TIMESTAMPADD(MONTH,1,'2022-12-31'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MONTH,-1,''2022-01-31'')', TIMESTAMPADD(MONTH,-1,'2022-01-31'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,1,''2022-02-29'')', TIMESTAMPADD(DAY,1,'2022-02-29'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(DAY,-1,''2022-01-01'')', TIMESTAMPADD(DAY,-1,'2022-01-01'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,49,''2022-10-30'')', TIMESTAMPADD(HOUR,49,'2022-10-30'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,-56,''2022-07-01'')', TIMESTAMPADD(HOUR,-56,'2022-07-01'));

insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,1,''2022-07-27 23:30:00'')', TIMESTAMPADD(HOUR,1,'2022-07-27 23:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(HOUR,-1,''2022-07-27 00:30:00'')', TIMESTAMPADD(HOUR,-1,'2022-07-27 00:30:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,1,''2022-07-27 08:59:59'')', TIMESTAMPADD(MINUTE,1,'2022-07-27 08:59:59'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(MINUTE,-1,''2022-07-27 08:00:00'')', TIMESTAMPADD(MINUTE,-1,'2022-07-27 08:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,1,''2022-07-27 08:30:59'')', TIMESTAMPADD(SECOND,1,'2022-07-27 08:30:59'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-1,''2022-07-27 00:00:00'')', TIMESTAMPADD(SECOND,-1,'2022-07-27 00:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,0.001,''2022-07-27 08:30:59.999'')', TIMESTAMPADD(SECOND,0.001,'2022-07-27 08:30:59.999'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(SECOND,-0.001,''2022-07-27 00:00:00'')', TIMESTAMPADD(SECOND,-0.001,'2022-07-27 00:00:00'));
-- 越界预判
insert into func_test2(functionName, result) values('TIMESTAMPADD(year,1,time''23:59:59'')', TIMESTAMPADD(year,1,time'23:59:59'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(qtr,1,time''23:59:59'')', TIMESTAMPADD(qtr,1,time'23:59:59'));
insert into func_test2(functionName, result) values('TIMESTAMPADD(month,1,time''23:59:59'')', TIMESTAMPADD(month,1,time'23:59:59'));

-- TIME、TIMESTAMP建表语法修改测试
create table t1(col timestamp('1'));
create table t2(col time('1'));
drop table t1;
drop table t2;

select * from func_test2;
drop schema b_time_funcs2 cascade;
reset current_schema;
