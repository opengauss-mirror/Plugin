\echo Use "CREATE EXTENSION custom_analyzer_demo" to load this file. \quit

CREATE FUNCTION pg_catalog.custom_analyzer_demo()
    RETURNS VOID AS '$libdir/custom_analyzer_demo','custom_analyzer_demo_invoke' LANGUAGE C STRICT;