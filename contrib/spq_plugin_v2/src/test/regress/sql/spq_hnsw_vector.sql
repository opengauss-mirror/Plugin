SET spq.propagate_set_commands = 'local';

-- L2

CREATE TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id');

INSERT INTO t3 (id, val) VALUES (1,'[0,0,0]'), (2,'[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops);

INSERT INTO t3 (id, val) VALUES (5, '[1,2,4]');

BEGIN;
SET local enable_seqscan = off;
EXPLAIN SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';
SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <-> (SELECT NULL::vector)) tt;
EXPLAIN SELECT COUNT(*) FROM t3;
SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <-> (SELECT NULL::vector)) tt;
SELECT COUNT(*) FROM t3;
COMMIT;

TRUNCATE t3;
SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';

DROP TABLE t3;

-- inner product

CREATE TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id');

INSERT INTO t3 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t3 USING hnsw (val vector_ip_ops);

INSERT INTO t3 (id, val) VALUES (5, '[1,2,4]');

BEGIN;
SET local enable_seqscan = off;
EXPLAIN SELECT * FROM t3 ORDER BY val <#> '[3,3,3]';

SELECT * FROM t3 ORDER BY val <#> '[3,3,3]';
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <#> (SELECT NULL::vector)) tt;
SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <#> (SELECT NULL::vector)) tt;
COMMIT;

DROP TABLE t3;

-- cosine

CREATE TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id');

INSERT INTO t3 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t3 USING hnsw (val vector_cosine_ops);

INSERT INTO t3 (id, val) VALUES (5, '[1,2,4]');

BEGIN;
SET local enable_seqscan = off;
EXPLAIN SELECT * FROM t3 ORDER BY val <=> '[3,3,3]';
SELECT * FROM t3 ORDER BY val <=> '[3,3,3]';

EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <=> '[0,0,0]') tt;
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <=> (SELECT NULL::vector)) tt;
SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <=> '[0,0,0]') tt;
SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <=> (SELECT NULL::vector)) tt;
COMMIT;

DROP TABLE t3;

-- L1

CREATE TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id');

INSERT INTO t3 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t3 USING hnsw (val vector_l1_ops);

INSERT INTO t3 (id, val) VALUES (5, '[1,2,4]');

BEGIN;
SET local enable_seqscan = off;
EXPLAIN SELECT * FROM t3 ORDER BY val <+> '[3,3,3]';

SELECT * FROM t3 ORDER BY val <+> '[3,3,3]';

EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <+> (SELECT NULL::vector)) tt;
SELECT COUNT(*) FROM (SELECT * FROM t3 ORDER BY val <+> (SELECT NULL::vector)) tt;
COMMIT;

DROP TABLE t3;

-- iterative

CREATE TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id');

INSERT INTO t3 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops);

BEGIN;
SET local enable_seqscan = off;

SET local hnsw.iterative_scan = strict_order;
SET local hnsw.ef_search = 1;
EXPLAIN SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';
SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';

SET local hnsw.iterative_scan = relaxed_order;
EXPLAIN SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';
SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';

COMMIT;

TRUNCATE t3;

BEGIN;
SET local enable_seqscan = off;

SET local hnsw.iterative_scan = strict_order;
SET local hnsw.ef_search = 1;
SET local hnsw.iterative_scan = relaxed_order;
EXPLAIN SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';
SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';
COMMIT;

DROP TABLE t3;

-- unlogged

CREATE UNLOGGED TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id');

INSERT INTO t3 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops);

BEGIN;
SET local enable_seqscan = off;
EXPLAIN SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';
SELECT * FROM t3 ORDER BY val <-> '[3,3,3]';
COMMIT;

DROP TABLE t3;

-- options

CREATE TABLE t3 (id int, val vector(3));
SELECT create_distributed_table('t3', 'id');

CREATE INDEX ON t3 USING hnsw (val vector_l2_ops) WITH (m = 1);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops) WITH (m = 101);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops) WITH (ef_construction = 3);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops) WITH (ef_construction = 1001);
CREATE INDEX ON t3 USING hnsw (val vector_l2_ops) WITH (m = 16, ef_construction = 31);


BEGIN;
SHOW hnsw.ef_search;

SET local hnsw.ef_search = 0;
SET local hnsw.ef_search = 1001;

SHOW hnsw.iterative_scan;

SET local hnsw.iterative_scan = on;
COMMIT;

DROP TABLE t3;
SET spq.propagate_set_commands = DEFAULT;
