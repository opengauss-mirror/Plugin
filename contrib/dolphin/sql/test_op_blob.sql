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

-- test blob op numeric
create table blob_table(`tinyblob` tinyblob,`blob` blob, `mediumblob` mediumblob, `longblob` longblob);
insert into blob_table values('1.23', '1.23', '1.23', '1.23'), ('1.0', '1.0', '1.0', '1.0'), ('1', '1', '1', '1');
create table res_table(res boolean);

select `tinyblob` < 1.1::float8 from blob_table;
select `blob` <= 1::int1 from blob_table;
select `mediumblob` > 1::numeric from blob_table;
insert into res_table select 1<>`longblob` from blob_table;
select * from res_table;
insert into blob_table values('a', 'a', 'a', 'a');
select `blob` = 1.1 from blob_table;
select `blob` = '1.0' from blob_table;
select `tinyblob` < 1.1::float8 from blob_table;
select `blob` <= 1::int1 from blob_table;
select `mediumblob` > 1::numeric from blob_table;
insert into res_table select 1<>`longblob` from blob_table;
select * from res_table;

drop schema test_op_blob cascade;
reset current_schema;
