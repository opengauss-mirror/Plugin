CREATE TABLE test (id int, val bit(3));
SELECT create_distributed_table('test', 'id', 'append');
SELECT create_distributed_table('test', 'id', 'range');
SELECT create_distributed_table('test', 'id', 'hash');
DROP TABLE test
