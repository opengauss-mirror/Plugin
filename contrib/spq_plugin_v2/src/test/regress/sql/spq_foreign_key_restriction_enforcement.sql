--
-- Tests multiple commands in transactions where
-- there is foreign key relation between reference
-- tables and distributed tables
--

CREATE SCHEMA test_fkey_to_ref_in_tx;
SET search_path TO 'test_fkey_to_ref_in_tx';

SET spq.next_shard_id TO 2380000;
SET spq.next_placement_id TO 2380000;

SET spq.shard_replication_factor TO 1;

CREATE TABLE transitive_reference_table(id int PRIMARY KEY);
SELECT create_reference_table('transitive_reference_table');

CREATE TABLE reference_table(id int PRIMARY KEY, value_1 int);
SELECT create_reference_table('reference_table');

CREATE TABLE on_update_fkey_table(id int PRIMARY KEY, value_1 int);
SELECT create_distributed_table('on_update_fkey_table', 'id');

CREATE TABLE unrelated_dist_table(id int PRIMARY KEY, value_1 int);
SELECT create_distributed_table('unrelated_dist_table', 'id');

ALTER TABLE on_update_fkey_table ADD CONSTRAINT fkey FOREIGN KEY(value_1) REFERENCES reference_table(id) ON UPDATE CASCADE;
ALTER TABLE reference_table ADD CONSTRAINT fkey FOREIGN KEY(value_1) REFERENCES transitive_reference_table(id) ON UPDATE CASCADE;

INSERT INTO transitive_reference_table SELECT i FROM generate_series(0, 100) i;
INSERT INTO reference_table SELECT i, i FROM generate_series(0, 100) i;

INSERT INTO on_update_fkey_table SELECT i, i % 100  FROM generate_series(0, 1000) i;
INSERT INTO unrelated_dist_table SELECT i, i % 100  FROM generate_series(0, 1000) i;

-- in order to see when the mode automatically swithces to sequential execution
--SET client_min_messages TO DEBUG1;

-- case 1.1: SELECT to a reference table is followed by a parallel SELECT to a distributed table
BEGIN;
	SELECT count(*) FROM reference_table;
	SELECT count(*) FROM on_update_fkey_table;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM transitive_reference_table;
	SELECT count(*) FROM on_update_fkey_table;
ROLLBACK;

-- case 1.2: SELECT to a reference table is followed by a multiple router SELECTs to a distributed table
BEGIN;
	SELECT count(*) FROM reference_table;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 15;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 16;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 17;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 18;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM transitive_reference_table;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 15;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 16;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 17;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 18;
ROLLBACK;

-- case 1.3: SELECT to a reference table is followed by a multi-shard UPDATE to a distributed table
BEGIN;
	SELECT count(*) FROM reference_table;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM transitive_reference_table;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
ROLLBACK;

-- case 1.4: SELECT to a reference table is followed by a multiple sing-shard UPDATE to a distributed table
BEGIN;
	SELECT count(*) FROM reference_table;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE id = 15;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE id = 16;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE id = 17;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE id = 18;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM transitive_reference_table;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE id = 15;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE id = 16;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE id = 17;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE id = 18;
ROLLBACK;

-- case 1.5: SELECT to a reference table is followed by a DDL that touches fkey column
BEGIN;
	SELECT count(*) FROM reference_table;
	ALTER TABLE on_update_fkey_table ALTER COLUMN value_1 SET DATA TYPE bigint;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM transitive_reference_table;
	ALTER TABLE on_update_fkey_table ALTER COLUMN value_1 SET DATA TYPE bigint;
ROLLBACK;

-- case 1.6: SELECT to a reference table is followed by an unrelated DDL
BEGIN;
	SELECT count(*) FROM reference_table;
	ALTER TABLE on_update_fkey_table ADD COLUMN X INT;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM transitive_reference_table;
	ALTER TABLE on_update_fkey_table ADD COLUMN X INT;
ROLLBACK;

