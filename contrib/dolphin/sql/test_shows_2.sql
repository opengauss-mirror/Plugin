drop database if exists db_show_2;
create database db_show_2 dbcompatibility 'b';
\c db_show_2

-- test show index

-- astore
create table show_index_tbl (a int primary key, b int);
show keys from show_index_tbl;
show indexes from show_index_tbl from public;
show index from show_index_tbl from public where "table" = 'show_index_tbl';

-- test error
show index from not_exist_tbl;
show index from not_exist_tbl from not_exist_schema;

-- non_unique/null/seq_in_index/collation/cardinality/index_comment
create index show_index_tbl_a_b_desc on show_index_tbl(a, b desc);
comment on index show_index_tbl_a_b_desc is 'index a and b';

insert into show_index_tbl values (generate_series(1,1000), generate_series(1,1000));
insert into show_index_tbl values (generate_series(1001,2000), generate_series(1,1000));
analyze show_index_tbl;

show index from show_index_tbl;

-- index_type btree/hash/gin/gist
create index show_index_tbl_a_hash on show_index_tbl using hash (a);

alter table show_index_tbl add column c int[];
create index show_index_tbl_c_gin on show_index_tbl using gin (c);

alter table show_index_tbl add column d box;
create index show_index_tbl_d_gist on show_index_tbl using gist (d);

show index from show_index_tbl;

alter index show_index_tbl_a_b_desc unusable;
show index from show_index_tbl;

drop table show_index_tbl;

-- cstore
create table show_index_tbl (a int primary key, b int) with (orientation = column);

-- non_unique/null/seq_in_index/collation/cardinality/index_comment
create index show_index_tbl_a_b_desc on show_index_tbl(a, b);
comment on index show_index_tbl_a_b_desc is 'index a and b';

insert into show_index_tbl values (generate_series(1,1000), generate_series(1,1000));
insert into show_index_tbl values (generate_series(1001,2000), generate_series(1,1000));
analyze show_index_tbl;
show index from show_index_tbl;

-- index_type cbtree/psort
create index show_index_tbl_a_psort on show_index_tbl using psort (a);
create index show_index_tbl_b_gin on show_index_tbl using gin (to_tsvector('english', b));

show index from show_index_tbl;

drop table show_index_tbl;

-- ustore
create table show_index_tbl (a int primary key, b int) with (storage_type = ustore);

-- non_unique/null/seq_in_index/collation/cardinality/index_comment
create index show_index_tbl_a_b_desc on show_index_tbl(a, b desc);
comment on index show_index_tbl_a_b_desc is 'index a and b';

insert into show_index_tbl values (generate_series(1,1000), generate_series(1,1000));
insert into show_index_tbl values (generate_series(1001,2000), generate_series(1,1000));
analyze show_index_tbl;
show index from show_index_tbl;

alter index show_index_tbl_a_b_desc unusable;
show index from show_index_tbl;

-- test index with including column
create index show_index_tbl_a_inclu_b on show_index_tbl(a) include (b);
show index from show_index_tbl;

drop table show_index_tbl;

\c postgres
drop database if exists db_show_2;
