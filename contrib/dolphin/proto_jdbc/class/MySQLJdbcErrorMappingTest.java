/*
 * Copyright (c) 2026 openGauss Contributors
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of the License at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 */

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Properties;

public class MySQLJdbcErrorMappingTest {
    private static SQLException executeExpectingError(Statement statement, String sql) throws SQLException {
        try {
            statement.executeUpdate(sql);
        } catch (SQLException exception) {
            return exception;
        }
        throw new AssertionError("statement unexpectedly succeeded: " + sql);
    }

    private static void assertError(String label, SQLException exception, int errorCode, String sqlState) {
        if (exception.getErrorCode() != errorCode || !sqlState.equals(exception.getSQLState())) {
            throw new AssertionError(label + " expected " + errorCode + "/" + sqlState
                + " but got " + exception.getErrorCode() + "/" + exception.getSQLState(), exception);
        }
        System.out.println(label + ": " + errorCode + "/" + sqlState);
    }

    public static void main(String[] args) throws Exception {
        Properties info = new Properties();
        info.setProperty("HOST", args[0]);
        info.setProperty("PORT", args[1]);
        info.setProperty("DBNAME", args[2]);
        info.setProperty("user", args[3]);
        info.setProperty("password", args[4]);

        try (Connection connection = DriverManager.getConnection("jdbc:mysql://", info);
             Statement statement = connection.createStatement()) {
            statement.executeUpdate("drop table if exists jdbc_error_mapping");
            statement.executeUpdate("create table jdbc_error_mapping (id int primary key)");
            statement.executeUpdate("insert into jdbc_error_mapping values (1)");
            SQLException duplicate = executeExpectingError(
                statement, "insert into jdbc_error_mapping values (1)");
            assertError("duplicate key", duplicate, 1062, "23000");

            statement.executeUpdate("drop procedure if exists jdbc_signal_23505");
            statement.executeUpdate("create procedure jdbc_signal_23505() "
                + "signal sqlstate '23505' set message_text = 'Constraint already exists'");
            SQLException generic23505 = executeExpectingError(statement, "call jdbc_signal_23505()");
            if (generic23505.getErrorCode() == 1062 || !"23505".equals(generic23505.getSQLState())) {
                throw new AssertionError("generic 23505 was incorrectly mapped to duplicate key", generic23505);
            }
            System.out.println("generic 23505 preserved: 23505");

            statement.executeUpdate("drop procedure if exists jdbc_signal_explicit");
            statement.executeUpdate("create procedure jdbc_signal_explicit() "
                + "signal sqlstate '45000' set message_text = 'custom signal', mysql_errno = 30001");
            SQLException signal = executeExpectingError(statement, "call jdbc_signal_explicit()");
            assertError("explicit signal", signal, 30001, "45000");

            statement.executeUpdate("drop procedure if exists jdbc_resignal_explicit");
            statement.executeUpdate("create procedure jdbc_resignal_explicit() "
                + "begin "
                + "declare exit handler for sqlexception "
                + "begin "
                + "resignal sqlstate '45001' set message_text = 'custom resignal', mysql_errno = 30002; "
                + "end; "
                + "signal sqlstate '45000' set message_text = 'trigger handler', mysql_errno = 30001; "
                + "end");
            SQLException resignal = executeExpectingError(statement, "call jdbc_resignal_explicit()");
            assertError("explicit resignal", resignal, 30002, "45001");

            statement.executeUpdate("drop table if exists jdbc_error_mapping");
            statement.executeUpdate("drop procedure if exists jdbc_signal_23505");
            statement.executeUpdate("drop procedure if exists jdbc_signal_explicit");
            statement.executeUpdate("drop procedure if exists jdbc_resignal_explicit");
        }
    }
}
