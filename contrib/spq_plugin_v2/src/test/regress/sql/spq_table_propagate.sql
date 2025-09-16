----------------------- test 1: alter table propagate -----------------------------
\c - - - :master_port
create table t1 (a int);
insert into t1 values (1), (2), (3), (4);
SELECT create_distributed_table('t1', 'a', shard_count:=4);

SET spq.explain_all_tasks TO ON;
EXPLAIN SELECT * FROM t1 ORDER BY a;

select * from t1 order by a;

delete from t1;
-- add column
ALTER TABLE t1 ADD COLUMN b INT;

insert into t1 values (1, 1), (2, 2), (3, 3), (4, 4);

select * from t1 order by a;

-- alter table
ALTER TABLE t1 ALTER COLUMN b TYPE BIGINT;

select * from t1 order by a;

ALTER TABLE t1 RENAME COLUMN b TO bb;

select * from t1 order by a;

ALTER TABLE t1 DROP COLUMN bb;

select * from t1 order by a;

-- create index
CREATE INDEX idx_t1_a ON t1 (a);

EXPLAIN SELECT * FROM t1 ORDER BY a;

-- alter index
alter index idx_t1_a set (fillfactor = 80);

\c - - - :worker_1_port

SELECT n.nspname as "Schema",
  c.relname as "Name",
  pg_catalog.pg_get_indexdef(i.indexrelid, 0, true) as "Definition",
  c.relpages as "Size"
FROM pg_catalog.pg_index i
JOIN pg_catalog.pg_class c ON c.oid = i.indexrelid
LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace
WHERE c.relkind = 'i'
  AND n.nspname <> 'pg_catalog'
  AND n.nspname <> 'information_schema'
  AND n.nspname !~ '^pg_toast'
  AND pg_catalog.pg_table_is_visible(i.indrelid)
ORDER BY 1,2;

\c - - - :worker_2_port

SELECT n.nspname as "Schema",
  c.relname as "Name",
  pg_catalog.pg_get_indexdef(i.indexrelid, 0, true) as "Definition",
  c.relpages as "Size"
FROM pg_catalog.pg_index i
JOIN pg_catalog.pg_class c ON c.oid = i.indexrelid
LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace
WHERE c.relkind = 'i'
  AND n.nspname <> 'pg_catalog'
  AND n.nspname <> 'information_schema'
  AND n.nspname !~ '^pg_toast'
  AND pg_catalog.pg_table_is_visible(i.indrelid)
ORDER BY 1,2;

\c - - - :master_port
-- reindex table
-- expect 0
\c - - - :worker_1_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;
\c - - - :worker_2_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;
\c - - - :master_port
reindex table t1;
-- expect 2
\c - - - :worker_1_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;
\c - - - :worker_2_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;

-- reindex index
\c - - - :master_port
drop index idx_t1_a;
CREATE INDEX idx_t1_a ON t1 (a);
-- expect 0
\c - - - :worker_1_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;
\c - - - :worker_2_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;

\c - - - :master_port
reindex index idx_t1_a;
-- expect 2
\c - - - :worker_1_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;
\c - - - :worker_2_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;

-- create and reindex index concurrently
\c - - - :master_port
drop index idx_t1_a;
CREATE INDEX concurrently idx_t1_a ON t1 (a);
-- expect 0
\c - - - :worker_1_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid = relfilenode;
\c - - - :worker_2_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid = relfilenode;

\c - - - :master_port
reindex index concurrently idx_t1_a;
-- expect 0 concurrently reindex make a new oid
\c - - - :worker_1_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid = relfilenode;
\c - - - :worker_2_port
select count(1) from pg_class where relname like 'idx_t1_a%' and oid = relfilenode;

-- drop index
\c - - - :master_port
drop index idx_t1_a;
\c - - - :worker_1_port
SELECT n.nspname as "Schema",
  c.relname as "Name",
  pg_catalog.pg_get_indexdef(i.indexrelid, 0, true) as "Definition",
  c.relpages as "Size"
