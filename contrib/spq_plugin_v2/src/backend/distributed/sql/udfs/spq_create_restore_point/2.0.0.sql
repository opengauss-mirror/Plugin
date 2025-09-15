-- distributed backups
CREATE OR REPLACE FUNCTION pg_catalog.spq_create_restore_point(IN restore_name text)
RETURNS TABLE (
    nodeid integer,
    nodename text,
    nodeport integer,
    xlogptr text
)
LANGUAGE C STRICT
AS 'MODULE_PATHNAME', $$spq_create_restore_point$$;

COMMENT ON FUNCTION pg_catalog.spq_create_restore_point(text)
  IS 'temporarily block writes and create a named restore point on all nodes';
