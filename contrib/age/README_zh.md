# Apache AGE (incubating) for openGauss

## 整体介绍
Apache AGE(https://age.apache.org/)是基于PostgreSQL开发的图数据库引擎，AGE的所有组件都运行在PostgreSQL事务缓存层和存储层之上，
AGE实现了一个存储引擎同时处理关系型和图数据模型，用户可使用标准的ANSI SQL和图查询语言openCypher对数据进行查询。
                ![AGE-think.png](doc%2Fimg%2FAGE-think.png#pic_center)  
AGE 在数据库内核的查询解析，查询重写，查询计划，查询执行，数据存储均有涉及，数据存储方面定义了图数据库的存储模型，
在其他方面使用数据库内核的hook点，对Cypher语言进行支持，实现了同时处理关系型和图数据的能力。

## 安装
### 安装步骤
>>前提条件：openGauss正常编译安装，并且配置到了环境变量中
>> 在age的源码目录执行命令
```shell
make install
```
### 成功示例
![install_example.jpg](doc%2Fimg%2Finstall_example.jpg)


## 使用
### 连接openGauss

### 创建插件
* 执行命令
```sql
create extension age;
```
* 示例
```sql
openGauss=# create extension age;
CREATE EXTENSION
```
### 加载插件
* 执行命令
```sql
load 'age';
```
* 示例
```sql
openGauss=# load 'age';
LOAD
```
### 设置查询空间
* 执行命令
```sql
SET search_path TO ag_catalog;
```
* 示例
```sql
openGauss=# SET search_path TO ag_catalog;
SET
```
### 创建图空间
* 执行命令
```sql
SELECT create_graph('test');
```
* 示例
```sql
openGauss=# SELECT create_graph('test');
NOTICE:  CREATE TABLE / PRIMARY KEY will create implicit index "_ag_label_vertex_pkey" for table "_ag_label_vertex"
CONTEXT:  referenced column: create_graph
NOTICE:  CREATE TABLE / PRIMARY KEY will create implicit index "_ag_label_edge_pkey" for table "_ag_label_edge"
CONTEXT:  referenced column: create_graph
NOTICE:  graph "test" has been created
CONTEXT:  referenced column: create_graph
 create_graph
--------------

(1 row)
```
### 执行cypher语句
* 执行命令
```sql
SELECT * FROM cypher(参数1 要查询的图空间,  参数2 cypher语句) AS (a agtype ,[返回元组的个数]);
```
* 示例
```sql
openGauss=# SELECT * FROM cypher('test', $$CREATE (:v {i: 0})$$) AS (a agtype);
a
---
(0 rows)

openGauss=# SELECT * FROM cypher('test', $$MATCH (n:v) RETURN n$$) AS (n agtype);
n
-----------------------------------------------------------------------
{"id": 844424930131969, "label": "v", "properties": {"i": 0}}::vertex
(1 row)

```

## 运行 test suite
### 执行步骤
>>前提条件：openGauss使用源码编译的方式安装
1. 将age源码放在openGauss源码的contrib目录下
2. 进入age源码目录,执行命令
```shell
make installcheck
```
### 成功示例
![test_suit_success_example.png](doc%2Fimg%2Ftest_suit_success_example.png)

