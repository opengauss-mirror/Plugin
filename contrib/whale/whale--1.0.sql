\echo Use "CREATE EXTENSION whale" to load this file. \quit
CREATE FUNCTION pg_catalog.whale_invoke()
    RETURNS VOID AS '$libdir/whale','whale_invoke' LANGUAGE C STRICT;

create function pg_catalog.regexp(a text, b boolean)
returns integer
as 
$$
begin
  if b = true then
    return 1;
  elsif b = false then
    return 0;
  else
    raise 'invalid regular expression';
  end if;
end;
$$
language plpgsql;