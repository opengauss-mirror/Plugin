DROP FUNCTION IF EXISTS pg_catalog.quote(text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.quote(t text)
returns text
as 
$$
begin
  return (select quote_literal(t));
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.quote(anyelement) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.quote(t anyelement)
returns text
as 
$$
begin
  return (select quote_literal(t));
end;
$$
language plpgsql;
