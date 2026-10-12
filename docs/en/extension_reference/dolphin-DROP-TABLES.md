# DROP TABLES

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:22:10.151Z pushedAt=2026-09-12T04:38:52.190Z -->

## Function Description<a name="zh-cn_topic_0283137126_zh-cn_topic_0237122076_zh-cn_topic_0059779051_s2baab5c876044795a12b5949f22d2144"></a>

Deletes one or more tables.

## Precautions<a name="zh-cn_topic_0283137126_zh-cn_topic_0237122076_zh-cn_topic_0059779051_s8ea536d5b8ff459e9e3614e35f53bc2a"></a>

- DROP TABLES forcibly deletes the specified tables. After a table is deleted, the indexes that depend on the table are also deleted, and the functions and stored procedures that reference the table will become unexecutable. Deleting a partitioned table also deletes all partitions within it.
- To execute this statement, you must have the DROP privilege for each specified table.
- If the specified tables include nonexistent ones, when IF EXISTS is used, the statement deletes all other existing tables and generates warnings for all nonexistent tables. When IF EXISTS is not used, the statement fails without making any data changes and returns an error message indicating the nonexistent tables encountered.

## Syntax<a name="zh-cn_topic_0283137126_zh-cn_topic_0237122076_zh-cn_topic_0059779051_s58bdce220c9f4292ba9af919b04ad25c"></a>

```
DROP TABLES [ IF EXISTS ] 
    { [schema.]table_name } [, ...] [ CASCADE | RESTRICT ] [ PURGE ];
```

## Parameter Description<a name="zh-cn_topic_0283137126_zh-cn_topic_0237122076_zh-cn_topic_0059779051_sf4962205ddf84312a5fd888bc662e5cf"></a>

- **IF EXISTS**

    If the specified table does not exist, a notice is issued instead of throwing an error.

- **schema**

    Schema name.

- **table\_name**

    Name of the table.

- **CASCADE | RESTRICT**
    - CASCADE: cascade deletes objects that depend on the table (such as views).
    - RESTRICT (default): if only view-dependent objects exist, the table is allowed to be deleted; otherwise, the deletion is rejected. When querying the view, a corresponding dependency error is prompted. If a newly created table has table names and column names that match the view dependencies, the dependency on the new table is re-established when querying the view, and the view is restored.

- **PURGE**

    This parameter indicates that even if the recycle bin feature is enabled, the table will be physically deleted directly when dropped, rather than being placed in the recycle bin.

> The parameter behavior involved is the same as that of [DROP TABLE](../sql_reference/drop_table.md).

## Examples<a name="zh-cn_topic_0283137126_zh-cn_topic_0237122076_zh-cn_topic_0059779051_se4f9dc97861c410bb51554bb58bcd76d"></a>

```
openGauss=# CREATE TABLE table_to_drop1(a int);
openGauss=# CREATE TABLE table_to_drop2(a int);
openGauss=# CREATE TABLE table_to_drop3(a int);
openGauss=# DROP TABLES table_to_drop1,table_to_drop2;
openGauss=# DROP TABLES table_to_drop3;
```

## Related Links<a name="section156744489391"></a>

[DROP TABLE](../sql_reference/drop_table.md)