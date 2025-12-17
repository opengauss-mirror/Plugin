DROP CAST IF EXISTS (varchar AS bytea);
DROP FUNCTION IF EXISTS pg_catalog.varchar_bytea(varchar);

DROP VIEW IF EXISTS performance_schema.threads;

do $$
begin
update pg_proc left join pg_depend on oid = objid set proconfig = NULL where refobjid = (select oid from pg_extension where extname = 'dolphin') and classid = 1255;
end
$$