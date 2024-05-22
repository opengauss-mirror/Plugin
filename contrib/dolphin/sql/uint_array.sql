create schema uint_array;
set current_schema to 'uint_array';

CREATE TABLE t1(`uint1_a` _uint1, `uint2_a` _uint2, `uint4_a` _uint4, `uint8_a` _uint8);
CREATE TABLE t2(`uint1_a` uint1[], `uint2_a` uint2[], `uint4_a` uint4[], `uint8_a` uint8[]);
\d t1;
\d t2;
DROP TABLE t2;
INSERT INTO t1 values(array[0,1,2,127], array[0,1,2,255], array[0,3,5,25456], array[234324,324324,543543,123234]);
SELECT * FROM t1;
select `uint1_a` :: tinyint[] from t1;
select `uint1_a` :: smallint[] from t1;
select `uint1_a` :: int[] from t1;
select `uint1_a` :: bigint[] from t1;
select `uint2_a` :: tinyint[] from t1;
select `uint2_a` :: smallint[] from t1;
select `uint2_a` :: int[] from t1;
select `uint2_a` :: bigint[] from t1;
select `uint4_a` :: tinyint[] from t1;
select `uint4_a` :: smallint[] from t1;
select `uint4_a` :: int[] from t1;
select `uint4_a` :: bigint[] from t1;
select `uint8_a` :: tinyint[] from t1;
select `uint8_a` :: smallint[] from t1;
select `uint8_a` :: int[] from t1;
select `uint8_a` :: bigint[] from t1;
DROP TABLE t1;

select a.oid=b.typarray from pg_type a, pg_type b where (a.typname='_uint1' and b.typname='uint1');
select a.oid=b.typarray from pg_type a, pg_type b where (a.typname='_uint2' and b.typname='uint2');
select a.oid=b.typarray from pg_type a, pg_type b where (a.typname='_uint4' and b.typname='uint4');
select a.oid=b.typarray from pg_type a, pg_type b where (a.typname='_uint8' and b.typname='uint8');

drop schema uint_array cascade;
reset current_schema;