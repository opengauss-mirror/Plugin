-- log10 float8
DROP FUNCTION IF EXISTS pg_catalog.log10(float8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.log10(float8)
RETURNS float8
AS
$$
BEGIN
    IF $1 <= 0 THEN
        RETURN NULL;
    end if;
    RETURN (SELECT dlog10($1));
END;
$$
LANGUAGE plpgsql;

-- log2 float8
DROP FUNCTION IF EXISTS pg_catalog.log2(float8) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.log2 (
float8
) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'dlog2';

-- log10 numeric
DROP FUNCTION IF EXISTS pg_catalog.log10(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.log10(numeric)
RETURNS numeric
AS
$$
BEGIN
    IF $1 <= 0 THEN
        RETURN NULL;
    end if;
    RETURN (SELECT log(10, $1));
END;
$$
LANGUAGE plpgsql;

-- log2 numeric
DROP FUNCTION IF EXISTS pg_catalog.log2(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.log2(numeric)
RETURNS numeric
AS
$$
BEGIN
    IF $1 <= 0 THEN
        RETURN NULL;
    end if;
    RETURN (SELECT log(2, $1));
END;
$$
LANGUAGE plpgsql;
