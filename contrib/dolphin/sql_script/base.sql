\echo Use "CREATE EXTENSION dolphin" to load this file. \quit
CREATE FUNCTION pg_catalog.dolphin_invoke()
    RETURNS VOID AS '$libdir/dolphin','dolphin_invoke' LANGUAGE C STRICT;

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
  return (select a::bool::integer # b::bool::integer);
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
) RETURNS tinyint LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_int1';

CREATE FUNCTION pg_catalog.varchar_int2 (
varchar
) RETURNS smallint LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'varchar_int2';

CREATE FUNCTION pg_catalog.bpchar_int1 (
char
) RETURNS tinyint LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bpchar_int1';

CREATE FUNCTION pg_catalog.bpchar_int2 (
char
) RETURNS smallint LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'bpchar_int2';

CREATE CAST (varchar AS tinyint) WITH FUNCTION varchar_int1(varchar) AS IMPLICIT;
CREATE CAST (char AS tinyint) WITH FUNCTION bpchar_int1(char) AS IMPLICIT;
CREATE CAST (varchar AS smallint) WITH FUNCTION varchar_int2(varchar) AS IMPLICIT;
CREATE CAST (char AS smallint) WITH FUNCTION bpchar_int2(char) AS IMPLICIT;

create or replace function pg_catalog.get_index_columns(OUT namespace name, OUT indexrelid oid, OUT indrelid oid, OUT indisunique bool, OUT indisusable bool, OUT seq_in_index int2, OUT attrnum int2, OUT collation int2) returns setof record
as $$
declare
query_str text;
item int2;
row_data record;
begin
query_str := 'select n.nspname, i.indexrelid, i.indrelid, i.indisunique, i.indisusable, i.indkey, i.indoption
              from pg_catalog.pg_index i
                left join pg_class c on c.oid = i.indexrelid
                left join pg_catalog.pg_namespace n on n.oid = c.relnamespace
              where n.nspname <> ''pg_catalog''
                and n.nspname <> ''db4ai''
                and n.nspname <> ''information_schema''
                and n.nspname !~ ''^pg_toast''';
for row_data in EXECUTE(query_str) LOOP
    for item in 0..array_length(row_data.indkey, 1) - 1 loop
        namespace := row_data.nspname;
        indexrelid := row_data.indexrelid;
        indrelid := row_data.indrelid;
        indisunique := row_data.indisunique;
        indisusable := row_data.indisusable;
        seq_in_index := item + 1;
        attrnum := row_data.indkey[item];
        collation := row_data.indoption[item];
        return next;
    end loop;
end loop;
end; $$
LANGUAGE 'plpgsql';

create view public.index_statistic as
select
  i.namespace as "namespace",
  (select relname from pg_class tc where tc.oid = i.indrelid) as "table",
  not i.indisunique as "non_unique",
  c.relname as "key_name",
  i.seq_in_index as "seq_in_index",
  a.attname as "column_name",
  (case when m.amcanorder
    then (
      case when i.collation & 1 then 'D' else 'A' END
    ) else null end
  ) as "collation",
  (select
      (case when ts.stadistinct = 0
        then NULL else (
          case when ts.stadistinct > 0 then ts.stadistinct else ts.stadistinct * tc.reltuples * -1 end
        ) end
      ) 
    from pg_class tc
      left join pg_statistic ts on tc.oid = ts.starelid
    where
      tc.oid = i.indrelid
      and ts.staattnum = i.attrnum
  ) as "cardinality",
  null as "sub_part",
  null as "packed",
  (case when a.attnotnull then '' else 'YES' end) as "null",
  m.amname as "index_type",
  (case when i.indisusable then '' else 'disabled' end) as "comment",
  (select description from pg_description where objoid = i.indexrelid) as "index_comment"
from
  (select * from get_index_columns()) i
  left join pg_class c on c.oid = i.indexrelid
  left join pg_attribute a on a.attrelid = i.indrelid
  and a.attnum = i.attrnum
  left join pg_am m on m.oid = c.relam
order by
  c.relname;

REVOKE ALL ON public.index_statistic FROM PUBLIC;
GRANT SELECT, REFERENCES ON public.index_statistic TO PUBLIC;

DROP FUNCTION IF EXISTS pg_catalog.rebuild_partition(text[]) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.remove_partitioning(text) CASCADE;

CREATE FUNCTION pg_catalog.remove_partitioning (
text
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'RemovePartitioning';

CREATE FUNCTION pg_catalog.rebuild_partition(text[])
RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'RebuildPartition';

CREATE FUNCTION pg_catalog.analyze_partition(text[],text,text)
RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'AnalyzePartitions';

CREATE OR REPLACE FUNCTION pg_catalog.analyze_tables (
IN tableName cstring[], OUT "Table" text, OUT "Op" text, OUT "Msg_type" text, OUT "Msg_text" text
) RETURNS setof record LANGUAGE C VOLATILE STRICT as '$libdir/dolphin',  'analyze_tables';
