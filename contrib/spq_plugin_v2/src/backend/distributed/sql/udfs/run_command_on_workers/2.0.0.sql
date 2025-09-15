CREATE FUNCTION pg_catalog.run_command_on_workers(command text,
 parallel bool default true,
 OUT nodename text,
 OUT nodeport int,
 OUT success bool,
 OUT result text)
 RETURNS SETOF record
 LANGUAGE plpgsql
 AS $function$
DECLARE
 workers text[];
 ports int[];
 commands text[];
 res RECORD;
BEGIN
 WITH spq_workers AS (
 SELECT * FROM spq_get_active_worker_nodes() ORDER BY node_name, node_port)
 SELECT array_agg(node_name) AS node_name, array_agg(node_port) AS node_port, array_agg(command) AS command
 INTO res
 FROM spq_workers;
 workers :=res.node_name;
 ports :=res.node_port;
 commands :=res.command;
 RETURN QUERY SELECT * FROM master_run_on_worker(workers, ports, commands, parallel);
END;
$function$;
CREATE FUNCTION pg_catalog.run_command_on_placements(table_name regclass,
 command text,
 parallel bool default true,
 OUT nodename text,
 OUT nodeport int,
 OUT shardid bigint,
 OUT success bool,
 OUT result text)
 RETURNS SETOF record
 LANGUAGE plpgsql
 AS $function$
DECLARE
 workers text[];
 ports int[];
 shards bigint[];
 commands text[];
 res RECORD;
BEGIN
 WITH spq_placements AS (
 SELECT
 ds.logicalrelid::regclass AS tablename,
 ds.shardid AS shardid,
 shard_name(ds.logicalrelid, ds.shardid) AS shardname,
 dsp.nodename AS nodename, dsp.nodeport::int AS nodeport
 FROM pg_dist_shard ds JOIN pg_dist_shard_placement dsp USING (shardid)
 WHERE dsp.shardstate = 1 and ds.logicalrelid::regclass = table_name
 ORDER BY ds.logicalrelid, ds.shardid, dsp.nodename, dsp.nodeport)
 SELECT
 array_agg(cp.nodename) AS nodename, array_agg(cp.nodeport) AS nodeport, array_agg(cp.shardid) AS shardid,
 array_agg(format(command, cp.shardname)) AS command
 INTO res
 FROM spq_placements cp;
 
 workers :=res.nodename;
 ports :=res.nodeport;
 shards :=res.shardid;
 commands :=res.command;
 
 RETURN QUERY
 SELECT r.node_name, r.node_port, shards[row_num],
 r.success, r.result
 FROM (
 SELECT *, ROW_NUMBER() OVER () AS row_num
 FROM master_run_on_worker(workers, ports, commands, parallel) 
 ) r;
END;
$function$;
CREATE FUNCTION pg_catalog.run_command_on_colocated_placements(
 table_name1 regclass,
 table_name2 regclass,
 command text,
 parallel bool default true,
 OUT nodename text,
 OUT nodeport int,
 OUT shardid1 bigint,
 OUT shardid2 bigint,
 OUT success bool,
 OUT result text)
 RETURNS SETOF record
 LANGUAGE plpgsql
 AS $function$
DECLARE
 workers text[];
 ports int[];
 shards1 bigint[];
 shards2 bigint[];
 commands text[];
 res RECORD;
