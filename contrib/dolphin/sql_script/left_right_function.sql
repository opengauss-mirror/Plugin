DROP FUNCTION IF EXISTS pg_catalog.left(bytea, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.left(bytea, int) RETURNS bytea LANGUAGE INTERNAL as 'text_left';

DROP FUNCTION IF EXISTS pg_catalog.right(bytea, int) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.right(bytea, int) RETURNS bytea LANGUAGE INTERNAL as 'text_right';