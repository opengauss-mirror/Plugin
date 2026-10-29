# @variable Variable

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:20:44.752Z pushedAt=2026-09-11T12:17:19.064Z -->

## Feature Description

In MySQL-compatible mode, openGauss supports the following two forms of using the user variable `@variable`:

- Supports declaring `@variable` using the SET command, as in: `set @variable = value` or `set @variable := value`.

- Supports assigning a value to the `@variable` variable using the SELECT command, as in `select @variable := value`.

The `@variable` feature enables recursive queries in SQL. For details, see [Example 3](#examples). A recursive query is a special query technique that traverses an entire dataset by repeatedly invoking a single query. Recursive queries are typically used to query tree-structured or graph-structured data, where parent-child relationships exist between nodes.

## Notes

- To use this feature, you need to enable the parameter enable_set_variable_b_format, which allows the database to use custom user variables in MySQL compatibility mode.

- User variables are private variables for the user currently logged in to openGauss. A declared `@variable` variable remains valid throughout the entire process of the client connection to the database instance.

- The SET command supports the assignment operators `:=` or `=`, while SELECT only supports `:=` for assignment.

- When using PREPARE FROM to name an SQL statement, the string stored in a user-defined variable only supports SELECT, INSERT, UPDATE, DELETE, and MERGE syntax, and must be a single statement.

- Both SET and SELECT can assign values to multiple variables simultaneously.

- Queries with variable assignment cannot use the SMP parallel execution feature.

## Examples

**Example 1:** Use the SET command to assign a value to a variable.

1. Set the parameter enable_set_variable_b_format to on.

```sql
SET enable_set_variable_b_format=on;
```

2. Use SET to define a variable.

```sql
set @VAR1_1102053=123;
set @var2_1102053:=1111::int4;
set @var3_1102053 := @var6_1102053 := @$var7_1102053:=12345678::int8;
```

3. Directly view the value of the variable.

```sql
select @VAR1_1102053,@VAR2_1102053,@VAR3_1102053;
```

The result is as follows:

```sql
 @var1_1102053 | @var2_1102053 | @var3_1102053
---------------+---------------+---------------
           123 |          1111 |      12345678
(1 row)
```

4. Create a test table.

```sql
create table table_1102053 (id int,name text);
```

5. Use the declared variables when inserting data into the test table.

```sql
insert into table_1102053 values(@VAR1_1102053,'test');
```

6. View the test table data.

```sql
select * from table_1102053;
```

The result is as follows. The value of variable `@VAR1_1102053` has been inserted into the corresponding position in the table:

```sql
 id  | name
-----+------
 123 | test
(1 row)
```

**Example 2:** Assign a value to a variable directly using SELECT and view it.

1. Set the parameter enable_set_variable_b_format to on.

```sql
SET enable_set_variable_b_format=on;
```

2. Execute the following SELECT statement.

```sql
select @var:=1 as col1,@va:=3;
```

The result is as follows:

```sql
 col1 | ?column?
------+----------
    1 |        3
(1 row)
```

**Example 3:**<a name="example3"></a> Use @ variables to implement a recursive query.

1. Set the parameter enable_set_variable_b_format to on.

```sql
SET enable_set_variable_b_format=on;
```

2. Create a test table and insert data.

```sql
CREATE TABLE my_table_1162203 (
id INT PRIMARY KEY AUTO_INCREMENT,
name VARCHAR(255) NOT NULL,
parent_id INT,
CONSTRAINT uc_name UNIQUE (name),
CONSTRAINT fk_parent FOREIGN KEY (parent_id) REFERENCES my_table_1162203(id)
);
-- Insert data
INSERT INTO my_table_1162203 (name, parent_id) VALUES
('a1', 1),
('a2', 2),
('a3', 1),
('a4', 2),
('a5', 1),
('a6', 2);
```

3. View the test table data.

```sql
select * from my_table_1162203;
```

The returned result is as follows. The three fields represent the ID, name, and parent node ID of each record, respectively.

```sql
 id | name | parent_id
----+------+-----------
  1 | a1   |         1
  2 | a2   |         2
  3 | a3   |         1
  4 | a4   |         2
  5 | a5   |         1
  6 | a6   |         2
(6 rows)
```

4. Use the SET command to assign a value to the variable, specifying <code>@parent_id</code> as 1.

```sql
SET @parent_id := 1;
```

5. Retrieve all sibling nodes of the given node, returning all records whose parent node ID is 1.

```sql
SELECT id, name
FROM my_table_1162203
WHERE parent_id = (
SELECT parent_id
FROM my_table_1162203
WHERE id = @parent_id
);
```

The result is as follows:

```sql
 id | name
----+------
  1 | a1
  3 | a3
  5 | a5
(3 rows)
```

6. The following statement is equivalent to the combination of step 4 (defining the variable) and step 5 (querying), achieving the same recursive query effect in a single statement:

```sql
SELECT id, name
FROM my_table_1162203
WHERE parent_id = (
SELECT parent_id
FROM my_table_1162203
WHERE id =(select @parent_id:= 1)
);
```

The result is as follows:

```sql
 id | name
----+------
  1 | a1
  3 | a3
  5 | a5
(3 rows)
```

**Example 4:** Use the SET syntax to create a PREPARE statement.

1. Set the parameter enable_set_variable_b_format to on.

```sql
SET enable_set_variable_b_format=on;
```

2. Create a test table.

```sql
create table tb1(id int);
```

3. Use the SET syntax to assign two user-defined variables as strings with SQL statement content.

```sql
set @sql1:='insert into tb1 values(5)';  --Statement 1
set @sql2:='select * from tb1';          --Statement 2
```

4. Name the two SQL statements from step 3.

```sql
PREPARE stmt1 from @sql1;
PREPARE stmt2 from @sql2;
```

5. Execute statement 1 to perform the insert operation.

```sql
EXECUTE stmt1;
```

6. Execute statement 2 to view the test table data.

```sql
EXECUTE stmt2;
```

The returned result is:

```sql
 id             
----
  5
(1 row)
```