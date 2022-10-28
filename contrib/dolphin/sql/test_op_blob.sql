drop database if exists test_op_blob;
create database test_op_blob dbcompatibility 'b';
\c test_op_blob

select '1'::blob ^ '1'::blob;
select '1'::blob ^ '1'::char;
-- error: different length
select '1'::blob ^ '11'::varchar;
select '1'::blob ^ 11::int1;
select '1'::blob ^ 11::int2;
select '1'::blob ^ 11::int4;
select '1'::blob ^ 11::uint1;
select '1'::blob ^ 11::uint2;
select '1'::blob ^ 11::uint4;
select '1'::blob ^ 11.0;
select '1'::blob ^ 11::float;
select '1'::blob ^ 11::float8;
select '1'::blob ^ 11::numeric;

\c postgres
drop database test_op_blob;
