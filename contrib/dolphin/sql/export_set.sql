drop database if exists export_set;
create database export_set dbcompatibility 'b';
\c export_set

-- 测试缺省值
SELECT EXPORT_SET(5,'Y','N',',',5);
SELECT EXPORT_SET(5,'Y','N',',');
SELECT EXPORT_SET(5,'Y','N');

-- 测试上界
SELECT EXPORT_SET(18446744073709551615,'Y','N',',',64);
SELECT EXPORT_SET(18446744073709551616,'Y','N',',',64);
SELECT EXPORT_SET(18446744073709551617,'Y','N',',',64);

-- 测试下界
SELECT EXPORT_SET(-9223372036854775807,'Y','N',',',64);
SELECT EXPORT_SET(-9223372036854775808,'Y','N',',',64);
SELECT EXPORT_SET(-9223372036854775809,'Y','N',',',64);

-- 测试超长参数
SELECT EXPORT_SET(-111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111,'Y','N',',',64);
SELECT EXPORT_SET(111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111,'Y','N',',',64);

-- 测试长度参数
SELECT EXPORT_SET(5,'Y','N',',',64);
SELECT EXPORT_SET(5,'Y','N',',',65);
SELECT EXPORT_SET(5,'Y','N',',',-1);
SELECT EXPORT_SET(5,'Y','N',',',111111111111111111111111111111111111111111111111111111111111111111111111111111111);
SELECT EXPORT_SET(5,'Y','N',',',-111111111111111111111111111111111111111111111111111111111111111111111111111111111);

-- 测试字符串
SELECT EXPORT_SET(5,'YYYYYYYYYYYYYYYY','N',',',5);
SELECT EXPORT_SET(5,'Y','NNNNNNNNNNNNNNN',',',5);
SELECT EXPORT_SET(5,'Y','N',',,,,,,,,,,,,',5);

\c postgres
drop database if exists export_set;