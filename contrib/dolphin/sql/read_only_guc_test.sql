-- b compatibility case
drop database if exists read_only_guc_test;
-- create database read_only_guc_test dbcompatibility 'b';
create database read_only_guc_test with DBCOMPATIBILITY = 'B';
\c read_only_guc_test

show version_comment;
show auto_increment_increment;
show character_set_client;
show character_set_connection;
show character_set_results;
show character_set_server;
show collation_server;
show collation_connection;
show init_connect;
show interactive_timeout;
show license;
show max_allowed_packet;
show net_buffer_length;
show net_write_timeout;
show query_cache_size;
show query_cache_type;
show system_time_zone;
show time_zone;
show wait_timeout;

SELECT * FROM pg_settings WHERE NAME='version_comment';
SELECT * FROM pg_settings WHERE NAME='auto_increment_increment';
SELECT * FROM pg_settings WHERE NAME='character_set_client';
SELECT * FROM pg_settings WHERE NAME='character_set_connection';
SELECT * FROM pg_settings WHERE NAME='character_set_results';
SELECT * FROM pg_settings WHERE NAME='character_set_server';
SELECT * FROM pg_settings WHERE NAME='collation_server';
SELECT * FROM pg_settings WHERE NAME='collation_connection';
SELECT * FROM pg_settings WHERE NAME='init_connect';
SELECT * FROM pg_settings WHERE NAME='interactive_timeout';
SELECT * FROM pg_settings WHERE NAME='license';
SELECT * FROM pg_settings WHERE NAME='max_allowed_packet';
SELECT * FROM pg_settings WHERE NAME='net_buffer_length';
SELECT * FROM pg_settings WHERE NAME='net_write_timeout';
SELECT * FROM pg_settings WHERE NAME='query_cache_size';
SELECT * FROM pg_settings WHERE NAME='query_cache_type';
SELECT * FROM pg_settings WHERE NAME='system_time_zone';
SELECT * FROM pg_settings WHERE NAME='time_zone';
SELECT * FROM pg_settings WHERE NAME='wait_timeout';

\c postgres
drop database if exists read_only_guc_test;
