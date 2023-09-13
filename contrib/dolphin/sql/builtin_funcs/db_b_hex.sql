create schema db_b_hex;
set current_schema to 'db_b_hex';

select hex(int1(255));
select hex(int1(256));

-- max value for int32
select hex(2147483647);
-- max value for int64
select hex(9223372036854775807);
-- out of range for int64
select hex(9223372036854775808);
select hex(9223372036854775811);
select hex(1546546574654561321324564564543453);
-- test for negative numbers
select hex(-1);
select hex(-2);
select hex(-123123123);
select hex(-9223372036854775808);
select hex(-9223372036854775807);
select hex(-9223372036854775809);

-- test for float
select hex(12.34);
select hex(12.55);

-- test for bit
select hex(b'0');
select hex(b'1');
select hex(b'1111');
select hex(b'111100');
select hex(B'1110110');
select hex(B'1110110111111100001110111110111');
select hex(b'0000');
select hex(b'00001');
select hex(b'00000000');
select hex(b'000000001');
select hex(b'000000000000');
select hex(b'0000000000001');

select hex(TRUE);
select hex(FALSE);

select hex('abcde');
select hex('12.34');
select hex(NULL);
select hex('');

create table bytea_to_hex_test(c1 bytea);
insert into bytea_to_hex_test values ('123');
insert into bytea_to_hex_test values ('abc');
select hex(c1) from bytea_to_hex_test;

-- test for bytea with hex format
drop table if exists bytea_to_hex_test;
create table bytea_to_hex_test(c1 bytea);
insert into bytea_to_hex_test values (E'\\xDEADBEEF');
select hex(c1) from bytea_to_hex_test;

drop schema db_b_hex cascade;
reset current_schema;
