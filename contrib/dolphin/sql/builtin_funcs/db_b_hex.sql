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

-- test for blob with hex format
set dolphin.b_compatibility_mode to on;
create table t1 (c1 tinyblob, c2 blob, c3 mediumblob, c4 longblob);
insert into t1 values('aa', 'aa', 'aa', 'aa');
insert into t1 values(12312, 12312, 12312, 12312);
select hex(c1) as "tinyblob_result", hex(c2) as "blob_result", hex(c3) as "mediumblob_result", hex(c4) as "longblob_result" from t1;

set b_format_behavior_compat_options=enable_set_variables;
set bytea_output=escape;

create table t_binary(
c1 int not null,
c2 binary,
c3 binary(10),
c4 binary(255),
c5 varbinary(1),
c6 varbinary(10),
c7 varbinary(255)) charset utf8mb3;

insert into t_binary values (
1, substr(hex('a熊猫竹竹爱吃竹子'),1,1), substr(hex('a熊猫竹竹爱吃竹子'),1,4), substr(repeat(hex('a熊猫竹竹爱吃竹子'), 10),1,255),
substr(hex('a熊猫竹竹爱吃竹子'),1,1), substr(hex('a熊猫竹竹爱吃竹子'),1,4), substr(repeat(hex('a熊猫竹竹爱吃竹子'), 10),1,255));

select c1, cast(c2 as char), cast(c3 as char), cast(c4 as char), cast(c5 as
char), cast(c6 as char), cast(c7 as char) from t_binary order by
1,2,3,4,5,6,7;

drop table if exists t_binary;
drop table if exists t1;
drop schema db_b_hex cascade;
reset current_schema;
