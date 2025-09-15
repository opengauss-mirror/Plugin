CREATE SCHEMA vacuum_and_analyze;
SET search_path TO vacuum_and_analyze;
SET spq.shard_count TO 2;

CREATE TABLE test_vacuum_and_analyze(
	id INT PRIMARY KEY,
	name TEXT NOT NULL,
	age INT CHECK(age > 0)
);

SELECT create_distributed_table('test_vacuum_and_analyze','id');

INSERT INTO test_vacuum_and_analyze VALUES(1, 'Alice', 25), (2, 'Bob', 30), (3, 'Jerry', 26);

SELECT * FROM test_vacuum_and_analyze ORDER BY id;

UPDATE test_vacuum_and_analyze SET age = 35 WHERE id = 1;

SELECT * FROM test_vacuum_and_analyze ORDER BY id;

DELETE FROM test_vacuum_and_analyze WHERE id = 2;

SELECT * FROM test_vacuum_and_analyze ORDER BY id;

analyze test_vacuum_and_analyze;

vacuum test_vacuum_and_analyze;

vacuum (analyze, freeze, full) test_vacuum_and_analyze;

drop table test_vacuum_and_analyze;

DROP SCHEMA vacuum_and_analyze CASCADE;
SET search_path TO DEFAULT;
