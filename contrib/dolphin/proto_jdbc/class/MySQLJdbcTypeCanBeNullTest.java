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

public class MySQLJdbcTypeCanBeNullTest {
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
            url_jdbc = "jdbc:mysql://?serverTimezone=UTC";
        } else {
            url_jdbc = "jdbc:mysql://?serverTimezone=UTC&useSSL=false";
        }

        try (Connection connection = DriverManager
                .getConnection(url_jdbc, info);
                Statement stmt = connection.createStatement()) {
            try {
                stmt.execute("drop table if exists zz_sys_ops_package3;");
                stmt.execute("CREATE TABLE IF NOT EXISTS zz_sys_ops_package3 ( id int NOT NULL AUTO_INCREMENT COMMENT 'id', create_time datetime NOT NULL COMMENT '添加时间', update_time datetime(0) COMMENT '.新时间', remark varchar(512) DEFAULT NULL COMMENT '备注', PRIMARY KEY (id) ) ENGINE=InnoDB AUTO_INCREMENT=100 DEFAULT CHARSET=utf8;");
                stmt.execute("insert into zz_sys_ops_package3 values(1,'2025-04-25 17:00:00',null,'test');");
                ResultSet rs = stmt.executeQuery("select * from zz_sys_ops_package3;");
                while (rs.next()) {
                    System.out.println(rs.getMetaData().getColumnName(1));
                    System.out.println(rs.getString(1));
                    System.out.println(rs.getMetaData().getColumnName(2));
                    System.out.println(rs.getString(2));
                    System.out.println(rs.getMetaData().getColumnName(3));
                    System.out.println(rs.getString(3));
                    System.out.println(rs.getMetaData().getColumnName(4));
                    System.out.println(rs.getString(4));
                }
                rs.close();
            } catch (SQLException e) {
                System.out.println("execute failed:" + e.getMessage());
            }
            stmt.close();
            connection.close();
        }
    }
}