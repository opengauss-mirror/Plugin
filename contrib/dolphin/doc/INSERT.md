# INSERT<a name="ZH-CN_TOPIC_0289900448"></a>

## 功能描述<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s86b6c9741c7741d3976c5e358e8d5486"></a>

向表中添加一行或多行数据。

## 注意事项<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_sdd2da7fe44624eb99ee77013ff96c6bd"></a>

-   只有拥有表INSERT权限的用户，才可以向表中插入数据。用户被授予insert any table权限，相当于用户对除系统模式之外的任何模式具有USAGE权限，并且拥有这些模式下表的INSERT权限
-   如果使用RETURNING子句，用户必须要有该表的SELECT权限。
-   如果使用ON DUPLICATE KEY UPDATE，用户必须要有该表的SELECT、UPDATE权限，唯一约束（主键或唯一索引）的SELECT权限。
-   如果使用query子句插入来自查询里的数据行，用户还需要拥有在查询里使用的表的SELECT权限。
-   当连接到TD兼容的数据库时，td\_compatible\_truncation参数设置为on时，将启用超长字符串自动截断功能，在后续的insert语句中（不包含外表的场景下），对目标表中char和varchar类型的列上插入超长字符串时，系统会自动按照目标表中相应列定义的最大长度对超长字符串进行截断。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >如果向字符集为字节类型编码（SQL\_ASCII，LATIN1等）的数据库中插入多字节字符数据（如汉字等），且字符数据跨越截断位置，这种情况下，按照字节长度自动截断，自动截断后会在尾部产生非预期结果。如果用户有对于截断结果正确性的要求，建议用户采用UTF8等能够按照字符截断的输入字符集作为数据库的编码集。


## 语法格式<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_se242be9719f44731b261539dbd42d7b9"></a>

```
[ WITH [ RECURSIVE ] with_query [, ...] ]
INSERT [/*+ plan_hint */] [IGNORE] [INTO] table_name [partition_clause] [ AS alias ] [ ( column_name [, ...] ) ]
    { DEFAULT VALUES
    | [ VALUES | VALUE ] [{( { expression | DEFAULT } [, ...] ) }][, ...]
    | query }
    [ ON DUPLICATE KEY UPDATE { NOTHING | { column_name = { expression | DEFAULT } } [, ...] [ WHERE condition ] }]
    [ RETURNING {* | {output_expression [ [ AS ] output_name ] }[, ...]} ];
```

## 参数说明<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s06dfa4f09bfd4e0d9826a80e6a91b0a6"></a>

- **WITH \[ RECURSIVE \] with\_query \[, ...\]**

  用于声明一个或多个可以在主查询中通过名称引用的子查询，相当于临时表。

  如果声明了RECURSIVE，那么允许SELECT子查询通过名称引用它自己。

  - 其中with\_query的详细格式为：

    ```
    with_query_name [ ( column_name [, ...] ) ] AS [ [ NOT ] MATERIALIZED ]
    ( {select | values | insert | update | delete} )
    ```

    – with\_query\_name指定子查询生成的结果集名称，在查询中可使用该名称访问子查询的结果集。

    – column\_name指定子查询结果集中显示的列名。

    – 每个子查询可以是SELECT，VALUES，INSERT，UPDATE或DELETE语句。

    – 用户可以使用MATERIALIZED / NOT MATERIALIZED对CTE进行修饰。

    -   如果声明为MATERIALIZED，WITH查询将被物化，生成一个子查询结果集的拷贝，在引用处直接查询该拷贝，因此WITH子查询无法和主干SELECT语句进行联合优化（如谓词下推、等价类传递等），对于此类场景可以使用NOT MATERIALIZED进行修饰，如果WITH查询语义上可以作为子查询内联执行，则可以进行上述优化。
    -   如果用户没有显示声明物化属性则遵守以下规则：如果CTE只在所属主干语句中被引用一次，且语义上支持内联执行，则会被改写为子查询内联执行，否则以CTE Scan的方式物化执行。

    >![](public_sys-resources/icon-note.gif) **说明：** 
    >INSERT ON DUPLICATE KEY UPDATE不支持WITH及WITH RECURSIVE子句。

  - **plan\_hint子句**

    以/\*+ \*/的形式在INSERT关键字后，用于对INSERT对应的语句块生成的计划进行hint调优，详细用法请参见章节[使用Plan Hint进行调优](zh-cn_topic_0289900289.md)。每条语句中只有第一个/\*+ plan\_hint \*/注释块会作为hint生效，里面可以写多条hint。

