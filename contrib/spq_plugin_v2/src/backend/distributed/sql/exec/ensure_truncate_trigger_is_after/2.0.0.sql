CREATE FUNCTION ensure_truncate_trigger_is_after()
    RETURNS void
    LANGUAGE plpgsql
    SET search_path = pg_catalog
    AS $$
DECLARE
    table_name regclass;
    command text;
    trigger_name text;
BEGIN
    --
    -- register triggers
    --
    FOR table_name, trigger_name IN SELECT tgrelid::regclass, tgname
      FROM pg_dist_partition
      JOIN pg_trigger ON tgrelid=logicalrelid
      JOIN pg_class ON pg_class.oid=logicalrelid
      WHERE
        tgname LIKE 'truncate_trigger_%' AND tgfoid = 'spq_truncate_trigger'::regproc
    LOOP
        command := 'drop trigger ' || trigger_name || ' on ' || table_name;
        EXECUTE command;
        command := 'create trigger ' || trigger_name || ' after truncate on ' || table_name || ' execute procedure pg_catalog.spq_truncate_trigger()';
        EXECUTE command;
        command := 'update pg_trigger set tgisinternal = true where tgname = ' || quote_literal(trigger_name);
        EXECUTE command;
    END LOOP;
END;
$$;

SELECT ensure_truncate_trigger_is_after();
DROP FUNCTION ensure_truncate_trigger_is_after;
