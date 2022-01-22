-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION db_b_parser" to load this file. \quit
CREATE FUNCTION pg_catalog.db_b_parser_invoke()
RETURNS VOID AS '$libdir/db_b_parser','db_b_parser_invoke' LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.mysql_parser_bit_length( bit )
RETURNS INTEGER AS '$libdir/db_b_parser','mp_bit_length_bit'  LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.mysql_parser_bit_length( bytea )
RETURNS INTEGER AS '$libdir/db_b_parser','mp_bit_length_bytea' LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.mysql_parser_bit_length( text )
RETURNS INTEGER AS '$libdir/db_b_parser','mp_bit_length_text' LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.time_sec(text, timestamp)
RETURNS float AS '$libdir/db_b_parser','time_sec' LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.second(text, timestamp)
RETURNS float AS '$libdir/db_b_parser','time_sec' LANGUAGE C STRICT;

CREATE FUNCTION pg_catalog.MICROSECOND(a timestamp)
RETURNS float 
AS
$$ 
begin
    return (select time_sec('microsecond', a)); 
end;
$$
language plpgsql;

create function pg_catalog.curdate()
returns date
as
$$
begin
    return (select current_date);
end;
$$
language plpgsql; 

-- create or replace type cast
create function pg_catalog.bool_text(a bool)
returns text
as
$$
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
$$
language plpgsql;

-- create some functions for operator
create function pg_catalog.b_mod(a numeric, b numeric)
returns numeric
as
$$
begin
  IF b = 0 then
    return null;
  else
    return (select a % b);
  end if;
end;
$$
language plpgsql;

create function pg_catalog.regexp(a text, b text)
returns integer
as 
$$
begin
  if b = '' then
    raise 'invalid regular expression';
  else
    return (select lower(a) ~ lower(b))::integer;
  end if;
end;
$$
language plpgsql;

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

create function pg_catalog.not_regexp(a text, b text)
returns integer
as
$$
begin
  if b = '' then
    raise 'invalid regular expression';
  else
    return (select lower(a) !~ lower(b))::integer;
  end if;
end;
$$
language plpgsql;

create function pg_catalog.not_regexp(a text, b boolean)
returns integer
as 
$$
begin
  if b = true then
    return 0;
  elsif b = false then
    return 1;
  else
    raise 'invalid regular expression';
  end if;
end;
$$
language plpgsql;

create function pg_catalog.rlike(a text, b text)
returns integer
as
$$
begin
  if b = '' then
    raise 'invalid regular expression';
  else
    return (select lower(a) ~ lower(b))::integer;
  end if;
end;
$$
language plpgsql;

create function pg_catalog.rlike(a text, b boolean)
returns integer
as 
$$
begin
  if b = true then
    return 1;
  elsif b = false then
    return b::Integer;
  else
    raise 'invalid regular expression';
  end if;
end;
$$
language plpgsql;

create function pg_catalog.xor(a integer, b integer)
returns integer
as
$$
begin
  return (select a::bool # b::bool)::integer;
end;
$$
language plpgsql;

CREATE FUNCTION pg_catalog.locate(t1 text, t2 text)
returns integer
as 
$$
begin
   return (select position(t2 in t1))::integer;
end;
$$
language plpgsql;

CREATE FUNCTION pg_catalog.locate(t1 text, t2 boolean)
returns integer
as 
$$
begin
   return 0;
end;
$$
language plpgsql;