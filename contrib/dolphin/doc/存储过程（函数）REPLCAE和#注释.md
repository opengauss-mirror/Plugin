# 兼容mysql语法兼容

## 功能描述<a name="zh-cn_topic_0283136653_zh-cn_topic_0237122119_section1163224811518"></a>
### 支持m* 创建（函数）中指定的特征部分的语法（仅语法）
- LANGUAGE SQL: 默认选项。用于说明存储过程语句块使用sql语言编写，将来会对其进行扩展，最有可能的第一个被支持的语言是php。
- [NOT] DETERMINISTIC：设置为DETERMINISTIC 表示存储过程对同样输入的参数产生相同的结果，设置为NOT DETERMINISTIC 表示产生不确定的结果。默认为NOT DETERMINISTIC。--这个确定性不会指定函数返回结果固定，而是告诉优化器，它结果固定，便于确定计划。
-  { CONTAINS SQL | NO SQL | READS SQL DATA | MODIFIES SQL DATA }: Contains sql,便是存储过程不包含读或者写数据的语句。No Sql,表示存储过程不包含sql 语句。Reads Sql Data 表示存储过程中包含读数据的语句，但不包含写数据的语句。Modifies Sql Data，表示存储过程包含写数据的语句。默认是 Contains Sql。
-  SQL SECURITY { DEFINER | INVOKER }：MySql对此存储过程的安全管理，默认为DEFINER,不做具体展开。
-  
#### m* 创建函数语法格式如下
```
CREATE
    [DEFINER = user]
    PROCEDURE [IF NOT EXISTS] sp_name ([proc_parameter[,...]])
    [characteristic ...] routine_body

CREATE
    [DEFINER = user]
    FUNCTION [IF NOT EXISTS] sp_name ([func_parameter[,...]])
    RETURNS type
    [characteristic ...] routine_body

proc_parameter:
    [ IN | OUT | INOUT ] param_name type

func_parameter:
    param_name type

type:
    Any valid MySQL data type

characteristic: {
    COMMENT 'string'
  | LANGUAGE SQL
  | [NOT] DETERMINISTIC
  | { CONTAINS SQL | NO SQL | READS SQL DATA | MODIFIES SQL DATA }
  | SQL SECURITY { DEFINER | INVOKER }
}

routine_body:
    Valid SQL routine statement
```

### 支持m* 修改存储过程（函数）中指定的特征部分的语法（仅语法）

- 支持LANGUAGE SQL
- 支持 { CONTAINS SQL | NO SQL | READS SQL DATA | MODIFIES SQL DATA }
- 支持 SQL SECURITY { DEFINER | INVOKER }
#### m*修改存储过程语法格式如下
```
ALTER FUNCTION func_name [characteristic ...]

characteristic: {
    COMMENT 'string'
  | LANGUAGE SQL
  | { CONTAINS SQL | NO SQL | READS SQL DATA | MODIFIES SQL DATA }
  | SQL SECURITY { DEFINER | INVOKER }
}
```
**注意：** 

  Gauss 中存储过程名有重载，所以修改时必须要带着参数，这个属于数据库处理方式差异，不做兼容，保留重载功能。

  由于M* 中允许类似 NO SQL NO SQL 这样两个相同的选项，但是opengauss 中不允许此类情况，所以本次兼容内容中，仅SQL SECURITY { DEFINER | INVOKER } 部分兼容open gauss 原始处理方式，及重复报错的情况。其余内容均兼容M* 的允许重复的方式。

### 支持m* Replace语法（仅语法）
**注意：** 

  由于Replace 语法本质上是Mysql对标准SQL中Insert 的扩展，所以本次语法兼容中，Replace 复用了部分Insert 处理的逻辑，支持了部分的功能，但本质上只是为使代码符合逻辑而加入的内容，实际目的是为让语法通过并有返回的结果。
- 支持除 row_constructor_list外的语法
  
#### m* Replace法格式如下
```
REPLACE [LOW_PRIORITY | DELAYED]
    [INTO] tbl_name
    [PARTITION (partition_name [, partition_name] ...)]
    [(col_name [, col_name] ...)]
    { {VALUES | VALUE} (value_list) [, (value_list)] ...
      |
      VALUES row_constructor_list
    }

REPLACE [LOW_PRIORITY | DELAYED]
    [INTO] tbl_name
    [PARTITION (partition_name [, partition_name] ...)]
    SET assignment_list

REPLACE [LOW_PRIORITY | DELAYED]
    [INTO] tbl_name
    [PARTITION (partition_name [, partition_name] ...)]
    [(col_name [, col_name] ...)]
    {SELECT ... | TABLE table_name}

value:
    {expr | DEFAULT}

value_list:
    value [, value] ...

row_constructor_list:
    ROW(value_list)[, ROW(value_list)][, ...]

assignment:
    col_name = value

assignment_list:
    assignment [, assignment] .
```
### 支持mysql 注释写法语法（# xxxx）
- 支持以“#” 开头的单行注释，注释的规范同原始支持的单行注释方法—— "--" 相同。
	```
	目前实现为：在b 类型数据库中，开启b_format_mod 参数时，将#作为注释的开始。
    原始含义中#是作为异或操作符或者 ident 中的一个字符（不为开始）
	```
