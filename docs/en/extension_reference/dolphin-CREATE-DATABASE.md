# CREATE DATABASE

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:22:10.609Z pushedAt=2026-09-12T04:38:06.065Z -->

## Feature Description<a name="zh-cn_topic_0283137050_zh-cn_topic_0237122099_zh-cn_topic_0059778277_s3ea6af3a84d74f1ab7dceb8bb54ed134"></a>

A new database is created. By default, the new database is created by copying the standard system database template0, and only template0 is supported for creation.

A new schema is created. The default character set and collation of the schema can be specified.

## Notes<a name="zh-cn_topic_0283137050_zh-cn_topic_0237122099_zh-cn_topic_0059778277_s818d4df5d095482f86d8e7258a75df1b"></a>

Compared with the original openGauss, the modifications made by dolphin to the CREATE DATABASE syntax are as follows:

- The modifiable items [ [DEFAULT] CHARACTER SET | CHARSET [ = ] default_charset ] [ [DEFAULT] COLLATE [ = ] default_collation ] are added.

## Syntax<a name="zh-cn_topic_0283137050_zh-cn_topic_0237122099_zh-cn_topic_0059778277_s819ed4de9ed04006954df8016e5e4858"></a>

```
CREATE DATABASE [IF NOT EXISTS] database_name
               [ [DEFAULT] CHARACTER SET | CHARSET [ = ] default_charset ] [ [DEFAULT] COLLATE [ = ] default_collation ];
```

## Parameter Description<a name="zh-cn_topic_0283137050_zh-cn_topic_0237122099_zh-cn_topic_0059778277_s1d6127a393bf4f6d8fdac63105932d16"></a>

- **database\_name**

  Database name.

  Value range: a string that complies with the naming conventions for identifiers.

- **[ [DEFAULT] CHARACTER SET | CHARSET [ = ] default_charset ]**

  Specifies the default character set for the schema. When specified alone, the default collation of the schema is set to the default collation of the specified character set.

- **[ [DEFAULT] COLLATE [ = ] default_collation ]**

  Specifies the default collation for the schema. When specified alone, the default character set of the schema is set to the character set corresponding to the specified collation.

  [!NOTE]Note

  - Under B compatibility, this syntax is supported only when dolphin.b_compatibility_mode is set to on.
  - When this syntax is used, it is equivalent to CREATE SCHEMA, meaning that a SCHEMA is actually created, and database\_name serves as the SCHEMA name.
  - Under B compatibility, when dolphin.b_compatibility_mode is set to on, if default_charset and default_collation are not specified but other CREATE DATABASE syntax options are specified, the syntax remains as the CREATE DATABASE syntax.
  - Under B compatibility, when dolphin.b_compatibility_mode is set to on and no options are specified, the syntax is equivalent to CREATE SCHEMA; when dolphin.b_compatibility_mode is set to off, the syntax is equivalent to CREATE DATABASE.
  - Under B compatibility, the b_format_behavior_compat_options parameter is configured with 'default_c
ollation' by default. For databases installed via OM, where Encoding=SQL_ASCII, Collate=C, and Ctype=C, when 'default_collation' is configured, specifying the varchar type during table creation results in the actual storage type being varbinary. Therefore, for MY compatibility, it is recommended to specify UTF8 encoding when creating a B-compatible database, that is, `create database testb with dbcompatibility = 'b' encoding 'UTF8';`.

## Examples<a name="zh-cn_topic_0283137050_zh-cn_topic_0237122099_zh-cn_topic_0059778277_s6be7b8abbb4b4aceb9dae686434d672c"></a>

```
-- Enable the dolphin.b_compatibility_mode switch.
openGauss=# set dolphin.b_compatibility_mode = on;
SET
openGauss=# create database test1;
CREATE SCHEMA
openGauss=# create database test2 charset 'utf8';
CREATE SCHEMA
openGauss=# drop database if exists test1;
```

## Related Links<a name="zh-cn_topic_0283137050_zh-cn_topic_0237122099_zh-cn_topic_0059778277_s4693856e1f6240dc98de7d6faf52f136"></a>

[ALTER DATABASE](dolphin-ALTER-DATABASE.md), [DROP DATABASE](dolphin-DROP-DATABASE.md),  [CREATE DATABASE](../sql_reference/create_database.md)