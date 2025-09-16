CREATE SCHEMA tools;
SET SEARCH_PATH TO 'tools';
SET spq.next_shard_id TO 1240000;

SELECT * FROM spq_run_local_command(
	'SELECT 1 + 1;'
);

SELECT * FROM spq_run_local_command(
	$$SELECT '包含中文、 单双引号'' ""、换行符\n';$$
);

SELECT * FROM spq_run_local_command(
	'CREATE TABLE IF NOT EXISTS worker_test(id INT, note TEXT);'
);

SELECT * FROM spq_run_local_command(
	'CREATE INDEX IF NOT EXISTS idx_note ON worker_test(note);'
);

SELECT * FROM spq_run_local_command(
	$$INSERT INTO worker_test VALUES(1,'hello worker');$$
);

SELECT * FROM worker_test;

SELECT * FROM spq_run_local_command(
	'SELECT row_to_json(worker_test) FROM worker_test LIMIT 1;'
);

SELECT * FROM spq_run_local_command(
	'DROP TABLE IF EXISTS worker_test;'
);

SELECT * FROM spq_run_local_command(
	$q$
	CREATE OR REPLACE FUNCTION greet(name TEXT) RETURNS TEXT AS $$
	BEGIN RETURN 'HELLO, ' || name;END;
	$$ LANGUAGE plpgsql;
	$q$
);

SELECT * FROM spq_run_local_command(
	'SELECT greet(''spq'');'
);

SELECT * FROM spq_run_local_command(
	$q$
	CREATE OR REPLACE PROCEDURE hello_world()
	AS
	BEGIN
		RAISE NOTICE 'Hello, openGauss!';
	END;
	$q$
);

SELECT * FROM spq_run_local_command(
	'CALL hello_world();'
);

DROP SCHEMA tools CASCADE;
RESET SEARCH_PATH;

