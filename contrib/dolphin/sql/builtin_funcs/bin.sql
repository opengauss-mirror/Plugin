create schema db_bin;
set current_schema to 'db_bin';
select bin(1);
select bin(0);
select bin('2');
select bin('a');
select bin('11a');
select bin(1.1);
select bin(null);
select bin(true);
select bin(false);
select bin('测试');

-- 测试小数、分数
SELECT BIN('9.2');
SELECT BIN('9.6');
SELECT BIN('-9.2');
SELECT BIN('-9.6');

-- 测试特殊符号
SELECT BIN('+19.6');
SELECT BIN('a9');
SELECT BIN('!9');
SELECT BIN('@#9');

SELECT BIN(9.2);
SELECT BIN(9.6);
SELECT BIN(-9.2);
SELECT BIN(-9.6);
select BIN(-13/4);
drop schema db_bin cascade;
reset current_schema;
