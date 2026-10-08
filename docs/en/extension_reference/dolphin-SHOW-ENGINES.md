# SHOW STORAGE ENGINES

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:24:35.475Z pushedAt=2026-09-12T05:29:05.611Z -->

## Description<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s86b6c9741c7741d3976c5e358e8d5486"></a>

Lists information about supported storage engines.

## Notes<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_sdd2da7fe44624eb99ee77013ff96c6bd"></a>

- The result shows the storage engines supported for the user executing the statement.

## Syntax<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_se242be9719f44731b261539dbd42d7b9"></a>

```
SHOW [STORAGE] ENGINES
```

## Field Description<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s06dfa4f09bfd4e0d9826a80e6a91b0a6"></a>

- **Engine**

       Name of the storage engine.

- **Support**

       Support status of the storage engine. Possible values are DEFAULT, YES, and NO. DEFAULT indicates that tables of this storage engine are created by default, YES indicates that creating tables of this storage engine is supported, and NO indicates that it is not supported.

- **Comment**

       Description of the storage engine.

- **Transactions**

       Transaction support status of the storage engine.

- **XA**

       Support for XA transactions by the storage engine (currently empty).

- **Savepoints**

       Support for savepoints by the storage engine.

## Example<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_sfff14489321642278317cf06cd89810d"></a>

```
testdb_m=# show storage engines;
 Engine  | Support |            Comment                    | Transactions | XA | Savepoints 
---------+---------+---------------------------------------+--------------+----+------------
 ASTORE  | DEFAULT | Append Update Storage Engine          | YES          |    | YES
 USTORE  | YES     | In-place Update Storage Engine        | YES          |    | YES
 row     | DEFAULT | Row-Oriented Table Storage Engine     | YES          |    | YES
 column  | YES     | Column-Oriented Table Storage Engine  | YES          |    | YES
 mot_fdw | NO      | Memory-Optimized Table Storage Engine | YES          |    | NO
(5 rows)

```