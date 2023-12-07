create schema uint_cast2;
set current_schema to 'uint_cast2';
set dolphin.b_compatibility_mode to on;

drop table if exists t1 ;
create table t1(a uint8);

insert into t1 values(1::text);
insert into t1 values(0::text);
insert into t1 values((-1)::text);

insert into t1 values(1::bool);
insert into t1 values(0::bool);
insert into t1 values((-1)::bool);

insert into t1 values(1.0::float4);
insert into t1 values(0.0::float4);
insert into t1 values((-1.0)::float4);
insert into t1 values(9223372036854775807.0::float4);
insert into t1 values(9223372036854775808.0::float4);

insert into t1 values(1.0::float8);
insert into t1 values(0.0::float8);
insert into t1 values((-1.0)::float8);
insert into t1 values(18446744073709550515.0::float8);
insert into t1 values(18446744073709550515.0::float8);
insert into t1 values(1::numeric);
insert into t1 values(0::numeric);
insert into t1 values((-1)::numeric);
insert into t1 values(18446744073709550515::numeric);
insert into t1 values(18446744073709550516::numeric);

insert into t1 values(1::int8);
insert into t1 values(0::int8);
insert into t1 values((-1)::int8);
insert into t1 values(9223372036854775807::int8);
insert into t1 values(9223372036854775808::int8);

insert into t1 values(1::int4);
insert into t1 values(0::int4);
insert into t1 values((-1)::int4);
insert into t1 values(2147483647::int4);
insert into t1 values(2147483648::int4);

insert into t1 values(1::int2);
insert into t1 values(0::int2);
insert into t1 values((-1)::int2);
insert into t1 values(32767::int2);
insert into t1 values(32768::int2);


insert into t1 values(1::int1);
insert into t1 values(0::int1);
insert into t1 values((-1)::int1);
insert into t1 values(127::int1);
insert into t1 values(256::int1);

insert into t1 values(1::uint4);
insert into t1 values(0::uint4);
insert into t1 values((-1)::uint4);
insert into t1 values(4294967295::uint4);
insert into t1 values(4294967296::uint4);

insert into t1 values(1::uint2);
insert into t1 values(0::uint2);
insert into t1 values((-1)::uint2);
insert into t1 values(65535::uint2);
insert into t1 values(65536::uint2);

insert into t1 values(1::uint1);
insert into t1 values(0::uint1);
insert into t1 values((-1)::uint1);
insert into t1 values(255::uint1);
insert into t1 values(256::uint1);

insert into t1 values('-1');
insert into t1 values('-1'::time);
insert into t1 values('-1'::text);
insert into t1 values('-1'::char(10));
insert into t1 values('-1'::varchar(10));
insert into t1 values('-1'::binary(10));
insert into t1 values('-1'::varbinary(10));
insert into t1 values('-1'::tinyblob);
insert into t1 values('-1'::blob);
insert into t1 values('-1'::mediumblob);
insert into t1 values('-1'::longblob);
insert into t1 values('-1'::json);

set dolphin.sql_mode = '';
delete from t1;
insert into t1 values('-1');
insert into t1 values('-1'::time);
insert into t1 values('-1'::text);
insert into t1 values('-1'::char(10));
insert into t1 values('-1'::varchar(10));
insert into t1 values('-1'::binary(10));
insert into t1 values('-1'::varbinary(10));
insert into t1 values('-1'::tinyblob);
insert into t1 values('-1'::blob);
insert into t1 values('-1'::mediumblob);
insert into t1 values('-1'::longblob);
insert into t1 values('-1'::json);
select * from t1;
reset dolphin.sql_mode;

drop table if exists t1 ;

create table t1(a uint4);

insert into t1 values(1::text);
insert into t1 values(0::text);
insert into t1 values((-1)::text);

insert into t1 values(1::bool);
insert into t1 values(0::bool);
insert into t1 values((-1)::bool);

insert into t1 values(1.0::float4);
insert into t1 values(0.0::float4);
insert into t1 values((-1.0)::float4);
insert into t1 values(4294967295::float4);
insert into t1 values(4294967296::float4);

insert into t1 values(1.0::float8);
insert into t1 values(0.0::float8);
insert into t1 values((-1.0)::float8);
insert into t1 values(4294967295::float8);
insert into t1 values(4294967296::float8);
insert into t1 values(1::numeric);
insert into t1 values(0::numeric);
insert into t1 values((-1)::numeric);
insert into t1 values(4294967295::numeric);
insert into t1 values(4294967296::numeric);

insert into t1 values(1::int8);
insert into t1 values(0::int8);
insert into t1 values((-1)::int8);
insert into t1 values(9223372036854775807::int8);
insert into t1 values(9223372036854775808::int8);
insert into t1 values(4294967295::int8);
insert into t1 values(4294967296::int8);

insert into t1 values(1::int4);
insert into t1 values(0::int4);
insert into t1 values((-1)::int4);
insert into t1 values(2147483647::int4);
insert into t1 values(2147483648::int4);

