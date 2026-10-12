# JSON Operator Compatibility

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:21:08.881Z pushedAt=2026-09-11T13:12:29.755Z -->

When the parameter switch dolphin.b_compatibility_mode is set to on, four arithmetic operations and MySQL compatibility for JSON type operators are enabled. Compared with the original openGauss, dolphin's modifications to JSON operators primarily support:

## Unary Left Operator

```+ - ~ not @```

Note: Type conversion is required, and this is allowed only when `sql_mode_strict` is disabled in `dolphin.sql_mode`.

## Binary Operators

### 1. Four Arithmetic Operations

```+ - * / div```

NOTE
Type conversion is required and is allowed only when sql_mode_strict is disabled in dolphin.sql_mode.

### 2. Other Non-Comparison Operators

 ```% mod ^ xor >> << | & || && and or```

Note:

1. Type conversion is required, and this is allowed only when `sql_mode_strict` is disabled in `dolphin.sql_mode`.
2. The `||` operator requires `pipes_as_concat` to be disabled in `dolphin.sql_mode`.

### 3. Comparison Operators

```= <=> != <> < <= > >=```

Comparison rule description:

1. When a JSON type is compared with NULL or other types, the JSON type is greater.
Note: When a json string is compared with a json type, json type > json string, which is consistent with MySQL behavior. Example:

 ```
select `id`, `json1`, '{"num": 1, "name": "edf"}' as `jsonc`,
       `json1` > `jsonc` as `json1>jsonc`, `json1` >= `jsonc` as `json1>=jsonc`,
       `json1` < `jsonc` as `json1<jsonc`, `json1` <= `jsonc` as `json1<=jsonc`,
       `json1` != `jsonc` as `json1!=jsonc`, `json1` <> `jsonc` as `json1<>jsonc`,
       `json1` = `jsonc` as `json1=jsonc`, `json1` <=> `jsonc` as `json1<=>jsonc` from test_json_cmp_table;
 id  |           json1           |           jsonc           | json1>jsonc | json1>=jsonc | json1<jsonc | json1<=jsonc | json1!=jsonc | json1<>jsonc | json1=jsonc | json1<=>jsonc 
-----+---------------------------+---------------------------+-------------+--------------+-------------+--------------+--------------+--------------+-------------+---------------
   1 |                           | {"num": 1, "name": "edf"} |             |              |             |              |              |              |             | f
   2 | {"num": 1, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
   3 |                           | {"num": 1, "name": "edf"} |             |              |             |              |              |              |             | f
  41 | {"num": 1, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
  42 | {"num": 1, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 501 | {"num": 1, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 502 | {"num": 1, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 503 | {"num": 1, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 504 | {"num": 1, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 504 | {"num": 1, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 505 | {"nam": 2, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 506 | {"num": 1, "name": "edf"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 507 | {"num": 1, "name": "abc"} | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 508 | ["a","b","c"]             | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 509 | [1,2,"a"]                 | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 510 | [1,2,3]                   | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 511 | [1,2,3]                   | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
 512 | [1,2,3]                   | {"num": 1, "name": "edf"} | t           | t            | f           | f            | t            | t            | f           | f
(18 rows)
 ```

2. JSON type and JSON type comparison: first sort the JSON data by key, and after sorting, the comparison specification is the same as that of the JSONB type
   1) First compare the key values
   2) For identical key values, compare the value types: object-jsonb > array-jsonb > bool-jsonb > num-jsonb > str-jsonb > null-jsonb
   3) For values of the same type, compare the content:
      - str-json type: based on the text comparison method, use the database default collation for comparison. A positive return value indicates greater than, a negative value indicates less than, and 0 indicates equality.
      - num-json type: numeric comparison.
      - bool-json type: true > false.
      - array-jsonb type: longer length > shorter length; if lengths are equal, each element is compared in sequence.
      - object-jsonb type: longer length > shorter length; if lengths are equal, each key-value pair is compared in sequence, with keys compared first, followed by values.

## Compatibility Details

### Operations between JSON and the following data types are supported

- Numeric types: tinyint(**unsigned**), smallint(**unsigned**), integer(**unsigned**), bigint(**unsigned**), float4, float8, decimal/numeric, and bit.
- String types: char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, and text.
- Date and time types: date, datetime, timestamp, time, and year.
- Boolean type: boolean.

### Support Details

