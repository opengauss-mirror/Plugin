SET spq.propagate_set_commands = 'local';
-- vector
CREATE TABLE t0 (val vector(3));
SELECT create_distributed_table('t0', 'val');
DROP TABLE t0;

CREATE TABLE t0 (id int, val vector(3));
SELECT create_distributed_table('t0', 'id');

INSERT INTO t0 (id, val) VALUES (1, '[0,0,0]'), (2, '[1,2,3]'), (3, '[1,1,1]'), (4, NULL);
CREATE INDEX ON t0 (val);

BEGIN;
SET local enable_seqscan = off;
EXPLAIN SELECT * FROM t0 WHERE val = '[1,2,3]';
EXPLAIN SELECT * FROM t0 ORDER BY val;

SELECT * FROM t0 WHERE val = '[1,2,3]';
SELECT * FROM t0 ORDER BY val;
COMMIT;

DROP TABLE t0;

-- sparsevec
CREATE TABLE t0 (val sparsevec(3));
SELECT create_distributed_table('t0', 'val');
DROP TABLE t0;

CREATE TABLE t0 (id int, val sparsevec(3));
SELECT create_distributed_table('t0', 'id');

INSERT INTO t0 (id, val) VALUES (1, '{}/3'), (2, '{1:1,2:2,3:3}/3'), (3, '{1:1,2:1,3:1}/3'), (4, NULL);
CREATE INDEX ON t0 (val);

BEGIN;
SET local enable_seqscan = off;
EXPLAIN SELECT * FROM t0 WHERE val = '{1:1,2:2,3:3}/3';
EXPLAIN SELECT * FROM t0 ORDER BY val;

SELECT * FROM t0 WHERE val = '{1:1,2:2,3:3}/3';
SELECT * FROM t0 ORDER BY val;
COMMIT;

SET spq.propagate_set_commands = DEFAULT;
DROP TABLE t0;
