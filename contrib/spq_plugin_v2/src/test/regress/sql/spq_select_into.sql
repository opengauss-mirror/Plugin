CREATE SCHEMA test_select_into;
SET search_path TO test_select_into;
SET spq.shard_count TO 2;

CREATE TABLE source_dist_table(
	id INT PRIMARY KEY,
	name TEXT NOT NULL,
	age INT CHECK(age > 0)
);

SELECT create_distributed_table('source_dist_table','id');

CREATE TABLE target_dist_table(
	id INT PRIMARY KEY,
	name TEXT NOT NULL,
	age INT CHECK(age > 0)
);

SELECT create_distributed_table('target_dist_table','id');

INSERT INTO source_dist_table VALUES(1, 'Alice', 25), (2, 'Bob', 30), (3, 'Jerry', 26);

SELECT * FROM source_dist_table ORDER BY id;;

INSERT INTO target_dist_table (id, name, age)
SELECT id, name, age FROM source_dist_table;

SELECT * FROM target_dist_table ORDER BY id;;

DROP SCHEMA test_select_into CASCADE;
RESET SEARCH_PATH;