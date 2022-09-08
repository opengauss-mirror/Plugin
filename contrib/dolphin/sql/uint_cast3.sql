drop database if exists uint_cast3;
create database uint_cast3 dbcompatibility 'b';
\c uint_cast3
select 1::uint1::int16;
select 1::int16::uint1;
select 1::uint2::int16;
select 1::int16::uint2;
select 1::uint4::int16;
select 1::int16::uint4;
select 1::uint8::int16;
select 1::int16::uint8;

select 1::uint1::clob;
select 1::clob::uint1;
select 1::uint2::clob;
select 1::clob::uint2;
select 1::uint4::clob;
select 1::clob::uint4;
select 1::uint8::clob;
select 1::clob::uint8;

select 1::uint4::abstime;
select 1::abstime::uint4;

select 1::uint4::reltime;
select 1::reltime::uint4;

select 1::uint1::bpchar;
select 1::bpchar::uint1;
select 1::uint2::bpchar;
select 1::bpchar::uint2;
select 1::uint4::bpchar;
select 1::bpchar::uint4;
select 1::uint8::bpchar;
select 1::bpchar::uint8;

select 1::uint1::varchar;
select 1::varchar::uint1;
select 1::uint2::varchar;
select 1::varchar::uint2;
select 1::uint4::varchar;
select 1::varchar::uint4;
select 1::uint8::varchar;
select 1::varchar::uint8;

select 1::uint4::bit;
select 1::bit::uint4;
select 1::uint8::bit;
select 1::bit::uint8;

select 1::uint1::nvarchar2;
select 1::nvarchar2::uint1;
select 1::uint2::nvarchar2;
select 1::nvarchar2::uint2;
select 1::uint4::nvarchar2;
select 1::nvarchar2::uint4;
select 1::uint8::nvarchar2;
select 1::nvarchar2::uint8;

select 1::uint1::money;
select 1::uint2::money;;
select 1::uint4::money;
select 1::uint8::money;

select 1::uint1::interval;
select 1::uint2::interval;;
select 1::uint4::interval;

select 151503::uint4::time;
select 19990101::uint4::date;

select 19990101::uint4::timestamp;
select 19990101::uint4::timestamptz;
select 19990101::uint4::datetime;

select 19990101222222::uint8::timestamp;
select 19990101222222::uint8::timestamptz;
select 19990101222222::uint8::datetime;

select 1999::uint4::year;
select 1999::year::uint4;

\c postgres
drop database uint_cast3;