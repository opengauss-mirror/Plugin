# UPDATE<a name="ZH-CN_TOPIC_0289900207"></a>

## 功能描述<a name="zh-cn_topic_0283137651_zh-cn_topic_0237122194_zh-cn_topic_0059778969_s85747c5f88e64562a8ff9ddacda19929"></a>

更新表中的数据。UPDATE修改满足条件的所有行中指定的字段值，WHERE子句声明条件，SET子句指定的字段会被修改，没有出现的字段则保持它们的原值。

## 注意事项<a name="zh-cn_topic_0283137651_zh-cn_topic_0237122194_zh-cn_topic_0059778969_s7e9e912f472543cbb190edb83e5f22d2"></a>

-   表的所有者、拥有表UPDATE权限的用户或拥有UPDATE ANY TABLE权限的用户，有权更新表中的数据，系统管理员默认拥有此权限。
-   对expression或condition条件里涉及到的任何表要有SELECT权限。
-   对于列存表，暂时不支持RETURNING子句。
-   列存表不支持结果不确定的更新\(non-deterministic update\)。试图对列存表用多行数据更新一行时会报错。
-   列存表的更新操作，旧记录空间不会回收，需要执行VACUUM FULL table\_name进行清理。
-   对于列存复制表，暂不支持UPDATE操作。

## 语法格式<a name="zh-cn_topic_0283137651_zh-cn_topic_0237122194_zh-cn_topic_0059778969_sd8d9ff15ff6c45c9aebd16c861936c06"></a>

```
UPDATE [/*+ plan_hint */] [IGNORE] [ ONLY ] table_name [ * ] [ [ AS ] alias ]
SET {column_name = { expression | DEFAULT } 
    |( column_name [, ...] ) = {( { expression | DEFAULT } [, ...] ) |sub_query }}[, ...]
    [ FROM from_list] [ WHERE condition ]
    [ RETURNING {* 
                | {output_expression [ [ AS ] output_name ]} [, ...] }];

where sub_query can be:
SELECT [ ALL | DISTINCT [ ON ( expression [, ...] ) ] ]
{ * | {expression [ [ AS ] output_name ]} [, ...] }
[ FROM from_item [, ...] ]
[ WHERE condition ]
[ GROUP BY grouping_element [, ...] ]
[ HAVING condition [, ...] ]
```

## 参数说明<a name="zh-cn_topic_0283137651_zh-cn_topic_0237122194_zh-cn_topic_0059778969_sf3e3262b89854b3d829a94054116838c"></a>

-   **plan\_hint子句**

    以/\*+ \*/的形式在UPDATE关键字后，用于对UPDATE对应的语句块生成的计划进行hint调优，详细用法请参见章节[使用Plan Hint进行调优](使用Plan-Hint进行调优.md)。每条语句中只有第一个/\*+ plan\_hint \*/注释块会作为hint生效，里面可以写多条hint。

- **IGNORE**
  
  若带有IGNORE关键字的UPDATE语句执行时在指定场景引发了Error，则会将Error降级为Warning，且继续语句的执行，不会影响其他数据的操作。能使Error降级的场景有：

  1.违反非空约束时：

  若执行的SQL语句违反了表的非空约束，使用此hint可将Error降级为Warning，并根据GUC参数sql_ignore_strategy的值采用以下策略的一种继续执行：

    -   sql_ignore_startegy为ignore_null时，忽略违反非空约束的行的UPDATE操作，并继续执行剩余数据操作。

    -   sql_ignore_startegy为overwrite_null时，将违反约束的null值覆写为目标类型的默认值，并继续执行剩余数据操作。

        >![](public_sys-resources/icon-note.gif) **说明：**
      >GUC参数sql_ignore_strategy为枚举类型，可选值有：ignore_null, overwrite_null

  2.违反唯一约束时：

  若执行的SQL语句违反了表的唯一约束，使用此hint可将Error降级为Warning，忽略违反约束的行的UPDATE操作，并继续执行剩余数据操作。

  3.分区表无法匹配到合法分区时

  在对分区表进行UPDATE操作时，若某行数据无法匹配到表格的合法分区，使用此hint可将Error降级为Warning，忽略该行操作，并继续执行剩余数据操作。

  4.更新值向目标列类型转换失败时：

  执行UPDATE语句时，若发现新值与目标列类型不匹配，使用此hint可将Error降级为Warning，并根据新值与目标列的具体类型采取以下策略的一种继续执行：

    -   当新值类型与列类型同为数值类型时：

        若新值在列类型的范围内，则直接进行更新；若新值在列类型范围外，则以列类型的最大/最小值替代。

    -   当新值类型与列类型同为字符串类型时：

        若新值长度在列类型限定范围内，则以直接进行更新；若新值长度在列类型的限定范围外，则保留列类型长度限制的前n个字符。

    -   若遇到新值类型与列类型不可转换时：

        更新为列类型的默认值。

  IGNORE关键字不支持列存，无法在列存表中生效。
  
