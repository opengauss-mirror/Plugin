# SQL MODE<a name="ZH-CN_TOPIC_0289900448"></a>

## 功能描述<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s86b6c9741c7741d3976c5e358e8d5486"></a>

1. 支持SQL mode参数设置
参数名称为sql_mode,设置范围为会话级，允许运行时候变更，允许全局变更与会话内变更。
参数值为逗号间隔的字符串，仅允许合法字符串设定，不合法情况下，启动后报warning。同样，设置时候，如果新值非法，则报warning并且不修改老值。当前sql_mode的默认字符串为sql_mode_strict,sql_mode_full_group。

2. 支持sql_mode_strict模式以及相关的四种特性场景。
(1. 插入不符合当前列类型的值时,进行数据转换;
分两种场景，insert into table values(…) 和insert into table select …
主要涉及到各种数据类型之间的互相转换，目前涉及的类型有tinyint（tinyint由于数据范围和mysql有差别，暂时不
考虑）,smallint,int,bigint,float,double,numeric,clob,char和varchar
(2. 插入的列值长度超过此列所限定的长度时,赋予该列最大或最小值（涉及的类型有tinyint,smallint,int,bigint,float,double,numeric,clob,char和
varchar）;
(3. insert时，属性是非空且没有默认值的列，且没有在insert的列表中，则为其添加默认值;（涉及的类型同上）
(4. 支持对属性是非空且没有默认值的列显式插入default;（涉及的类型同上）

3. 支持sql_mode_full_group模式。
主要是针对出现在select列表中的列（不使用聚合函数），是否一定要出现在group by子句中。当处在sql_mode_full_group模式（默认模式）下，如果select列表中的列没有使用聚合函数，也没有出现在group by子句，那么会报错，如果不在此模式下，则会执行成功，并在所有符合条件的元组中选取第一个元组。

## 示例<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_sfff14489321642278317cf06cd89810d"></a>

```
--创建表test1。
openGauss=# CREATE TABLE test1
(
  a1 smallint not null,
  a2 int not null,
  a3 bigint not null,
  a4 float not null,
  a5 double not null,
  a6 numeric not null,
  a7 varchar(5) not null
);

--向表中插入记录失败。
insert into test1(a1,a2) values(123412342342314,3453453453434324);
insert into test1(a1,a2,default,default,default,default,default);
--向表中插入记录成功。
set sql_mode = '';
insert into test1(a1,a2) values(123412342342314,3453453453434324);
insert into test1(a1,a2,default,default,default,default,default);

--查询表失败
select a1,a2 from test1 group by a1;
--查询表成功
select a1,a2 from test1 group by a1;

```
