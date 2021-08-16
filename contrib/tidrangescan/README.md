Tidrangescan provides a new scan type. Its implementation is based on the extensible operator of openGauss.

Tidrangescan adds a new exector node to support the use of the inequality operator to access system column *ctid* of a relation. For example, WHERE ctid >= '(10,0)'; will return all tuples on page 10 and over.
``` sql
postgres=# EXPLAIN (costs off) SELECT * FROM t1 WHERE ctid >= '(10,0)';
             QUERY PLAN              
-------------------------------------
 Tid Range Scan on t1
   TID Cond: (ctid >= '(10,0)'::tid)
(2 rows)
```