-   **table\_name**

    要更新的表名，可以使用模式修饰。

    取值范围：已存在的表名称。

- **partition\_clause**

  指定分区更新操作

  PARTITION \{ \( partition\_name \) | FOR \( partition\_value \[, ...\] \) \} |

  SUBPARTITION \{ \( subpartition\_name \) | FOR \( subpartition\_value \[, ...\] \) \}

  关键字详见[SELECT](SELECT.md)一节介绍

  示例详见[CREATE TABLE SUBPARTITION](zh-cn_topic_0000001198046401.md)

-   **alias**

    目标表的别名。

    取值范围：字符串，符合标识符命名规范。

-   **column\_name**

    要修改的字段名。

    支持使用目标表的别名加字段名来引用这个字段。例如：

    UPDATE foo AS f SET f.col\_name = 'namecol';

    取值范围：已存在的字段名。

-   **expression**

    赋给字段的值或表达式。

-   **DEFAULT**

    用对应字段的缺省值填充该字段。

    如果没有缺省值，则为NULL。

-   **sub\_query**

    子查询。

    使用同一数据库里其他表的信息来更新一个表可以使用子查询的方法。其中SELECT子句具体介绍请参考[SELECT](SELECT.md)。

-   **from\_list**

    一个表的表达式列表，允许在WHERE条件里使用其他表的字段。与在一个SELECT语句的FROM子句里声明表列表类似。

    >![](public_sys-resources/icon-notice.gif) **须知：** 
    >目标表绝对不能出现在from\_list里，除非在使用一个自连接（此时它必须以from\_list的别名出现）。

-   **condition**

    一个返回Boolean类型结果的表达式。只有这个表达式返回true的行才会被更新。不建议使用int等数值类型作为condition，因为int等数值类型可以隐式转换为bool值（非0值隐式转换为true，0转换为false），可能导致非预期的结果。

-   **output\_expression**

    在所有需要更新的行都被更新之后，UPDATE命令用于计算返回值的表达式。

    取值范围：使用任何table以及FROM中列出的表的字段。\*表示返回所有字段。

-   **output\_name**

    字段的返回名称。


## 示例<a name="zh-cn_topic_0283137651_zh-cn_topic_0237122194_zh-cn_topic_0059778969_s23d933f56bc745e1bd819083b4e50155"></a>

```
--创建表student1。
openGauss=# CREATE TABLE student1
(
   stuno     int,
   classno   int 
);

--插入数据。
openGauss=# INSERT INTO student1 VALUES(1,1);
openGauss=# INSERT INTO student1 VALUES(2,2);
openGauss=# INSERT INTO student1 VALUES(3,3);

--查看数据。
openGauss=# SELECT * FROM student1;

--直接更新所有记录的值。
openGauss=# UPDATE student1 SET classno = classno*2;

--查看数据。
openGauss=# SELECT * FROM student1;

--删除表。
openGauss=# DROP TABLE student1;
```

