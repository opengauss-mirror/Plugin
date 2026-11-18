# Alias

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:20:57.922Z pushedAt=2026-09-12T03:42:37.694Z -->

SQL can rename a field's name, and this name is called the alias of the table or the field. The alias can be a single-quoted string. When ansi_quotes is turned off, a double-quoted string can also be used.

## Syntax Format<a name="section119531510131916"></a>

- Column alias syntax

    ```
    SELECT 
    { * | [column [ AS ] output_name, ...] }
    [ FROM from_item [, ...] ]
    [ WHERE condition ];
    ```

## Parameter Description<a name="section012816161027"></a>

- **output\_name**

    AS is an optional keyword. By using the AS clause (optional), you can assign an alias to an output field via output\_name. The output\_name supports strings in single quotation marks.

## Examples<a name="section73551858131418"></a>

```
openGauss=# select id 'test_id' from tab;
 test_id
---------
      22
(1 row)

openGauss=# select id as 'test_id' from tab;
 test_id
---------
      22
(1 row)
```