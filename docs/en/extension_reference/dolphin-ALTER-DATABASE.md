# ALTER DATABASE

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:21:37.627Z pushedAt=2026-09-12T04:33:21.132Z -->

## Function Description<a name="zh-cn_topic_0283136981_zh-cn_topic_0237122055_zh-cn_topic_0059779247_sbb9c79973fbf4b4b8f8e8355b0f67f63"></a>

Modifies the attributes of a database, including its name, owner, connection limit, object isolation attribute, and so on.

Modifies the attributes of a schema. It is interpreted as a schema only when the default character set and collation are being modified.

## Precautions<a name="zh-cn_topic_0283136981_zh-cn_topic_0237122055_zh-cn_topic_0059779247_sb8bbb55d049b42e688a2e152d2f6c737"></a>

Compared with the original openGauss, dolphin modifies the ALTER DATABASE syntax as follows:

- Adds the modifiable items [ [DEFAULT] CHARACTER SET | CHARSET [ = ] default_charset ] [ [DEFAULT] COLLATE [ = ] default_collation ].

## Syntax<a name="zh-cn_topic_0283136981_zh-cn_topic_0237122055_zh-cn_topic_0059779247_s2eca2e2a5fc04ac798bbdf1dce3e7303"></a>

- Modify the default character set and collation of a schema.

  ```
  ALTER DATABASE schema_name [ [DEFAULT] CHARACTER SET | CHARSET [ = ] default_charset ] [ [DEFAULT] COLLATE [ = ] default_collation ];
  ```

  [!NOTE]Note



  - In B-compatibility mode, this syntax is supported only when dolphin.b_compatibility_mode is set to on.
  - When this syntax is used, it is semantically equivalent to ALTER SCHEMA.

## Parameter Description<a name="zh-cn_topic_0283136981_zh-cn_topic_0237122055_zh-cn_topic_0059779247_s4d6b72484e3b43969af25757fda7ad81"></a>

- **schema_name**

  Name of the database whose attributes are to be modified.

  Value range: a string that complies with the naming conventions for identifiers.

- **[ [DEFAULT] CHARACTER SET | CHARSET [ = ] default_charset ]**

  Specifies the default character set of the schema. When specified alone, the default collation of the schema is set to the default collation of the specified character set.

- **[ [DEFAULT] COLLATE [ = ] default_collation ]**

  Specifies the default collation of the schema. When specified alone, the default character set of the schema is set to the character set corresponding to the specified collation.

## Examples<a name="zh-cn_topic_0283136981_zh-cn_topic_0237122055_zh-cn_topic_0059779247_sb089bcdb51bd4932a2967c246217d29e"></a>

See the [Examples](dolphin-CREATE-DATABASE.md#zh-cn_topic_0283137050_zh-cn_topic_0237122099_zh-cn_topic_0059778277_s6be7b8abbb4b4aceb9dae686434d672c) in CREATE DATABASE.

## Related Links<a name="zh-cn_topic_0283136981_zh-cn_topic_0237122055_zh-cn_topic_0059779247_saa1e5193215b4927989f304541d2ecbd"></a>

[CREATE DATABASE](dolphin-CREATE-DATABASE.md), [DROP DATABASE](dolphin-DROP-DATABASE.md), [ALTER DATABASE](dolphin-ALTER-DATABASE.md)