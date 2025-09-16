CREATE SCHEMA test_curd;
SET search_path TO test_curd;
SET spq.shard_count TO 2;

CREATE TABLE test_dist_curd(
	id INT PRIMARY KEY,
	name TEXT NOT NULL,
	age INT CHECK(age > 0)
);

SELECT create_distributed_table('test_dist_curd','id');

INSERT INTO test_dist_curd VALUES(1, 'Alice', 25), (2, 'Bob', 30), (3, 'Jerry', 26);

SELECT * FROM test_dist_curd ORDER BY id;

UPDATE test_dist_curd SET age = 35 WHERE id = 1;

SELECT * FROM test_dist_curd ORDER BY id;

DELETE FROM test_dist_curd WHERE id = 2;

SELECT * FROM test_dist_curd ORDER BY id;

DROP SCHEMA test_curd CASCADE;
RESET SEARCH_PATH;