CREATE SCHEMA test_table_size;
SET search_path TO test_table_size;
SET spq.shard_count TO 2;

CREATE TABLE test_dist_curd(
	id INT PRIMARY KEY,
	name TEXT NOT NULL,
	age INT CHECK(age > 0)
);

SELECT create_distributed_table('test_dist_curd','id');

INSERT INTO test_dist_curd VALUES(1, 'Alice', 25), (2, 'Bob', 30), (3, 'Jerry', 26);

SELECT * FROM spq_table_size('test_dist_curd');

DROP SCHEMA test_table_size CASCADE;
RESET SEARCH_PATH;