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
import java.util.HashMap;
import java.util.Map;

public class MySQLJdbcTypeCanBeNullTest {
    private static String host;
    private static String port = "3307";
    private static String dbname;
    private static String user;
    private static String password;
    private static String jar_version = "old";
    private static String url_jdbc;

    private static final Map<Integer, String> TYPE_MAPPER = new HashMap<Integer, String>() {{
        put(Types.TINYINT, "getInt");
        put(Types.SMALLINT, "getInt");
        put(Types.INTEGER, "getInt");
        put(Types.BIGINT, "getLong");
        put(Types.BIT, "getBytes");
        put(Types.BOOLEAN, "getBoolean");
        put(Types.FLOAT, "getFloat");
        put(Types.DOUBLE, "getDouble");
        put(Types.DECIMAL, "getBigDecimal");
        put(Types.DATE, "getDate");
        put(Types.TIME, "getTime");
        put(Types.TIMESTAMP, "getTimestamp");
        put(Types.BINARY, "getBytes");
        put(Types.VARBINARY, "getBytes");
        put(Types.LONGVARBINARY, "getBytes");
        put(Types.BLOB, "getBytes");
        put(Types.CHAR, "getString");
        put(Types.VARCHAR, "getString");
        put(Types.LONGVARCHAR, "getString");
    }};

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
                stmt.execute("drop table if exists all_mysql_data_types;");
                stmt.execute("CREATE TABLE IF NOT EXISTS all_mysql_data_types (tinyint_col TINYINT,tinyint_unsigned_col TINYINT UNSIGNED,smallint_col SMALLINT,smallint_unsigned_col SMALLINT UNSIGNED,int_col INT,int_unsigned_col INT UNSIGNED,bigint_col BIGINT,bigint_col_unsigned BIGINT UNSIGNED,bit1_col BIT(1),bit64_col BIT(64),bool_col BOOLEAN,float_col FLOAT,double_col DOUBLE,decimal_col DECIMAL(10,2),date_col DATE,datetime_col DATETIME(6),timestamp_col TIMESTAMP(3),time_col TIME(2),year_col YEAR(4),char_col CHAR(10),varchar_col VARCHAR(255),binary_col BINARY(16),varbinary_col VARBINARY(255),tinyblob_col TINYBLOB,blob_col BLOB,mediumblob_col MEDIUMBLOB,longblob_col LONGBLOB,text_col TEXT,enum_col ENUM('Option1','Option2','Option3'),set_col SET('A','B','C','D'),json_col JSON);");
                stmt.execute("INSERT INTO all_mysql_data_types VALUES (NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);");
                ResultSet rs = stmt.executeQuery("select * from all_mysql_data_types;");
                ResultSetMetaData metaData = rs.getMetaData();
                int columnCount = metaData.getColumnCount();
                while (rs.next()) {
                    for (int i = 1; i <= columnCount; i++) {
                        int columnType = metaData.getColumnType(i);
                        String methodName = TYPE_MAPPER.getOrDefault(columnType, "getObject");
                        Object value = invokeGetter(rs, i, methodName);

                        System.out.printf("%s: %s%n", metaData.getColumnTypeName(i), value != null ? value : "NULL");
                    }
                }
                rs.close();
            } catch (SQLException e) {
                System.out.println("execute failed:" + e.getMessage());
            }
            stmt.close();
            connection.close();
        }

        try (Connection connection = DriverManager
                .getConnection(url_jdbc, info);
                PreparedStatement stmt = connection.prepareStatement("select * from all_mysql_data_types limit ?;")) {
            try {
                stmt.setInt(1,1);
                ResultSet rs = stmt.executeQuery();
                ResultSetMetaData metaData = rs.getMetaData();
                int columnCount = metaData.getColumnCount();
                while (rs.next()) {
                    for (int i = 1; i <= columnCount; i++) {
                        int columnType = metaData.getColumnType(i);
                        String methodName = TYPE_MAPPER.getOrDefault(columnType, "getObject");
                        Object value = invokeGetter(rs, i, methodName);

                        System.out.printf("%s: %s%n", metaData.getColumnTypeName(i), value != null ? value : "NULL");
                    }
                }
                rs.close();
            } catch (SQLException e) {
                System.out.println("execute failed:" + e.getMessage());
            }
            stmt.close();
            connection.close();
        }
    }

    private static Object invokeGetter(ResultSet rs, int columnIndex, String methodName) throws SQLException {
        try {
            return rs.getClass().getMethod(methodName, int.class).invoke(rs, columnIndex);
        } catch (Exception e) {
            return rs.getObject(columnIndex);
        }
    }
}