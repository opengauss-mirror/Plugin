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

/**
 * Verify that MySQL-protocol metadata access (information_schema / SHOW COLUMNS,
 * i.e. reading column type names exposed by format_type()) returns MySQL type names.
 * For a MySQL-protocol session dolphin maps the openGauss type names exposed by
 * information_schema.columns / SHOW COLUMNS to MySQL names, e.g.
 * character varying -> varchar, double precision -> double, boolean -> tinyint(1).
 */
public class MySQLJdbcTypeNameMetadataTest {
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
            url_jdbc = "jdbc:mysql://?useSSL=false";
        }

        try (Connection connection = DriverManager.getConnection(url_jdbc, info);
             Statement statement = connection.createStatement()) {
            statement.executeUpdate("drop table if exists type_name_metadata_probe");
            statement.executeUpdate("create table type_name_metadata_probe (" +
                    "v varchar(50)," +
                    "d double," +
                    "b boolean," +
                    "ts timestamp," +
                    "dt datetime," +
                    "n numeric(10,2)," +
                    "i int" +
                    ")");

            // information_schema.columns.data_type/column_type should return MySQL type names
            ResultSet resultSet = statement.executeQuery(
                    "select column_name, data_type, column_type from information_schema.columns " +
                    "where table_schema = database() and table_name = 'type_name_metadata_probe' " +
                    "order by ordinal_position");
            while (resultSet.next()) {
                System.out.println(resultSet.getString(1) + ":" + resultSet.getString(2) + ":" + resultSet.getString(3));
            }

            // SHOW COLUMNS Type column should return MySQL type names
            resultSet = statement.executeQuery("show columns from type_name_metadata_probe");
            while (resultSet.next()) {
                System.out.println("Field:" + resultSet.getString("Field") + ":" + resultSet.getString("Type"));
            }

            statement.executeUpdate("drop table if exists type_name_metadata_probe");
        }
    }
}
