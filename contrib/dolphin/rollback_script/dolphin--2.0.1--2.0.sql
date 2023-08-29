drop schema if exists performance_schema CASCADE;
create schema performance_schema;

CREATE OR REPLACE FUNCTION pg_catalog.get_statement_history(
OUT schema_name name,OUT unique_query_id bigint,OUT debug_query_id bigint,OUT query text,
OUT start_time timestamp with time zone, OUT finish_time timestamp with time zone,OUT thread_id bigint,
OUT session_id bigint, OUT n_returned_rows bigint, OUT n_tuples_inserted bigint,OUT n_tuples_updated bigint,
OUT n_tuples_deleted bigint,OUT cpu_time bigint,OUT lock_time bigint)
RETURNS setof record
AS $$
DECLARE
  row_data record;
  port text;
  query_str text;
  query_str_nodes text;
  user_name text;
  dblink_ext int;
  BEGIN
    --check privileges only sysadm or omsysadm can access the dbe_perf.statement table;
    SELECT SESSION_USER INTO user_name;
    IF pg_catalog.has_schema_privilege(user_name, 'dbe_perf', 'USAGE') = false 
    THEN 
       RAISE EXCEPTION 'user % has no privilege to use schema dbe_perf',user_name;
    END IF;

    IF pg_catalog.has_table_privilege(user_name, 'dbe_perf.statement_history', 'SELECT') = false 
    THEN 
       RAISE EXCEPTION 'user % has no privilege to access dbe_perf.statement_history',user_name;
    END IF;

    --check if dblink exists, lite mode does not have dblink
    EXECUTE('SELECT count(*) from pg_extension where extname=''dblink''') into dblink_ext;
    IF dblink_ext = 0
    THEN
        RAISE EXCEPTION 'dblink is needed by this function, please load dblink extension first';
    END IF;

    --get the port number 
    EXECUTE('SELECT setting FROM pg_settings WHERE name = ''port''') into port;
    
    query_str := concat(concat('select * from public.dblink(''dbname=postgres port=',port),''',''select schema_name,unique_query_id,debug_query_id,query ,start_time,finish_time, thread_id ,session_id ,n_returned_rows ,n_tuples_inserted ,n_tuples_updated ,n_tuples_deleted ,cpu_time , lock_time from dbe_perf.statement_history'') as t(schema_name name,unique_query_id bigint,debug_query_id bigint,query text,start_time timestamp with time zone, finish_time timestamp with time zone,thread_id bigint,session_id bigint, n_returned_rows bigint, n_tuples_inserted bigint, n_tuples_updated bigint,n_tuples_deleted bigint,cpu_time bigint,lock_time bigint)');
    FOR row_data IN EXECUTE(query_str) LOOP
        schema_name :=row_data.schema_name;
    unique_query_id := row_data.unique_query_id;
    debug_query_id := row_data.debug_query_id;
    query := row_data.query;
    start_time := row_data.start_time;
    finish_time := row_data.finish_time;
    thread_id := row_data.thread_id;
    session_id := row_data.session_id;
    n_returned_rows := row_data.n_returned_rows;
    n_tuples_inserted := row_data.n_tuples_inserted;
    n_tuples_updated := row_data.n_tuples_updated;
    n_tuples_deleted := row_data.n_tuples_deleted;
    cpu_time := row_data.cpu_time;
    lock_time:= row_data.lock_time;
    return next;
    END LOOP;
    return;
  END; $$
LANGUAGE 'plpgsql'
SECURITY DEFINER;

CREATE VIEW performance_schema.statement_history AS 
select * from pg_catalog.get_statement_history();

create view performance_schema.events_statements_current as 
select
  s.pid as THREAD_ID,
  NULL as EVENT_ID,  
  NULL as END_EVENT_ID,  
  NULL AS EVENT_NAME,   
  s.application_name AS SOURCE,  
  s.query_start AS TIMER_START,
  CASE
    WHEN s.state = 'active' THEN CURRENT_TIMESTAMP
    ELSE s.state_change
  END AS TIMER_END,
  CASE
    WHEN s.state = 'active' THEN CURRENT_TIMESTAMP - s.query_start
    ELSE s.state_change - s.query_start
  END AS TIMER_WAIT,
  NULL AS LOCK_TIME,   
  s.QUERY AS SQL_TEXT,
  SHA2(t.QUERY,256) AS DIGEST, 
  t.QUERY AS DIGEST_TEXT,
  h.schema_name AS SCHEMA_NAME, 
  NULL AS OBJECT_TYPE, 
  NULL AS OBJECT_SCHEMA, 
  NULL AS OBJECT_NAME, 
  NULL AS OBJECT_INSTANCE_BEGIN, 
  NULL AS MYSQL_ERRNO, 
  NULL AS RETURNED_SQLSTATE, 
  NULL AS MESSAGE_TEXT, 
  NULL AS ERRORS,      
  NULL AS WARNING_COUNT,
  (h.n_tuples_deleted+h.n_tuples_inserted+h.n_tuples_inserted)  AS ROW_AFFECTED,
  h.n_returned_rows AS ROW_SENT,
  NULL AS ROW_EXAMINED, 
  NULL AS CREATED_TMP_DISK_TABLES, 
  NULL AS CREATED_TMP_TABLES, 
  NULL AS SELECT_FULL_JOIN, 
  NULL AS SELECT_FULL_RANGE_JOIN, 
  NULL AS SELECT_RANGE, 
  NULL AS SELECT_RANGE_CHECK, 
  NULL AS SELECT_SCAN, 
  NULL AS SORT_MERGE_PASSES, 
  NULL AS SORT_RANGE, 
  NULL AS SORT_ROWS, 
  NULL AS SORT_SCAN, 
  NULL AS NO_INDEX_USED, 
  NULL AS NO_GOOD_INDEX_USED, 
  NULL AS NESTING_EVENT_ID, 
  NULL AS NESTING_EVENT_TYPE, 
  NULL AS NESTING_EVENT_LEVEL, 
  h.debug_query_id AS STATEMENT_ID,
  h.CPU_TIME AS CPU_TIME,    
  NULL AS MAX_CONTROLLED_MEMORY, 
  NULL AS MAX_TOTAL_MEMORY, 
  NULL AS EXECUTION_ENGINE 
  FROM pg_catalog.pg_stat_activity s
  left outer join performance_schema.statement_history h
  on s.query_id = h.debug_query_id
  and h.session_id = s.sessionid
  left outer join dbe_perf.statement t 
  on s.unique_sql_id = t.unique_sql_id
  where s.query_start IS NOT NULL
;
/* ----------------------
 *    Create performance_schema.events_statements_history
 * ----------------------
 */
create view performance_schema.events_statements_history as 
select
  h.thread_id as THREAD_ID,
  NULL as EVENT_ID,  
  NULL as END_EVENT_ID,  
  NULL AS EVENT_NAME,   
  s.application_name AS SOURCE,  
  h.start_time AS TIMER_START,
  h.finish_time AS TIMER_END,
  h.finish_time - h.start_time AS TIMER_WAIT,
  h.lock_time AS LOCK_TIME,   
  h.QUERY AS SQL_TEXT,
  SHA2(t.QUERY,256) AS DIGEST,
  t.QUERY AS DIGEST_TEXT,
  h.schema_name AS SCHEMA_NAME, 
  NULL AS OBJECT_TYPE, 
  NULL AS OBJECT_SCHEMA, 
  NULL AS OBJECT_NAME, 
  NULL AS OBJECT_INSTANCE_BEGIN, 
  NULL AS MYSQL_ERRNO, 
  NULL AS RETURNED_SQLSTATE, 
  NULL AS MESSAGE_TEXT, 
  0    AS ERRORS, 
  NULL AS WARNING_COUNT,
  (h.n_tuples_deleted+h.n_tuples_inserted+h.n_tuples_inserted)  AS ROW_AFFECTED,
  h.n_returned_rows AS ROW_SENT,
  NULL AS ROW_EXAMINED, 
  NULL AS CREATED_TMP_DISK_TABLES, 
  NULL AS CREATED_TMP_TABLES, 
  NULL AS SELECT_FULL_JOIN, 
  NULL AS SELECT_FULL_RANGE_JOIN, 
  NULL AS SELECT_RANGE, 
  NULL AS SELECT_RANGE_CHECK, 
  NULL AS SELECT_SCAN, 
  NULL AS SORT_MERGE_PASSES, 
  NULL AS SORT_RANGE, 
  NULL AS SORT_ROWS, 
  NULL AS SORT_SCAN, 
  NULL AS NO_INDEX_USED, 
  NULL AS NO_GOOD_INDEX_USED, 
  NULL AS NESTING_EVENT_ID, 
  NULL AS NESTING_EVENT_TYPE, 
  NULL AS NESTING_EVENT_LEVEL, 
  h.debug_query_id AS STATEMENT_ID, 
  h.CPU_TIME AS CPU_TIME,    
  NULL AS MAX_CONTROLLED_MEMORY, 
  NULL AS MAX_TOTAL_MEMORY, 
  NULL AS EXECUTION_ENGINE 
  FROM performance_schema.statement_history h 
  left outer join dbe_perf.statement t
  on h.unique_query_id = t.unique_sql_id
  inner join pg_catalog.pg_stat_activity s
  on h.session_id = s.sessionid
;
/* ----------------------
 *    Create performance_schema.events_statements_summary_by_digest
 * ----------------------
 */
create view performance_schema.events_statements_summary_by_digest as 
SELECT 
  h.schema_name AS SCHEMA_NAME, 
  SHA2(t.QUERY,256) AS DIGEST,
  t.QUERY AS DIGEST_TEXT,
  t.N_CALLS AS COUNT_STAR,
  t.TOTAL_ELAPSE_TIME AS SUM_TIMER_WAIT,
  t.MIN_ELAPSE_TIME AS MIN_TIMER_WAIT,
  CASE
    WHEN t.N_CALLS THEN round(t.TOTAL_ELAPSE_TIME/t.N_CALLS)
    ELSE 0
  END AS AVG_TIMER_WAIT,
  t.MAX_ELAPSE_TIME AS MAX_TIMER_WAIT,
  NULL AS SUM_LOCK_TIME,  
  NULL AS SUM_ERRORS,  
  NULL AS SUM_WARNINGS, 
  (t.n_tuples_deleted+t.n_tuples_inserted+t.n_tuples_inserted) AS SUM_ROWS_AFFECTED,
  t.N_RETURNED_ROWS AS SUM_ROWS_SENT,
  NULL AS SUM_ROWS_EXAMINED,
  NULL AS SUM_CREATED_TMP_DISK_TABLES,
  NULL AS SUM_CREATED_TMP_TABLES,
  NULL AS SUM_SELECT_FULL_JOIN,
  NULL AS SUM_SELECT_FULL_RANGE_JOIN,
  NULL AS SUM_SELECT_RANGE,
  NULL AS SUM_SELECT_RANGE_CHECK,
  NULL AS SUM_SELECT_SCAN,
  NULL AS SUM_SORT_MERGE_PASSES,
  NULL AS SUM_SORT_RANGE,
  NULL AS SUM_SORT_ROWS,
  t.sort_count AS SUM_SORT_SCAN,
  NULL AS SUM_NO_INDEX_USED,
  NULL AS SUM_NO_GOOD_INDEX_USED,
  t.CPU_TIME AS SUM_CPU_TIME,
  NULL AS MAX_CONTROLLED_MEMORY,
  NULL AS MAX_TOTAL_MEMORY,
  NULL AS COUNT_SECONDARY,
  NULL AS FIRST_SEEN,
  t.last_updated AS LAST_SEEN,
  NULL AS QUANTILE_95,
  NULL AS QUANTILE_99,
  NULL AS QUANTILE_999,
  NULL AS QUERY_SAMPLE_TEXT,
  NULL AS QUERY_SAMPLE_SEEN,
  NULL AS QUERY_SAMPLE_TIMER_WAIT
FROM dbe_perf.statement t
left outer join performance_schema.statement_history h
on h.unique_query_id = t.unique_sql_id
;
  
  
create view performance_schema.events_waits_current as 
SELECT 
  t.tid AS THREAD_ID,
  NULL AS EVENT_ID,
  NULL AS END_EVENT_ID,
  concat(concat(concat(concat(t.thread_name,'/'),t.WAIT_STATUS),'/'),t.WAIT_EVENT) AS EVENT_NAME,
  s.application_name AS SOURCE,
  s.query_start AS TIMER_START,
  CASE
    WHEN s.state = 'active' THEN CURRENT_TIMESTAMP
    ELSE s.state_change
  END AS TIMER_END,
  CASE
    WHEN s.state = 'active' THEN CURRENT_TIMESTAMP - s.query_start
    ELSE s.state_change - s.query_start
  END AS TIMER_WAIT,
  NULL AS SPINS,
  n.nspname AS OBJECT_SCHEMA,
  CASE
    WHEN c.reltype !=0 THEN c.relname
    ELSE NULL 
  END AS OBJECT_NAME,
  CASE
    WHEN c.reltype = 0 THEN c.relname
    ELSE NULL
  END AS INDEX_NAME,
  l.locktype AS OBJECT_TYPE,
  NULL AS OBJECT_INSTANCE_BEGIN,
  NULL AS NESTING_EVENT_ID,
  NULL AS NESTING_EVENT_TYPE,
  NULL AS OPERATION,
  NULL AS NUMBER_OF_BYTES,
  NULL AS FLAGS
FROM pg_catalog.pg_thread_wait_status t
inner join pg_catalog.pg_stat_activity s
  on t.sessionid = s.sessionid
  LEFT OUTER join pg_catalog.pg_locks l
  on t.locktag = l.locktag and l.pid = s.pid
  LEFT OUTER join pg_catalog.pg_class c
  on l.relation = c.oid 
  LEFT OUTER join pg_catalog.pg_namespace n
  on c.relnamespace = n.oid;
      

  
CREATE VIEW performance_schema.events_waits_summary_global_by_event_name AS
SELECT 
  concat(concat(w.type,''),w.event) AS EVENT_NAME,
  w.wait AS COUNT_STAR,
  w.total_wait_time as SUM_TIMER_WAIT,
  w.min_wait_time as MIN_TIMER_WAIT,
  w.avg_wait_time as AVG_TIMER_WAIT,
  w.max_wait_time as MAX_TIMER_WAIT
FROM dbe_perf.wait_events as w;


CREATE VIEW performance_schema.file_summary_by_instance AS
SELECT 
f.filenum AS FILE_NAME,
NULL AS EVENT_NAME,
NULL AS OBJECT_INSTANCE_BEGIN,
NULL AS COUNT_STAR,
NULL AS SUM_TIMER_WAIT,
NULL AS MIN_TIMER_WAIT,
NULL AS AVG_TIMER_WAIT,
NULL AS MAX_TIMER_WAIT,
f.phyrds AS COUNT_READ,
f.readtim AS SUM_TIMER_READ,
f.miniotim AS MIN_TIMER_READ, 
f.avgiotim AS AVG_TIMER_READ,
f.maxiowtm AS MAX_TIMER_READ,
f.phyblkrd*8192 AS SUM_NUMBER_OF_BYTES_READ, 
f.phywrts AS COUNT_WRITE,
f.writetim AS SUM_TIMER_WRITE,
f.miniotim AS MIN_TIMER_WRITE, 
f.avgiotim AVG_TIMER_WRITE,
f.maxiowtm AS MAX_TIMER_WRITE,
f.phyblkwrt*8192 AS SUM_NUMBER_OF_BYTES_WRITE,
NULL AS COUNT_MISC,
NULL AS SUM_TIMER_MISC,
NULL AS MIN_TIMER_MISC,
NULL AS AVG_TIMER_MISC,
NULL AS MAX_TIMER_MISC
FROM dbe_perf.file_iostat f;

CREATE VIEW performance_schema.table_io_waits_summary_by_table AS
SELECT
'TABLE' AS OBJECT_TYPE,  
n.nspname AS OBJECT_SCHEMA,
c.relname AS OBJECT_NAME,
NULL AS       COUNT_STAR,
NULL AS   SUM_TIMER_WAIT,
NULL AS   MIN_TIMER_WAIT,
NULL AS   AVG_TIMER_WAIT,
NULL AS   MAX_TIMER_WAIT,
SUM(f.phyrds) AS       COUNT_READ,
SUM(f.readtim) AS   SUM_TIMER_READ,
MIN(f.miniotim) AS   MIN_TIMER_READ,
AVG(f.avgiotim) AS   AVG_TIMER_READ,
MAX(f.maxiowtm) AS   MAX_TIMER_READ,
SUM(f.phywrts) AS      COUNT_WRITE,
SUM(f.writetim) AS  SUM_TIMER_WRITE,
MIN(f.miniotim) AS  MIN_TIMER_WRITE,
AVG(f.avgiotim) AS  AVG_TIMER_WRITE,
MAX(f.maxiowtm) AS  MAX_TIMER_WRITE,
NULL AS      COUNT_FETCH,
NULL AS  SUM_TIMER_FETCH,
NULL AS  MIN_TIMER_FETCH,
NULL AS  AVG_TIMER_FETCH,
NULL AS  MAX_TIMER_FETCH,
NULL AS     COUNT_INSERT,
NULL AS SUM_TIMER_INSERT,
NULL AS MIN_TIMER_INSERT,
NULL AS AVG_TIMER_INSERT,
NULL AS MAX_TIMER_INSERT,
NULL AS     COUNT_UPDATE,
NULL AS SUM_TIMER_UPDATE,
NULL AS MIN_TIMER_UPDATE,
NULL AS AVG_TIMER_UPDATE,
NULL AS MAX_TIMER_UPDATE,
NULL AS     COUNT_DELETE,
NULL AS SUM_TIMER_DELETE,
NULL AS MIN_TIMER_DELETE,
NULL AS AVG_TIMER_DELETE,
NULL AS MAX_TIMER_DELETE
FROM dbe_perf.file_iostat f, pg_class c, pg_namespace n
WHERE f.filenum = c.relfilenode
and c.relnamespace = n.oid
and c.relkind = 'r'
GROUP BY OBJECT_SCHEMA,OBJECT_NAME;

CREATE VIEW performance_schema.table_io_waits_summary_by_index_usage AS
SELECT
'TABLE' AS OBJECT_TYPE,  
n.nspname AS OBJECT_SCHEMA,
t.relname AS OBJECT_NAME,
c.relname AS INDEX_NAME,
NULL AS       COUNT_STAR,
NULL AS   SUM_TIMER_WAIT,
NULL AS   MIN_TIMER_WAIT,
NULL AS   AVG_TIMER_WAIT,
NULL AS   MAX_TIMER_WAIT,
SUM(f.phyrds) AS       COUNT_READ,
SUM(f.readtim) AS   SUM_TIMER_READ,
MIN(f.miniotim) AS   MIN_TIMER_READ,
AVG(f.avgiotim) AS   AVG_TIMER_READ,
MAX(f.maxiowtm) AS   MAX_TIMER_READ,
SUM(f.phywrts) AS      COUNT_WRITE,
SUM(f.writetim) AS  SUM_TIMER_WRITE,
MIN(f.miniotim) AS  MIN_TIMER_WRITE,
AVG(f.avgiotim) AS  AVG_TIMER_WRITE,
MAX(f.maxiowtm) AS  MAX_TIMER_WRITE,
NULL AS      COUNT_FETCH,
NULL AS  SUM_TIMER_FETCH,
NULL AS  MIN_TIMER_FETCH,
NULL AS  AVG_TIMER_FETCH,
NULL AS  MAX_TIMER_FETCH,
NULL AS     COUNT_INSERT,
NULL AS SUM_TIMER_INSERT,
NULL AS MIN_TIMER_INSERT,
NULL AS AVG_TIMER_INSERT,
NULL AS MAX_TIMER_INSERT,
NULL AS     COUNT_UPDATE,
NULL AS SUM_TIMER_UPDATE,
NULL AS MIN_TIMER_UPDATE,
NULL AS AVG_TIMER_UPDATE,
NULL AS MAX_TIMER_UPDATE,
NULL AS     COUNT_DELETE,
NULL AS SUM_TIMER_DELETE,
NULL AS MIN_TIMER_DELETE,
NULL AS AVG_TIMER_DELETE,
NULL AS MAX_TIMER_DELETE
FROM dbe_perf.file_iostat f, pg_class c, pg_namespace n,pg_index i, pg_class t
WHERE f.filenum = c.relfilenode
and c.relnamespace = n.oid
and c.relkind in ( 'i','I') 
and c.oid = i.indexrelid
and t.oid = i.indrelid
GROUP BY OBJECT_SCHEMA,OBJECT_NAME,INDEX_NAME;

GRANT USAGE ON schema performance_schema TO PUBLIC; 

GRANT SELECT,REFERENCES on all tables in schema performance_schema to public;