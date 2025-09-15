CREATE SCHEMA fast_route_test;
SET search_path TO fast_route_test;
SET spq.next_shard_id TO 1240000;
SET spq.shard_count TO 2;

CREATE TABLE fast_route_test(
	id INT PRIMARY KEY,
	name TEXT NOT NULL,
	age INT CHECK(age > 0)
);

SELECT create_distributed_table('fast_route_test','id');

INSERT INTO fast_route_test VALUES(1, 'Alice', 25), (2, 'Bob', 30), (3, 'Jerry', 26);

SELECT * FROM fast_route_test WHERE id = 1;

EXPLAIN SELECT * FROM fast_route_test WHERE id = 1;

EXPLAIN ANALYZE SELECT * FROM fast_route_test WHERE id = 1;

DROP SCHEMA fast_route_test CASCADE;
RESET SEARCH_PATH;