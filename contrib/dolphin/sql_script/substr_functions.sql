DROP FUNCTION IF EXISTS pg_catalog.mid(text, int, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.mid(text, int, int) RETURNS text AS
$$
BEGIN
    RETURN (SELECT substr($1, $2, $3));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.mid(text, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.mid(text, int) RETURNS text AS
$$
BEGIN
    RETURN (SELECT substr($1, $2));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.mid(bytea, int, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.mid(bytea, int, int) RETURNS bytea AS
$$
BEGIN
    RETURN (SELECT substr($1, $2, $3));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.mid(bytea, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.mid(bytea, int) RETURNS bytea AS
$$
BEGIN
    RETURN (SELECT substr($1, $2));
END;
$$
LANGUAGE plpgsql;
