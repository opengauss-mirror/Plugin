---
description: Introduces UPDATE in openGauss, covering Function Description, Notes, Examples.
keywords: [openGauss, UPDATE]
---

# UPDATE

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-10-29T02:30:12.770Z pushedAt=2026-10-29T06:32:21.808Z nl=2 -->

## Function Description

Updates data in a table. UPDATE modifies the specified field values in all rows that meet the conditions. The WHERE clause declares the conditions, the fields specified in the SET clause are modified, and the fields that do not appear retain their original values.

## Notes

- This chapter covers only the multi-server parallel feature newly added by spqplugin. The original openGauss syntax has not been deleted or modified.

## Examples

```sql
--Create table t1.
openGauss=# create table t1(c1 int, c2 char);
CREATE TABLE
openGauss=# insert into t1 values(1, 'a');
INSERT 0 1

--Enable spq update.
openGauss=# set spqplugin.enable_spq = on;
SET
openGauss=# set spqplugin.spq_optimizer_enable_dml = true;
SET
openGauss=# set spqplugin.spq_optimizer_enable_dml_constraints = true;
SET
openGauss=# set spqplugin.spq_enable_update = on;
SET
openGauss=# set query_dop = 2;
SET
--View the multi-node plan.
openGauss=# explain update t1 set c1 = 2 where c1 < 2;
                                               QUERY PLAN
---------------------------------------------------------------------------------------------------
 Streaming (type: GATHER)  (cost=0.00..0.00 rows=0 width=1)
   ->  Update on t1  (cost=0.00..431.03 rows=1 width=1)
         ->  SPQ Result  (cost=0.00..0.00 rows=0 width=0)
               ->  Streaming(type: DML REDISTRIBUTE dop: 1/2)  (cost=0.00..431.00 rows=2 width=22)
                     ->  Split  (cost=0.00..431.00 rows=2 width=22)
                           ->  Spq Seq Scan on t1  (cost=0.00..431.00 rows=1 width=11)
                                 Filter: (c1 < 2)
(7 rows)

--Drop the table.
openGauss=# drop table t1;
```

## Related Links

[UPDATE](https://docs.opengauss.org/en/docs/latest/sql_reference/update.html)
