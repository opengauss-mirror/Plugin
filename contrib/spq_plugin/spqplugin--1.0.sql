-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION spqplugin" to load this file. \quit

CREATE FUNCTION pg_catalog.spqplugin_invoke()
    RETURNS VOID AS '$libdir/spqplugin','spqplugin_invoke' LANGUAGE C STRICT;
