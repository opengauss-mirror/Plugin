\echo Use "CREATE EXTENSION db_b_parser" to load this file. \quit
CREATE FUNCTION pg_catalog.db_b_parser_invoke()
    RETURNS VOID AS '$libdir/db_b_parser','db_b_parser_invoke' LANGUAGE C STRICT;
