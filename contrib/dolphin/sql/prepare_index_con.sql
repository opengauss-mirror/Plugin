create schema prepare_index_con;
set current_schema to 'prepare_index_con';

create table test1(c1 int1, c2 int2, c3 int4, c4 int8, c5 uint1, c6 uint2, c7 uint4, c8 uint8);

insert into test1 values (generate_series(0, 100), generate_series(0, 100), generate_series(0, 100), generate_series(0, 100), generate_series(0, 100), generate_series(0, 100), generate_series(0, 100), generate_series(0, 100));


create index idx1 on test1(c1);
create index idx2 on test1(c2);
create index idx3 on test1(c3);
create index idx4 on test1(c4);
create index idx5 on test1(c5);
create index idx6 on test1(c6);
create index idx7 on test1(c7);
create index idx8 on test1(c8);
create index idx9 on test1(c2, c3);
create index idx10 on test1(c2, c3, c4);
create index idx11 on test1(c4, c3, c2);
analyze test1;

set enable_seqscan = off;
set dolphin.transform_unknown_param_type_as_column_type_first = true;

prepare s1 as select * from test1 where c1 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where ? = c1;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where c2 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where ? = c2;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where c3 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where ? = c3;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where c4 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where ? = c4;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where c5 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where ? = c5;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where c6 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where ? = c6;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where c7 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where ? = c7;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where c8 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;

prepare s1 as select * from test1 where ? = c8;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;


prepare s1 as select * from test1 where c2 = ? and c3 = ? and c4 = ?;
explain (costs off) execute s1 using 1, 2, 3;
explain (costs off) execute s1 using 1.23, 1.23, 1.23;

prepare s1 as select * from test1 where c4 = ? and c3 = ? and c2 = ?;
explain (costs off) execute s1 using 1, 2, 3;
explain (costs off) execute s1 using 1.23, 1.23, 1.23;

prepare s1 as select * from test1 where c2 = ? and c3 = ?;
explain (costs off) execute s1 using 1, 2;
explain (costs off) execute s1 using 1.23, 1.23;


prepare s1 as select * from test1 where c2 = ? and c4 = ?;
explain (costs off) execute s1 using 1, 2;
explain (costs off) execute s1 using 1.23, 1.23;


prepare s1 as select * from test1 where c2 = ? + ? - ?;
explain (costs off) execute s1 using 1, 2, 3;
explain (costs off) execute s1 using 1.23, 1.23, 1.23;



create table test_type_table
(
   a_int1 tinyint,
   a_uint1 tinyint unsigned,
   a_int2 smallint,
   a_uint2 smallint unsigned,
   a_int4 integer,
   a_uint4 integer unsigned,
   a_int8 bigint,
   a_uint8 bigint unsigned,
   a_float4 float4,
   a_float8 float8,
   a_numeric decimal(20, 6),
   a_bit1 bit(1),
   a_bit64 bit(64),
   a_boolean boolean,
   a_date date,
   a_time time,
   a_datetime datetime,
   a_timestamp timestamp,
   a_year year,
   a_char char(100),
   a_varchar varchar(100), 
   a_binary binary(100),
   a_varbinary varbinary(100),
   a_tinyblob tinyblob,
   a_blob blob,
   a_mediumblob mediumblob,
   a_longblob longblob,
   a_text text 
);

