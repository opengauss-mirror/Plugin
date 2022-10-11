## Introduction
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

## Installation
Copy tidrangescan directoty to openGauss-server/contrib and make.
```
cp contrib/tidrangescan /YOUR_PATH/openGauss-server/contrib -r
cd /YOUR_PATH/openGauss-server/contrib/tidrangescan
make && make install
```
## Use extension
Connect to gaussdb and create extension.
```
CREATE EXTENSION tidrangescan;
```
Also need to add *tidrangescan* into a guc parameter named 'shared_preload_libraries' and restart the database to make it work.
```
# the default is 'security_plugin'
shared_preload_libraries = 'security_plugin, tidrangescan'
```