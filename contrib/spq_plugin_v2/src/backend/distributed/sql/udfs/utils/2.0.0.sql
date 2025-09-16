-- like ::regclass but never throw error
CREATE FUNCTION __$spq_internal$__.to_regclass(text)
RETURNS oid
AS $$
DECLARE
    result oid;
BEGIN
    SELECT c.oid INTO result
    FROM pg_class c
    JOIN pg_namespace n ON c.relnamespace = n.oid
    WHERE n.nspname = COALESCE(substring($1 from '([^.]+)\.'), 'public')
    AND c.relname = substring($1 from '([^.]+)$')
    AND c.relkind = 'r';

    RETURN result;
EXCEPTION WHEN OTHERS THEN
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;
