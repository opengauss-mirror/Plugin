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
import java.sql.PreparedStatement;
import java.sql.Statement;
import java.sql.ResultSet;
import java.util.Properties;

public class MySQLJdbcAutoCommitTest {
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
            url_jdbc = "jdbc:mysql://?useServerPrepStmts=true&serverTimezone=UTC";
        } else {
            url_jdbc = "jdbc:mysql://?useServerPrepStmts=true&serverTimezone=UTC&useSSL=false";
        }

        try (Connection connection = DriverManager
                .getConnection(url_jdbc, info);
                Statement statement = connection.createStatement()) {
            statement.execute("set autocommit to false");
            statement.execute("create table autocommit_test1(a int)"); // not exists in \d
            statement.execute("rollback");
            statement.execute("create table autocommit_test2(a int)");
            statement.execute("commit");
            statement.execute("create table autocommit_test3(a int)");
            statement.execute("set autocommit to true");

            connection.setAutoCommit(false);
            statement.execute("create table autocommit_test4(a int)"); // not exists in \d
            connection.rollback();
            statement.execute("create table autocommit_test5(a int)");
            connection.commit();
            statement.execute("create table autocommit_test6(a int)");
            connection.setAutoCommit(true);

            connection.setAutoCommit(false);
            PreparedStatement preparedStatement = connection.prepareStatement("insert into autocommit_test6 values(?)");
            preparedStatement.setInt(1, 1);
            preparedStatement.execute();
            connection.rollback();
            preparedStatement.setInt(1, 2);
            preparedStatement.execute();
            connection.commit();

        }

        try (Connection connection = DriverManager
                .getConnection(url_jdbc, info);
             Statement stmt = connection.createStatement()) {
            stmt.execute("drop table if exists t_autocommit_0016 cascade;");
            stmt.execute("create table t_autocommit_0016(c_int int primary key, c_float float);");
            stmt.execute("insert into t_autocommit_0016 values(1, 105.5), (20, 655.45);");

            stmt.execute("set autocommit = 0;");
            try {
                stmt.execute("insert into t_autocommit_0016 values(1, 2, 3);");
            } catch (Exception e) {
            }
            try {
                stmt.execute("delete from t_autocommit_0016;");
            } catch (Exception e) {
            }
            try {
                stmt.execute("insert into t_autocommit_0016 values(1234, 1234.12);");
            } catch (Exception e) {
            }
            System.out.println("before commit;");
            stmt.execute("commit;");

            ResultSet rs = stmt.executeQuery("select* from t_autocommit_0016 order by 1;");
            while (rs.next()) {
                System.out.println("result");
            }
            stmt.execute("drop table if exists t_autocommit_0016 cascade;");
        }
    }
}
