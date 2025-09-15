CREATE OR REPLACE FUNCTION pg_catalog.spq_run_local_command(command text)
RETURNS void AS $$
BEGIN
    EXECUTE $1;
END;
$$ LANGUAGE PLPGSQL;
COMMENT ON FUNCTION pg_catalog.spq_run_local_command(text)
    IS 'spq_run_local_command executes the input command';
