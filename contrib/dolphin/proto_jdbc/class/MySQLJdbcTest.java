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
            statement.executeUpdate("drop table if exists t3");
            statement.executeUpdate("create table t3 (id int, name varchar(50))");
            statement.executeUpdate("insert into t3 set id='1', name='mysql'");
            statement.executeUpdate("insert into t3 values('2', 'dolphin'), ('3', 'postgres')");
            statement.executeUpdate("update t3 set name='dolphin-server' where id ='2'");
            ResultSet resultSet = statement.executeQuery("select * from t3 order by id");
            ResultSetMetaData resultSetMetaData = resultSet.getMetaData();
            while (resultSet.next()) {
                for (int i = 1; i <= resultSetMetaData.getColumnCount(); i++) {
                    System.out.println(resultSet.getObject(i));
                }
            }
        }
    }
}
