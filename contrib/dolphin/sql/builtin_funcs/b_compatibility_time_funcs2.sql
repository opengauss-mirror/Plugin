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
insert into func_test2(functionName, result) values('subtime(8385959, ''-1:00:00'')', subtime(8385959,'-1:00:00'));
insert into func_test2(functionName, result) values('subtime(8375959.9999999, ''-1:00:00'')', subtime(8375959.9999999,'-1:00:00'));
insert into func_test2(functionName, result) values('subtime(-8385959, ''1:00:00'')', subtime(-8385959,'1:00:00'));
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
-- test for invalid format character
select time_format('2024-11-18 11:12:35', 'qwerqwerqwer');
select time_format('2024-11-18 11:12:35', 'qwerqwerqwer%Y-%m-%d %H:%i:%s');
select time_format('2024-10-19 23:30:50', 'qwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwer');
select time_format('2024-10-19 23:30:50', 'qwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqwerqw%Y-%m-%d %H:%i:%serqwerqwerqwerqwerqwer');
select time_format('2024-10-19 23:30:50', 'SELECT i.namespace, (SELECT tc.relname FROM pg_class tc WHERE (tc.oid = i.indrelid)) AS \"table\", (NOT i.indisunique) AS non_unique, c.relname AS key_name, i.seq_in_index, a.attname AS column_name, a.atttypid AS column_type, a.attstattarget AS column_stattarget, a.attlen AS column_len, a.attnum AS column_num, a.attcacheoff, a.atttypmod AS column_typmod, a.attbval, a.atthasdef, a.attisdropped, a.attislocal AS column_isdropped, a.attcmprmode AS column_cmprmode, a.attinhcount, a.attcollation AS column_collation, a.attacl AS column_acl, a.attoptions AS column_options, a.attfdwoptions AS column_fdwoptions, a.attinitdefval AS column_initdefval, a.attkvtype AS column_kvtype, a.attdroppedname AS column_droppedname, c.relname, c.relnamespace, c.reltype, c.reloftype, c.relowner, c.relam, c.relfilenode, c.reltablespace, c.relpages, c.reltuples, c.relallvisible, c.reltoastrelid, c.reltoastidxid, c.reldeltarelid, c.reldeltaidx, c.relcudescrelid, c.relcudescidx, c.relhasindex, c.relisshared, c.relpersistence, c.relkind, c.relnatts, c.relchecks, c.relhasoids, c.relhaspkey, c.relhasrules, c.relhastriggers, c.relhassubclass, c.relcmprs, c.relhasclusterkey, c.relrowmovement, c.parttype, c.relfrozenxid, c.relacl, c.reloptions, c.relreplident, c.relfrozenxid64, c.relbucket, c.relbucketkey, c.relminmxid FROM pg_class c join pg_index i on c.Oid = i.attrelid join pg_attribute a on c.Oid = a.indexrelid;');
-- test for invalid time
select time_format('2012-2-25','%T||%r||%H||%h||%I||%i||%S||%f||%p||%k');
select time_format('2021-12-32','%T||%r||%H||%h||%I||%i||%S||%f||%p||%k');
select time_format('0:-1:0','%T||%r||%H||%h||%I||%i||%S||%f||%p||%k');
select time_format('0:60:0','%T||%r||%H||%h||%I||%i||%S||%f||%p||%k');
select time_format('0:59.5:0','%T||%r||%H||%h||%I||%i||%S||%f||%p||%k');
select time_format('0:0:-1','%T||%r||%H||%h||%I||%i||%S||%f||%p||%k');
select time_format('0:0:60','%T||%r||%H||%h||%I||%i||%S||%f||%p||%k');
select time_format('0000-12-31 23:59:59','%T||%r||%H||%h||%I||%i||%S||%f||%p||%k');
select time_format('2021-12-32 01:01:01','%T||%r||%H||%h||%I||%i||%S||%f||%p||%k');

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
insert into func_test2(functionName, result) values('TIMESTAMP(''4714-11-24bc'')', TIMESTAMP('4714-11-24bc'));
insert into func_test2(functionName, result) values('TIMESTAMP(''4714-11-23bc'')', TIMESTAMP('4714-11-23bc'));
insert into func_test2(functionName, result) values('TIMESTAMP(''294276-12-31'')', TIMESTAMP('294276-12-31'));
insert into func_test2(functionName, result) values('TIMESTAMP(''294277-01-01'')', TIMESTAMP('294277-01-01'));
insert into func_test2(functionName, result) values('TIMESTAMP(''4714-11-24 00:00:00.000000bc'')', TIMESTAMP('4714-11-24 00:00:00.000000bc'));
insert into func_test2(functionName, result) values('TIMESTAMP(''4714-11-23 23:59:59.999999bc'')', TIMESTAMP('4714-11-23 23:59:59.999999bc'));
insert into func_test2(functionName, result) values('TIMESTAMP(''294276-12-31 23:59:59.999999'')', TIMESTAMP('294276-12-31 23:59:59.999999'));
insert into func_test2(functionName, result) values('TIMESTAMP(''294277-01-01 00:00:00.000000'')', TIMESTAMP('294277-01-01 00:00:00.000000'));
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
insert into func_test2(functionName, result) values('TIMESTAMP(''294276-12-31'',''25:00:00'')', TIMESTAMP('294276-12-31', '25:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMP(''294277-12-31'',''25:00:00'')', TIMESTAMP('294277-12-31', '25:00:00'));
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
insert into func_test2(functionName, result) values('TIMESTAMP(''1000-12-31 00:00:00'',''839:00:00'')', TIMESTAMP('1000-12-31 00:00:00', '839:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMP(''4714-11-24bc'', ''00:00:00'')', TIMESTAMP('4714-11-24bc', '00:00:00'));
insert into func_test2(functionName, result) values('TIMESTAMP(''4714-11-24 00:00:00bc'', ''-00:00:00.000001'')', TIMESTAMP('4714-11-24 00:00:00bc', '-00:00:00.000001'));
insert into func_test2(functionName, result) values('TIMESTAMP(''294276-12-31 00:00:00'',''23:59:59.999999'')', TIMESTAMP('294276-12-31 00:00:00', '23:59:59.999999'));
insert into func_test2(functionName, result) values('TIMESTAMP(''294276-12-31 00:00:00'',''24:00:00.000000'')', TIMESTAMP('294276-12-31 00:00:00', '24:00:00.000000'));
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

-- timetz
insert into func_test2(functionName, result) values('timestampadd(day, 1, timetz''1:1:0+05'')', timestampadd(day, 1, timetz'1:1:0+05'));

insert into func_test2(functionName, result) values('TIME_FORMAT(''00:00:00'', ''%l %p'')', TIME_FORMAT('00:00:00', '%l %p'));
insert into func_test2(functionName, result) values('TIME_FORMAT(''12:00:00'', ''%l %p'')', TIME_FORMAT('12:00:00', '%l %p'));
insert into func_test2(functionName, result) values('TIME_FORMAT(''23:00:00'', ''%l %p'')', TIME_FORMAT('23:00:00', '%l %p'));
insert into func_test2(functionName, result) values('TIME_FORMAT(''24:00:00'', ''%l %p'')', TIME_FORMAT('24:00:00', '%l %p'));

-- TIME、TIMESTAMP建表语法修改测试
create table t1(col timestamp('1'));
create table t2(col time('1'));
drop table t1;
drop table t2;

select * from func_test2 order by 1, 2;

-- 单独select边界测试
select subtime('838:59:59', '-25:00');
select subtime('-838:59:59', '25:00');
select subtime('839:59:59', '837:59:59');
select subtime('-837:59:59', '-839:59:59') ;
select subtime(8385959,'-1:00:00');
select subtime(8375959.9999999,'-1:00:00');
select subtime(-8385959,'1:00:00');

-- 非严格模式下的写测试
truncate table func_test2;
set dolphin.sql_mode = 'sql_mode_full_group,pipes_as_concat,ansi_quotes';
insert into func_test2(functionName, result) values('subtime(''838:59:59'', ''-25:00'')', subtime('838:59:59', '-25:00'));
insert into func_test2(functionName, result) values('subtime(''-838:59:59'', ''25:00'')', subtime('-838:59:59', '25:00'));
insert into func_test2(functionName, result) values('subtime(''839:59:59'', ''837:59:59'')', subtime('839:59:59', '837:59:59'));
insert into func_test2(functionName, result) values('subtime(''-837:59:59'', ''-839:59:59'')', subtime('-837:59:59', '-839:59:59'));
insert into func_test2(functionName, result) values('subtime(8385959, ''-1:00:00'')', subtime(8385959,'-1:00:00'));
insert into func_test2(functionName, result) values('subtime(8375959.9999999, ''-1:00:00'')', subtime(8375959.9999999,'-1:00:00'));
insert into func_test2(functionName, result) values('subtime(-8385959, ''1:00:00'')', subtime(-8385959,'1:00:00'));

insert into func_test2(functionName, result) values('subtime(B''0'',B''1'')', subtime(B'0',B'1'));
insert into func_test2(functionName, result) values('TIMEDIFF(B''101'', B''101'')', TIMEDIFF(B'101', B'101'));
insert into func_test2(functionName, result) values('TIME(B''1'')', TIME(B'1'));
select * from func_test2 order by functionName;

set dolphin.b_compatibility_mode=on;
set b_format_behavior_compat_options='enable_set_variables,enable_multi_charset';

drop table if exists t_number;
create table t_number(c1 int,c2 tinyint,c3 tinyint(255),c4 smallint,c5 smallint(255),c6 mediumint,c7 mediumint(255),c8 int,c9 int(255),c10 integer,c11 integer(255),c12 bigint,c13 bigint(255));
drop table if exists t_number_decimal;
create table t_number_decimal(
    c1 int,c2 decimal,c3 decimal(10),c4 decimal(10, 3),c5 decimal(30, 15),c6 fixed,c7 fixed(10),c8 fixed(10, 3),c9 fixed(30, 15),
    c10 float,c11 float(10),c12 float(10, 3),c13 float(30, 15),c14 float4,c15 float4(10),c16 float4(10, 3),c17 float4(30, 15),
    c18 numeric,c19 numeric(10),c20 numeric(10, 3),c21 numeric(30, 15),c22 double,c23 float8,c24 double precision,c25 double(10, 3),
    c26 real(10, 3),c27 double precision(10, 3),c28 double(30, 15),c29 real(30, 15),c30 double precision(30, 15)
    );
drop table if exists t_date_time;
create table t_date_time(c1 int,c2 date,c3 datetime,c4 datetime(6),c5 year,c6 year(4),c7 time,c8 time(6),c9 timestamp,c10 timestamp(6) default current_timestamp(6));
drop table if exists t_bin;
create table t_bin(c1 int,c2 bit,c3 bit(64),c4 binary,c5 binary(255),c6 varbinary(10),c7 varbinary(255),c8 blob,c9 tinyblob,c10 mediumblob,c11 longblob) charset utf8mb3;
drop table if exists t_set;
create table t_set(c1 int,c2 set('0', '1', '1.01314') default null);
drop table if exists t_enum;
create table t_enum(c1 int,c2 enum('red', 'yellow', 'blue') not null);
drop table if exists t_bool;
create table t_bool(c1 int,c2 boolean,c3 tinyint(1),c4 tinyint(1) unsigned);
drop table if exists t_json;
create table t_json(c1 int,c2 json);

insert into t_number values (1, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127);
insert into t_number values (2, 127, 127, 127, 127, 127, 127, 127, 1000, 10000, 10000, 10000, 10000);
set @v1 = 3.14159265327896457;
insert into t_number_decimal values (1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1, @v1);
set @v2 = 'abcdef熊猫竹竹';
insert into t_bin values (1, 0x01, 0xFFFF, 9, hex(@v2), hex(12354), hex(@v2), '[B@5e265ba4','[B@5e265ba4', '[B@5e265ba4', '[B@5e265ba4'),
                         (2, b'1', b'1011', 1, @v2, 121314, @v2, 10000, 10000, 10000, 10000);
insert into t_date_time values (1, '1999-01-01', '1999-01-01 01:01:01', '00-01-01 00:00:01', 27, '1904','23:59:59', 121314, '1970-01-01 08:00:01.123', '2038-01-19 11:14:07');
insert into t_set values (1, '1.01314'), (2, '0');
insert into t_enum values (1, 'red');
insert into t_bool values (1, true, -1, false);
insert into t_json values (1, '["a", "b", "c"]'), (2, '{"k1":"v1"}');

-- test year number
select c1, year(c2), year(c3), year(c4), year(c5), year(c6), year(c7),year(c8), year(c9), year(c10), year(c11), year(c12), year(c13) from t_number order by c1;
-- error test, month or day can't be zero
select c1, year(c9), year(c10), year(c11), year(c12), year(c13) from t_number order by c1;
select year(0);
-- test year binary
select c1, year(c2), year(c3), year(c4), year(c5), year(c6), year(c7), year(c8), year(c9), year(c10), year(c11) from t_bin order by c1;
-- test year set
select c1, year(c2) from t_set order by c1;
-- test year bool
select c1, year(c2), year(c3), year(c4) from t_bool order by c1;

-- test hour number
select hour(-100), hour(0), hour(121314);
-- test hour binary
select c1, hour(c2), hour(c3), hour(c4), hour(c5), hour(c6), hour(c7), hour(c8), hour(c9), hour(c10), hour(c11) from t_bin order by c1;
-- test hour bool
select c1, hour(c2), hour(c3), hour(c4) from t_bool order by c1;
-- test hour json
select c1, hour(c2) from t_json order by c1;

-- test minute number
select c1, minute(c2), minute(c3), minute(c4), minute(c5), minute(c6), minute(c7), minute(c8), minute(c9), minute(c10), minute(c11), minute(c12), minute(c13) from t_number order by c1;
select minute(-127), minute(0), minute(100);
-- test minute binary
select c1, minute(c2), minute(c3), minute(c4), minute(c5), minute(c6), minute(c7), minute(c8), minute(c9), minute(c10), minute(c11) from t_bin order by c1;
-- test minute bool
select c1, minute(c2), minute(c3), minute(c4) from t_bool order by c1;
-- test minute json
select c1, minute(c2) from t_json order by c1;

-- test second number
select c1, second(c2), second(c3), second(c4), second(c5), second(c6), second(c7), second(c8), second(c9), second(c10), second(c11), second(c12), second(c13) from t_number order by c1;
select second(-127), second(0), second(100);
-- test second decimal
select c1, second(c2), second(c3), second(c4), second(c5), second(c6), second(c7), second(c8), second(c9), second(c10), second(c11), second(c12),
    second(c13), second(c14), second(c15), second(c16), second(c17), second(c18), second(c19), second(c20), second(c21), second(c22), second(c23),
    second(c24), second(c25), second(c26), second(c27), second(c28), second(c29), second(c30)
from t_number_decimal order by c1;
-- test second binary
select c1, second(c2), second(c3), second(c4), second(c5), second(c6), second(c7), second(c8), second(c9), second(c10), second(c11) from t_bin order by c1;
-- test second bool
select c1, second(c2), second(c3), second(c4) from t_bool order by c1;
-- test second json
select c1, second(c2) from t_json order by c1;

-- test microsecond datetime
select c1, microsecond(c2), microsecond(c3), microsecond(c4), microsecond(c5), microsecond(c6), microsecond(c7), microsecond(c8), microsecond(c9), microsecond(c10) from t_date_time order by c1;
-- test microsecond binary
select c1, microsecond(c2), microsecond(c3), microsecond(c4), microsecond(c5), microsecond(c6), microsecond(c7), microsecond(c8), microsecond(c9), microsecond(c10), microsecond(c11) from t_bin order by c1;
-- test microsecond bool
select c1, microsecond(c2), microsecond(c3), microsecond(c4) from t_bool order by c1;
-- test microsecond json
select c1, microsecond(c2) from t_json order by c1;
-- test microsecond number
select microsecond(-127), microsecond(0), microsecond(100.23);

-- test bit type insert ignore, if first char is '\0'
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero';
create table bit_ignore (c1 integer, `func_name` varchar(100), `result` bigint);
-- warnings
insert ignore into bit_ignore values (1, 'hour(bit)', hour(b'000000000011000000111001'));
insert ignore into bit_ignore values (2, 'minute(bit)', minute(b'000000000011000000111001'));
insert ignore into bit_ignore values (3, 'second(bit)', second(b'000000000011000000111001'));
insert ignore into bit_ignore values (4, 'microsecond(bit)', microsecond(b'000000000011000000111001'));
-- error
insert into bit_ignore values (5, 'hour(bit)', hour(b'000000000011000000111001'));
insert into bit_ignore values (6, 'minute(bit)', minute(b'000000000011000000111001'));
insert into bit_ignore values (7, 'second(bit)', second(b'000000000011000000111001'));
insert into bit_ignore values (8, 'microsecond(bit)', microsecond(b'000000000011000000111001'));
select c1, `func_name`, `result` from bit_ignore order by c1;

drop table t_number;
drop table t_number_decimal;
drop table t_date_time;
drop table t_bin;
drop table t_set;
drop table t_enum;
drop table t_bool;
drop table t_json;
drop table bit_ignore;

-- test weekofyear
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero';
create table t_time (c1 int, `date` date,`time` time, `time4` time(4), `datetime` datetime, `datetime4` datetime(4) default '2023-12-30 12:00:12', `timestamp` timestamp, `timestamp4` timestamp(4) default '2023-12-30 12:00:12', `year` year, `year4` year(4));
create table t_char (c1 int, `char` char(100), `varchar` varchar(100), `binary` binary(100), `varbinary` varbinary(100), `text` text);

insert into t_time values (1, '2024-01-11', '11:47:58', '11:47:58.7896', '2024-08-21 11:49:25', '2024-04-29 11:49:25.1234', '2024-12-30 11:49:25', '2024-01-11 11:49:25.1234', 70, '2024');
insert into t_time values (2, '1999-01-01', 121314, '00:00:01.1234', '00-9-19 00:00:01', '1970-10-02 08:00:01.123', '2024-08-20 11:14:07', '1970-04-08 08:00:01.123', 27, '1904');
insert into t_char values (1, '2008-04-01', '1968-02-28', '2077-08-20', '2001-12-12', '2024-3-14');
insert into t_char values (2, '30215', '31215 12345', '10000', '10023', '700923');
insert into t_char values (3, '3.145@6七八九', '3.145@6七八九', '3.145@6七八九', '3.145@6七八九', '3.145@6七八九');

SELECT c1, weekofyear(`time`), weekofyear(`time4`) from t_time order by c1;
SELECT c1, weekofyear(`date`), weekofyear(`datetime`), weekofyear(`datetime4`), weekofyear(`timestamp`), weekofyear(`timestamp4`), weekofyear(`year`), weekofyear(`year4`) from t_time order by c1;
SELECT c1, pg_typeof(weekofyear(`date`)), pg_typeof(weekofyear(`time`)), pg_typeof(weekofyear(`time4`)), pg_typeof(weekofyear(`datetime`)), pg_typeof(weekofyear(`datetime4`)), 
pg_typeof(weekofyear(`timestamp`)), pg_typeof(weekofyear(`timestamp4`)), pg_typeof(weekofyear(`year`)), pg_typeof(weekofyear(`year4`)) from t_time order by c1;
SELECT c1, weekofyear(`char`), weekofyear(`varchar`), weekofyear(`binary`), weekofyear(`varbinary`), weekofyear(`text`) from t_char order by c1;
SELECT c1, pg_typeof(weekofyear(`char`)), pg_typeof(weekofyear(`varchar`)), pg_typeof(weekofyear(`binary`)), pg_typeof(weekofyear(`varbinary`)), pg_typeof(weekofyear(`text`)) from t_char order by c1;

-- test weekofyear insert ignore with strict_mode
create table t1 (c1 bigint, c2 bigint, c3 bigint, c4 bigint, c5 bigint, c6 bigint, c7 bigint, c8 bigint);
create table t2 (c1 integer, c2 integer, c3 integer, c4 integer, c5 integer, c6 integer);

insert ignore into t1 SELECT c1, weekofyear(`date`), weekofyear(`datetime`), weekofyear(`datetime4`), weekofyear(`timestamp`), weekofyear(`timestamp4`), weekofyear(`year`), weekofyear(`year4`) from t_time order by c1;
insert ignore into t2 SELECT c1, weekofyear(`char`), weekofyear(`varchar`), weekofyear(`binary`), weekofyear(`varbinary`), weekofyear(`text`) from t_char order by c1;
select * from t1 order by c1;
select * from t2 order by c1;

truncate t1;
truncate t2;
insert into t1 SELECT c1, weekofyear(`date`), weekofyear(`datetime`), weekofyear(`datetime4`), weekofyear(`timestamp`), weekofyear(`timestamp4`), weekofyear(`year`), weekofyear(`year4`) from t_time order by c1;
insert into t2 SELECT c1, weekofyear(`char`), weekofyear(`varchar`), weekofyear(`binary`), weekofyear(`varbinary`), weekofyear(`text`) from t_char order by c1;
select * from t1 order by c1;
select * from t2 order by c1;

-- test weekofyear insert ignore with strict_mode
set dolphin.sql_mode = '';
truncate t1;
truncate t2;
insert ignore into t1 SELECT c1, weekofyear(`date`), weekofyear(`datetime`), weekofyear(`datetime4`), weekofyear(`timestamp`), weekofyear(`timestamp4`), weekofyear(`year`), weekofyear(`year4`) from t_time order by c1;
insert ignore into t2 SELECT c1, weekofyear(`char`), weekofyear(`varchar`), weekofyear(`binary`), weekofyear(`varbinary`), weekofyear(`text`) from t_char order by c1;
select * from t1 order by c1;
select * from t2 order by c1;

truncate t1;
truncate t2;
insert into t1 SELECT c1, weekofyear(`date`), weekofyear(`datetime`), weekofyear(`datetime4`), weekofyear(`timestamp`), weekofyear(`timestamp4`), weekofyear(`year`), weekofyear(`year4`) from t_time order by c1;
insert into t2 SELECT c1, weekofyear(`char`), weekofyear(`varchar`), weekofyear(`binary`), weekofyear(`varbinary`), weekofyear(`text`) from t_char order by c1;
select * from t1 order by c1;
select * from t2 order by c1;

select TIME_FORMAT('838:59:59.999999','%f');
select TIME_FORMAT('837:59:59.999999','%f');
select TIME_FORMAT('-838:59:59.999999','%f');
select TIME_FORMAT('-837:59:59.999999','%f');
select TIMEDIFF(time'23:59:59','a0eebc99-9cOb-4ef8-bb6d-6bb9bd380a11');
select TIMEDIFF(time'23:59:59','01eebc99-9cOb-4ef8-bb6d-6bb9bd380a11');
select TIMEDIFF(time'23:59:59','a1eebc99');
select TIMEDIFF(time'23:59:59','1aeebc99');
select timestampadd(microsecond, 1,'0000-1-1 00:00:00');
select timestampadd(day,-1,'0001-01-01'); 
select timestampadd(week, 1, '0000-01-01');
select timestampadd(year, 1, '0000-01-01');
select timestampadd(month, 1, '0000-01-01');
select timestampadd(hour, 1, '0000-01-01');
select timestampadd(minute, 1, '0000-01-01');
select timestampadd(second, 1, '0000-01-01');

show dolphin.sql_mode;
reset dolphin.sql_mode;
create table test_diff(c1 time);
insert into test_diff select TIMEDIFF(time'23:59:59','a0eebc99-9cOb-4ef8-bb6d-6bb9bd380a11');
insert into test_diff select TIMEDIFF(time'23:59:59','01eebc99-9cOb-4ef8-bb6d-6bb9bd380a11');
insert into test_diff select TIMEDIFF(time'23:59:59','a1eebc99');
insert into test_diff select TIMEDIFF(time'23:59:59','1aeebc99');
insert ignore into test_diff select TIMEDIFF(time'23:59:59','a0eebc99-9cOb-4ef8-bb6d-6bb9bd380a11');
insert ignore into test_diff select TIMEDIFF(time'23:59:59','01eebc99-9cOb-4ef8-bb6d-6bb9bd380a11');
insert ignore into test_diff select TIMEDIFF(time'23:59:59','a1eebc99');
insert ignore into test_diff select TIMEDIFF(time'23:59:59','1aeebc99');
select * from test_diff;
drop table test_diff;

drop table t_time;
drop table t_char;
drop table t1;
drop table t2;

reset b_format_behavior_compat_options;
reset dolphin.sql_mode;

drop schema b_time_funcs2 cascade;
reset current_schema;
