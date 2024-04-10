# **AGE图插件的jdbc适配**
AGE适配openGauss,jdbc支持AGType使用说明

## 编译安装要求
java  1.8
maven 3.6.2+
## 通过源码构建

```bash
git https://gitee.com/opengauss/Plugin.git
cd contrib/age/drivers/jdbc/lib

mvn clean install -DskipTests=true
```
>> 注意 编译过程使用了 contrib/age/drivers/Agtype.g4 文件，具体使用方式参考 antlr4-maven-plugin 插件的用法
## mvn依赖引入
```xml
<dependency>
    <groupId>org.opengauss</groupId>
    <artifactId>apache-age-driver</artifactId>
    <version>1.0.0</version>
</dependency>
```
## jdbc使用样例

```java
package com.example.opengaussage;


import org.apache.age.jdbc.base.Agtype;
import org.apache.age.jdbc.base.AgtypeFactory;
import org.apache.age.jdbc.base.AgtypeUtil;
import org.apache.age.jdbc.base.type.AgtypeMap;
import org.postgresql.jdbc.PgConnection;

import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.Statement;

public class OpengaussageApplication {

    static final String DB_URL = "jdbc:postgresql://10.8.8.151:5432/postgres";
    static final String USER = "omm";
    static final String PASS = "ZAQ1234esz";

    public static void main(String[] args) {

        // Open a connection
        try {
            Class.forName("org.postgresql.Driver");
            // 新建连接
            PgConnection connection = DriverManager.getConnection(DB_URL, USER, PASS).unwrap(PgConnection.class);
            // 添加Agtype数据类型解析
            connection.addDataType("agtype", Agtype.class);

            // 创建插件，不需要每次都执行
            Statement stmt = connection.createStatement();
            stmt.execute("CREATE EXTENSION IF NOT EXISTS age;");
            stmt.execute("LOAD 'age'");

            // AGE相关函数均存储在ag_catalog命名空间
            stmt.execute("SET search_path = ag_catalog, \"$user\", public;");
            // cascade 删除 demo 图空间 ，测试用，如果此处报错请注释这一行代码再执行
            stmt.execute("SELECT drop_graph('demo',true);");
            // 创建 demo 图空间
            stmt.execute("SELECT create_graph('demo');");

            // 使用prepareStatement  cypher 函数的第三个参数设置为 ？ 用来为cypher语句传递参数
            PreparedStatement preparedStatement =
                    connection.prepareStatement("SELECT * FROM cypher('demo', $$CREATE (:v {name: $name, age:$age})$$,?) AS (a agtype);");
            // 使用 AgtypeMap 传递参数
            AgtypeMap build = AgtypeUtil.createMapBuilder().add("name", "test").add("age", 18).build();
            // 这里不可以将 AgtypeMap 直接作为参数 ,因为这样会和 hstore类型的参数冲突
            preparedStatement.setObject(1, AgtypeFactory.create(build));
            preparedStatement.execute();

            // 执行Cypher语句
            ResultSet rs = stmt.executeQuery("SELECT * from cypher('demo', $$ MATCH (n) RETURN n $$) as (n agtype);");

            while (rs.next()) {

                // Returning Result as Agtype
                Agtype returnedAgtype = rs.getObject(1, Agtype.class);

                String nodeLabel = returnedAgtype.getMap().getObject("label").toString();
                String nodeProp =  returnedAgtype.getMap().getObject("properties").toString();

                System.out.println("Vertex : " + nodeLabel + ", \tProps : " + nodeProp);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

```

输出

```
Vertex : v, 	Props : {name=test, age=18}
```
## 其他编程语言的接口
1. 源码中中包含了相关的英文文档的介绍及样例
2. 可参考jdbc的使用方式

## 更多相关资料可以参考 [Apache AGE](https://age.apache.org/)

- AGE 官网 : [https://age.apache.org/](https://age.apache.org/)
- AGE GitHub仓库 : [https://github.com/apache/age](https://github.com/apache/age)
- AGE 官方文档 : [https://age.apache.org/age-manual/master/index.html](https://age.apache.org/age-manual/master/index.html)