**注意：** 
  
由于插件中无法融入psql 代码，所以在客户端中，没有办法将# 视为注释的开始，因此也带来如下问题：
```
若写为： select 1 --asdasd;
客户端会等待用户输入终结符 
而 select 1 #asdasd;
客户端仍然认为;是一个终结符，则会发送sql。
```
### 支持mysql call 存储过程省略括号语法

- 支持 CALL sp_name ; 调用不含有参数的存储过程或者函数。
## 示例
- 示例1： 创建函数
    ```
    --指定 CONTAINS SQL
    CREATE FUNCTION f1 (s CHAR(20)) RETURNS int  CONTAINS SQL AS $$ select 1 $$ ;

    --指定 DETERMINISTIC
    CREATE FUNCTION f2 (s int) RETURNS int CONTAINS SQL DETERMINISTIC  AS $$ select s; $$ ;

    --指定 LANGUAGE SQL
    CREATE FUNCTION f3 (s int) RETURNS int  CONTAINS SQL LANGUAGE SQL AS $$ select s; $$ ;

    --指定 NO SQL
    CREATE FUNCTION f4 (s int) RETURNS int  NO SQL AS $$ select s; $$ ;

    --指定  READS SQL DATA
    CREATE FUNCTION f5 (s int) RETURNS int CONTAINS SQL  READS SQL DATA  AS $$ select s; $$ ;

    --指定 MODIFIES SQL DATA
    CREATE FUNCTION f6 (s int) RETURNS int  CONTAINS SQL LANGUAGE SQL NO SQL  MODIFIES SQL DATA AS $$ select s; $$ ;

    --指定 SECURITY DEFINER
    CREATE FUNCTION f7 (s int) RETURNS int NO SQL SQL SECURITY DEFINER AS $$ select s; $$ ;

    --指定 SECURITY INVOKER
    CREATE FUNCTION f8 (s int) RETURNS int  SQL SECURITY INVOKER  READS SQL DATA LANGUAGE SQL AS $$ select s; $$ ;

    --指定 默认LANGUGAE SQL
    CREATE FUNCTION f9 (s int) RETURNS int AS $$ select s; $$ ;

    ```
- 示例2：修改函数
  ```
    --指定 NO SQL
    ALTER FUNCTION f1 (s char(20)) NO SQL;

    --指定 CONTAINS SQL
    ALTER FUNCTION f1 (s char(20)) CONTAINS SQL;

    --指定 LANGUAGE SQL
    ALTER FUNCTION f1 (s char(20)) LANGUAGE SQL ;

    --指定 MODIFIES SQL DATA
    ALTER FUNCTION f1 (s char(20)) MODIFIES SQL DATA;

    --指定  READS SQL DATA
    ALTER FUNCTION f1 (s char(20)) READS SQL DATA;

    --指定 SECURITY INVOKER
    ALTER FUNCTION f1 (s char(20)) SQL SECURITY INVOKER;

    --指定 SECURITY DEFINER
    ALTER FUNCTION f1 (s char(20)) SQL SECURITY DEFINER;

    -- error 不允许修改
    ALTER FUNCTION f1 (s char(20)) NOT DETERMINISTIC;
  ```
- 示例3：Replace 
    ```
    create table parts (id int) partition by range(id)
    ( partition p1 values less than(10),
    partition p2 values less than(20));

    Replace t1 table t2;
    Replace into t1 table t2;
    Replace LOW_PRIORITY into t1 table t2;
    Replace DELAYED into t1 table t2;
    Replace DELAYED t1 table t2;
    Replace t1 table t2;
    Replace t1 values (3);
    Replace t1 (a) values (3),(4);
    Replace t1 (a) select a from t2;
    Replace into parts partition(p1) table t2;
    Replace DELAYED into parts partition(p1) values(4);
    ```

- 示例4：Call sp_name
    ```
    CREATE FUNCTION f1 () RETURNS int  no SQL AS $$ select 1 $$ ;
    call f1;

    ```

- 示例5：#注释
  ```
    #123123123
    select 1;
  ```