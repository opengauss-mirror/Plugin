\echo Use "CREATE EXTENSION db_b_parser" to load this file. \quit
CREATE FUNCTION pg_catalog.db_b_parser_invoke()
    RETURNS VOID AS '$libdir/db_b_parser','db_b_parser_invoke' LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.mysql_parser_bit_length( bit )
RETURNS INTEGER AS '$libdir/db_b_parser','mp_bit_length_bit'  LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.mysql_parser_bit_length( bytea )
RETURNS INTEGER AS '$libdir/db_b_parser','mp_bit_length_bytea' LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.mysql_parser_bit_length( text )
RETURNS INTEGER AS '$libdir/db_b_parser','mp_bit_length_text' LANGUAGE C STRICT;

create function pg_catalog.curdate()
return date
as
begin
    return (select current_date);
end;

-- create or replace type cast
create function pg_catalog.bool_numeric(a bool)
return numeric
as
begin
  IF a = true then 
    return 1;
  else
    return 0;
  end if;
end;
create function pg_catalog.numeric_bool(a numeric)
return bool
as
begin
  IF a is null then 
    return null;
  else 
    if a = 0 then
      return false;
    else
      return true;
    end if;
  end if;
end;

create cast(bool as numeric) with FUNCTION pg_catalog.bool_numeric(bool) AS IMPLICIT;
create cast(numeric as bool) with FUNCTION pg_catalog.numeric_bool(numeric) AS IMPLICIT;
create function pg_catalog.bool_text(a bool)
return text
as
begin
  IF a is null then 
    return null;
  else
    if a = true then
      return '1';
    else 
      return '0';
    end if;
  end if;
end;

-- update cast(bool as text)
--DO $$
--BEGIN
--  update pg_cast set castcontext='i', castfunc = (select oid from pg_proc where proname like 'bool_text' limit 1) where castsource = 16 and casttarget = 25;
--END$$;


-- create some functions for operator
create function pg_catalog.b_mod(a numeric, b numeric)
return numeric
as
begin
  IF b = 0 then
    return null;
  else
    return (select a % b);
  end if;
end;

create function pg_catalog.regexp(a text, b text)
return integer
as
begin
  if b = '' then
    raise 'invalid regular expression';
  else
    return (select lower(a) ~ lower(b))::integer;
  end if;
end;

create function pg_catalog.not_regexp(a text, b text)
return integer
as
begin
  if b = '' then
    raise 'invalid regular expression';
  else
    return (select lower(a) !~ lower(b))::integer;
  end if;
end;

create function pg_catalog.rlike(a text, b text)
return integer
as
begin
  if b = '' then
    raise 'invalid regular expression';
  else
    return (select lower(a) ~ lower(b))::integer;
  end if;
end;

create function pg_catalog.xor(a integer, b integer)
return integer
as
begin
  return (select a::bool # b::bool)::integer;
end;
