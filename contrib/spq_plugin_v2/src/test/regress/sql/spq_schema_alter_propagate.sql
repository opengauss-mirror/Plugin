----------------------- test 1: alter table's schema propagate -----------------------------
CREATE SCHEMA schema21;
SET search_path='schema21';

CREATE TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id', shard_count:=4);
INSERT INTO t3 (id, val) VALUES (1,'[0,0,0]'), (2,'[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops);
INSERT INTO t3 (id, val) VALUES (5, '[1,2,4]');
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid AND nspname='schema21' GROUP BY nspname;

\c - - - :worker_1_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid AND nspname='schema21' GROUP BY nspname;
\c - - - :worker_2_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid AND nspname='schema21' GROUP BY nspname;

\c - - - :master_port
ALTER TABLE schema21.t3 SET SCHEMA schema22;
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :worker_1_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :worker_2_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :master_port
CREATE SCHEMA schema22;
ALTER TABLE schema21.t3 SET SCHEMA schema22;
\c - - - :worker_1_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :worker_2_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :master_port
DROP SCHEMA schema21;
DROP SCHEMA schema22;
DROP SCHEMA schema22 CASCADE;

\c - - - :worker_1_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

  \c - - - :worker_2_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :master_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

----------------------- test 2: rename schema's name propagate -----------------------------
CREATE SCHEMA schema21;
SET search_path='schema21';

CREATE TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id', shard_count:=4);
INSERT INTO t3 (id, val) VALUES (1,'[0,0,0]'), (2,'[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops);
INSERT INTO t3 (id, val) VALUES (5, '[1,2,4]');
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid AND nspname='schema21' GROUP BY nspname;

\c - - - :worker_1_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid AND nspname='schema21' GROUP BY nspname;
\c - - - :worker_2_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid AND nspname='schema21' GROUP BY nspname;

\c - - - :master_port
ALTER SCHEMA schema21 RENAME TO schema22;
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :worker_1_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :worker_2_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :master_port
DROP SCHEMA schema21;
DROP SCHEMA schema22;
DROP SCHEMA schema22 CASCADE;

----------------------- test 3: rename schema's name error propagate -----------------------------
CREATE SCHEMA schema21;
CREATE SCHEMA schema22;
SET search_path='schema21';

CREATE TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id', shard_count:=4);
INSERT INTO t3 (id, val) VALUES (1,'[0,0,0]'), (2,'[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops);
INSERT INTO t3 (id, val) VALUES (5, '[1,2,4]');
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid AND nspname='schema21' GROUP BY nspname;

\c - - - :worker_1_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid AND nspname='schema21' GROUP BY nspname;
\c - - - :worker_2_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid AND nspname='schema21' GROUP BY nspname;

\c - - - :master_port
ALTER SCHEMA schema21 RENAME TO schema22;
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :worker_1_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :worker_2_port
SELECT nspname, COUNT(*) FROM
  pg_class JOIN pg_namespace ON pg_class.relnamespace = pg_namespace.oid 
  AND (nspname='schema22' or nspname='schema21')
  GROUP BY nspname;

\c - - - :master_port
DROP SCHEMA schema21;
DROP SCHEMA schema22;
DROP SCHEMA schema21 CASCADE;