insert into t1 values(1::int2);
insert into t1 values(0::int2);
insert into t1 values((-1)::int2);
insert into t1 values(32767::int2);
insert into t1 values(32768::int2);


insert into t1 values(1::int1);
insert into t1 values(0::int1);
insert into t1 values((-1)::int1);
insert into t1 values(127::int1);
insert into t1 values(256::int1);


insert into t1 values(1::uint8);
insert into t1 values(0::uint8);
insert into t1 values((-1)::uint8);
insert into t1 values(4294967295::uint8);
insert into t1 values(4294967296::uint8);

insert into t1 values(1::uint2);
insert into t1 values(0::uint2);
insert into t1 values((-1)::uint2);
insert into t1 values(65535::uint2);
insert into t1 values(65536::uint2);

insert into t1 values(1::uint1);
insert into t1 values(0::uint1);
insert into t1 values((-1)::uint1);
insert into t1 values(255::uint1);
insert into t1 values(256::uint1);

insert into t1 values('-1');
insert into t1 values('-1'::time);
insert into t1 values('-1'::text);
insert into t1 values('-1'::char(10));
insert into t1 values('-1'::varchar(10));
insert into t1 values('-1'::binary(10));
insert into t1 values('-1'::varbinary(10));
insert into t1 values('-1'::tinyblob);
insert into t1 values('-1'::blob);
insert into t1 values('-1'::mediumblob);
insert into t1 values('-1'::longblob);
insert into t1 values('-1'::json);

set dolphin.sql_mode = '';
delete from t1;
insert into t1 values('-1');
insert into t1 values('-1'::time);
insert into t1 values('-1'::text);
insert into t1 values('-1'::char(10));
insert into t1 values('-1'::varchar(10));
insert into t1 values('-1'::binary(10));
insert into t1 values('-1'::varbinary(10));
insert into t1 values('-1'::tinyblob);
insert into t1 values('-1'::blob);
insert into t1 values('-1'::mediumblob);
insert into t1 values('-1'::longblob);
insert into t1 values('-1'::json);
select * from t1;
reset dolphin.sql_mode;

drop table if exists t1 ;


create table t1(a uint2);

insert into t1 values(1::text);
insert into t1 values(0::text);
insert into t1 values((-1)::text);

insert into t1 values(1::bool);
insert into t1 values(0::bool);
insert into t1 values((-1)::bool);

insert into t1 values(1.0::float4);
insert into t1 values(0.0::float4);
insert into t1 values((-1.0)::float4);
insert into t1 values(65535::float4);
insert into t1 values(65536::float4);

insert into t1 values(1.0::float8);
insert into t1 values(0.0::float8);
insert into t1 values((-1.0)::float8);
insert into t1 values(65535::float8);
insert into t1 values(65536::float8);

insert into t1 values(1::numeric);
insert into t1 values(0::numeric);
insert into t1 values((-1)::numeric);
insert into t1 values(65535::numeric);
insert into t1 values(65536::numeric);


insert into t1 values(1::int8);
insert into t1 values(0::int8);
insert into t1 values((-1)::int8);
insert into t1 values(9223372036854775807::int8);
insert into t1 values(9223372036854775808::int8);
insert into t1 values(65535::int8);
insert into t1 values(65536::int8);

insert into t1 values(1::int4);
insert into t1 values(0::int4);
insert into t1 values((-1)::int4);
insert into t1 values(2147483647::int4);
insert into t1 values(2147483648::int4);
insert into t1 values(65535::int4);
insert into t1 values(65536::int4);

insert into t1 values(1::int2);
insert into t1 values(0::int2);
insert into t1 values((-1)::int2);
insert into t1 values(32767::int2);
insert into t1 values(32768::int2);

insert into t1 values(1::int1);
insert into t1 values(0::int1);
insert into t1 values((-1)::int1);
insert into t1 values(127::int1);
insert into t1 values(256::int1);


insert into t1 values(1::uint8);
insert into t1 values(0::uint8);
insert into t1 values((-1)::uint8);
insert into t1 values(4294967295::uint8);
insert into t1 values(4294967296::uint8);
insert into t1 values(65535::uint8);
insert into t1 values(65536::uint8);

insert into t1 values(1::uint4);
insert into t1 values(0::uint4);
insert into t1 values((-1)::uint4);
insert into t1 values(4294967295::uint4);
insert into t1 values(4294967296::uint4);
insert into t1 values(65535::uint4);
insert into t1 values(65536::uint4);

insert into t1 values(1::uint1);
insert into t1 values(0::uint1);
insert into t1 values((-1)::uint1);
insert into t1 values(255::uint1);
insert into t1 values(256::uint1);

insert into t1 values('-1');
insert into t1 values('-1'::time);
insert into t1 values('-1'::text);
insert into t1 values('-1'::char(10));
insert into t1 values('-1'::varchar(10));
insert into t1 values('-1'::binary(10));
insert into t1 values('-1'::varbinary(10));
insert into t1 values('-1'::tinyblob);
insert into t1 values('-1'::blob);
insert into t1 values('-1'::mediumblob);
insert into t1 values('-1'::longblob);
insert into t1 values('-1'::json);

