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

import java.sql.*;
import java.util.Properties;

public class MySQLJdbcAllowZeroDayQueryTest {
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
            url_jdbc = "jdbc:mysql://?allowMultiQueries=true&serverTimezone=UTC&zeroDateTimeBehavior=round";
        } else {
            url_jdbc = "jdbc:mysql://?allowMultiQueries=true&serverTimezone=UTC&useSSL=false&zeroDateTimeBehavior=round";
        }

        try (Connection connection = DriverManager
                .getConnection(url_jdbc, info);
                Statement stmt = connection.createStatement()) {
            String sql = "select 0::char::timestamp";

            ResultSet rs = stmt.executeQuery(sql);
            while (rs.next()) {
                Timestamp data1 = rs.getTimestamp(1);
                System.out.println(data1);
            }
        }
    }
}