-- case 1.7.1: SELECT to a reference table is followed by a DDL that is on
-- the foreign key column
BEGIN;
	SELECT count(*) FROM reference_table;

	-- make sure that the output isn't too verbose
 	SET LOCAL client_min_messages TO ERROR;
	ALTER TABLE on_update_fkey_table DROP COLUMN value_1 CASCADE;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM transitive_reference_table;

	-- make sure that the output isn't too verbose
 	SET LOCAL client_min_messages TO ERROR;
	ALTER TABLE on_update_fkey_table DROP COLUMN value_1 CASCADE;
ROLLBACK;

-- case 1.7.2: SELECT to a reference table is followed by a DDL that is on
-- the foreign key column after a parallel query has been executed
BEGIN;
	SELECT count(*) FROM unrelated_dist_table;
	SELECT count(*) FROM reference_table;

	ALTER TABLE on_update_fkey_table DROP COLUMN value_1 CASCADE;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM unrelated_dist_table;
	SELECT count(*) FROM transitive_reference_table;

	ALTER TABLE on_update_fkey_table DROP COLUMN value_1 CASCADE;
ROLLBACK;

-- case 1.7.3: SELECT to a reference table is followed by a DDL that is not on
-- the foreign key column, and a parallel query has already been executed
BEGIN;
	SELECT count(*) FROM unrelated_dist_table;
	SELECT count(*) FROM reference_table;
	ALTER TABLE on_update_fkey_table ADD COLUMN X INT;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM unrelated_dist_table;
	SELECT count(*) FROM transitive_reference_table;
	ALTER TABLE on_update_fkey_table ADD COLUMN X INT;
ROLLBACK;

-- case 1.8: SELECT to a reference table is followed by a COPY
BEGIN;
	SELECT count(*) FROM reference_table;
	COPY on_update_fkey_table FROM STDIN WITH CSV;
1001,99
1002,99
1003,99
1004,99
1005,99
\.
ROLLBACK;

BEGIN;
	SELECT count(*) FROM transitive_reference_table;
	COPY on_update_fkey_table FROM STDIN WITH CSV;
1001,99
1002,99
1003,99
1004,99
1005,99
\.
ROLLBACK;

-- case 2.1: UPDATE to a reference table is followed by a multi-shard SELECT
BEGIN;
	UPDATE reference_table SET id = 101 WHERE id = 99;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101;
ROLLBACK;

BEGIN;
	UPDATE transitive_reference_table SET id = 101 WHERE id = 99;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101;
ROLLBACK;

-- -- case 2.2: UPDATE to a reference table is followed by multiple router SELECT
-- BEGIN;
-- 	UPDATE reference_table SET id = 101 WHERE id = 99;
-- 	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101 AND id = 99;
-- 	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101 AND id = 199;
-- 	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101 AND id = 299;
-- 	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101 AND id = 399;
-- ROLLBACK;

BEGIN;
	UPDATE transitive_reference_table SET id = 101 WHERE id = 99;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101 AND id = 99;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101 AND id = 199;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101 AND id = 299;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 101 AND id = 399;
ROLLBACK;

-- case 2.3: UPDATE to a reference table is followed by a multi-shard UPDATE
BEGIN;
	UPDATE reference_table SET id = 101 WHERE id = 99;
	UPDATE on_update_fkey_table SET value_1 = 15;
ROLLBACK;

BEGIN;
	UPDATE transitive_reference_table SET id = 101 WHERE id = 99;
	UPDATE on_update_fkey_table SET value_1 = 15;
ROLLBACK;

-- case 2.4: UPDATE to a reference table is followed by multiple router UPDATEs
BEGIN;
	UPDATE reference_table SET id = 101 WHERE id = 99;
	UPDATE on_update_fkey_table SET value_1 = 101 WHERE id = 1;
	UPDATE on_update_fkey_table SET value_1 = 101 WHERE id = 2;
	UPDATE on_update_fkey_table SET value_1 = 101 WHERE id = 3;
	UPDATE on_update_fkey_table SET value_1 = 101 WHERE id = 4;
ROLLBACK;

