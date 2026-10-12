# CALL

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:22:00.705Z pushedAt=2026-09-24T10:02:59.988Z -->

## Function Description<a name="zh-cn_topic_0283137636_zh-cn_topic_0237122088_zh-cn_topic_0059778236_s17e49a7670334c61978f059adf2cb65d"></a>

The CALL command is used to invoke defined functions and stored procedures.

## Notes<a name="zh-cn_topic_0283137636_zh-cn_topic_0237122088_zh-cn_topic_0059778236_sdf7f29bdebc44178a9581f41f4257f09"></a>

Compared with the original openGauss, the modifications made by dolphin to the CALL syntax are as follows:

1. The CALL syntax can be used to invoke stored procedures that contain query statements.

2. MySQL 5.7.x client tools. For example, mysql-connector-java-5.1.47 can use the CALL syntax to invoke stored procedures that contain query statements.

## Syntax<a name="zh-cn_topic_0283137636_zh-cn_topic_0237122088_zh-cn_topic_0059778236_sdf8eb47ae3d945fea7582a7753cdd985"></a>

```
CALL [schema.|package.] {func_name| procedure_name} ( param_expr );
```

## Parameter Description<a name="zh-cn_topic_0283137636_zh-cn_topic_0237122088_zh-cn_topic_0059778236_sf183d9684eb54414b8f5c370a1c7038b"></a>

- **schema**

    The name of the schema where the function or stored procedure resides.

- **package**

    The name of the package where the function or stored procedure resides.

- **func\_name**

    The name of the function or stored procedure to be called.

    Value range: an existing function name.

- **param\_expr**

    In the parameter list, the parameter name and parameter value can be separated by the symbol ":=" or "=\>". This approach allows parameters to be arranged in any order. If only parameter values appear in the parameter list, the order of the parameter values must be the same as that defined in the function or stored procedure.

    Value range: names of existing function parameters or stored procedure parameters.

    >[!NOTE] Note
    >When the parameter dolphin.sql_mode is set to 'block_return_multi_results' and CALL is used to invoke a stored procedure or function, the following restrictions apply:
    - User-defined variables must be used as output parameters.
    - Only stored procedures written in PL/pgSQL language are supported.

## Examples<a name="zh-cn_topic_0283137636_zh-cn_topic_0237122088_zh-cn_topic_0059778236_s299dc001fa4b48cd9b56412a73db23c0"></a>

```
---- Set the parameter.
openGauss=# set dolphin.sql_mode = 'block_return_multi_results';

---- Create a stored procedure that returns results of multiple query statements.
openGauss=# CREATE   PROCEDURE proc_a_2 ()  as 
begin
 select * from t;
 select * from test_1;
end;
/

---- Call the stored procedure.
openGauss=# call proc_a_2();

```

## Related Links<a name="zh-cn_topic_0283136646_zh-cn_topic_0237122110_zh-cn_topic_0059778640_sfe39b39f278f4933914a438f40c63954"></a>

[CALL](../sql_reference/call.md)