
--create mysql compatible database
CREATE DATABASE mysqldb01 DBCOMPATIBILITY = 'B';
CREATE USER monadmin01 monadmin PASSWORD 'Cmcc@123';
CREATE USER user01 PASSWORD 'Cmcc@123';

\c mysqldb01
SET track_stmt_stat_level = 'L1,L0';

SHOW track_stmt_stat_level;
SHOW track_stmt_parameter;
SHOW enable_thread_pool;

SET SESSION AUTHORIZATION monadmin01 PASSWORD 'Cmcc@123';

SELECT COUNT(*) FROM performance_schema.events_statements_current;
SELECT * FROM performance_schema.events_statements_current limit 1;
SELECT COUNT(*) FROM performance_schema.events_statements_history;
SELECT * FROM performance_schema.events_statements_history limit 1;
SELECT COUNT(*) FROM performance_schema.events_statements_summary_by_digest;
SELECT * FROM performance_schema.events_statements_summary_by_digest limit 1;

SELECT COUNT(*) FROM performance_schema.events_waits_current;
SELECT * FROM performance_schema.events_waits_current limit 1;
SELECT COUNT(*) FROM performance_schema.events_waits_summary_global_by_event_name;
SELECT * FROM performance_schema.events_waits_summary_global_by_event_name limit 1;

SELECT COUNT(*) FROM performance_schema.events_waits_current;
SELECT * FROM performance_schema.events_waits_current limit 1;
SELECT COUNT(*) FROM performance_schema.events_waits_summary_global_by_event_name;
SELECT * FROM performance_schema.events_waits_summary_global_by_event_name limit 1;

SELECT count(*) FROM performance_schema.file_summary_by_instance;
SELECT * FROM performance_schema.file_summary_by_instance limit 1;
SELECT count(*) FROM performance_schema.table_io_waits_summary_by_table;
SELECT * FROM performance_schema.table_io_waits_summary_by_table limit 1;
SELECT count(*) FROM performance_schema.table_io_waits_summary_by_index_usage;
SELECT * FROM performance_schema.table_io_waits_summary_by_index_usage limit 1;

SET SESSION AUTHORIZATION user01 PASSWORD 'Cmcc@123';

SELECT COUNT(*) FROM performance_schema.events_statements_current;
SELECT COUNT(*) FROM performance_schema.events_statements_history;
SELECT COUNT(*) FROM performance_schema.events_statements_summary_by_digest;

SELECT COUNT(*) FROM performance_schema.events_waits_current;
SELECT COUNT(*) FROM performance_schema.events_waits_summary_global_by_event_name;

SELECT COUNT(*) FROM performance_schema.events_waits_current;
SELECT COUNT(*) FROM performance_schema.events_waits_summary_global_by_event_name;

SELECT count(*) FROM performance_schema.file_summary_by_instance;
SELECT count(*) FROM performance_schema.table_io_waits_summary_by_table;
SELECT count(*) FROM performance_schema.table_io_waits_summary_by_index_usage;

\c contrib_regression
DROP user monadmin01 CASCADE;
DROP user user01 CASCADE;
DROP DATABASE mysqldb01;