BEGIN;
	UPDATE transitive_reference_table SET id = 101 WHERE id = 99;
	UPDATE on_update_fkey_table SET value_1 = 101 WHERE id = 1;
	UPDATE on_update_fkey_table SET value_1 = 101 WHERE id = 2;
	UPDATE on_update_fkey_table SET value_1 = 101 WHERE id = 3;
	UPDATE on_update_fkey_table SET value_1 = 101 WHERE id = 4;
ROLLBACK;

-- case 2.5: UPDATE to a reference table is followed by a DDL that touches fkey column
BEGIN;
	UPDATE reference_table SET id = 101 WHERE id = 99;
	ALTER TABLE on_update_fkey_table ALTER COLUMN value_1 SET DATA TYPE bigint;
ROLLBACK;

BEGIN;
	UPDATE transitive_reference_table SET id = 101 WHERE id = 99;
	ALTER TABLE on_update_fkey_table ALTER COLUMN value_1 SET DATA TYPE bigint;
ROLLBACK;

-- case 2.6: UPDATE to a reference table is followed by an unrelated DDL
BEGIN;
	UPDATE reference_table SET id = 101 WHERE id = 99;
	ALTER TABLE on_update_fkey_table ADD COLUMN value_1_X INT;
ROLLBACK;

BEGIN;
	UPDATE transitive_reference_table SET id = 101 WHERE id = 99;
	ALTER TABLE on_update_fkey_table ADD COLUMN value_1_X INT;
ROLLBACK;

-- case 2.7: UPDATE to a reference table is followed by COPY
BEGIN;
	UPDATE reference_table SET id = 101 WHERE id = 99;
	COPY on_update_fkey_table FROM STDIN WITH CSV;
1001,101
1002,101
1003,101
1004,101
1005,101
\.
ROLLBACK;

BEGIN;
	UPDATE transitive_reference_table SET id = 101 WHERE id = 99;
	COPY on_update_fkey_table FROM STDIN WITH CSV;
1001,101
1002,101
1003,101
1004,101
1005,101
\.
ROLLBACK;

-- case 2.8: UPDATE to a reference table is followed by TRUNCATE
BEGIN;
	UPDATE reference_table SET id = 101 WHERE id = 99;
	TRUNCATE on_update_fkey_table;
ROLLBACK;

BEGIN;
	UPDATE transitive_reference_table SET id = 101 WHERE id = 99;
	TRUNCATE on_update_fkey_table;
ROLLBACK;

-- case 3.1: an unrelated DDL to a reference table is followed by a real-time SELECT
BEGIN;
	ALTER TABLE reference_table ALTER COLUMN id SET DEFAULT 1001;
	SELECT count(*) FROM on_update_fkey_table;
ROLLBACK;

BEGIN;
	ALTER TABLE transitive_reference_table ALTER COLUMN id SET DEFAULT 1001;
	SELECT count(*) FROM on_update_fkey_table;
ROLLBACK;

-- case 3.2: DDL that touches fkey column to a reference table is followed by a real-time SELECT
BEGIN;
	ALTER TABLE reference_table ALTER COLUMN id SET DATA TYPE int;
	SELECT count(*) FROM on_update_fkey_table;
ROLLBACK;

BEGIN;
	ALTER TABLE transitive_reference_table ALTER COLUMN id SET DATA TYPE int;
	SELECT count(*) FROM on_update_fkey_table;
ROLLBACK;

-- case 3.3: DDL to a reference table followed by a multi shard UPDATE
BEGIN;
	ALTER TABLE reference_table ALTER COLUMN id SET DEFAULT 1001;
	UPDATE on_update_fkey_table SET value_1 = 5 WHERE id != 11;
ROLLBACK;

BEGIN;
	ALTER TABLE transitive_reference_table ALTER COLUMN id SET DEFAULT 1001;
	UPDATE on_update_fkey_table SET value_1 = 5 WHERE id != 11;
ROLLBACK;

