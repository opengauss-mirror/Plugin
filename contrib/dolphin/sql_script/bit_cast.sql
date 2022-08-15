DROP FUNCTION IF EXISTS pg_catalog.bit2float4(bit) CASCADE;

CREATE FUNCTION pg_catalog.bit2float4 (bit) RETURNS float4 AS
$$
BEGIN
    RETURN (SELECT int4($1));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.bit2float8(bit) CASCADE;

CREATE FUNCTION pg_catalog.bit2float8 (bit) RETURNS float8 AS
$$
BEGIN
    RETURN (SELECT int8($1));
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.bit2numeric(bit) CASCADE;

CREATE FUNCTION pg_catalog.bit2numeric (bit) RETURNS numeric AS
$$
BEGIN
    RETURN (SELECT int8($1));
END;
$$
LANGUAGE plpgsql;

CREATE CAST (bit AS float4) WITH FUNCTION bit2float4(bit) AS IMPLICIT;
CREATE CAST (bit AS float8) WITH FUNCTION bit2float8(bit) AS IMPLICIT;
CREATE CAST (bit AS numeric) WITH FUNCTION bit2numeric(bit) AS IMPLICIT;