drop database if exists to_base64_test;
create database to_base64_test dbcompatibility 'b';
\c to_base64_test

--测试字符串作为输入，返回base64编码的编码结果
SELECT TO_BASE64('123456');
SELECT TO_BASE64('12345');
SELECT TO_BASE64('1234');
SELECT TO_BASE64('to_base64');
SELECT FROM_BASE64(TO_BASE64('123456'));
SELECT FROM_BASE64(TO_BASE64('to_base64'));

--测试数字作为输入，将数字变成字符串后，返回base64编码的编码结果
SELECT TO_BASE64(11111111111111111111111111111111111111);
SELECT TO_BASE64('11111111111111111111111111111111111111');

--测试NULL作为输入，返回空值
SELECT TO_BASE64(NULL);

--测试bit类型、bool类型、date类型作为输入
SELECT TO_BASE64(b'01100001011000100110001101100100');
SELECT TO_BASE64('abcd');
SELECT TO_BASE64(0);
SELECT TO_BASE64(1);
SELECT TO_BASE64('0');
SELECT TO_BASE64('1');
SELECT TO_BASE64(FALSE);
SELECT TO_BASE64(TRUE);
SELECT TO_BASE64('2022-10-31');

-- 测试table类型作为输入
CREATE TABLE test_base64 (name text);
INSERT INTO test_base64 values('123456'), ('to_base64');
SELECT TO_BASE64(name) from test_base64;

\c postgres
drop database if exists to_base64_test;
