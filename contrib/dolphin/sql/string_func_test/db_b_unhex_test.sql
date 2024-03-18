create schema unhex_test;
set current_schema to 'unhex_test';
-- 非严格模式，参数不合法，报warning，返回null
set dolphin.sql_mode = 'sql_mode_full_group';
--测试字符串作为输入，返回十六进制编码的编码结果
SELECT UNHEX('6f70656e4761757373');
SELECT UNHEX(HEX('string'));
SELECT HEX(UNHEX('1267'));
SELECT UNHEX(HEX('测试'));
SELECT UNHEX('');

--测试非十六进制符号和二进制类型作为输入，返回NULL
SELECT UNHEX('GG');
SELECT UNHEX('4142'::bytea);

--测试十六进制串字符数为奇数的情况，在高位补0后返回解码结果
SELECT UNHEX('941424344');
SELECT UNHEX('0941424344');

--测试numeric作为输入，返回十六进制编码的编码结果，这里是的数字是十六进制编码，若想将十六进制数转为十进制数，需使用其它函数
SELECT UNHEX('41424344454647484950515253545556575859');
SELECT UNHEX(41424344454647484950515253545556575859);

--测试日期类型、bool类型、空值作为输入
SELECT UNHEX('2022-12-31'::date);
SELECT HEX(UNHEX(TRUE));
SELECT HEX(UNHEX(FALSE));
SELECT UNHEX(NULL);

--测试表格类型作为输入
CREATE TABLE test_unhex (name text);
INSERT INTO test_unhex values('4142'), ('6f70656e4761757373');
SELECT UNHEX(name) from test_unhex;

set bytea_output = escape;
SELECT unhex(32) ^ 3;
SELECT unhex(35 + 1), unhex(33 - 1), unhex(12 * 3);

-- 严格模式，参数不合法，报error
set dolphin.sql_mode = 'sql_mode_strict';
SELECT UNHEX('GG');

select +unhex(36),-unhex('43');

drop schema unhex_test cascade;
reset current_schema;
