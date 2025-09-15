CREATE SCHEMA tools;
SET SEARCH_PATH TO 'tools';
SET spq.next_shard_id TO 1240000;

-- test with invalid port, prevent OS dependent warning from being displayed
SET client_min_messages to ERROR;

SELECT * FROM master_run_on_worker(ARRAY['localhost']::text[], ARRAY['666']::int[],
								   ARRAY['select count(*) from pg_dist_shard']::text[],
								   false);

SELECT * FROM master_run_on_worker(ARRAY['localhost']::text[], ARRAY['666']::int[],
								   ARRAY['select count(*) from pg_dist_shard']::text[],
								   true);
RESET client_min_messages;

-- store worker node name and port
\set node_name '''localhost'''
SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY['SELECT 1 + 1;'],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY[$q$SELECT '包含中文、 单双引号'' ""、换行符\n';$q$],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY['CREATE TABLE IF NOT EXISTS worker_test(id INT, note TEXT);'],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY['CREATE INDEX IF NOT EXISTS idx_note ON worker_test(note);'],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY[$q$INSERT INTO worker_test VALUES(1,'hello worker');$q$],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY['SELECT row_to_json(worker_test) FROM worker_test LIMIT 1;'],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY['DROP TABLE IF EXISTS worker_test;'],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY[$q$
	CREATE OR REPLACE FUNCTION greet(name TEXT) RETURNS TEXT AS $$
	BEGIN RETURN 'HELLO, ' || name;END;
	$$ LANGUAGE plpgsql;
	$q$],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY['SELECT greet(''spq'');'],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY[$q$
	CREATE OR REPLACE PROCEDURE hello_world()
	AS
	BEGIN
		RAISE NOTICE 'Hello, openGauss!';
	END;
	$q$],
	FALSE
);

SELECT * FROM master_run_on_worker(
	ARRAY[:node_name]::text[],
	ARRAY[:worker_1_port]::int[],
	ARRAY['CALL hello_world();'],
	FALSE
);

DROP SCHEMA tools CASCADE;
RESET SEARCH_PATH;

