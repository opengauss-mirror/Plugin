# DELETE

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-10-27T11:01:14.590Z pushedAt=2026-10-27T11:15:44.756Z nl=2 -->

## Function Description

DELETE deletes rows that meet the WHERE clause from the specified table. If the WHERE clause is absent, all rows in the table are deleted, and only the table structure is retained.

## Notes

- This section covers only the multi-node parallel features newly added by spqplugin. The original openGauss syntax has not been deleted or modified.

## Examples

```sql
--Create table t1.
openGauss=# create table t1(c1 int, c2 char);
CREATE TABLE
openGauss=# insert into t1 values(1, 'a');
INSERT 0 1

--Enable spq delete.
openGauss=# set spqplugin.enable_spq = on;
SET
openGauss=# set spqplugin.spq_optimizer_enable_dml = true;
SET
openGauss=# set spqplugin.spq_enable_delete = on;
SET
openGauss=# set query_dop = 2;
SET
--View the multi-node plan.
openGauss=# explain delete from t1 where c1 = 1;
                                              QUERY PLAN
---------------------------------------------------------------------------------------------------
 Streaming (type: GATHER)  (cost=0.00..0.00 rows=0 width=1)
   ->  Delete on t1  (cost=0.00..431.01 rows=1 width=1)
         ->  SPQ Result  (cost=0.00..431.00 rows=1 width=22)
               ->  Streaming(type: DML REDISTRIBUTE dop: 1/2)  (cost=0.00..431.00 rows=1 width=11)
                     ->  Spq Seq Scan on t1  (cost=0.00..431.00 rows=1 width=11)
                           Filter: (c1 = 1)
(6 rows)

--Drop the table.
openGauss=# drop table t1;
```

## Related Links

[DELETE](https://docs.opengauss.org/en/docs/latest/sql_reference/delete.html)
