/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.Statement;
import java.util.Properties;

public class MySQLJdbcTest {
    private static String host;
    private static String port = "3307";
    private static String dbname;
    private static String user;
    private static String password;
    
    public static void main(String[] args) throws Exception {
        if (args.length == 5) {
            host = args[0];
            port = args[1];
            dbname = args[2];
            user = args[3];
            password = args[4];
        }
        
        Properties info = new Properties();
        info.setProperty("HOST", host);
        info.setProperty("PORT", port);
        info.setProperty("DBNAME", dbname);
        info.setProperty("user", user);
        info.setProperty("password", password);
        
        try (Connection connection = DriverManager.getConnection("jdbc:mysql://?useSSL=false", info);
             Statement statement = connection.createStatement()) {
            ResultSet resultSet;
            ResultSetMetaData resultSetMetaData;
            statement.executeUpdate("drop table if exists t3");
            statement.executeUpdate("create table t3 (" +
                    "c1 integer," +
                    "c2 integer unsigned," +
                    "c3 tinyint," +
                    "c4 tinyint unsigned," +
                    "c5 smallint," +
                    "c6 smallint unsigned," +
                    "c7 bigint," +
                    "c8 bigint unsigned," +
                    "c9 float4," +
                    "c10 double," +
                    "c11 bit(5)," +
                    "c12 bool," +
                    "c13 char(5)," +
                    "c14 binary(5)," +
                    "c15 varchar(5)," +
                    "c16 nvarchar(5)," +
                    "c17 varbinary(5)," +
                    "c18 year," +
                    "c19 date ," +
                    "c20 time, " +
                    "c21 timestamp," +
                    "c22 datetime," +
                    "c23 tinyblob," +
                    "c24 blob," +
                    "c25 mediumblob," +
                    "c26 longblob," +
                    "c27 point," +
                    "c28 polygon," +
                    "c29 text," +
                    "c30 decimal(5, 3)," +
                    "c31 json," +
                    "c32 enum('a', 'b')," +
                    "c33 set('a', 'b')" +
                    ")");
            resultSet = statement.executeQuery("select * from t3");
            resultSetMetaData = resultSet.getMetaData();
            for (int i = 1; i <= resultSetMetaData.getColumnCount(); i++) {
                System.out.println(resultSetMetaData.getColumnName(i) + ":" + resultSetMetaData.getColumnTypeName(i));
            }

            resultSet = statement.executeQuery("explain (costs off) select * from t3 a join t3 b on a.c1=b.c2");
            resultSetMetaData = resultSet.getMetaData();
            for (int i = 1; i <= resultSetMetaData.getColumnCount(); i++) {
                System.out.println(resultSetMetaData.getColumnName(i) + ":" + resultSetMetaData.getColumnTypeName(i));
            }

            while (resultSet.next()) {
                final String res_getString = resultSet.getString(1);
                System.out.println(res_getString);
            }

            // test for jdbc.getBoolean and getInt
            statement.executeUpdate("drop table if exists t3");
            statement.executeUpdate("create table t3 (id int, c boolean)");
            statement.executeUpdate("insert into t3 values(1, true), (2, false)");
            resultSet = statement.executeQuery("select * from t3");
            while (resultSet.next()) {
                final boolean res_getBoolean = resultSet.getBoolean("c");
                final int res_getInt = resultSet.getInt("c");
                final String res_getString = resultSet.getString("c");
                System.out.println("res_getBoolean: " + res_getBoolean);
                System.out.println("res_getInt: " + res_getInt);
                System.out.println("res_getString: " + res_getString);
            }

            statement.executeUpdate("drop table if exists t3");
            statement.executeUpdate("create table t3 (id int, name varchar(50))");
            statement.executeUpdate("insert into t3 set id='1', name='mysql'");
            statement.executeUpdate("insert into t3 values('2', 'dolphin'), ('3', 'postgres')");
            statement.executeUpdate("update t3 set name='dolphin-server' where id ='2'");
            resultSet = statement.executeQuery("select * from t3 order by id");
            resultSetMetaData = resultSet.getMetaData();
            while (resultSet.next()) {
                for (int i = 1; i <= resultSetMetaData.getColumnCount(); i++) {
                    System.out.println(resultSet.getObject(i));
                }
            }
        }
    }
}
