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
import java.sql.PreparedStatement;
import java.util.Properties;
import java.sql.Time;
import java.sql.Timestamp;
import java.math.BigDecimal;
import java.sql.Blob;
import org.newsclub.net.mysql.AFUNIXDatabaseSocketFactory;

public class MySQLJdbcAutoIncrementTest {
    private static String host;
    private static String port = "3307";
    private static String dbname;
    private static String user;
    private static String password;
    
    public static void main(String[] args) throws Exception {
        if (args.length == 3) {
            port = args[0];
            dbname = args[1];
            user = args[2];
        }
        
        Properties info = new Properties();
        info.setProperty("DBNAME", dbname);
        info.setProperty("user", user);
        info.put("socketFactory", AFUNIXDatabaseSocketFactory.class.getName());
        info.put("junixsocket.file", "/tmp/.s.PGSQL." + port);

        try (Connection connection = DriverManager.getConnection("jdbc:mysql://?useServerPrepStmts=true&serverTimezone=UTC&useSSL=false&createDatabaseIfNotExist=true", info);
             Statement statement = connection.createStatement()) {
            statement.executeUpdate("drop table if exists t_auto");
            statement.executeUpdate("create table t_auto (" +
                "c1 integer auto_increment," +
                "c2 varchar(10)," +
                "PRIMARY KEY (c1)" +
                ")");
            
            PreparedStatement p1 = connection.prepareStatement("insert into t_auto(c2) values(?),(?),(?),(?)", Statement.RETURN_GENERATED_KEYS);
            p1.setString(1, "a");
            p1.setString(2, "a");
            p1.setString(3, "a");
            p1.setString(4, "a");
            
            p1.executeUpdate();
            ResultSet resultSet = p1.getGeneratedKeys();
            int count = resultSet.getMetaData().getColumnCount();
            while (resultSet.next()) {
                for (int i = 1; i <= count; i++) {
                    System.out.println(resultSet.getObject(i));
                }
            }
            p1.close();
        }
    }
}