set dolphin.sql_mode = '';
delete from t1;
insert into t1 values('-1');
insert into t1 values('-1'::time);
insert into t1 values('-1'::text);
insert into t1 values('-1'::char(10));
insert into t1 values('-1'::varchar(10));
insert into t1 values('-1'::binary(10));
insert into t1 values('-1'::varbinary(10));
insert into t1 values('-1'::tinyblob);
insert into t1 values('-1'::blob);
insert into t1 values('-1'::mediumblob);
insert into t1 values('-1'::longblob);
insert into t1 values('-1'::json);
select * from t1;
reset dolphin.sql_mode;

drop table if exists t1 ;

create table t1(a uint1);

insert into t1 values(1::text);
insert into t1 values(0::text);
insert into t1 values((-1)::text);

insert into t1 values(1::int8);
insert into t1 values(0::int8);
insert into t1 values((-1)::int8);
insert into t1 values(9223372036854775807::int8);
insert into t1 values(9223372036854775808::int8);
insert into t1 values(255::int8);
insert into t1 values(256::int8);

insert into t1 values(1::bool);
insert into t1 values(0::bool);
insert into t1 values((-1)::bool);

insert into t1 values(1.0::float4);
insert into t1 values(0.0::float4);
insert into t1 values((-1.0)::float4);
insert into t1 values(255::float4);
insert into t1 values(256::float4);

insert into t1 values(1.0::float8);
insert into t1 values(0.0::float8);
insert into t1 values((-1.0)::float8);
insert into t1 values(255::float8);
insert into t1 values(256::float8);

insert into t1 values(1::numeric);
insert into t1 values(0::numeric);
insert into t1 values((-1)::numeric);
insert into t1 values(255::numeric);
insert into t1 values(256::numeric);

insert into t1 values(1::int4);
insert into t1 values(0::int4);
insert into t1 values((-1)::int4);
insert into t1 values(2147483647::int4);
insert into t1 values(2147483648::int4);
insert into t1 values(255::int4);
insert into t1 values(256::int4);

insert into t1 values(1::int2);
insert into t1 values(0::int2);
insert into t1 values((-1)::int2);
insert into t1 values(32767::int2);
insert into t1 values(32768::int2);
insert into t1 values(255::int4);
insert into t1 values(256::int4);

insert into t1 values(1::int1);
insert into t1 values(0::int1);
insert into t1 values((-1)::int1);
insert into t1 values(127::int1);
insert into t1 values(256::int1);

insert into t1 values(1::uint8);
insert into t1 values(0::uint8);
insert into t1 values((-1)::uint8);
insert into t1 values(4294967295::uint8);
insert into t1 values(4294967296::uint8);
insert into t1 values(255::uint8);
insert into t1 values(256::uint8);

insert into t1 values(1::uint4);
insert into t1 values(0::uint4);
insert into t1 values((-1)::uint4);
insert into t1 values(4294967295::uint4);
insert into t1 values(4294967296::uint4);
insert into t1 values(255::uint4);
insert into t1 values(256::uint4);

insert into t1 values(1::uint1);
insert into t1 values(0::uint1);
insert into t1 values((-1)::uint1);
insert into t1 values(255::uint1);
insert into t1 values(256::uint1);

insert into t1 values('-1');
insert into t1 values('-1'::time);
insert into t1 values('-1'::text);
insert into t1 values('-1'::char(10));
insert into t1 values('-1'::varchar(10));
insert into t1 values('-1'::binary(10));
insert into t1 values('-1'::varbinary(10));
insert into t1 values('-1'::tinyblob);
insert into t1 values('-1'::blob);
insert into t1 values('-1'::mediumblob);
insert into t1 values('-1'::longblob);
insert into t1 values('-1'::json);

set dolphin.sql_mode = '';
delete from t1;
insert into t1 values('-1');
insert into t1 values('-1'::time);
insert into t1 values('-1'::text);
insert into t1 values('-1'::char(10));
insert into t1 values('-1'::varchar(10));
insert into t1 values('-1'::binary(10));
insert into t1 values('-1'::varbinary(10));
insert into t1 values('-1'::tinyblob);
insert into t1 values('-1'::blob);
insert into t1 values('-1'::mediumblob);
insert into t1 values('-1'::longblob);
insert into t1 values('-1'::json);
select * from t1;
reset dolphin.sql_mode;

drop table if exists t1;

--test for db core
select varlena_cast_ui1(1);
select varlena_cast_ui2(1);
select varlena_cast_ui4(1);
select varlena_cast_ui8(1);
select varlena_cast_int8(1);
select varlena2float8(1);
select varlena2numeric(1);
select Varlena2Bpchar(1);
select Varlena2Varchar(1);
select Varlena2Text(1);
select varlena2bit(1,10);

create OR REPLACE procedure t_p(out ret varchar) as
begin
select '{"a":"b"}'::json into ret;
end;
/
select t_p();
drop procedure t_p;

drop schema uint_cast2 cascade;
reset current_schema;