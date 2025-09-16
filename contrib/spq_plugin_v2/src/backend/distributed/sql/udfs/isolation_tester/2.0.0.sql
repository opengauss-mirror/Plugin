CREATE FUNCTION __$spq_internal$__.replace_isolation_tester_func()
RETURNS void AS $$
  DECLARE
    version integer := current_setting('server_version_num');
  BEGIN
    ALTER FUNCTION pg_catalog.pg_blocking_pids(integer)
      RENAME TO old_pg_blocking_pids;
    ALTER FUNCTION pg_catalog.spq_blocking_pids(integer)
      RENAME TO pg_blocking_pids;
  END;
$$ LANGUAGE plpgsql;

CREATE FUNCTION __$spq_internal$__.restore_isolation_tester_func()
RETURNS void AS $$
  DECLARE
    version integer := current_setting('server_version_num');
  BEGIN
    ALTER FUNCTION pg_catalog.pg_blocking_pids(integer)
      RENAME TO spq_blocking_pids;
    ALTER FUNCTION pg_catalog.old_pg_blocking_pids(integer)
      RENAME TO pg_blocking_pids;
  END;
$$ LANGUAGE plpgsql;

CREATE FUNCTION __$spq_internal$__.refresh_isolation_tester_prepared_statement()
RETURNS void AS $$
  BEGIN
    -- isolation creates a prepared statement using the old function before tests have a
    -- chance to call replace_isolation_tester_func. By calling that prepared statement
    -- with a different search_path we force a re-parse which picks up the new function
    SET search_path = '__$spq$__';
    EXECUTE 'EXECUTE isolationtester_waiting (0)';
    RESET search_path;
  END;
$$ LANGUAGE plpgsql;
