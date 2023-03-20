-- This file and its contents are licensed under the Timescale License.
-- Please see the included NOTICE for copyright information and
-- LICENSE-TIMESCALE for a copy of the license.

CREATE TABLE  with_oids (a INTEGER NOT NULL, b INTEGER) WITH OIDS;
SELECT table_name FROM create_hypertable('with_oids', 'a', chunk_time_interval=> 10);

\set ON_ERROR_STOP 0
ALTER TABLE with_oids SET (timescaledb.compress, timescaledb.compress_orderby='a');
\set ON_ERROR_STOP 1

DROP TABLE with_oids;
