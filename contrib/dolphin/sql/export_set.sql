create schema export_set;
set current_schema to 'export_set';

-- 测试缺省值
SELECT EXPORT_SET(5,'Y','N',',',5);
SELECT EXPORT_SET(5,'Y','N',',');
SELECT EXPORT_SET(5,'Y','N');

-- 测试上界
SELECT EXPORT_SET(9223372036854775807,'Y','N',',',64);
SELECT EXPORT_SET(9223372036854775808,'Y','N',',',64);
SELECT EXPORT_SET(9223372036854775809,'Y','N',',',64);

-- 测试下界
SELECT EXPORT_SET(-9223372036854775807,'Y','N',',',64);
SELECT EXPORT_SET(-9223372036854775808,'Y','N',',',64);
SELECT EXPORT_SET(-9223372036854775809,'Y','N',',',64);

-- 测试长度参数
SELECT EXPORT_SET(5,'Y','N',',',64);
SELECT EXPORT_SET(5,'Y','N',',',65);
SELECT EXPORT_SET(5,'Y','N',',',-1);

-- 测试字符串
SELECT EXPORT_SET(5,'YYYYYYYYYYYYYYYY','N',',',5);
SELECT EXPORT_SET(5,'Y','NNNNNNNNNNNNNNN',',',5);
SELECT EXPORT_SET(5,'Y','N',',,,,,,,,,,,,',5);

select export_set(7,'q',b'111',',',5);
select export_set(7,b'111','111',',',5);
select export_set(7,'q','111',b'111',5);

select export_set(7,'q',True,',',5);
select export_set(7,True,'111',',',5);
select export_set(7,'q','111',False,5);

select export_set(7,'q',True,',',5);
select export_set(7,True,'111',',',5);
select export_set(7,'q','111',False,5);

drop schema export_set cascade;
reset current_schema;