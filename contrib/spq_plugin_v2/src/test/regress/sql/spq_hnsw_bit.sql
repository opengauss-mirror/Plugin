SET spq.propagate_set_commands = 'local';

-- hamming

CREATE TABLE t1 (id int, val bit(3));
SELECT create_distributed_table('t1', 'id', shard_count:=4);

INSERT INTO t1 (id, val) VALUES (1, B'000'), (2, B'100'), (3, B'111'), (4, NULL);
CREATE INDEX ON t1 USING hnsw (val bit_hamming_ops);

INSERT INTO t1 (id, val) VALUES (5, B'110');

BEGIN;
SET local enable_seqscan = off;
EXPLAIN SELECT * FROM t1 ORDER BY val <~> B'111';
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t1 ORDER BY val <~> (SELECT NULL::bit)) tt;

SELECT * FROM t1 ORDER BY val <~> B'111';
SELECT COUNT(*) FROM (SELECT * FROM t1 ORDER BY val <~> (SELECT NULL::bit)) tt;
COMMIT;

DROP TABLE t1;

-- jaccard

CREATE TABLE t1 (id int, val bit(4));
SELECT create_distributed_table('t1', 'id', shard_count:=4);

INSERT INTO t1 (id, val) VALUES (1, B'0000'), (2, B'1100'), (3, B'1111'), (4, NULL);
CREATE INDEX ON t1 USING hnsw (val bit_jaccard_ops);

INSERT INTO t1 (id, val) VALUES (5, B'1110');

BEGIN;
SET local enable_seqscan = off;

EXPLAIN SELECT * FROM t1 ORDER BY val <%> B'1111';
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t1 ORDER BY val <%> (SELECT NULL::bit)) tt;

SELECT * FROM t1 ORDER BY val <%> B'1111';
SELECT COUNT(*) FROM (SELECT * FROM t1 ORDER BY val <%> (SELECT NULL::bit)) tt;
COMMIT;

DROP TABLE t1;

-- varbit

CREATE TABLE t1 (id int, val varbit(3));
SELECT create_distributed_table('t1', 'id', shard_count:=4);

CREATE INDEX ON t1 USING hnsw (val bit_hamming_ops);
CREATE INDEX ON t1 USING hnsw ((val::bit(3)) bit_hamming_ops);
CREATE INDEX ON t1 USING hnsw ((val::bit(64001)) bit_hamming_ops);
DROP TABLE t1;
SET spq.propagate_set_commands = DEFAULT;
