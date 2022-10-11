drop database if exists test_op_blob;
create database test_op_blob dbcompatibility 'b';
\c test_op_blob

select '1'::blob ^ '1'::blob;
select '1'::blob ^ '1'::char;
select '1'::blob ^ '1'::varchar;
select '1'::blob ^ 1::int1;
select '1'::blob ^ 1::int2;
select '1'::blob ^ 1::int4;
select '1'::blob ^ 1::uint1;
select '1'::blob ^ 1::uint2;
select '1'::blob ^ 1::uint4;
select '1'::blob ^ 1.0;
select '1'::blob ^ 1::float;
select '1'::blob ^ 1::float8;
select '1'::blob ^ 1::numeric;

\c postgres
drop database test_op_blob;