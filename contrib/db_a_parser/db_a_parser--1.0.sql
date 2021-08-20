\echo Use "CREATE EXTENSION db_a_parser" to load this file. \quit
CREATE FUNCTION pg_catalog.db_a_parser_invoke()
    RETURNS VOID AS '$libdir/db_a_parser','db_a_parser_invoke' LANGUAGE C STRICT;