-- case 3.4: DDL to a reference table followed by multiple router UPDATEs
BEGIN;
	ALTER TABLE reference_table ALTER COLUMN id SET DEFAULT 1001;
	UPDATE on_update_fkey_table SET value_1 = 98 WHERE id = 1;
	UPDATE on_update_fkey_table SET value_1 = 98 WHERE id = 2;
	UPDATE on_update_fkey_table SET value_1 = 98 WHERE id = 3;
	UPDATE on_update_fkey_table SET value_1 = 98 WHERE id = 4;
ROLLBACK;

BEGIN;
	ALTER TABLE transitive_reference_table ALTER COLUMN id SET DEFAULT 1001;
	UPDATE on_update_fkey_table SET value_1 = 98 WHERE id = 1;
	UPDATE on_update_fkey_table SET value_1 = 98 WHERE id = 2;
	UPDATE on_update_fkey_table SET value_1 = 98 WHERE id = 3;
	UPDATE on_update_fkey_table SET value_1 = 98 WHERE id = 4;
ROLLBACK;

-- case 3.5: DDL to reference table followed by a DDL to dist table
BEGIN;
	ALTER TABLE reference_table ALTER COLUMN id SET DATA TYPE smallint;
	CREATE INDEX fkey_test_index_1 ON on_update_fkey_table(value_1);
ROLLBACK;

BEGIN;
	ALTER TABLE transitive_reference_table ALTER COLUMN id SET DATA TYPE smallint;
	CREATE INDEX fkey_test_index_1 ON on_update_fkey_table(value_1);
ROLLBACK;

-- case 4.6: DDL to reference table followed by a DDL to dist table, both touching fkey columns
BEGIN;
	ALTER TABLE reference_table ALTER COLUMN id SET DATA TYPE smallint;
	ALTER TABLE on_update_fkey_table ALTER COLUMN value_1 SET DATA TYPE smallint;
ROLLBACK;

BEGIN;
	ALTER TABLE transitive_reference_table ALTER COLUMN id SET DATA TYPE smallint;
	ALTER TABLE on_update_fkey_table ALTER COLUMN value_1 SET DATA TYPE smallint;
ROLLBACK;

-- case 3.7: DDL to a reference table is followed by COPY
BEGIN;
	ALTER TABLE reference_table  ADD COLUMN X int;
	COPY on_update_fkey_table FROM STDIN WITH CSV;
1001,99
1002,99
1003,99
1004,99
1005,99
\.
ROLLBACK;

BEGIN;
	ALTER TABLE transitive_reference_table  ADD COLUMN X int;
	COPY on_update_fkey_table FROM STDIN WITH CSV;
1001,99
1002,99
1003,99
1004,99
1005,99
\.
ROLLBACK;

-- case 3.8: DDL to a reference table is followed by TRUNCATE
BEGIN;
	ALTER TABLE reference_table  ADD COLUMN X int;
	TRUNCATE on_update_fkey_table;
ROLLBACK;

BEGIN;
	ALTER TABLE transitive_reference_table  ADD COLUMN X int;
	TRUNCATE on_update_fkey_table;
ROLLBACK;

-- case 3.9: DDL to a reference table is followed by TRUNCATE
BEGIN;
	ALTER TABLE reference_table ALTER COLUMN id SET DATA TYPE smallint;
	TRUNCATE on_update_fkey_table;
ROLLBACK;

BEGIN;
	ALTER TABLE transitive_reference_table ALTER COLUMN id SET DATA TYPE smallint;
	TRUNCATE on_update_fkey_table;
ROLLBACK;

-----
--- Now, start testing the other way araound
-----

-- case 4.1: SELECT to a dist table is follwed by a SELECT to a reference table
BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	SELECT count(*) FROM reference_table;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	SELECT count(*) FROM transitive_reference_table;
ROLLBACK;

-- case 4.2: SELECT to a dist table is follwed by a DML to a reference table
BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	UPDATE reference_table SET id = 101 WHERE id = 99;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	UPDATE transitive_reference_table SET id = 101 WHERE id = 99;
ROLLBACK;

