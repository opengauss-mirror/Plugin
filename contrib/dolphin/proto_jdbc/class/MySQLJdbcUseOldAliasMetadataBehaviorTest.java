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
import java.sql.PreparedStatement;
import java.sql.Statement;
import java.util.Properties;
import java.sql.SQLException;
import java.sql.ResultSetMetaData;

public class MySQLJdbcUseOldAliasMetadataBehaviorTest {
    private static String host;
    private static String port = "3307";
    private static String dbname;
    private static String user;
    private static String password;
    private static String jar_version = "old";
    private static String url_jdbc;

    public static void main(String[] args) throws Exception {
        if (args.length >= 5) {
            host = args[0];
            port = args[1];
            dbname = args[2];
            user = args[3];
            password = args[4];
            if (args.length == 6) {
                jar_version = args[5];
            }
        }

        Properties info = new Properties();
        info.setProperty("HOST", host);
        info.setProperty("PORT", port);
        info.setProperty("DBNAME", dbname);
        info.setProperty("user", user);
        info.setProperty("password", password);
        
        if (jar_version.equals("new")) {
            url_jdbc = "jdbc:mysql://?useOldAliasMetadataBehavior=false&serverTimezone=UTC";
        } else {
            url_jdbc = "jdbc:mysql://?useOldAliasMetadataBehavior=false&serverTimezone=UTC&useSSL=false";
        }

        try (Connection connection = DriverManager
                .getConnection(url_jdbc, info);
                Statement stmt = connection.createStatement()) {
            try {
                stmt.execute("drop table if exists test_big;");
                stmt.execute("create table test_big (id int, val varchar(100));");
                stmt.execute("insert into test_big values(999,'Value_999');");
                ResultSet rs = stmt.executeQuery("select id + 1 as ID, val as VAL, id as ID2 from test_big where id = 999");
                ResultSetMetaData metaData = rs.getMetaData();
                int columnCount = metaData.getColumnCount();
                while (rs.next()) {
                    for (int i = 1; i <= columnCount; i++) {
                        System.out.println(rs.getMetaData().getColumnName(i));
                    }
                }
                rs.close();
            } catch (SQLException e) {
                System.out.println("execute failed:" + e.getMessage());
            }
            stmt.close();
            connection.close();
        }

        if (jar_version.equals("new")) {
            url_jdbc = "jdbc:mysql://?useOldAliasMetadataBehavior=true&serverTimezone=UTC";
        } else {
            url_jdbc = "jdbc:mysql://?useOldAliasMetadataBehavior=true&serverTimezone=UTC&useSSL=false";
        }

        try (Connection connection = DriverManager
                .getConnection(url_jdbc, info);
                Statement stmt = connection.createStatement()) {
            stmt.execute("drop table if exists test_big;");
            stmt.execute("create table test_big (id int, val varchar(100));");
            stmt.execute("insert into test_big values(999,'Value_999');");
            ResultSet rs = stmt.executeQuery("select id + 1 as ID, val as VAL, id as ID2 from test_big where id = 999");
            ResultSetMetaData metaData = rs.getMetaData();
            int columnCount = metaData.getColumnCount();
            while (rs.next()) {
                for (int i = 1; i <= columnCount; i++) {
                    System.out.println(rs.getMetaData().getColumnName(i));
                }
            }
            rs.close();
            stmt.close();
            connection.close();
        }
    }
}