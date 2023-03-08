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

public class MySQLJdbcPrepareTest {
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
    
        try (Connection connection = DriverManager.getConnection("jdbc:mysql://?useServerPrepStmts=true&cachePrepStmts=true&serverTimezone=UTC&useSSL=false", info);
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
                "c11 bit(10)," +
                "c12 bool," +
                "c13 char(10)," +
                "c14 binary(10)," +
                "c15 varchar(10)," +
                "c16 nvarchar(10)," +
                "c17 varbinary(10)," +
                "c18 year," +
                "c19 date ," +
                "c20 time, " +
                "c21 timestamp(3)," +
                "c22 datetime(3)," +
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
            
            PreparedStatement p1 = connection.prepareStatement("insert into t3 values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
            p1.setInt(1, 1);
            p1.setLong(2, 2000);
            p1.setInt(3, 1);
            p1.setInt(4, 4);
            p1.setInt(5, 5);
            p1.setInt(6, 6);
            p1.setLong(7, 7);
            p1.setLong(8, 8);
            p1.setFloat(9, 9.9f);
            p1.setDouble(10, 10.0);
            p1.setBytes(11, "c".getBytes());
            p1.setBoolean(12, true);
            p1.setString(13, "char");
            p1.setBytes(14, "binary".getBytes());
            p1.setString(15, "varchar");
            p1.setString(16, "nvarchar");
            p1.setBytes(17, "varbinary".getBytes());
            
            p1.setInt(18, 2023);
            p1.setDate(19, java.sql.Date.valueOf("2023-02-27"));
            p1.setTime(20, Time.valueOf("14:46:30"));
    
            p1.setTimestamp(21, Timestamp.valueOf("2023-03-07 16:16:16.666"));
            p1.setObject(22, Timestamp.valueOf("2023-03-07 16:16:16.666"));
            
            p1.setObject(23, null); // support blob type in next phase.
            p1.setObject(24, null);
            p1.setObject(25, null);
            p1.setObject(26, null);
            
            p1.setObject(27, "(1000,0)");
            p1.setObject(28, "(1000,0,200,3)");
            p1.setString(29, "text");
            p1.setBigDecimal(30, new BigDecimal(20));
            p1.setString(31, null); // json type could not supported by dolphin well
            p1.setString(32, "a");
            p1.setString(33, "a");
            
            p1.executeUpdate();
            
            PreparedStatement p2 = connection.prepareStatement("select * from t3 where c1=?");
            p2.setObject(1, 1);
            resultSet = p2.executeQuery();
            
            resultSetMetaData = resultSet.getMetaData();
            while (resultSet.next()) {
                for (int i = 1; i <= resultSetMetaData.getColumnCount(); i++) {
                    byte[] val = resultSet.getBytes(i);
                    String out = val != null ? new String(val) : null;
                    System.out.println(resultSetMetaData.getColumnName(i) + ":" +resultSetMetaData.getColumnTypeName(i) + ":" + out);
                }
            }
        }
    }
}
