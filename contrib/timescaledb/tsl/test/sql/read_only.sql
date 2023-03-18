-- This file and its contents are licensed under the Timescale License.
-- Please see the included NOTICE for copyright information and
-- LICENSE-TIMESCALE for a copy of the license.

\c :TEST_DBNAME :ROLE_SUPERUSER

-- Following tests checks that API functions which modify data (including catalog)
-- properly recognize read-only transaction state
--

-- create_hypertable()
--
CREATE TABLE test_table(time bigint NOT NULL, device int);

SET default_transaction_read_only TO on;

\set ON_ERROR_STOP 0
SELECT * FROM create_hypertable('test_table', 'time');
\set ON_ERROR_STOP 1

SET default_transaction_read_only TO off;
SELECT * FROM create_hypertable('test_table', 'time', chunk_time_interval => 1000000::bigint);

SET default_transaction_read_only TO on;

\set ON_ERROR_STOP 0

-- set_chunk_time_interval()
--
SELECT * FROM set_chunk_time_interval('test_table', 2000000000::bigint);

-- set_number_partitions()
--
SELECT * FROM set_number_partitions('test_table', 2);

-- set_adaptive_chunking()
--
SELECT * FROM set_adaptive_chunking('test_table', '2MB');

-- drop_chunks()
--
SELECT * FROM drop_chunks(table_name => 'test_table', older_than => 10);
SELECT * FROM drop_chunks(table_name => 'test_table', older_than => 10, cascade_to_materializations => true);

-- add_dimension()
--
SELECT * FROM add_dimension('test_table', 'device', chunk_time_interval => 100);

\set ON_ERROR_STOP 1

-- tablespaces
--
SET default_transaction_read_only TO off;

SET client_min_messages TO error;
DROP TABLESPACE IF EXISTS tablespace1;
RESET client_min_messages;
CREATE TABLESPACE tablespace1 OWNER :ROLE_SUPERUSER LOCATION :TEST_TABLESPACE1_PATH;

SET default_transaction_read_only TO on;

-- attach_tablespace()
--
\set ON_ERROR_STOP 0
SELECT * FROM attach_tablespace('tablespace1', 'test_table');
\set ON_ERROR_STOP 1

SET default_transaction_read_only TO off;
SELECT * FROM attach_tablespace('tablespace1', 'test_table');

-- detach_tablespace()
--
SET default_transaction_read_only TO on;
\set ON_ERROR_STOP 0
SELECT * FROM detach_tablespace('tablespace1', 'test_table');
\set ON_ERROR_STOP 1

-- detach_tablespaces()
--
\set ON_ERROR_STOP 0
SELECT * FROM detach_tablespaces('test_table');
\set ON_ERROR_STOP 1
SET default_transaction_read_only TO off;
SELECT * FROM detach_tablespaces('test_table');

DROP TABLESPACE tablespace1;

-- drop hypertable
--
SET default_transaction_read_only TO on;
\set ON_ERROR_STOP 0
DROP TABLE test_table;
\set ON_ERROR_STOP 1
SET default_transaction_read_only TO off;
DROP TABLE test_table;

-- Test some read-only cases of DDL operations
-- 
CREATE TABLE test_table(time bigint NOT NULL, device int);
SELECT * FROM create_hypertable('test_table', 'time', chunk_time_interval => 1000000::bigint);
INSERT INTO test_table VALUES (0, 1), (1, 1), (2, 2);

SET default_transaction_read_only TO on;

-- CREATE INDEX
--
\set ON_ERROR_STOP 0
CREATE INDEX test_table_device_idx ON test_table(device);
\set ON_ERROR_STOP 1

-- TRUNCATE
--
\set ON_ERROR_STOP 0
TRUNCATE test_table;
\set ON_ERROR_STOP 1

-- ALTER TABLE
--
\set ON_ERROR_STOP 0
ALTER TABLE test_table DROP COLUMN device;
ALTER TABLE test_table ADD CONSTRAINT device_check CHECK (device > 0);
\set ON_ERROR_STOP 1

-- VACUUM
--
\set ON_ERROR_STOP 0
VACUUM test_table;
\set ON_ERROR_STOP 1

-- CLUSTER
--
\set ON_ERROR_STOP 0
CLUSTER test_table USING test_table_time_idx;
\set ON_ERROR_STOP 1

-- COPY FROM
--
\set ON_ERROR_STOP 0
COPY test_table (time, device) FROM STDIN DELIMITER ',';
\set ON_ERROR_STOP 1

-- COPY TO (expect to be working in read-only mode)
--
COPY (SELECT * FROM test_Table ORDER BY time) TO STDOUT;

-- Test Continuous Aggregates
-- 
SET default_transaction_read_only TO off;

CREATE TABLE test_contagg (
  observation_time  TIMESTAMPTZ       NOT NULL,
  device_id         TEXT              NOT NULL,
  metric            DOUBLE PRECISION  NOT NULL,
  PRIMARY KEY(observation_time, device_id)
);

SELECT create_hypertable('test_contagg', 'observation_time');

SET default_transaction_read_only TO on;

-- CREATE VIEW
--
\set ON_ERROR_STOP 0

CREATE VIEW test_contagg_view
WITH (timescaledb.continuous)
AS
SELECT
  time_bucket('1 hour', observation_time) as bucket,
  device_id,
  avg(metric) as metric_avg,
  max(metric)-min(metric) as metric_spread
FROM
  test_contagg
GROUP BY bucket, device_id;

\set ON_ERROR_STOP 1
