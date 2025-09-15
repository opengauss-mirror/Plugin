SET spq.propagate_set_commands = 'local';
-- L2

CREATE TABLE t2 (id int, val sparsevec(3));
SELECT create_distributed_table('t2', 'id', shard_count:=4);
INSERT INTO t2 (id, val) VALUES (1, '{}/3'), (2, '{1:1,2:2,3:3}/3'), (3, '{1:1,2:1,3:1}/3'), (4, NULL);
CREATE INDEX ON t2 USING hnsw (val sparsevec_l2_ops);

INSERT INTO t2 (id, val) VALUES (5, '{1:1,2:2,3:4}/3');

BEGIN;
SET local enable_seqscan = off;

EXPLAIN SELECT * FROM t2 ORDER BY val <-> '{1:3,2:3,3:3}/3';
SELECT * FROM t2 ORDER BY val <-> '{1:3,2:3,3:3}/3';
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <-> (SELECT NULL::sparsevec)) tt;
EXPLAIN SELECT COUNT(*) FROM t2;
SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <-> (SELECT NULL::sparsevec)) tt;
SELECT COUNT(*) FROM t2;
COMMIT;

TRUNCATE t2;

BEGIN;
SET local enable_seqscan = off;
SELECT * FROM t2 ORDER BY val <-> '{1:3,2:3,3:3}/3';
COMMIT;

DROP TABLE t2;

-- inner product

CREATE TABLE t2 (id int, val sparsevec(3));
SELECT create_distributed_table('t2', 'id', shard_count:=4);
INSERT INTO t2 (id, val) VALUES (1, '{}/3'), (2, '{1:1,2:2,3:3}/3'), (3, '{1:1,2:1,3:1}/3'), (4, NULL);
CREATE INDEX ON t2 USING hnsw (val sparsevec_ip_ops);

INSERT INTO t2 (id, val) VALUES (5, '{1:1,2:2,3:4}/3');

BEGIN;
SET local enable_seqscan = off;

EXPLAIN SELECT * FROM t2 ORDER BY val <#> '{1:3,2:3,3:3}/3';

SELECT * FROM t2 ORDER BY val <#> '{1:3,2:3,3:3}/3';
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <#> (SELECT NULL::sparsevec)) tt;
SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <#> (SELECT NULL::sparsevec)) tt;
COMMIT;

DROP TABLE t2;

-- cosine

CREATE TABLE t2 (id int, val sparsevec(3));
SELECT create_distributed_table('t2', 'id', shard_count:=4);
INSERT INTO t2 (id, val) VALUES (1, '{}/3'), (2, '{1:1,2:2,3:3}/3'), (3, '{1:1,2:1,3:1}/3'), (4, NULL);
CREATE INDEX ON t2 USING hnsw (val sparsevec_cosine_ops);

INSERT INTO t2 (id, val) VALUES (5, '{1:1,2:2,3:4}/3');

BEGIN;
SET local enable_seqscan = off;

EXPLAIN SELECT * FROM t2 ORDER BY val <=> '{1:3,2:3,3:3}/3';
SELECT * FROM t2 ORDER BY val <=> '{1:3,2:3,3:3}/3';
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <=> '{}/3') tt;
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <=> (SELECT NULL::sparsevec)) tt;
SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <=> '{}/3') tt;
SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <=> (SELECT NULL::sparsevec)) tt;
COMMIT;

DROP TABLE t2;

-- L1

CREATE TABLE t2 (id int, val sparsevec(3));
SELECT create_distributed_table('t2', 'id', shard_count:=4);
INSERT INTO t2 (id, val) VALUES (1, '{}/3'), (2, '{1:1,2:2,3:3}/3'), (3, '{1:1,2:1,3:1}/3'), (4, NULL);
CREATE INDEX ON t2 USING hnsw (val sparsevec_l1_ops);

INSERT INTO t2 (id, val) VALUES (5, '{1:1,2:2,3:4}/3');

BEGIN;
SET local enable_seqscan = off;

EXPLAIN SELECT * FROM t2 ORDER BY val <+> '{1:3,2:3,3:3}/3';

SELECT * FROM t2 ORDER BY val <+> '{1:3,2:3,3:3}/3';
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <+> (SELECT NULL::sparsevec)) tt;
SELECT COUNT(*) FROM (SELECT * FROM t2 ORDER BY val <+> (SELECT NULL::sparsevec)) tt;
COMMIT;

DROP TABLE t2;

-- non-zero elements

CREATE TABLE t2 (id int, val sparsevec(1001));
SELECT create_distributed_table('t2', 'id', shard_count:=4);
INSERT INTO t2 (id, val) VALUES (1, array_fill(1, ARRAY[1001])::vector::sparsevec);
CREATE INDEX ON t2 USING hnsw (val sparsevec_l2_ops);
TRUNCATE t2;
CREATE INDEX ON t2 USING hnsw (val sparsevec_l2_ops);
INSERT INTO t2 (id, val) VALUES (2, array_fill(1, ARRAY[1001])::vector::sparsevec);
DROP TABLE t2;

SET spq.propagate_set_commands = DEFAULT;
