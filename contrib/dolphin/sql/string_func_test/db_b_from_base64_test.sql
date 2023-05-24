create schema from_base64;
set current_schema to 'from_base64';

--测试正常base64编码作为输入，返回base64编码的解码结果
SELECT FROM_BASE64('YWJj');
SELECT FROM_BASE64('MTIzNDU2');
SELECT FROM_BASE64(TO_BASE64('测试'));

--测试非base64编码和NULL作为输入，均返回空值
SELECT FROM_BASE64('asjeifj');
SELECT FROM_BASE64(NULL);

--测试bit类型、bool类型、numeric类型、date类型作为输入
SELECT FROM_BASE64(b'01011001010101110100101001101010');
SELECT FROM_BASE64(TRUE);
SELECT FROM_BASE64(FALSE);
SELECT TO_BASE64(FROM_BASE64(123456781234567812345678));
SELECT TO_BASE64(FROM_BASE64('123456781234567812345678'));
SELECT FROM_BASE64(now());

-- 测试table类型作为输入
CREATE TABLE test_base64 (name text);
INSERT INTO test_base64 values('YWJj'), ('MTIzNDU2'), ('asjeifj'), (from_base64('as!@#$'));
SELECT FROM_BASE64(name) from test_base64;
SELECT * FROM test_base64 WHERE name IS NULL;

drop schema from_base64 cascade;
reset current_schema;