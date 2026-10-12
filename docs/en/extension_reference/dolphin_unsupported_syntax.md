# Unsupported Syntax

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:21:31.190Z pushedAt=2026-09-22T11:49:37.242Z -->

Based on certain new features, the dolphin plugin disables some syntax features. This chapter lists the syntax that is not supported by default after the dolphin plugin is used.

## ANALYSE

### Syntax Change Notes

- The ANALYSE keyword is no longer supported for collecting statistics related to the content of ordinary tables in the database. Use the ANALYZE keyword for related functionality.

```sql
-- Previously supported syntax:
ANALYSE;
-- Available related syntax
ANALYZE; 
```

## ANALYZE

- The ANALYZE syntax cannot directly use the verbose option. In actual use, it must be wrapped in parentheses ().

```sql
-- Previously supported syntax:
ANALYZE VERBOSE;
-- Available related syntax
ANALYZE (VERBOSE);
-- When the table VERBOSE exists, the following statement performs ANALYZE on the VERBOSE table.
ANALYZE VERBOSE;
```

### Replacement Syntax

## VACUUM

### Syntax Change Notes

- The VACUUM-related options such as COMPACT are disabled.

```sql
-- The previously supported syntax, such as the following, will no longer be supported
vacuum full compact table_name;
-- Available related syntax
vacuum full table_name;
```

- The VACUUM syntax cannot directly use the verbose and freeze options. In actual use, they need to be enclosed in parentheses ().

```sql
-- -- Previously supported syntax, which will no longer be supported.
vacuum full freeze verbose table_name;
-- -- Available alternative syntax.
vacuum full table_name;
-- -- Or
vacuum (full, freeze, verbose) table_name;

```

## CLUSTER

- The CLUSTER syntax cannot directly use verbose. In actual use, it needs to be wrapped in parentheses ().

```sql
-- Previously supported syntax
cluster verbose index_name;
-- Available related syntax
cluster (verbose) index_name;
```

## ROWNUM

- ROWNUM is no longer used as a pseudocolumn.

## USER

-- Remove the syntax where SELECT USER returns the current user. Use the SELECT USER() syntax instead.

```sql
-- Previously supported syntax, which will no longer be supported
SELECT USER;
-- Available alternative syntax
SELECT USER();
```

## CREATE-TRIGGER

-- Do not use the USER name as the name of an openGauss trigger.

## SYSDATE

-- The SELECT SYSDATE syntax is disabled. Use SELECT SYSDATE() instead.

```sql
-- Syntax previously supported, which will no longer be supported.
SELECT SYSDATE;
-- Available alternative syntax.
SELECT SYSDATE();
```

## COLLATION FOR

-- The COLLATION FOR syntax is disabled and can be replaced with pg_collation_for.

```sql
CREATE TABLE test(id INT);
-- Previously supported syntax, which will no longer be supported
SELECT COLLATION FOR (id) from test;
-- Available alternative syntax
SELECT pg_collcation_for(id) from test;
```

## CREATE RULE

-- When using the CREATE RULE syntax, if the filtering condition of the rule requires using the PRIOR/BINARY field column alone, parentheses must be added around that field (other fields are not affected).

```sql
create table test(binary boolean);
create table test2(id boolean);
-- Previously supported syntax, which will no longer be supported
create rule "test_rule" as on select to test where binary do instead select * from test2;
-- Available alternative syntax (only indicates syntactic compliance, not guaranteed to be functionally available)
create rule "test_rule" as on select to test where (binary) do instead select * from test2;
```

## CASE WHEN END

-- When using CASE WHEN END statements, if a PRIOR/BINARY field column needs to be returned at the end, the field must be enclosed in parentheses (other fields are not affected).

```sql
create table test(id int,binary int);
select case when id % 2 =0 then 1 else binary end from test;
-- Available related syntax
select case when id % 2 =0 then 1 else (binary) end from test;
```

## ONLY

ONLY indicates whether to operate on child tables of a table. For example, in the ALTER TABLE clause, when modifying table attributes, whether to modify child tables is determined by whether ONLY is declared. In actual use, if ONLY is needed, parentheses must be added around ONLY. Currently, openGauss does not support inheritance tables, so this syntax is not elaborated here.

## ON DUPLICATE KEY UPDATE

- In openGauss, the ON DUPLICATE KEY UPDATE syntax uses `excluded` as the alias for new data by default. Using `excluded` as an alias will no longer be supported.

```sql
create table test(a int ,b int,c int);
create unique index on test(b);
-- -- Previously supported syntax, which will no longer be supported
insert into test values(1,2,3) on duplicate key update b = excluded.b + 1;
-- -- Available alternative syntax
insert into test values(1,2,3) on duplicate key update b = b + 1;
```

## MINUS

openGauss can perform related operations based on query datasets. MINUS represents the exclusion operation between sets. The MINUS feature will become unavailable in the future; use the EXCEPT keyword as a replacement.

```sql
-- Previously supported syntax, which will no longer be supported
select generate_series(1,10) MINUS select generate_series(2,10);
-- Available alternative syntax
select generate_series(1,10) EXCEPT select generate_series(2,10);
```

## Factorial Operator

openGauss allows using ! as a suffix to compute factorials. In the future, the ! factorial functionality will be disabled and replaced by the factorial function.

```sql
-- The previously supported syntax will no longer be supported.
select 10!;
-- Available alternative syntax
select factorial(10);
```

## abstime'' Syntax

openGauss supports type conversion using abstime''. The abstime'' type conversion functionality will become unavailable in the future. Use :: or cast as a replacement.

```sql
-- Available related syntax
select cast('2000-1-1 1:1:1' as abstime);
select '2000-1-1 1:1:1'::abstime;
```

## Year 1 BC in Date and Timestamp

openGauss date and timestamp types are compatible with MySQL's year 0000, while also supporting openGauss's original BC years. Since the year values of 1 BC (0001 BC) and year 0000 are equivalent in the internal implementation, these two years are effectively identical. For MySQL compatibility, they are uniformly output as year 0000, and output as the original 0001 BC is no longer supported. If 0001 BC is required, year 0000 can be used as a substitute.

```sql
compat_b=# SELECT date '0000-01-01';
    date    
------------
 0000-01-01
(1 row)

compat_b=# SELECT date '0001-01-01 BC';
    date    
------------
 0000-01-01
(1 row)

```