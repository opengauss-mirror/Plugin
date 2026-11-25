---
description: PERFORMANCE_SCHEMA.STATEMENT_HISTORY is created only in B-compatible databases. It exposes 14 columns from dbe_perf.statement_history in the postgres database on the local node through pg_catalog.get_.
keywords: [openGauss, PERFORMANCE_SCHEMA.STATEMENT_HISTORY]
---

# PERFORMANCE_SCHEMA.STATEMENT_HISTORY

PERFORMANCE_SCHEMA.STATEMENT_HISTORY is created only in B-compatible databases. It exposes 14 columns from dbe_perf.statement_history in the postgres database on the local node through pg_catalog.get_statement_history().

- The underlying function checks that the session user (SESSION_USER) has USAGE on schema dbe_perf and SELECT on dbe_perf.statement_history.
- The current database must have the dblink extension installed, with its functions in schema public. A connection to the local postgres database must also be available.
- The available records depend on the collection and retention settings of the underlying statement_history, including track_stmt_stat_level and log_min_duration_statement.

**Table 1** STATEMENT_HISTORY fields

| Name | Type | Description |
| --- | --- | --- |
| schema_name | name | Schema name. |
| unique_query_id | bigint | Identifier of the normalized SQL statement. |
| debug_query_id | bigint | Debug query identifier of the statement. |
| query | text | Recorded SQL text. |
| start_time | timestamp with time zone | Statement start time. |
| finish_time | timestamp with time zone | Statement end time. |
| thread_id | bigint | Identifier of the executing thread. |
| session_id | bigint | Session identifier. |
| n_returned_rows | bigint | Number of rows returned. |
| n_tuples_inserted | bigint | Number of tuples inserted. |
| n_tuples_updated | bigint | Number of tuples updated. |
| n_tuples_deleted | bigint | Number of tuples deleted. |
| cpu_time | bigint | CPU time, in microseconds. |
| lock_time | bigint | Time spent acquiring locks. |

Example (after the prerequisites above are met):

```sql
SELECT schema_name, unique_query_id, query, start_time, finish_time
FROM performance_schema.statement_history
ORDER BY start_time DESC
LIMIT 10;
```
