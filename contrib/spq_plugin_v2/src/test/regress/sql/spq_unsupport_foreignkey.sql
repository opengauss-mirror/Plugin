SET spq.next_shard_id TO 15000000;
CREATE SCHEMA fkey;
SET search_path to fkey;
SET spq.shard_replication_factor TO 1;
SET spq.shard_count to 8;

-- check if master_move_shard_placement with logical replication creates the
-- foreign constraints properly after moving the shard
CREATE TABLE referenced_table(test_column int, test_column2 int UNIQUE, PRIMARY KEY(test_column), table_id int);
CREATE TABLE referencing_table(id int PRIMARY KEY, ref_id int, FOREIGN KEY (id) REFERENCES referenced_table(test_column) ON DELETE CASCADE);
CREATE TABLE referencing_table2(id int, ref_id int, FOREIGN KEY (ref_id) REFERENCES referenced_table(test_column2) ON DELETE CASCADE, FOREIGN KEY (id) REFERENCES referencing_table(id) ON DELETE CASCADE);
SELECT create_distributed_table('referencing_table', 'id');
SELECT create_distributed_table('referencing_table2', 'id');
CREATE TABLE referenced_table2(test_column int, test_column2 int, PRIMARY KEY(test_column), table_id int);
SELECT create_distributed_table('referenced_table2', 'test_column');

CREATE TABLE table1(table_id BIGINT PRIMARY KEY, name TEXT);
CREATE TABLE table2(table2_id BIGINT PRIMARY KEY, name2 TEXT);
CREATE TABLE table3(table3_id BIGINT PRIMARY KEY, name3 TEXT);

--test for explicit invocation
ALTER TABLE referenced_table2 ADD CONSTRAINT fk_table FOREIGN KEY (table_id)
REFERENCES table1(table_id);

--test for implicit conversion
ALTER TABLE referenced_table2 ADD COLUMN fk_table2 INT
REFERENCES table1(table_id);

----test for local explicit invocation
ALTER TABLE table2 ADD COLUMN table0_id INT; 

ALTER TABLE table2 ADD CONSTRAINT fk_table FOREIGN KEY (table0_id)
REFERENCES table1(table_id);

--test for local implicit conversion
ALTER TABLE table2 ADD COLUMN fk_table2 INT
REFERENCES table3(table3_id);

DROP SCHEMA fkey CASCADE;