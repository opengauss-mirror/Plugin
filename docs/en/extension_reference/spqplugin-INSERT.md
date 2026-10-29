# INSERT

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-10-27T11:01:34.841Z pushedAt=2026-10-27T11:17:55.993Z nl=2 -->

## Function Description

Adds one or more rows to a table.

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

--Enable spq insert.
openGauss=# set spqplugin.enable_spq = on;
SET
openGauss=# set spqplugin.spq_optimizer_enable_dml = true;
SET
openGauss=# set spqplugin.spq_optimizer_enable_dml_constraints = true;
SET
openGauss=# set spqplugin.spq_enable_insert_select = on;
SET
openGauss=# set query_dop = 2;
SET

--View the multi-node plan.
openGauss=# explain insert into t2 select * from t1;
                                               QUERY PLAN
---------------------------------------------------------------------------------------------------
 Streaming (type: GATHER)  (cost=0.00..0.00 rows=0 width=5)
   ->  Insert on t2  (cost=0.00..431.01 rows=1 width=5)
         ->  SPQ Result  (cost=0.00..0.00 rows=0 width=0)
               ->  Streaming(type: DML REDISTRIBUTE dop: 1/2)  (cost=0.00..431.00 rows=1 width=16)
                     ->  Spq Seq Scan on t1  (cost=0.00..431.00 rows=1 width=5)
(5 rows)


--Drop the tables.
openGauss=# drop table t1;
openGauss=# drop table t2;
```

## Related Links

[INSERT](https://docs.opengauss.org/en/docs/latest/sql_reference/insert.html)
