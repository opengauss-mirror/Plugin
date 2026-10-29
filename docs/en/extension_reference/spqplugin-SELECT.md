# SELECT

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-10-27T11:01:31.892Z pushedAt=2026-10-27T11:19:18.051Z nl=2 -->

## Function Description

SELECT is used to retrieve data from a table or view.

A SELECT statement acts like a filter overlaid on a database table, using SQL keywords to filter out the data that users need from the data table.

## Notes

- This section covers only the multi-node parallel features newly added by spqplugin. The original openGauss syntax has not been deleted or modified.

## Examples

```sql
--Create tables t1 and t2.
openGauss=# create table t1(c1 int, c2 char);
CREATE TABLE
openGauss=# insert into t1 values(1, 'a');
INSERT 0 1
openGauss=# create table t2(c1 int, c2 char);
CREATE TABLE
openGauss=# insert into t2 values(1, 'b');
INSERT 0 1

--Enable spq.
openGauss=# set spqplugin.enable_spq = on;
SET

--Perform a join query.
openGauss=# explain select * from t1, t2 where t1.c1 = t2.c1;
                                           QUERY PLAN
-------------------------------------------------------------------------------------------------
 Streaming (type: GATHER)  (cost=0.00..862.00 rows=1 width=10)
   ->  Streaming(type: LOCAL GATHER dop: 1/2)  (cost=0.00..862.00 rows=1 width=10)
         ->  Hash Join  (cost=0.00..862.00 rows=1 width=10)
               Hash Cond: (t1.c1 = t2.c1)
               ->  Spq Seq Scan on t1  (cost=0.00..431.00 rows=1 width=5)
               ->  Hash  (cost=431.00..431.00 rows=6 width=5)
                     ->  Streaming(type: BROADCAST dop: 2/2)  (cost=0.00..431.00 rows=6 width=5)
                           ->  Spq Seq Scan on t2  (cost=0.00..431.00 rows=1 width=5)
(8 rows)

--Drop the tables.
openGauss=# drop table t1;
openGauss=# drop table t2;
```

## Related Links

[SELECT](https://docs.opengauss.org/en/docs/latest/sql_reference/select.html)
