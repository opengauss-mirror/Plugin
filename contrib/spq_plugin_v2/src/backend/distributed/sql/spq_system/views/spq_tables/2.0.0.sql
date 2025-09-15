DO $$
declare
spq_tables_create_query text;
BEGIN
spq_tables_create_query=$CTCQ$
    CREATE OR REPLACE VIEW %I.spq_tables AS
    SELECT
        logicalrelid AS table_name,
        CASE WHEN partkey IS NOT NULL THEN 'distributed'
            WHEN repmodel = 't' THEN 'reference'
            WHEN colocationid = 0 THEN 'local'
            ELSE 'distributed'
        END AS table_type,
        coalesce(column_to_column_name(logicalrelid, partkey), '<none>') AS distribution_column,
        pg_size_pretty(table_sizes.table_size) AS table_size,
        (select count(*) from pg_dist_shard where logicalrelid = p.logicalrelid) AS shard_count,
        pg_get_userbyid(relowner) AS table_owner,
        amname AS access_method
    FROM
        pg_dist_partition p
    JOIN
        pg_class c ON (p.logicalrelid = c.oid)
    LEFT JOIN
        pg_am a ON (a.oid = c.relam)
    JOIN
        (
            SELECT ds.logicalrelid AS table_id, SUM(sss.size) AS table_size
            FROM spq_shard_sizes() sss, pg_dist_shard ds
            WHERE sss.shard_id = ds.shardid
            GROUP BY ds.logicalrelid
        ) table_sizes ON (table_sizes.table_id = p.logicalrelid)
    WHERE
        -- filter out tables owned by extensions
        logicalrelid NOT IN (
            SELECT
                objid
            FROM
                pg_depend
            WHERE
                classid = 'pg_class'::regclass AND refclassid = 'pg_extension'::regclass AND deptype = 'e'
        )
    ORDER BY
        logicalrelid::text;
$CTCQ$;

IF EXISTS (SELECT 1 FROM pg_namespace WHERE nspname = 'public') THEN
    EXECUTE format(spq_tables_create_query, 'public');
    GRANT SELECT ON public.spq_tables TO public;
ELSE
    EXECUTE format(spq_tables_create_query, '__$spq$__');
    ALTER VIEW __$spq$__.spq_tables SET SCHEMA pg_catalog;
    GRANT SELECT ON pg_catalog.spq_tables TO public;
END IF;

END;
$$;
