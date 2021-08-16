-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION tidrangescan" to load this file. \quit

CREATE FUNCTION pg_catalog.tidrangescan_invoke()
    RETURNS VOID AS '$libdir/tidrangescan','tidrangescan_invoke' LANGUAGE C STRICT;