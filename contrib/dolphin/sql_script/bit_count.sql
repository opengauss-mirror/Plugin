DROP FUNCTION IF EXISTS pg_catalog.bit_count(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bit_count(numeric) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bit_count(bit) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.bit_count(t1 text) RETURNS text AS
$$
BEGIN
    RETURN (SELECT LENGTH(REPLACE(bin(t1),'0','')));
END;
$$
LANGUAGE plpgsql;
    
CREATE OR REPLACE FUNCTION pg_catalog.bit_count(t1 numeric) RETURNS text AS
$$
BEGIN
    RETURN (SELECT LENGTH(REPLACE(bin(t1),'0','')));
END;
$$
LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.bit_count(t1 bit) RETURNS text AS
$$
DECLARE num NUMBER := LENGTH(REPLACE(CAST(t1 AS text),'0',''));
BEGIN
    IF num > 64 then
        RETURN 64;
    ELSE
        RETURN num;
    END IF;
END;
$$
LANGUAGE plpgsql;
