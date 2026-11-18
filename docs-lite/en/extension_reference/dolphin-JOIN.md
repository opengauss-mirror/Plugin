# JOIN

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:22:39.591Z pushedAt=2026-09-12T04:42:03.651Z -->

The JOIN clause is used to combine rows from two or more tables, based on common fields between these tables.

In the openGauss dolphin plugin, JOIN has five join types:

- CROSS JOIN: cross join
- INNER JOIN: inner join
- LEFT OUTER JOIN: left outer join
- RIGHT OUTER JOIN: right outer join
- FULL OUTER JOIN: full outer join
- STRAIGHT_JOIN: an inner join that specifies the join order

The following uses table1 and table2 as examples to illustrate different join types. The table data is as follows:

```
openGauss=# SELECT * FROM table1;
 name  | city
-------+------
 Joes  | BJ
 Lily  | BJ
 James | SH
 Grace | SZ
(4 rows)

openGauss=# SELECT * FROM table2;
 id | name  | dept
----+-------+-------
  1 | Tommy | IT
  2 | Lily  | IT
  3 | Li    | sales
  4 | Grace | IT
(4 rows)
```

## Cross Join <a name="section273517016141"></a>

A cross join is the Cartesian product, which refers to any combination of all tuples from two relations.

Example: Perform a cross join on table1 and table2. The result is as follows.

```
openGauss=# SELECT * FROM table1 CROSS JOIN table2;
 name  | city | id | name  | dept
-------+------+----+-------+-------
 Joes  | BJ   |  1 | Tommy | IT
 Joes  | BJ   |  2 | Lily  | IT
 Joes  | BJ   |  3 | Li    | sales
 Joes  | BJ   |  4 | Grace | IT
 Lily  | BJ   |  1 | Tommy | IT
 Lily  | BJ   |  2 | Lily  | IT
 Lily  | BJ   |  3 | Li    | sales
 Lily  | BJ   |  4 | Grace | IT
 James | SH   |  1 | Tommy | IT
 James | SH   |  2 | Lily  | IT
 James | SH   |  3 | Li    | sales
 James | SH   |  4 | Grace | IT
 Grace | SZ   |  1 | Tommy | IT
 Grace | SZ   |  2 | Lily  | IT
 Grace | SZ   |  3 | Li    | sales
 Grace | SZ   |  4 | Grace | IT
(16 rows)
```

## Inner Join<a name="section16248851415"></a>

When using an inner join, if the related fields of the two tables satisfy the join condition, data is extracted from both tables and combined into new records, creating a new result table. That is, in an inner join query, only tuples that satisfy the condition can appear in the result relation.

The inner join is the most commonly used join type and is also the default join type.

Example: Query the columns where the name field is the same in table1 and table2.

```
openGauss=# SELECT * FROM table1 a INNER JOIN table2 b ON a.name = b.name;
 name  | city | id | name  | dept
-------+------+----+-------+------
 Lily  | BJ   |  2 | Lily  | IT
 Grace | SZ   |  4 | Grace | IT
(2 rows)
```

## Left Outer Join<a name="section9773191361416"></a>

A left outer join refers to listing all tuples from the table on the left side of the keyword in a join query. If matching tuples can be found in the table on the right side, the matching tuple content is displayed. If no matching tuples can be found in the table on the right side, the corresponding tuple is NULL.

The keyword used in a left outer join query is LEFT OUTER JOIN. That is, the meaning of a left outer join is that the data in the table on the right side of the join keyword must satisfy the join condition, while the data in the table on the left side is output regardless of whether it satisfies the join condition.

Example:

```
openGauss=# SELECT * FROM table1 a LEFT OUTER JOIN table2 b ON a.name = b.name;
 name  | city | id | name  | dept
-------+------+----+-------+------
 Joes  | BJ   |    |       |
 Lily  | BJ   |  2 | Lily  | IT
 James | SH   |    |       |
 Grace | SZ   |  4 | Grace | IT
(4 rows)
```

## Right Outer Join<a name="section5250122061415"></a>

The right outer join is similar to the left outer join, except that all tuples in the table on the right side of the keyword are listed. The data in the table on the left side must satisfy the join condition, while the data in the table on the right side is output regardless of whether it satisfies the join condition.

The keyword for a right outer join query is RIGHT OUTER JOIN.

Example:

```
openGauss=# SELECT * FROM table1 a RIGHT OUTER JOIN table2 b ON a.name = b.name;
 name  | city | id | name  | dept
-------+------+----+-------+-------
 Lily  | BJ   |  2 | Lily  | IT
 Grace | SZ   |  4 | Grace | IT
       |      |  3 | Li    | sales
       |      |  1 | Tommy | IT
(4 rows)
```

## Full Outer Join<a name="section8954153161410"></a>

The characteristic of a full outer join query is that tuples from both the left and right tables are output. If no matching tuple is found, NULL is used instead.

Example:

```
openGauss=# SELECT * FROM table1 a FULL OUTER JOIN table2 b ON a.name = b.name;
 name  | city | id | name  | dept
-------+------+----+-------+-------
 Joes  | BJ   |    |       |
 Lily  | BJ   |  2 | Lily  | IT
 James | SH   |    |       |
 Grace | SZ   |  4 | Grace | IT
       |      |  3 | Li    | sales
       |      |  1 | Tommy | IT
(6 rows)
```

## STRAIGHT_JOIN

STRAIGHT_JOIN is similar to INNER JOIN, except that it always uses the left table as the driving table and the right table as the driven table. It can be used in cases where the join optimizer processes tables in a suboptimal order.

When STRAIGHT_JOIN is used together with a hint that specifies the join order, STRAIGHT_JOIN takes precedence.

Example:

```
openGauss=# EXPLAIN SELECT * FROM table1 a STRAIGHT_JOIN table2 b ON a.name = b.name;
                               QUERY PLAN                               
------------------------------------------------------------------------
 Hash Join  (cost=28.74..90.14 rows=3619 width=132)
   Hash Cond: ((a.name)::text = (b.name)::text)
   ->  Seq Scan on table1 a  (cost=0.00..18.69 rows=869 width=64)
   ->  Hash  (cost=18.33..18.33 rows=833 width=68)
         ->  Seq Scan on table2 b  (cost=0.00..18.33 rows=833 width=68)
(5 rows)

openGauss=# EXPLAIN SELECT STRAIGHT_JOIN * FROM table1 a JOIN table2 b ON a.name = b.name;
                               QUERY PLAN                               
------------------------------------------------------------------------
 Hash Join  (cost=28.74..90.14 rows=3619 width=132)
   Hash Cond: ((a.name)::text = (b.name)::text)
   ->  Seq Scan on table1 a  (cost=0.00..18.69 rows=869 width=64)
   ->  Hash  (cost=18.33..18.33 rows=833 width=68)
         ->  Seq Scan on table2 b  (cost=0.00..18.33 rows=833 width=68)
(5 rows)

```