insert into test_type_table values (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, b'1', b'111', true, '2023-02-05', '19:10:50', '2023-02-05 19:10:50', '2023-02-05 19:10:50', '2023', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a');


create index a_id1 on test_type_table(a_int1);
create index a_id2 on test_type_table(a_uint1);
create index a_id3 on test_type_table(a_int2);
create index a_id4 on test_type_table(a_uint2);
create index a_id5 on test_type_table(a_int4);
create index a_id6 on test_type_table(a_uint4);
create index a_id7 on test_type_table(a_int8);
create index a_id8 on test_type_table(a_uint8);
create index a_id9 on test_type_table(a_float4);
create index a_id10 on test_type_table(a_float8);
create index a_id12 on test_type_table(a_numeric);
create index a_id13 on test_type_table(a_bit1);
create index a_id14 on test_type_table(a_bit64);
create index a_id15 on test_type_table(a_boolean);
create index a_id16 on test_type_table(a_date);
create index a_id17 on test_type_table(a_time);
create index a_id18 on test_type_table(a_datetime);
create index a_id18 on test_type_table(a_timestamp);
create index a_id18 on test_type_table(a_year);
create index a_id18 on test_type_table(a_char);
create index a_id18 on test_type_table(a_varchar);
create index a_id18 on test_type_table(a_binary);
create index a_id18 on test_type_table(a_varbinary);
create index a_id18 on test_type_table(a_tinyblob);
create index a_id18 on test_type_table(a_blob);
create index a_id18 on test_type_table(a_mediumblob);
create index a_id18 on test_type_table(a_longblob);
create index a_id18 on test_type_table(a_text);


prepare s1 as select * from test_type_table where a_int1 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_uint1 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_int2 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_uint2 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';


prepare s1 as select * from test_type_table where a_int4 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_uint4 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_int8 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';


prepare s1 as select * from test_type_table where a_uint8 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_float4 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_float8 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_numeric = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_bit1 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_bit64 = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_boolean = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_date = ?;
explain (costs off) execute s1 using '2024-10-10';

prepare s1 as select * from test_type_table where a_datetime = ?;
explain (costs off) execute s1 using '2024-10-10 10:10:10';

prepare s1 as select * from test_type_table where a_timestamp = ?;
explain (costs off) execute s1 using '2024-10-10 10:10:10';

prepare s1 as select * from test_type_table where a_year = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_char = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_varchar = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_binary = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_varbinary = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_tinyblob = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';


prepare s1 as select * from test_type_table where a_blob = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

prepare s1 as select * from test_type_table where a_longblob = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';


prepare s1 as select * from test_type_table where a_text = ?;
explain (costs off) execute s1 using 1;
explain (costs off) execute s1 using 1.23;
explain (costs off) execute s1 using '1';

drop table test_type_table;



set dolphin.transform_unknown_param_type_as_column_type_first = true;
set enable_beta_opfusion = on;


CREATE TABLE sbtest1 (id integer NOT NULL, k integer DEFAULT 0 NOT NULL, k1 int2 DEFAULT 0 NOT NULL, k2 float DEFAULT 0 NOT NULL, c character(120) DEFAULT ''::bpchar NOT NULL, pad character(60) DEFAULT ''::bpchar NOT NULL) WITH (orientation=row, compression=no);

insert into sbtest1 values (generate_series(1,100), generate_series(1,100), generate_series(1,100), generate_series(1,100), 'a', 'a');

CREATE INDEX k_1 ON sbtest1 USING btree (k) TABLESPACE pg_default;
CREATE INDEX k_2 ON sbtest1 USING btree (k1) TABLESPACE pg_default;
CREATE INDEX k_3 ON sbtest1 USING btree (k2) TABLESPACE pg_default;
ALTER TABLE sbtest1 ADD CONSTRAINT sbtest1_pkey PRIMARY KEY USING btree (id);

set opfusion_debug_mode = 'log';
prepare p3 as select sum(k) from sbtest1 where id between $1 and $2;
prepare p4 as select sum(k1) from sbtest1 where id between $1 and $2;
prepare p5 as select sum(k2) from sbtest1 where id between $1 and $2;
explain execute p3 (1, 100);
execute p3 (1, 100);
explain execute p4 (1, 100);
execute p4 (1, 100);
explain execute p5 (1, 100);
execute p5 (1, 100);
drop table sbtest1;

drop schema prepare_index_con cascade;
reset current_schema;