- **IGNORE**
  
  若带有IGNORE关键字的INSERT语句执行时在指定场景引发了Error，则会将Error降级为Warning，且继续语句的执行，不会影响其他数据的操作。能使Error降级的场景有：

  1.违反非空约束时：

  若执行的SQL语句违反了表的非空约束，使用此hint可将Error降级为Warning，并根据GUC参数sql_ignore_strategy的值采用以下策略的一种继续执行：

    -   sql_ignore_startegy为ignore_null时，忽略违反非空约束的行的INSERT操作，并继续执行剩余数据操作。

    -   sql_ignore_startegy为overwrite_null时，将违反约束的null值覆写为目标类型的默认值，并继续执行剩余数据操作。

        >![](public_sys-resources/icon-note.gif) **说明：**
      >GUC参数sql_ignore_strategy为枚举类型，可选值有：ignore_null, overwrite_null

  2.违反唯一约束时：

  若执行的SQL语句违反了表的唯一约束，使用此hint可将Error降级为Warning，忽略违反约束的行的INSERT操作，并继续执行剩余数据操作。

  3.分区表无法匹配到合法分区时

  在对分区表进行INSERT操作时，若某行数据无法匹配到表格的合法分区，使用此hint可将Error降级为Warning，忽略该行操作，并继续执行剩余数据操作。

  4.插入值向目标列类型转换失败时：

  执行INSERT语句时，若发现新值与目标列类型不匹配，使用此hint可将Error降级为Warning，并根据新值与目标列的具体类型采取以下策略的一种继续执行：

    -   当新值类型与列类型同为数值类型时：

        若新值在列类型的范围内，则直接进行插入；若新值在列类型范围外，则以列类型的最大/最小值替代。

    -   当新值类型与列类型同为字符串类型时：

        若新值长度在列类型限定范围内，则以直接进行插入；若新值长度在列类型的限定范围外，则保留列类型长度限制的前n个字符。

    -   若遇到新值类型与列类型不可转换时：

        插入列类型的默认值。

-   **table\_name**

    要插入数据的目标表名。

    取值范围：已存在的表名。

- **partition\_clause**

  指定分区插入操作

  ```
  PARTITION { ( partition_name ) | FOR ( partition_value [, ...] ) } |
  SUBPARTITION { ( subpartition_name ) | FOR ( subpartition_value [, ...] ) }
  ```

  关键字详见[SELECT](SELECT.md)一节介绍

  如果value子句的值和指定分区不一致，会抛出异常。

  示例详见[CREATE TABLE SUBPARTITION](zh-cn_topic_0000001198046401.md)

-   **column\_name**

    目标表中的字段名：

    -   字段名可以有子字段名或者数组下标修饰。
    -   没有在字段列表中出现的每个字段，将由系统默认值，或者声明时的默认值填充，若都没有则用NULL填充。例如，向一个复合类型中的某些字段插入数据的话，其他字段将是NULL。
    -   目标字段（column\_name）可以按顺序排列。如果没有列出任何字段，则默认全部字段，且顺序为表声明时的顺序。
    -   如果value子句和query中只提供了N个字段，则目标字段为前N个字段。
    -   value子句和query提供的值在表中从左到右关联到对应列。

    取值范围：已存在的字段名。

-   **expression**

    赋予对应column的一个有效表达式或值：

    -   如果是INSERT ON DUPLICATE KEY UPDATE语句下，expression可以为VALUES\(column\_name\)或EXCLUDED.column\_name用来表示引用冲突行对应的column\_name字段的值。需注意，其中VALUES\(column\_name\)不支持嵌套在表达式中（例如VALUES\(column\_name\)+1），但EXCLUDED不受此限制。

    -   向表中字段插入单引号 “ ' ”时需要使用单引号自身进行转义。
    -   如果插入行的表达式不是正确的数据类型，系统试图进行类型转换，若转换不成功，则插入数据失败，系统返回错误信息。

-   **DEFAULT**

    对应字段名的缺省值。如果没有缺省值，则为NULL。

-   **VALUES()**

    当GUC参数sql_mode为stric_all_tables时，为所有列插入NULL，否则如果对应字段名有缺省值，插入缺省值，如果没有缺省值，判断对应字段是否有not_null约束，有插入NULL，没有则插入类型基础值，具体基础值参考视图pg_type_basic_value。

