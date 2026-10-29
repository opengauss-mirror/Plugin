# CREATE INDEX

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-10-27T11:01:15.126Z pushedAt=2026-10-27T11:14:26.418Z nl=2 -->

## Function Description

Creates an index on a specified table.

An index can be used to improve database query performance, but improper use will degrade database performance. It is recommended to create an index in the following scenarios:

- Fields that are frequently queried.
- Create an index on join conditions. For queries involving joins on multiple fields, it is recommended to create a composite index on these fields. For example, for `select \* from t1 join t2 on t1.a=t2.a and t1.b=t2.b`, you can create a composite index on fields `a` and `b` of table `t1`.
- Fields used as filter conditions in the WHERE clause (especially range conditions).
- Fields that frequently appear after ORDER BY, GROUP BY, and DISTINCT.

The syntax for creating an index on a partitioned table is slightly different from that for creating an index on a regular table. Pay attention to the differences when using it. For example, creating an index on a partial table is not supported.

## Notes

- This section covers only the multi-node parallel features newly added by spqplugin. The original openGauss syntax has not been deleted or modified.

## Examples

```sql
--Create table t1.
openGauss=# create table t1(c1 int, c2 char);
CREATE TABLE
openGauss=# insert into t1 values(1, 'a');
INSERT 0 1

--Create a default B-tree index on column c1 of table t1.
openGauss=# set spqplugin.spq_enable_btbuild = on;
SET
openGauss=# create index idx1 on t1 (c1) with (spq_build=on);
CREATE INDEX
openGauss=# \d+ t1
                             Table "public.t1"
 Column |     Type     | Modifiers | Storage  | Stats target | Description
--------+--------------+-----------+----------+--------------+-------------
 c1     | integer      |           | plain    |              |
 c2     | character(1) |           | extended |              |
Indexes:
    "idx1" btree (c1) WITH (spq_build=finish) TABLESPACE pg_default
Has OIDs: no
Options: orientation=row, compression=no

--Drop the index.
openGauss=# drop index idx1;

--Create an online B-tree index on column c1 of table t1.
openGauss=# set spqplugin.spq_enable_btbuild = on;
openGauss=# set spqplugin.spq_enable_btbuild_cic = on;
SET
SET
openGauss=# create index concurrently idx1 on t1 (c1) with (spq_build=on);
CREATE INDEX
openGauss=# \d+ t1
                             Table "public.t1"
 Column |     Type     | Modifiers | Storage  | Stats target | Description
--------+--------------+-----------+----------+--------------+-------------
 c1     | integer      |           | plain    |              |
 c2     | character(1) |           | extended |              |
Indexes:
    "idx1" btree (c1) WITH (spq_build=finish) TABLESPACE pg_default
Has OIDs: no
Options: orientation=row, compression=no

--Drop the index and the table.
openGauss=# drop index idx1;
openGauss=# drop table t1;
```

Currently, only multi-node parallel btree indexes are supported.

## Related Links

[CREATE INDEX](https://docs.opengauss.org/en/docs/latest/sql_reference/create_index.html)
