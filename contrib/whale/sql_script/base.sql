\echo Use "CREATE EXTENSION whale" to load this file. \quit
CREATE FUNCTION pg_catalog.whale_invoke()
    RETURNS VOID AS '$libdir/whale','whale_invoke' LANGUAGE C STRICT;