SET spq.propagate_set_commands = 'local';

-- hamming

CREATE TABLE t4 (val bit(3));
SELECT create_distributed_table('t4', 'val');
DROP TABLE t4;

CREATE TABLE t4 (id int, val bit(3));
SELECT create_distributed_table('t4', 'id');
INSERT INTO t4 (id, val) VALUES (1, B'000'), (2, B'100'), (3, B'111'), (4, NULL);
CREATE INDEX ON t4 USING ivfflat (val bit_hamming_ops) WITH (lists = 1);

INSERT INTO t4 (id, val) VALUES (5, B'110');

BEGIN;
SET local enable_seqscan = off;
EXPLAIN SELECT * FROM t4 ORDER BY val <~> B'111';

SELECT * FROM t4 ORDER BY val <~> B'111';
EXPLAIN SELECT COUNT(*) FROM (SELECT * FROM t4 ORDER BY val <~> (SELECT NULL::bit)) tt;
SELECT COUNT(*) FROM (SELECT * FROM t4 ORDER BY val <~> (SELECT NULL::bit)) tt;
COMMIT;

DROP TABLE t4;

-- varbit

CREATE TABLE t4 (val varbit(3));
SELECT create_distributed_table('t4', 'val');
DROP TABLE t4;

CREATE TABLE t4 (id int, val varbit(3));
SELECT create_distributed_table('t4', 'id');

CREATE INDEX ON t4 USING ivfflat (val bit_hamming_ops) WITH (lists = 1);
CREATE INDEX ON t4 USING ivfflat ((val::bit(3)) bit_hamming_ops) WITH (lists = 1);
CREATE INDEX ON t4 USING ivfflat ((val::bit(64001)) bit_hamming_ops) WITH (lists = 1);
CREATE INDEX ON t4 USING ivfflat ((val::bit(2)) bit_hamming_ops) WITH (lists = 5);
DROP TABLE t4;

SET spq.propagate_set_commands = DEFAULT;