-- case 4.3: SELECT to a dist table is follwed by an unrelated DDL to a reference table
BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	ALTER TABLE reference_table ADD COLUMN X INT;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	ALTER TABLE transitive_reference_table ADD COLUMN X INT;
ROLLBACK;

-- case 4.4: SELECT to a dist table is follwed by a DDL to a reference table
BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	ALTER TABLE reference_table ALTER COLUMN id SET DATA TYPE smallint;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	ALTER TABLE transitive_reference_table ALTER COLUMN id SET DATA TYPE smallint;
ROLLBACK;

-- case 4.5: SELECT to a dist table is follwed by a TRUNCATE
\set VERBOSITY terse
--SET client_min_messages to LOG;

BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	TRUNCATE reference_table CASCADE;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE value_1 = 99;
	TRUNCATE transitive_reference_table CASCADE;
ROLLBACK;

-- case 4.6: Router SELECT to a dist table is followed by a TRUNCATE
BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 9;
	TRUNCATE reference_table CASCADE;
ROLLBACK;

BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 9;
	TRUNCATE transitive_reference_table CASCADE;
ROLLBACK;

-- case 4.7: SELECT to a dist table is followed by a DROP
-- DROP following SELECT is important as we error out after
-- the standart process utility hook drops the table.
-- That could cause SIGSEGV before the patch.
-- Below block should "successfully" error out
BEGIN;
	SELECT count(*) FROM on_update_fkey_table;
	DROP TABLE reference_table CASCADE;
ROLLBACK;

-- case 4.8: Router SELECT to a dist table is followed by a TRUNCATE
-- No errors expected from below block as SELECT there is a router
-- query
BEGIN;
	SELECT count(*) FROM on_update_fkey_table WHERE id = 9;
	DROP TABLE reference_table CASCADE;
ROLLBACK;

RESET client_min_messages;
\set VERBOSITY default

-- case 5.1: Parallel UPDATE on distributed table follow by a SELECT
BEGIN;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
	SELECT count(*) FROM reference_table;
ROLLBACK;

BEGIN;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
	SELECT count(*) FROM transitive_reference_table;
ROLLBACK;

-- case 5.2: Parallel UPDATE on distributed table follow by a UPDATE
BEGIN;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
	UPDATE reference_table SET id = 160 WHERE id = 15;
ROLLBACK;

BEGIN;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
	UPDATE transitive_reference_table SET id = 160 WHERE id = 15;
ROLLBACK;

-- case 5.3: Parallel UPDATE on distributed table follow by an unrelated DDL on reference table
BEGIN;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
	ALTER TABLE reference_table ADD COLUMN X INT;
ROLLBACK;

BEGIN;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
	ALTER TABLE transitive_reference_table ADD COLUMN X INT;
ROLLBACK;

-- case 5.4: Parallel UPDATE on distributed table follow by a related DDL on reference table
BEGIN;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
	ALTER TABLE reference_table ALTER COLUMN id SET DATA TYPE smallint;
ROLLBACK;

BEGIN;
	UPDATE on_update_fkey_table SET value_1 = 16 WHERE value_1 = 15;
	ALTER TABLE transitive_reference_table ALTER COLUMN id SET DATA TYPE smallint;
ROLLBACK;

-- case 6:2: Related parallel DDL on distributed table followed by SELECT on ref. table
BEGIN;
	ALTER TABLE on_update_fkey_table ALTER COLUMN value_1 SET DATA TYPE smallint;
	UPDATE reference_table SET id = 160 WHERE id = 15;
ROLLBACK;

BEGIN;
	ALTER TABLE on_update_fkey_table ALTER COLUMN value_1 SET DATA TYPE smallint;
	UPDATE transitive_reference_table SET id = 160 WHERE id = 15;
ROLLBACK;


-- case 6:4: Related parallel DDL on distributed table followed by SELECT on ref. table
BEGIN;
	ALTER TABLE on_update_fkey_table ADD COLUMN X int;
	UPDATE reference_table SET id = 160 WHERE id = 15;
