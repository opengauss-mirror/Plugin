# SHOW OPEN TABLES

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:25:06.750Z pushedAt=2026-09-12T05:33:28.392Z -->

## Function Description<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s86b6c9741c7741d3976c5e358e8d5486"></a>

Lists all opened non-temporary tables.

## NOTE<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_sdd2da7fe44624eb99ee77013ff96c6bd"></a>

- In the result of SHOW OPEN TABLES in a MySQL database, the Database field indicates the database name, whereas in openGauss it indicates the schema name where the table resides.

## Syntax Format<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_se242be9719f44731b261539dbd42d7b9"></a>

```
SHOW OPEN TABLES
    [{FROM | IN} schema_name]
    [LIKE 'pattern' | WHERE expr]
```

## Parameter Description<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s06dfa4f09bfd4e0d9826a80e6a91b0a6"></a>

- **[{FROM | IN} schema_name]**

       Both are equivalent.

- **[LIKE 'pattern' | WHERE expr]**

       pattern supports the LIKE syntax and can be the full name or a part of schema_name, used for fuzzy queries; expr supports any expression. Typical usage is as follows:
       show open tables where Database = 'schema_name'; 
       show open tables where "Table" = 'table_name';

## Field Description<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s06dfa4f09bfd4e0d9826a80e6a91b0a6"></a>

- **Database**

       The name of the schema where the table resides.

- **Table**

       The table name.

- **In_use**

       The number of table locks or lock requests. For example, if a client acquires a table lock using LOCK TABLE t1 WRITE, In_use will be 1. If another client issues LOCK TABLE t1 WRITE while the table remains locked, the client will enter lock wait, and In_use will be 2 at this point. If the count is zero, the table is open but not currently in use.

- **Name_locked**

       Whether the table is locked by ACCESS EXCLUSIVE.

## Example<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_sfff14489321642278317cf06cd89810d"></a>

```
testdb_m=# show open tables;
  Database  |         Table         | In_use | Name_locked 
------------+-----------------------+--------+-------------
 pg_catalog | pg_type               |      0 |           0
 pg_catalog | pg_statistic          |      0 |           0
 pg_catalog | pg_settings           |      0 |           0
 pg_catalog | pg_subscription       |      0 |           0
 pg_catalog | pg_attribute          |      0 |           0
 pg_catalog | pg_proc               |      0 |           0
 pg_catalog | pg_class              |      0 |           0
 pg_catalog | gs_package            |      0 |           0
 pg_catalog | pg_authid             |      0 |           0
 pg_catalog | pg_index              |      0 |           0
 pg_catalog | pg_operator           |      0 |           0
 pg_catalog | pg_opclass            |      0 |           0
 pg_catalog | pg_amop               |      0 |           0
 pg_catalog | pg_amproc             |      0 |           0
 pg_catalog | pg_rewrite            |      0 |           0
 pg_catalog | pg_cast               |      0 |           0
 pg_catalog | pg_namespace          |      0 |           0
 pg_catalog | pg_database           |      0 |           0
 pg_catalog | pg_db_role_setting    |      0 |           0
 pg_catalog | pg_tablespace         |      0 |           0
 pg_catalog | pg_auth_members       |      0 |           0
 pg_catalog | pg_extension          |      0 |           0
 pg_catalog | pgxc_node             |      0 |           0
 pg_catalog | pgxc_group            |      0 |           0
 pg_catalog | pg_resource_pool      |      0 |           0
 pg_catalog | pg_default_acl        |      0 |           0
 pg_catalog | gs_client_global_keys |      0 |           0
 pg_catalog | pg_job                |      0 |           0
 pg_catalog | pg_synonym            |      0 |           0
 pg_catalog | gs_txn_snapshot       |      0 |           0
 pg_catalog | gs_sql_patch          |      0 |           0
 pg_catalog | pg_user_status        |      0 |           0
 pg_catalog | pg_auth_history       |      0 |           0
(33 rows)

testdb_m=# show open tables in show_open_tables_scm;
       Database       |          Table          | In_use | Name_locked 
----------------------+-------------------------+--------+-------------
 show_open_tables_scm | show_open_tables_test_1 |      0 |           0
 show_open_tables_scm | show_open_tables_test_2 |      0 |           0
(2 rows)

testdb_m=# show open tables like 'show%';
       Database       |          Table          | In_use | Name_locked 
----------------------+-------------------------+--------+-------------
 show_open_tables_scm | show_open_tables_test_1 |      0 |           0
 show_open_tables_scm | show_open_tables_test_2 |      0 |           0
(2 rows)

testdb_m=# show open tables where "Table" = 'show_open_tables_test_1';
       Database       |          Table          | In_use | Name_locked 
----------------------+-------------------------+--------+-------------
 show_open_tables_scm | show_open_tables_test_1 |      0 |           0
(1 row)

testdb_m=# show open tables where Database = 'show_open_tables_scm';
       Database       |          Table          | In_use | Name_locked 
----------------------+-------------------------+--------+-------------
 show_open_tables_scm | show_open_tables_test_1 |      0 |           0
 show_open_tables_scm | show_open_tables_test_2 |      0 |           0
(2 rows)

testdb_m=# lock tables show_open_tables_test_1 read;
LOCK TABLE
testdb_m=# show open tables where In_use > 0;
       Database       |          Table          | In_use | Name_locked 
----------------------+-------------------------+--------+-------------
 show_open_tables_scm | show_open_tables_test_1 |      1 |           0
(1 row)

testdb_m=# unlock tables;
COMMIT
```