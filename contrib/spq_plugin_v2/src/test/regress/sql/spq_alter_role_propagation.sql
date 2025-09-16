CREATE SCHEMA alter_role;
CREATE SCHEMA ",CitUs,.TeeN!?";

-- test if the passowrd of the extension owner can be upgraded
CREATE ROLE test_role with login password 'spq_test1';
ALTER ROLE test_role PASSWORD 'Test@123' VALID UNTIL 'infinity';
SELECT run_command_on_workers($$SELECT row(rolname, rolsuper, rolinherit,  rolcreaterole, rolcreatedb, rolcanlogin, rolreplication, rolconnlimit, EXTRACT (year FROM rolvaliduntil)) FROM pg_authid WHERE rolname = 'test_role'$$);
SELECT workers.result = pg_authid.rolpassword AS password_is_same FROM run_command_on_workers($$SELECT rolpassword FROM pg_authid WHERE rolname = 'test_role'$$) workers, pg_authid WHERE pg_authid.rolname = 'test_role';

-- test if the password and some connection settings are propagated when a node gets added
ALTER ROLE test_role WITH CONNECTION LIMIT 66 VALID UNTIL '2032-05-05' PASSWORD 'Test@1234';
SELECT row(rolname, rolsuper, rolinherit,  rolcreaterole, rolcreatedb, rolcanlogin, rolreplication, rolconnlimit, rolpassword, EXTRACT (year FROM rolvaliduntil)) FROM pg_authid WHERE rolname = 'alter_role_1';
SELECT run_command_on_workers($$SELECT row(rolname, rolsuper, rolinherit,  rolcreaterole, rolcreatedb, rolcanlogin, rolreplication, rolconnlimit, EXTRACT (year FROM rolvaliduntil)) FROM pg_authid WHERE rolname = 'test_role'$$);
SELECT workers.result = pg_authid.rolpassword AS password_is_same FROM run_command_on_workers($$SELECT rolpassword FROM pg_authid WHERE rolname = 'test_role'$$) workers, pg_authid WHERE pg_authid.rolname = 'test_role';
SELECT spq_remove_node('localhost', :worker_1_port);
ALTER ROLE test_role WITH CONNECTION LIMIT 0 VALID UNTIL '2052-05-05' PASSWORD 'Test@12345';
SELECT row(rolname, rolsuper, rolinherit,  rolcreaterole, rolcreatedb, rolcanlogin, rolreplication, rolconnlimit, rolpassword, EXTRACT (year FROM rolvaliduntil)) FROM pg_authid WHERE rolname = 'alter_role_1';
SELECT run_command_on_workers($$SELECT row(rolname, rolsuper, rolinherit,  rolcreaterole, rolcreatedb, rolcanlogin, rolreplication, rolconnlimit, EXTRACT (year FROM rolvaliduntil)) FROM pg_authid WHERE rolname = 'test_role'$$);
SELECT workers.result = pg_authid.rolpassword AS password_is_same FROM run_command_on_workers($$SELECT rolpassword FROM pg_authid WHERE rolname = 'test_role'$$) workers, pg_authid WHERE pg_authid.rolname = 'test_role';
SELECT 1 FROM spq_add_node('localhost', :worker_1_port);
SELECT row(rolname, rolsuper, rolinherit,  rolcreaterole, rolcreatedb, rolcanlogin, rolreplication, rolconnlimit, rolpassword, EXTRACT (year FROM rolvaliduntil)) FROM pg_authid WHERE rolname = 'alter_role_1';
SELECT run_command_on_workers($$SELECT row(rolname, rolsuper, rolinherit,  rolcreaterole, rolcreatedb, rolcanlogin, rolreplication, rolconnlimit, EXTRACT (year FROM rolvaliduntil)) FROM pg_authid WHERE rolname = 'test_role'$$);
SELECT workers.result = pg_authid.rolpassword AS password_is_same FROM run_command_on_workers($$SELECT rolpassword FROM pg_authid WHERE rolname = 'test_role'$$) workers, pg_authid WHERE pg_authid.rolname = 'test_role';

-- check user, database and postgres wide SET settings.
-- pre check
SELECT run_command_on_workers('SHOW enable_hashjoin');
SELECT run_command_on_workers('SHOW enable_indexonlyscan');
SELECT run_command_on_workers('SHOW enable_hashagg');