-   **query**

    一个查询语句（SELECT语句），将查询结果作为插入的数据。

-   **RETURNING**

    返回实际插入的行，RETURNING列表的语法与SELECT的输出列表一致。注意：INSERT ON DUPLICATE KEY UPDATE不支持RETURNING子句。

-   **output\_expression**

    INSERT命令在每一行都被插入之后用于计算输出结果的表达式。

    取值范围：该表达式可以使用table的任意字段。可以使用\*返回被插入行的所有字段。

-   **output\_name**

    字段的输出名称。

    取值范围：字符串，符合标识符命名规范。

-   **ON DUPLICATE KEY UPDATE**

    对于带有唯一约束（UNIQUE INDEX或PRIMARY KEY）的表，如果插入数据违反唯一约束，则对冲突行执行UPDATE子句完成更新，对于不带唯一约束的表，则仅执行插入。UPDATE时，若指定NOTHING则忽略此条插入，可通过“EXCLUDE.” 或者 "VALUES\(\)" 来选择源数据相应的列。

    -   支持触发器，触发器执行顺序由实际执行流程决定：
        -   执行insert：触发 before insert、after insert触发器。
        -   执行update：触发before insert、before update、after update触发器。
        -   执行update nothing：触发before insert触发器。

    -   不支持延迟生效（DEFERRABLE）的唯一约束或主键。

    -   如果表中存在多个唯一约束，如果所插入数据违反多个唯一约束，对于检测到冲突的第一行进行更新，其他冲突行不更新（检查顺序与索引维护具有强相关性，一般先创建的索引先进行冲突检查）。
    -   如果插入多行，这些行均与表中同一行数据存在唯一约束冲突，则按照顺序，第一条执行插入或更新，之后依次执行更新。

    -   主键、唯一索引列不允许UPDATE。
    -   不支持列存，不支持外表、内存表。
    -   expression支持使用子查询表达式，其语法与功能同UPDATE。子查询表达式中支持使用“EXCLUDED.”来选择源数据相应的列。


## 示例<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_sfff14489321642278317cf06cd89810d"></a>

```
--创建表tpcds.reason_t2。
openGauss=# CREATE TABLE tpcds.reason_t2
(
  r_reason_sk    integer,
  r_reason_id    character(16),
  r_reason_desc  character(100)
);

--向表中插入一条记录。
openGauss=# INSERT INTO tpcds.reason_t2(r_reason_sk, r_reason_id, r_reason_desc) VALUES (1, 'AAAAAAAABAAAAAAA', 'reason1');

--向表中插入一条记录，和上一条语法等效。
openGauss=# INSERT INTO tpcds.reason_t2 VALUES (2, 'AAAAAAAABAAAAAAA', 'reason2');

--向表中插入多条记录。
openGauss=# INSERT INTO tpcds.reason_t2 VALUES (3, 'AAAAAAAACAAAAAAA','reason3'),(4, 'AAAAAAAADAAAAAAA', 'reason4'),(5, 'AAAAAAAAEAAAAAAA','reason5');

--向表中插入tpcds.reason中r_reason_sk小于5的记录。
openGauss=# INSERT INTO tpcds.reason_t2 SELECT * FROM tpcds.reason WHERE r_reason_sk <5;

--对表创建唯一索引
openGauss=# CREATE UNIQUE INDEX reason_t2_u_index ON tpcds.reason_t2(r_reason_sk);

--向表中插入多条记录，如果冲突则更新冲突数据行中r_reason_id字段为'BBBBBBBBCAAAAAAA'。
openGauss=# INSERT INTO tpcds.reason_t2 VALUES (5, 'BBBBBBBBCAAAAAAA','reason5'),(6, 'AAAAAAAADAAAAAAA', 'reason6') ON DUPLICATE KEY UPDATE r_reason_id = 'BBBBBBBBCAAAAAAA';
 
--删除表tpcds.reason_t2。
openGauss=# DROP TABLE tpcds.reason_t2;
```

## 优化建议<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_section3855297014560"></a>

-   VALUES

    通过insert语句批量插入数据时，建议将多条记录合并入一条语句中执行插入，以提高数据加载性能。例如，INSERT INTO sections VALUES \(30, 'Administration', 31, 1900\)、\(40, 'Development', 35, 2000\)、 \(50, 'Development' , 60 , 2001\)。

