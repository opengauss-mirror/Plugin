SET spq.propagate_set_commands = 'local';

-- L2

CREATE TABLE t5 (id int, val vector(3));
SELECT create_distributed_table('t5', 'id', shard_count:=4);

INSERT INTO t5 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t5 USING ivfflat (val vector_l2_ops) WITH (lists = 1);

INSERT INTO t5 (id, val) VALUES (5, '[1,2,4]');

BEGIN;
SET local enable_seqscan = off;
SELECT * FROM t5 ORDER BY val <-> '[3,3,3]';

SELECT COUNT(*) FROM (SELECT * FROM t5 ORDER BY val <-> (SELECT NULL::vector)) tt;
SELECT COUNT(*) FROM t5;
COMMIT;

TRUNCATE t5;

BEGIN;
SET local enable_seqscan = off;
SELECT * FROM t5 ORDER BY val <-> '[3,3,3]';
COMMIT;

DROP TABLE t5;

-- inner product

CREATE TABLE t5 (id int, val vector(3));
SELECT create_distributed_table('t5', 'id', shard_count:=4);

INSERT INTO t5 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t5 USING ivfflat (val vector_ip_ops) WITH (lists = 1);

INSERT INTO t5 (id, val) VALUES (5, '[1,2,4]');

BEGIN;
SET local enable_seqscan = off;

SELECT * FROM t5 ORDER BY val <#> '[3,3,3]';
SELECT COUNT(*) FROM (SELECT * FROM t5 ORDER BY val <#> (SELECT NULL::vector)) tt;
COMMIT;

DROP TABLE t5;

-- cosine

CREATE TABLE t5 (id int, val vector(3));
SELECT create_distributed_table('t5', 'id', shard_count:=4);

INSERT INTO t5 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t5 USING ivfflat (val vector_cosine_ops) WITH (lists = 1);

INSERT INTO t5 (id, val) VALUES (5, '[1,2,4]');

BEGIN;
SET local enable_seqscan = off;

SELECT * FROM t5 ORDER BY val <=> '[3,3,3]';
SELECT COUNT(*) FROM (SELECT * FROM t5 ORDER BY val <=> '[0,0,0]') tt;
SELECT COUNT(*) FROM (SELECT * FROM t5 ORDER BY val <=> (SELECT NULL::vector)) tt;
COMMIT;

DROP TABLE t5;

-- iterative

CREATE TABLE t5 (id int, val vector(3));
SELECT create_distributed_table('t5', 'id', shard_count:=4);

INSERT INTO t5 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t5 USING ivfflat (val vector_l2_ops) WITH (lists = 3);

BEGIN;
SET local enable_seqscan = off;

SET local ivfflat.iterative_scan = relaxed_order;
SELECT * FROM t5 ORDER BY val <-> '[3,3,3]';

SET local  ivfflat.max_probes = 1;
SELECT * FROM t5 ORDER BY val <-> '[3,3,3]';

SET local ivfflat.max_probes = 2;
SELECT * FROM t5 ORDER BY val <-> '[3,3,3]';
COMMIT;


TRUNCATE t5;

BEGIN;
SET local enable_seqscan = off;
SET local ivfflat.iterative_scan = relaxed_order;
SET local  ivfflat.max_probes = 1;
SET local ivfflat.max_probes = 2;
SELECT * FROM t5 ORDER BY val <-> '[3,3,3]';
COMMIT;

DROP TABLE t5;

-- unlogged

CREATE UNLOGGED TABLE t5 (id int, val vector(3));
SELECT create_distributed_table('t5', 'id', shard_count:=4);

INSERT INTO t5 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t5 USING ivfflat (val vector_l2_ops) WITH (lists = 1);

BEGIN;
SET local enable_seqscan = off;

SELECT * FROM t5 ORDER BY val <-> '[3,3,3]';
COMMIT;

DROP TABLE t5;

-- options

CREATE TABLE t5 (id int, val vector(3));
SELECT create_distributed_table('t5', 'id', shard_count:=4);

CREATE INDEX ON t5 USING ivfflat (val vector_l2_ops) WITH (lists = 0);
CREATE INDEX ON t5 USING ivfflat (val vector_l2_ops) WITH (lists = 32769);

BEGIN;
SET local enable_seqscan = off;

SHOW ivfflat.iterative_scan;

SET local ivfflat.iterative_scan = on;
COMMIT;

DROP TABLE t5;
SET spq.propagate_set_commands = DEFAULT;
