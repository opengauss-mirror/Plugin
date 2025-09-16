-- spq only support in A compatibility mode
DO $$
DECLARE
    db_compat VARCHAR;
BEGIN
    SELECT datcompatibility INTO db_compat 
    FROM pg_database 
    WHERE datname = current_database();
    
    -- error if not in A compatibility mode
    IF db_compat != 'A' THEN
        RAISE EXCEPTION 'SPQ can only be installed in a database with A compatibility mode';
    END IF;
END $$;