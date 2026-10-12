# List of MySQL Features Not Compatible

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:20:36.537Z pushedAt=2026-09-11T12:48:06.278Z -->

The dolphin plugin aims to achieve compatibility with MySQL database syntax, features, and more. However, during actual usage and comparison with MySQL, it has been found that certain MySQL behaviors do not align with typical usage patterns. These are listed here.

## Index Invalidation Caused by Different Implicit Conversion Rules

It is well known that implicit conversion is a common cause of index invalidation. If an index is created on an integer column and the integer column is compared with a floating-point number, openGauss needs to convert the integer type to a floating-point type, which leads to index invalidation, whereas MySQL does not. The test SQL is as follows:

```
create table t_int(a int);   -- Create a table with the int data type.
insert into t_int values(57); -- Insert data.

select * from t_int where a=57.2;  -- Use a floating-point number as the filter condition. Both MySQL and openGauss return 0 rows.
select * from t_int where a=degrees(1); -- Use a function that returns a floating-point number as the filter condition. Both MySQL and openGauss return 0 rows (the result of degrees(1) is 57.295779513082).

create index i_int on t_int(a); -- Create an index on the integer column.
select * from t_int where a=57.2; -- Use a floating-point number as the filter condition. openGauss returns 0 rows and cannot use the index. MySQL 5.7 returns 1 row and can use the index; MySQL 8.0 returns 0 rows and directly optimizes the filter condition to constant false.
select * from t_int where a=degrees(1); -- Use a function that returns a floating-point number as the filter condition. openGauss returns 0 rows and cannot use the index. Both MySQL 5.7 and 8.0 return 1 row and can use the index.
```

From the above examples, it can be seen that when an index exists on an integer column and a floating-point number is used as the filter condition, MySQL tends to forcibly cast the floating-point number to an integer before filtering, thereby allowing the index to be used normally. However, such a result is actually problematic, as it leads to situations where 57 = 57.2. MySQL 8.0 has made certain optimizations for this case, but the issue persists when a function is used as the filter condition.

openGauss currently maintains the current behavior: when an integer column has an index and a floating-point number is used as a filter condition, the integer column is cast to a floating-point number before comparison, even though the filtered results are inconsistent with those of MySQL.

## Comparison Operators Between `TIME` Type and Strings

For comparison operations such as `<`, `>`, and `BETWEEN` involving `TIME` type and string type, MySQL exhibits different behaviors depending on the scenario:

1. In constant comparison scenarios, MySQL directly uses the lexicographic order of strings.
2. When `TIME` is a field type, MySQL first attempts to convert the string to `TIME` type for comparison; if the conversion fails, it falls back to lexicographic order.
3. When `TIME` is an indexed column, MySQL first attempts to convert the string to `TIME` type for index filtering; if the conversion fails, it still uses the failed conversion result for filtering.

The test SQL is as follows:

```sql
mysql> select * from t where v < '2018-01-01';
+----------+
| v        |
+----------+
| 19:18:00 |
| 00:18:00 |
+----------+
2 rows in set, 1 warning (0.00 sec)

mysql> create index idx_t on t (v);
Query OK, 0 rows affected (0.03 sec)
Records: 0  Duplicates: 0  Warnings: 0

mysql> select * from t where v < '2018-01-01';
+----------+
| v        |
+----------+
| 00:18:00 |
+----------+
1 row in set, 2 warnings (0.00 sec)

```

This example describes scenario 3, where MySQL attempts to convert `2018-01-01` to the `TIME` type. Although the conversion fails, MySQL still uses the abnormal conversion result `00:20:18` for index filtering. As a result, the query results before and after using the index become inconsistent.

In contrast, when openGauss compares `TIME` with a string type, it always attempts to convert the string to the `TIME` type (the same as MySQL's behavior when an index is used). Therefore, openGauss maintains consistent comparison behavior across different scenarios and is able to support indexes.