FROM pg_catalog.pg_index i
JOIN pg_catalog.pg_class c ON c.oid = i.indexrelid
LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace
WHERE c.relkind = 'i'
  AND n.nspname <> 'pg_catalog'
  AND n.nspname <> 'information_schema'
  AND n.nspname !~ '^pg_toast'
  AND pg_catalog.pg_table_is_visible(i.indrelid)
ORDER BY 1,2;

\c - - - :worker_2_port
SELECT n.nspname as "Schema",
  c.relname as "Name",
  pg_catalog.pg_get_indexdef(i.indexrelid, 0, true) as "Definition",
  c.relpages as "Size"
FROM pg_catalog.pg_index i
JOIN pg_catalog.pg_class c ON c.oid = i.indexrelid
LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace
WHERE c.relkind = 'i'
  AND n.nspname <> 'pg_catalog'
  AND n.nspname <> 'information_schema'
  AND n.nspname !~ '^pg_toast'
  AND pg_catalog.pg_table_is_visible(i.indrelid)
ORDER BY 1,2;

-- rename table
\c - - - :master_port
ALTER TABLE t1 RENAME TO t2;
select * from t2 order by a;

-- drop table
drop table t2;
\c - - - :worker_1_port
SELECT table_schema AS "Schema", table_name AS "Name", table_type AS "Type"
  FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_schema, table_name;

\c - - - :worker_2_port
SELECT table_schema AS "Schema", table_name AS "Name", table_type AS "Type"
  FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_schema, table_name;

----------------------- test 2: vacuum propagate -----------------------------
\c - - - :master_port

create table t1 (a int);
insert into t1 values (1), (2), (3), (4);
SELECT create_distributed_table('t1', 'a', shard_count:=4);

set spq.log_remote_commands = true;
set spq.grep_remote_commands = '%VACUUM%';

vacuum analyze t1;

set spq.log_remote_commands = false;

drop table t1;
create table t1 (a int);
insert into t1 values (1), (2), (3), (4);
SELECT create_distributed_table('t1', 'a', shard_count:=4);

set spq.log_remote_commands = true;
set spq.grep_remote_commands = '%VACUUM%';

vacuum;

set spq.log_remote_commands = false;

\c - - - :master_port
drop table t1;


----------------------- test 3: test different collumn type as partition collum -----------------------------
\c - - - :master_port
-- test varchar as partition column
create table t1 (a varchar(10), b int, c varchar(20));
SELECT create_distributed_table('t1', 'a', shard_count:=4);
insert into t1 values ('aaa1', 1, 'ccc1');
insert into t1 values ('aaa2', 2, 'ccc2');
insert into t1 values ('aaa3', 3, 'ccc3');
insert into t1 values ('aaa4', 4, 'ccc4');
select * from t1 order by a;
drop table t1;

create table t1 (a int, b bigint, c tinyint, d char(10), e varchar(10), f text, g date, h vector, i bit,j sparsevec);
SELECT create_distributed_table('t1', 'a', shard_count:=4);
select undistribute_table('t1');

SELECT create_distributed_table('t1', 'b', shard_count:=4);
select undistribute_table('t1');

SELECT create_distributed_table('t1', 'c', shard_count:=4);
select undistribute_table('t1');

SELECT create_distributed_table('t1', 'd', shard_count:=4);
select undistribute_table('t1');

SELECT create_distributed_table('t1', 'e', shard_count:=4);
select undistribute_table('t1');

SELECT create_distributed_table('t1', 'f', shard_count:=4);
select undistribute_table('t1');

SELECT create_distributed_table('t1', 'g', shard_count:=4);
select undistribute_table('t1');

-- expect error
SELECT create_distributed_table('t1', 'h', shard_count:=4);
-- expect error
SELECT create_distributed_table('t1', 'i', shard_count:=4);
-- expect error
SELECT create_distributed_table('t1', 'j', shard_count:=4);

SELECT create_distributed_table('t1', 'a', shard_count:=4);
select alter_distributed_table('t1', 'b');
select alter_distributed_table('t1', 'c');
select alter_distributed_table('t1', 'd');
select alter_distributed_table('t1', 'e');
select alter_distributed_table('t1', 'f');
select alter_distributed_table('t1', 'g');
-- expect error
select alter_distributed_table('t1', 'h');
-- expect error
select alter_distributed_table('t1', 'i');
-- expect error
select alter_distributed_table('t1', 'j');

