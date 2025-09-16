CREATE TYPE pg_catalog.noderole AS ENUM (
  'primary', -- node is available and accepting writes
  'secondary', -- node is available but only accepts reads
  'unavailable' -- node is in recovery or otherwise not usable
-- adding new values to a type inside of a transaction (such as during an ALTER EXTENSION
-- citus UPDATE) isn't allowed in PG 9.6, and only allowed in PG10 if you don't use the
-- new values inside of the same transaction. You might need to replace this type with a
-- new one and then change the column type in pg_dist_node. There's a list of
-- alternatives here:
-- https:
);
