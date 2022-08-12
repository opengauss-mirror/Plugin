DROP FUNCTION IF EXISTS pg_catalog.b_db_sys_real_timestamp(integer) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.b_db_sys_real_timestamp(integer) returns datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_sys_real_timestamp';

DROP FUNCTION IF EXISTS pg_catalog.b_db_statement_start_time(integer) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.b_db_statement_start_time(integer) returns time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_statement_start_time';

DROP FUNCTION IF EXISTS pg_catalog.b_db_statement_start_timestamp(integer) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.b_db_statement_start_timestamp(integer) returns datetime LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'b_db_statement_start_timestamp';

DROP FUNCTION IF EXISTS pg_catalog.curdate() CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.curdate() returns date LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'curdate';
