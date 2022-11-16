drop database if exists test_bit_count;
create database test_bit_count dbcompatibility 'b';
\c test_bit_count

-- 测试数字，字符串，二进制输入
SELECT bit_count(29);
SELECT bit_count('29');
SELECT bit_count(b'101010');

-- 测试数字越界（超过64无符号数范围）
select bit_count(18446744073709551614);
select bit_count(18446744073709551615);
select bit_count(18446744073709551616);

-- 测试字符串越界（超过64无符号数范围）
select bit_count('18446744073709551614');
select bit_count('18446744073709551615');
select bit_count('18446744073709551616');

-- 测试bit越界（超过64位）
select bit_count(b'111111111111111111111111111111111111111111111111111111111111111');
select bit_count(b'1111111111111111111111111111111111111111111111111111111111111111');
select bit_count(b'10000000111111111111111111111111111111111111111111111111111111111111111');

\c postgres
drop database test_bit_count;