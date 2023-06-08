create schema test_bit_count;
set current_schema to 'test_bit_count';

-- 测试数字，字符串，二进制输入
SELECT bit_count(29);
SELECT bit_count('29');
SELECT bit_count(b'101010');

-- 测试数字越界（超过64无符号数范围）
select bit_count(18446744073709551614);
select bit_count(18446744073709551615);
select bit_count(18446744073709551616);
select bit_count(-9223372036854775807);
select bit_count(-9223372036854775808);
select bit_count(-9223372036854775809);

-- 测试字符串越界（超过64无符号数范围）
select bit_count('18446744073709551614');
select bit_count('18446744073709551615');
select bit_count('18446744073709551616');
select bit_count('-9223372036854775807');
select bit_count('-9223372036854775808');
select bit_count('-9223372036854775809');

set dolphin.sql_mode = '';
select bit_count('18446744073709551614');
select bit_count('18446744073709551615');
select bit_count('18446744073709551616');
select bit_count('-9223372036854775807');
select bit_count('-9223372036854775808');
select bit_count('-9223372036854775809');
select bit_count('1.21');
select bit_count('-1.21');

-- 测试bit越界（超过64位）
select bit_count(b'111111111111111111111111111111111111111111111111111111111111111');
select bit_count(b'1111111111111111111111111111111111111111111111111111111111111111');
select bit_count(b'10000000111111111111111111111111111111111111111111111111111111111111111');

set dolphin.sql_mode = '';

-- 测试数字越界（超过64无符号数范围）
select bit_count(18446744073709551614);
select bit_count(18446744073709551615);
select bit_count(18446744073709551616);
select bit_count(-9223372036854775807);
select bit_count(-9223372036854775808);
select bit_count(-9223372036854775809);

-- 测试字符串越界（超过64无符号数范围）
select bit_count('18446744073709551614');
select bit_count('18446744073709551615');
select bit_count('18446744073709551616');
select bit_count('-9223372036854775807');
select bit_count('-9223372036854775808');
select bit_count('-9223372036854775809');

-- 测试bit越界（超过64位）
select bit_count(b'111111111111111111111111111111111111111111111111111111111111111');
select bit_count(b'1111111111111111111111111111111111111111111111111111111111111111');
select bit_count(b'10000000111111111111111111111111111111111111111111111111111111111111111');

select bit_count('2022-12-12'::date);

drop schema test_bit_count cascade;
reset current_schema;