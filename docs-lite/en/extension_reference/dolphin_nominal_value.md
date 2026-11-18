# dolphin-Literal Values

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-10-27T11:01:11.256Z pushedAt=2026-10-27T11:07:21.450Z nl=2 -->

## String Literals

Compared with the original openGauss, the modifications made by dolphin to string literals are mainly as follows:

- Adjacent quoted strings are concatenated into a single string.

Example:

```sql
openGauss=# select 'a string' as col;
   col
----------
 a string
(1 row)

openGauss=# select 'a' ' ' 'string' as col;
   col
----------
 a string
(1 row)
```