drop table t1;
----------------------- test 4: test local table ddl -----------------------------
\c - - - :master_port
create table t1 (a int);
insert into t1 values (1), (2), (3), (4);

EXPLAIN SELECT * FROM t1 ORDER BY a;

select * from t1 order by a;

delete from t1;
-- add column
ALTER TABLE t1 ADD COLUMN b INT;

insert into t1 values (1, 1), (2, 2), (3, 3), (4, 4);

select * from t1 order by a;

-- alter table
ALTER TABLE t1 ALTER COLUMN b TYPE BIGINT;

select * from t1 order by a;

ALTER TABLE t1 RENAME COLUMN b TO bb;

select * from t1 order by a;

ALTER TABLE t1 DROP COLUMN bb;

select * from t1 order by a;

-- create index
CREATE INDEX idx_t1_a ON t1 (a);

EXPLAIN SELECT * FROM t1 ORDER BY a;

-- alter index
alter index idx_t1_a set (fillfactor = 80);

SELECT n.nspname as "Schema",
  c.relname as "Name",
  pg_catalog.pg_get_indexdef(i.indexrelid, 0, true) as "Definition",
  c.relpages as "Size"
FROM pg_catalog.pg_index i
JOIN pg_catalog.pg_class c ON c.oid = i.indexrelid
LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace
WHERE c.relkind = 'i'
  AND n.nspname <> 'pg_catalog'
  AND n.nspname <> 'information_schema'
  AND n.nspname !~ '^pg_toast'
  AND pg_catalog.pg_table_is_visible(i.indrelid)
ORDER BY 1,2;

-- reindex table
-- expect 0
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;

reindex table t1;
-- expect 1
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;

-- reindex index
drop index idx_t1_a;
CREATE INDEX idx_t1_a ON t1 (a);
-- expect 0
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;

reindex index idx_t1_a;
-- expect 1
select count(1) from pg_class where relname like 'idx_t1_a%' and oid != relfilenode;

-- create and reindex index concurrently
drop index idx_t1_a;
CREATE INDEX concurrently idx_t1_a ON t1 (a);
-- expect 0
select count(1) from pg_class where relname like 'idx_t1_a%' and oid = relfilenode;

reindex index concurrently idx_t1_a;
-- expect 0 concurrently reindex make a new oid
select count(1) from pg_class where relname like 'idx_t1_a%' and oid = relfilenode;

-- drop index
drop index idx_t1_a;
SELECT n.nspname as "Schema",
  c.relname as "Name",
  pg_catalog.pg_get_indexdef(i.indexrelid, 0, true) as "Definition",
  c.relpages as "Size"
FROM pg_catalog.pg_index i
JOIN pg_catalog.pg_class c ON c.oid = i.indexrelid
LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace
WHERE c.relkind = 'i'
  AND n.nspname <> 'pg_catalog'
  AND n.nspname <> 'information_schema'
  AND n.nspname !~ '^pg_toast'
  AND pg_catalog.pg_table_is_visible(i.indrelid)
ORDER BY 1,2;

-- rename table
ALTER TABLE t1 RENAME TO t2;

-- drop table
drop table t2;
SELECT table_schema AS "Schema", table_name AS "Name", table_type AS "Type"
  FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_schema, table_name;

create table t1 (a int);
insert into t1 values (1), (2), (3), (4);
-- expect 0
select count(1) from pg_stat_all_tables where relname = 't1' and last_vacuum is not null;

vacuum analyze t1;
-- expect 1
select count(1) from pg_stat_all_tables where relname = 't1' and last_vacuum is not null;

drop table t1;
create table t1 (a int);
insert into t1 values (1), (2), (3), (4);
-- expect 0
select count(1) from pg_stat_all_tables where relname = 't1' and last_vacuum is not null;

vacuum;
-- expect 1
select count(1) from pg_stat_all_tables where relname = 't1' and last_vacuum is not null;
drop table t1;