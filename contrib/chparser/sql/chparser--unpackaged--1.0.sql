-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION chparser" to load this file. \quit

ALTER EXTENSION chparser ADD function chprs_start(internal,integer);
ALTER EXTENSION chparser ADD function chprs_getlexeme(internal,internal,internal);
ALTER EXTENSION chparser ADD function chprs_end(internal);
ALTER EXTENSION chparser ADD function chprs_lextype(internal);
ALTER EXTENSION chparser ADD text search parser chparser;