-- remove 1 node to verify settings are copied when the node gets added back
SELECT spq_remove_node('localhost', :worker_1_port);

-- change a setting for all users
ALTER ROLE test_role SET enable_hashjoin TO FALSE;
SELECT run_command_on_workers('SHOW enable_hashjoin');
ALTER ROLE test_role IN DATABASE regression SET enable_indexonlyscan TO FALSE;
SELECT run_command_on_workers('SHOW enable_indexonlyscan');

-- alter configuration_parameter defaults for a user
ALTER ROLE test_role SET enable_hashagg TO FALSE;
SELECT run_command_on_workers('SHOW enable_hashagg');

-- provide a list of values in a supported configuration
ALTER ROLE test_role SET search_path TO ",CitUs,.TeeN!?", alter_role, public;
-- test user defined GUCs that appear to be a list, but instead a single string
ALTER ROLE test_role SET public.myguc TO "Hello, World";

-- test for configuration values that should not be downcased even when unquoted
ALTER ROLE test_role SET lc_messages TO 'C';

-- add worker and check all settings are copied
SELECT 1 FROM spq_add_node('localhost', :worker_1_port);
SELECT run_command_on_workers('SHOW enable_hashjoin');
SELECT run_command_on_workers('SHOW enable_indexonlyscan');
SELECT run_command_on_workers('SHOW enable_hashagg');
SELECT run_command_on_workers('SHOW search_path');
SELECT run_command_on_workers('SHOW lc_messages');

-- reset to default values
ALTER ROLE test_role RESET enable_hashagg;
SELECT run_command_on_workers('SHOW enable_hashagg');

-- RESET ALL with IN DATABASE clause
ALTER ROLE test_role RESET ALL;

-- post check 1 - should have settings reset except for database specific settings
SELECT run_command_on_workers('SHOW enable_hashjoin');
SELECT run_command_on_workers('SHOW enable_indexonlyscan');
SELECT run_command_on_workers('SHOW enable_hashagg');

ALTER ROLE test_role IN DATABASE regression RESET ALL;

-- post check 2 - should have all settings reset
SELECT run_command_on_workers('SHOW enable_hashjoin');
SELECT run_command_on_workers('SHOW enable_indexonlyscan');
SELECT run_command_on_workers('SHOW enable_hashagg');

-- check that ALTER ROLE SET is not propagated when scoped to a different database
-- also test case sensitivity
CREATE DATABASE "REGRESSION";
ALTER ROLE test_role IN DATABASE "REGRESSION" SET public.myguc TO "Hello from coordinator only";
SELECT d.datname, r.setconfig FROM pg_db_role_setting r LEFT JOIN pg_database d ON r.setdatabase=d.oid WHERE r.setconfig::text LIKE '%Hello from coordinator only%';
SELECT run_command_on_workers($$SELECT json_agg((d.datname, r.setconfig)) FROM pg_db_role_setting r LEFT JOIN pg_database d ON r.setdatabase=d.oid WHERE r.setconfig::text LIKE '%Hello from coordinator only%'$$);
DROP DATABASE "REGRESSION";

-- make sure alter role set is not propagated when the feature is deliberately turned off
SET spq.enable_alter_role_set_propagation TO off;
-- remove 1 node to verify settings are NOT copied when the node gets added back
SELECT spq_remove_node('localhost', :worker_1_port);
ALTER ROLE test_role SET enable_hashjoin TO FALSE;
SELECT 1 FROM spq_add_node('localhost', :worker_1_port);
SELECT run_command_on_workers('SHOW enable_hashjoin');
ALTER ROLE test_role RESET enable_hashjoin;
SELECT run_command_on_workers('SHOW enable_hashjoin');

-- check altering search path won't cause public shards being not found
CREATE TABLE test_search_path(a int);
SELECT create_distributed_table('test_search_path', 'a');
CREATE SCHEMA test_sp;
ALTER USER test_role SET search_path TO test_sp;
SELECT COUNT(*) FROM public.test_search_path;
ALTER USER test_role RESET search_path;

DROP TABLE test_search_path;
DROP SCHEMA alter_role, ",CitUs,.TeeN!?", test_sp CASCADE;