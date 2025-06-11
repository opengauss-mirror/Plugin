drop schema if exists test_guc_select_and_set;
create schema test_guc_select_and_set;
set enable_set_variable_b_format to on;
select @@session.auto_increment_increment AS auto_increment_increment, 
@@character_set_client AS character_set_client,
@@character_set_connection AS character_set_connection, 
@@character_set_results AS character_set_results, 
@@character_set_server AS character_set_server, 
@@collation_server AS collation_server, 
@@collation_connection AS collation_connection, 
@@init_connect AS init_connect, 
@@interactive_timeout AS interactive_timeout,
@@license AS license, 
@@lower_case_table_names AS lower_case_table_names, 
@@max_allowed_packet AS max_allowed_packet, 
@@net_buffer_length AS net_buffer_length, 
@@net_write_timeout AS net_write_timeout, 
@@query_cache_size AS query_cache_size, 
@@query_cache_type AS query_cache_type, 
@@sql_mode AS sql_mode, 
@@system_time_zone AS system_time_zone, 
@@time_zone AS time_zone, 
@@transaction_isolation AS transaction_isolation, 
@@wait_timeout AS wait_timeout;

set auto_increment_increment = 1;
set @@auto_increment_increment = 1;
set @@session.auto_increment_increment = 1;

set character_set_client = utf8;
set @@character_set_client = utf8;
set @@session.character_set_client = utf8;

set character_set_connection = utf8;
set @@character_set_connection = utf8;
set @@session.character_set_connection = utf8;

set character_set_results = utf8;
set @@character_set_results = utf8;
set @@session.character_set_results = utf8;

set character_set_results = null;
show character_set_results;
set @@character_set_results = null;
show character_set_results;
set @@session.character_set_results = null;
show character_set_results;
set session character_set_results = null;
show character_set_results;

set character_set_server = latin1;
set @@character_set_server = latin1;
set @@session.character_set_server = latin1;

set collation_server = latin1_swedish_ci;
set @@collation_server = latin1_swedish_ci;
set @@session.collation_server = latin1_swedish_ci;

set collation_connection = "collation_connection";
set @@collation_connection = "collation_connection";
set @@session.collation_connection = "collation_connection";

set init_connect = "init_connect";
set @@init_connect = "init_connect";
set @@session.init_connect = "init_connect";

set interactive_timeout = 28800;
set @@interactive_timeout = 28800;
set @@session.interactive_timeout = 28800;

set max_allowed_packet = 4194304;
set @@max_allowed_packet = 4194304;
set @@session.max_allowed_packet = 4194304;

set net_buffer_length = 16384;
set @@net_buffer_length = 16384;
set @@session.net_buffer_length = 16384;

set net_write_timeout = 60;
set @@net_write_timeout = 60;
set @@session.net_write_timeout = 60;

set query_cache_size = 1048576;
set @@query_cache_size = 1048576;
set @@session.query_cache_size = 1048576;

set query_cache_type = 0;
set @@query_cache_type = 0;
set @@session.query_cache_type = 0;

set system_time_zone = "system_time_zone";
set @@system_time_zone = "system_time_zone";
set @@session.system_time_zone = "system_time_zone";

set time_zone = "time_zone";
set @@time_zone = "time_zone";
set @@session.time_zone = "time_zone";

set transaction_isolation = "default";
set @@transaction_isolation = "default";
set @@session.transaction_isolation = "default";

set wait_timeout = 28800;
set @@wait_timeout = 28800;
set @@session.wait_timeout = 28800;  

set sql_mode = "sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length";
set @@sql_mode = "sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length";
set @@session.sql_mode = "sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length";

set lower_case_table_names = 1;
set @@lower_case_table_names = 1;
set @@session.lower_case_table_names = 1;

set license = "MulanPSL-2.0";
set @@license = "MulanPSL-2.0";
set @@session.license = "MulanPSL-2.0";

show dolphin.sql_mode;
show sql_mode;
set sql_mode = '';
show dolphin.sql_mode;
show sql_mode;

show dolphin.lower_case_table_names;
show lower_case_table_names;
set lower_case_table_names = 0;
show dolphin.lower_case_table_names;
show lower_case_table_names;

set @ta=@@license;
set @tb=@ta;
select @ta;
select @ta=@@license;
select @ta!=@@license;
select @ta<>@@license;
select @ta>@@license;
select @ta<@@license;
select @ta=@tb;
select @ta!=@tb;
select @ta<>@tb;
select @ta>@tb;
select @ta<@tb;

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS;
SET UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS;
SET FOREIGN_KEY_CHECKS=0;

SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

show b_format_behavior_compat_options;
show character_set_connection;
set character_set_connection = binary;
set b_format_behavior_compat_options = 'enable_multi_charset';
set character_set_connection = binary;
show character_set_connection;
show collation_connection;

reset b_format_behavior_compat_options;

drop schema test_guc_select_and_set cascade;