ROLLBACK;

BEGIN;
	ALTER TABLE on_update_fkey_table ADD COLUMN X int;
	UPDATE transitive_reference_table SET id = 160 WHERE id = 15;
ROLLBACK;

-- case 6:5: Unrelated parallel DDL on distributed table followed by unrelated DDL on ref. table
BEGIN;
	ALTER TABLE on_update_fkey_table ADD COLUMN X int;
	ALTER TABLE reference_table ADD COLUMN X int;
ROLLBACK;

BEGIN;
	ALTER TABLE on_update_fkey_table ADD COLUMN X int;
	ALTER TABLE transitive_reference_table ADD COLUMN X int;
ROLLBACK;

-- case 6:6: Unrelated parallel DDL on distributed table followed by related DDL on ref. table
BEGIN;
	ALTER TABLE on_update_fkey_table ADD COLUMN X int;
	ALTER TABLE on_update_fkey_table ALTER COLUMN value_1 SET DATA TYPE smallint;
ROLLBACK;

-- some more extensive tests

-- UPDATE on dist table is followed by DELETE to reference table
BEGIN;
	UPDATE on_update_fkey_table SET value_1 = 5 WHERE id != 11;
	DELETE FROM reference_table  WHERE id = 99;
ROLLBACK;


-- an unrelated update followed by update on the reference table and update
-- on the cascading distributed table
-- note that the UPDATE on the reference table will try to set the execution
-- mode to sequential, which will fail since there is an already opened
-- parallel connections
BEGIN;
	UPDATE unrelated_dist_table SET value_1 = 15;
	UPDATE reference_table SET id = 101 WHERE id = 99;
	UPDATE on_update_fkey_table SET value_1 = 5 WHERE id != 11;
ROLLBACK;


CREATE TABLE distributed_table(id int PRIMARY KEY, value_1 int);
SELECT create_distributed_table('distributed_table', 'id');

ALTER TABLE
	distributed_table
ADD CONSTRAINT
	fkey_delete FOREIGN KEY(value_1)
REFERENCES
	reference_table(id) ON DELETE CASCADE;

-- load some more data for one more test with real-time selects
INSERT INTO distributed_table SELECT i, i % 10  FROM generate_series(0, 100) i;

-- this query returns 100 rows in Postgres, but not in Citus
-- see https://github.com/citusdata/citus_docs/issues/664 for the discussion
WITH t1 AS (DELETE FROM reference_table RETURNING id)
	SELECT count(*) FROM distributed_table, t1 WHERE  value_1 = t1.id;

-- this query should fail since we first to a parallel access to a distributed table
-- with t1, and then access to t2
WITH t1 AS (DELETE FROM distributed_table RETURNING id),
	t2 AS (DELETE FROM reference_table RETURNING id)
	SELECT count(*) FROM distributed_table, t1, t2 WHERE  value_1 = t1.id AND value_1 = t2.id;

-- similarly this should fail since we first access to a distributed
-- table via t1, and then access to the reference table in the main query
WITH t1 AS (DELETE FROM distributed_table RETURNING id)
	DELETE FROM reference_table RETURNING id;


-- finally, make sure that we can execute the same queries
-- in the sequential mode
BEGIN;

	SET LOCAL spq.multi_shard_modify_mode TO 'sequential';

	WITH t1 AS (DELETE FROM distributed_table RETURNING id),
		t2 AS (DELETE FROM reference_table RETURNING id)
		SELECT count(*) FROM distributed_table, t1, t2 WHERE  value_1 = t1.id AND value_1 = t2.id;
ROLLBACK;

BEGIN;

	SET LOCAL spq.multi_shard_modify_mode TO 'sequential';

	WITH t1 AS (DELETE FROM distributed_table RETURNING id)
		DELETE FROM reference_table RETURNING id;
ROLLBACK;

RESET client_min_messages;

\set VERBOSITY terse
DROP SCHEMA test_fkey_to_ref_in_tx CASCADE;
\set VERBOSITY default

SET search_path TO public;