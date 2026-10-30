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
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Properties;

public class MySQLJdbcErrorMappingTest {
    private static Properties connectionProperties(
            String host, String port, String database, String user, String password) {
        Properties info = new Properties();
        info.setProperty("HOST", host);
        info.setProperty("PORT", port);
        info.setProperty("DBNAME", database);
        info.setProperty("user", user);
        info.setProperty("password", password);
        return info;
    }

    private static SQLException connectExpectingError(Properties info) throws SQLException {
        try (Connection ignored = DriverManager.getConnection("jdbc:mysql://", info)) {
            // The connection must not succeed.
        } catch (SQLException exception) {
            return exception;
        }
        throw new AssertionError("connection unexpectedly succeeded for database " + info.getProperty("DBNAME"));
    }

    private static SQLException executeExpectingError(Statement statement, String sql) throws SQLException {
        try {
            statement.execute(sql);
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

    private static void assertConnectionUsable(Statement statement) throws SQLException {
        try (ResultSet result = statement.executeQuery("select 1")) {
            if (!result.next() || result.getInt(1) != 1 || result.wasNull() || result.next()) {
                throw new AssertionError("connection did not return the expected SELECT result after an error");
            }
        }
    }

    public static void main(String[] args) throws Exception {
        Properties wrongPasswordInfo = connectionProperties(args[0], args[1], args[2], args[3], "wrong_password");
        SQLException wrongPassword = connectExpectingError(wrongPasswordInfo);
        assertError("access denied", wrongPassword, 1045, "28000");

        Properties info = connectionProperties(args[0], args[1], args[2], args[3], args[4]);

        try (Connection connection = DriverManager.getConnection("jdbc:mysql://", info);
             Statement statement = connection.createStatement()) {
            assertConnectionUsable(statement);

            SQLException missingTable = executeExpectingError(
                statement, "select * from jdbc_error_mapping_missing_table");
            assertError("missing table", missingTable, 1146, "42S02");
            assertConnectionUsable(statement);

            SQLException syntaxError = executeExpectingError(statement, "SELEC 1");
            assertError("syntax error", syntaxError, 1064, "42000");
            assertConnectionUsable(statement);

            SQLException missingColumn = executeExpectingError(
                statement, "select jdbc_error_mapping_missing_column");
            // Keep the 6.0.0 fallback: the internal error code is truncated to uint16.
            assertError("unmapped error preserved", missingColumn, 28804, "HY000");
            assertConnectionUsable(statement);

            SQLException unsupportedPrepare = executeExpectingError(statement,
                "prepare jdbc_error_mapping_unsupported from 'create table jdbc_error_mapping_unused(a int)'");
            assertError("unsupported prepare preserved", unsupportedPrepare, 1295, "HY000");
            assertConnectionUsable(statement);
            System.out.println("connection remains usable: 1");
        }
    }
}
