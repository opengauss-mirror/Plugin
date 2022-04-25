\echo Use "CREATE EXTENSION b_sql_plugin" to load this file. \quit
CREATE FUNCTION pg_catalog.b_sql_plugin_invoke()
    RETURNS VOID AS '$libdir/b_sql_plugin','b_sql_plugin_invoke' LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION pg_catalog.curdate()
returns date
as
$$
begin
    return (select current_date);
end;
$$
language plpgsql; 

-- create or replace type cast
CREATE OR REPLACE FUNCTION pg_catalog.bool_text(a bool)
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
CREATE OR REPLACE FUNCTION pg_catalog.b_mod(a numeric, b numeric)
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

CREATE OR REPLACE FUNCTION pg_catalog.regexp(a text, b text)
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

CREATE OR REPLACE FUNCTION pg_catalog.regexp(a text, b boolean)
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

CREATE OR REPLACE FUNCTION pg_catalog.not_regexp(a text, b text)
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

CREATE OR REPLACE FUNCTION pg_catalog.not_regexp(a text, b boolean)
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

CREATE OR REPLACE FUNCTION pg_catalog.rlike(a text, b text)
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

CREATE OR REPLACE FUNCTION pg_catalog.rlike(a text, b boolean)
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

CREATE OR REPLACE FUNCTION pg_catalog.xor(a integer, b integer)
returns integer
as
$$
begin
  return (select a::bool # b::bool)::integer;
end;
$$
language plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.locate(t1 text, t2 text)
returns integer
as 
$$
begin
   return (select position(t2 in t1))::integer;
end;
$$
language plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.locate(t1 text, t2 boolean)
returns integer
as 
$$
begin
   return 0;
end;
$$
language plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.locate(t1 text, t2 text, t3 integer)
returns integer
as 
$$
begin
  if t3 < 0 then
    return 0;
  else 
    return (select instr(t1, t2, t3));
  end if;
end;
$$
language plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.lcase(t1 text)
returns text
as 
$$
begin
   return (select lower(t1))::text;
end;
$$
language plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.ucase(t1 text)
returns text
as 
$$
begin
   return (select upper(t1))::text;
end;
$$
language plpgsql;

CREATE OR REPLACE FUNCTION pg_catalog.rand()
returns double precision       
as 
$$
begin
   return (select random());
end;
$$
language plpgsql;

CREATE OR REPLACE FUNCTION PG_CATALOG.truncate(t1 numeric, t2 integer)
returns numeric
as 
$$
begin
  return (select trunc(t1, t2));
end;
$$
language plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.varchar_int1(varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.varchar_int2(varchar) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_int1(char) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.bpchar_int2(char) CASCADE;

CREATE FUNCTION pg_catalog.varchar_int1 (
varchar
) RETURNS tinyint LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'varchar_int1';

CREATE FUNCTION pg_catalog.varchar_int2 (
varchar
) RETURNS smallint LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'varchar_int2';

CREATE FUNCTION pg_catalog.bpchar_int1 (
char
) RETURNS tinyint LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'bpchar_int1';

CREATE FUNCTION pg_catalog.bpchar_int2 (
char
) RETURNS smallint LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'bpchar_int2';

CREATE CAST (varchar AS tinyint) WITH FUNCTION varchar_int1(varchar) AS IMPLICIT;
CREATE CAST (char AS tinyint) WITH FUNCTION bpchar_int1(char) AS IMPLICIT;
CREATE CAST (varchar AS smallint) WITH FUNCTION varchar_int2(varchar) AS IMPLICIT;
CREATE CAST (char AS smallint) WITH FUNCTION bpchar_int2(char) AS IMPLICIT;