|Operator|Left Operand Type|Right Operand Type|
|--------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|+<br/>-<br/>*<br/>/|json|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean|
|div|json|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note: The json type is parsed as 0 in four arithmetic operations, therefore as the dividend the result is 0|
|^<br/>xor|json|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/><br/>Note: boolean is not supported. For example, json^boolean reports an error:<br/>ERROR:  failed to find conversion function from boolean to double precision|
|%|json|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note: binary, varbinary, tinyblob, blob, mediumblob, and longblob are not supported. For example, json%binary reports an error:<br/>ERROR:  failed to find conversion function from "binary" to numeric|
|mod|json|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note: The json type is parsed as 0 in the operation, therefore the mod result is 0|
|\>><br/><<|json|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric<br/>char, varchar, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note:<br/>1. binary, varbinary, tinyblob, blob, mediumblob, and longblob are not supported. For example, json>>binary reports an error:<br/>ERROR:  failed to find conversion function from "binary" to integer<br/>2. bit is not supported. For example, json>>bit reports an error:<br/>ERROR:  operator does not exist: double precision >> bit|
|\|<br/>&|json|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note:<br/>binary, varbinary, tinyblob, blob, mediumblob, and longblob are not supported. For example, json\|binary reports an error:<br/>ERROR:  failed to find conversion function from "binary" to bigint|
|\|\|<br/>&&<br/>and<br/>or|json|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, json, text<br/>date, time<br/>boolean<br/><br/>Note:<br/>1. enum, set, datetime, timestamp, year, binary, varbinary, tinyblob, blob, mediumblob, and longblob are not supported. For example, json\|\|binary reports an error:<br/>ERROR:  argument of OR must be type boolean, not type "binary"<br/>2. The \|\| operator requires that pipes_as_concat in dolphin.sql_mode is closed|
|=<br/><=><br/>!=<br/><><br/><<br/><=<br/>><br/>>=|json|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean|
|+<br/>-<br/>*<br/>/|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note: The json type is parsed as 0 in four arithmetic operations, therefore when used as the divisor an error is reported:<br/>ERROR:  division by zero|json|
|div|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note: The json type is parsed as 0 in four arithmetic operations, therefore when used as the divisor the result is NULL|json|
|^<br/>xor|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/><br/>Note: boolean is not supported. For example, boolean^json reports an error:<br/>ERROR:  failed to find conversion function from boolean to double precision|json|
|%|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note: binary, varbinary, tinyblob, blob, mediumblob, and longblob are not supported. For example, binary%json reports an error:<br/>ERROR:  failed to find conversion function from "binary" to numeric|json|
|mod|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, tinyblob, blob, mediumblob, longblob, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note: The json type is parsed as 0 in the operation, therefore the mod result is NULL|json|
|\>><br/><<|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note: binary, varbinary, tinyblob, blob, mediumblob, and longblob are not supported. For example, binary>>json reports an error:<br/>ERROR:  failed to find conversion function from "binary" to bigint|json|
|\|<br/>&|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, enum, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note:<br/>binary, varbinary, tinyblob, blob, mediumblob, and longblob are not supported. For example, binary\|json reports an error:<br/>ERROR:  failed to find conversion function from "binary" to bigint|json|
|\|\|<br/>&&<br/>and<br/>or|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, json, text<br/>date, time<br/>boolean<br/><br/>Note:<br/>1. enum, set, datetime, timestamp, year, binary, varbinary, tinyblob, blob, mediumblob, and longblob are not supported. For example, binary\|\|json reports an error:<br/>ERROR:  argument of OR must be type boolean, not type "binary"<br/>2. The \|\| operator requires that pipes_as_concat in dolphin.sql_mode is closed|json|
|=<br/><=><br/>!=<br/><><br/><<br/><=<br/>><br/>>=|tinyint(unsigned), smallint(unsigned), integer(unsigned), bigint(unsigned), float4, float8, decimal/numeric, bit<br/>char, varchar, binary, varbinary, set, json, text<br/>date, datetime, timestamp, time, year<br/>boolean<br/><br/>Note: enum, tinyblob, blob, mediumblob, and longblob are not supported. For example, blob>json reports an error:<br/>ERROR:  operator does not exist: blob > json<br/>HINT:No operator matches the given name and argument type(s). You might need to add explicit type casts.|json|
|+<br/>-<br/>~<br/>@|N/A|json|
|not|N/A|json|

### Other Restrictions

- The div, mod, and xor operators are not supported for json type and unknown type, and the error is as follows:

```
testdb_m=# select jsonval div NULL from testtb_m;
ERROR:  function pg_catalog.div(json, unknown) is not unique
LINE 1: select jsonval div NULL from testtb_m;
                       ^
HINT:  Could not choose a best candidate function. You might need to add explicit type casts.
CONTEXT:  referenced column: div

testdb_m=# select jsonval mod NULL from testtb_m;
ERROR:  function pg_catalog.b_mod(json, unknown) is not unique
LINE 1: select jsonval mod NULL from testtb_m;
                       ^
HINT:  Could not choose a best candidate function. You might need to add explicit type casts.
CONTEXT:  referenced column: b_mod

testdb_m=# select jsonval xor NULL from testtb_m;
ERROR:  function pg_catalog.xor(json, unknown) is not unique
LINE 1: select jsonval xor NULL from testtb_m;
                       ^
HINT:  Could not choose a best candidate function. You might need to add explicit type casts.
CONTEXT:  referenced column: xor
```