BEGIN
 IF NOT (SELECT distributed_tables_colocated(table_name1, table_name2)) THEN
 RAISE EXCEPTION 'tables % and % are not co-located', table_name1, table_name2;
 END IF;
 WITH active_shard_placements AS (
 SELECT
 ds.logicalrelid,
 ds.shardid AS shardid,
 shard_name(ds.logicalrelid, ds.shardid) AS shardname,
 ds.shardminvalue AS shardminvalue,
 ds.shardmaxvalue AS shardmaxvalue,
 dsp.nodename AS nodename,
 dsp.nodeport::int AS nodeport
 FROM pg_dist_shard ds JOIN pg_dist_shard_placement dsp USING (shardid)
 WHERE dsp.shardstate = 1 and (ds.logicalrelid::regclass = table_name1 or
 ds.logicalrelid::regclass = table_name2)
 ORDER BY ds.logicalrelid, ds.shardid, dsp.nodename, dsp.nodeport),
 spq_colocated_placements AS (
 SELECT
 a.logicalrelid::regclass AS tablename1,
 a.shardid AS shardid1,
 shard_name(a.logicalrelid, a.shardid) AS shardname1,
 b.logicalrelid::regclass AS tablename2,
 b.shardid AS shardid2,
 shard_name(b.logicalrelid, b.shardid) AS shardname2,
 a.nodename AS nodename,
 a.nodeport::int AS nodeport
 FROM
 active_shard_placements a, active_shard_placements b
 WHERE
 a.shardminvalue = b.shardminvalue AND
 a.shardmaxvalue = b.shardmaxvalue AND
 a.logicalrelid != b.logicalrelid AND
 a.nodename = b.nodename AND
 a.nodeport = b.nodeport AND
 a.logicalrelid::regclass = table_name1 AND
 b.logicalrelid::regclass = table_name2
 ORDER BY a.logicalrelid, a.shardid, nodename, nodeport)
 SELECT
 array_agg(cp.nodename) AS nodename, array_agg(cp.nodeport) AS nodeport, array_agg(cp.shardid1) AS shardid1,
 array_agg(cp.shardid2) AS shardid2 , array_agg(format(command, cp.shardname1, cp.shardname2)) AS command
 INTO res
 FROM spq_colocated_placements cp;
 workers := res.nodename;
 ports := res.nodeport;
 shards1 := res.shardid1;
 shards2 := res.shardid2;
 commands := res.command;
 RETURN QUERY SELECT r.node_name, r.node_port, shards1[row_num],
 shards2[row_num], r.success, r.result
 FROM(
 SELECT *, ROW_NUMBER() OVER () AS row_num
 FROM master_run_on_worker(workers, ports, commands, parallel) 
 )r;
END;
$function$;
CREATE FUNCTION pg_catalog.run_command_on_shards(table_name regclass,
 command text,
 parallel bool default true,
 OUT shardid bigint,
 OUT success bool,
 OUT result text)
 RETURNS SETOF record
 LANGUAGE plpgsql
 AS $function$
DECLARE
 workers text[];
 ports int[];
 shards bigint[];
 commands text[];
 shard_count int;
 res RECORD;
BEGIN
 SELECT COUNT(*) INTO shard_count FROM pg_dist_shard
 WHERE logicalrelid = table_name;
 WITH spq_shards AS (
 SELECT ds.logicalrelid::regclass AS tablename,
 ds.shardid AS shardid,
 shard_name(ds.logicalrelid, ds.shardid) AS shardname,
 array_agg(dsp.nodename) AS nodenames,
 array_agg(dsp.nodeport) AS nodeports
 FROM pg_dist_shard ds LEFT JOIN pg_dist_shard_placement dsp USING (shardid)
 WHERE dsp.shardstate = 1 and ds.logicalrelid::regclass = table_name
 GROUP BY ds.logicalrelid, ds.shardid
 ORDER BY ds.logicalrelid, ds.shardid)
 SELECT
 array_agg(cs.nodenames[1]) AS nodename, array_agg(cs.nodeports[1]) AS nodeport, array_agg(cs.shardid) AS shardid,
 array_agg(format(command, cs.shardname)) AS command
 INTO res
 FROM spq_shards cs;
 workers := res.nodename;
 ports := res.nodeport;
 shards := res.shardid;
 commands := res.command;
 IF (shard_count != array_length(workers, 1)) THEN
 RAISE NOTICE 'some shards do  not have active placements';
 END IF;
 RETURN QUERY
 SELECT shards[row_num], r.success, r.result
 FROM(
 SELECT *, ROW_NUMBER() OVER () AS row_num
 FROM master_run_on_worker(workers, ports, commands, parallel)
 ) r;
END;
$function$;