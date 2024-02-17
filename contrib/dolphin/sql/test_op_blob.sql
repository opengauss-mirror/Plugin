create schema test_op_blob;
set current_schema to 'test_op_blob';

set dolphin.b_compatibility_mode to on;
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

drop schema test_op_blob cascade;
reset current_schema;
