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

public class MySQLJdbcOkPacketTest {
    private static long generatedKey(Statement statement) throws Exception {
        try (ResultSet keys = statement.getGeneratedKeys()) {
            return keys.next() ? keys.getLong(1) : 0;
        }
    }

    private static void expectDuplicateKey(Statement statement, long insertId, String label) throws Exception {
        try {
            // An explicit auto-increment value is assigned before the primary-key check fails.
            statement.executeUpdate("insert into jdbc_ok_packet (id, score) values (" + insertId + ", 99)",
                Statement.RETURN_GENERATED_KEYS);
        } catch (SQLException exception) {
            if (exception.getErrorCode() != 1062 || !"23000".equals(exception.getSQLState())) {
                throw new AssertionError("Expected duplicate key 1062/23000, got " +
                    exception.getErrorCode() + "/" + exception.getSQLState(), exception);
            }
            System.out.println(label + ": 1062/23000");
            return;
        }
        throw new AssertionError("Duplicate INSERT unexpectedly succeeded");
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
            statement.executeUpdate("drop table if exists jdbc_ok_packet");
            try {
                statement.executeUpdate("drop table if exists jdbc_ok_packet_no_auto");
                statement.executeUpdate("drop table if exists jdbc_ok_packet_second");
                statement.executeUpdate("create table jdbc_ok_packet_no_auto (id int primary key)");
                statement.executeUpdate(
                    "create table jdbc_ok_packet (id bigint auto_increment primary key, score int not null)");
                statement.executeUpdate(
                    "create table jdbc_ok_packet_second (id bigint auto_increment primary key, score int not null)");

                statement.executeUpdate("insert into jdbc_ok_packet (score) values (10)",
                    Statement.RETURN_GENERATED_KEYS);
                long insertId = generatedKey(statement);
                if (insertId <= 0) {
                    throw new AssertionError("INSERT should return a positive generated key, got " + insertId);
                }
                System.out.println("insert generated key: positive");

                statement.executeUpdate("insert into jdbc_ok_packet_second (score) values (20)",
                    Statement.RETURN_GENERATED_KEYS);
                long secondInsertId = generatedKey(statement);
                if (secondInsertId <= 0) {
                    throw new AssertionError("INSERT into the second table should return a positive generated key, got " +
                        secondInsertId);
                }
                System.out.println("second table insert generated key: positive");

                statement.executeUpdate("update jdbc_ok_packet set score = 11 where id = " + insertId,
                    Statement.RETURN_GENERATED_KEYS);
                long updateId = generatedKey(statement);
                if (updateId != 0) {
                    throw new AssertionError("UPDATE should return generated key 0, got " + updateId);
                }
                System.out.println("update generated key: 0");

                statement.executeUpdate("delete from jdbc_ok_packet where id = " + insertId,
                    Statement.RETURN_GENERATED_KEYS);
                long deleteId = generatedKey(statement);
                if (deleteId != 0) {
                    throw new AssertionError("DELETE should return generated key 0, got " + deleteId);
                }
                System.out.println("delete generated key: 0");

                statement.executeUpdate("insert into jdbc_ok_packet (score) values (20)",
                    Statement.RETURN_GENERATED_KEYS);
                long errorPathId = generatedKey(statement);
                if (errorPathId <= 0) {
                    throw new AssertionError("Error-path INSERT should return a positive generated key");
                }
                System.out.println("error-path insert generated key: positive");

                // Reuse the same connection and statement immediately after the server-side error.
                expectDuplicateKey(statement, errorPathId, "duplicate key before update");
                int updatedRows = statement.executeUpdate(
                    "update jdbc_ok_packet set score = 21 where id = " + errorPathId,
                    Statement.RETURN_GENERATED_KEYS);
                long updateAfterErrorId = generatedKey(statement);
                if (updatedRows != 1 || updateAfterErrorId != 0) {
                    throw new AssertionError("UPDATE after error expected 1 row and generated key 0, got " +
                        updatedRows + "/" + updateAfterErrorId);
                }
                System.out.println("update after error generated key: 0");
                try (ResultSet rows = statement.executeQuery(
                        "select score from jdbc_ok_packet where id = " + errorPathId)) {
                    if (!rows.next() || rows.getInt(1) != 21 || rows.next()) {
                        throw new AssertionError("UPDATE after error did not persist the expected row");
                    }
                }

                // UPDATE masks stale IDs itself. An INSERT into a non-auto-increment table
                // exposes error-path leakage; do not run another successful SQL in between.
                expectDuplicateKey(statement, errorPathId, "duplicate key before plain insert");
                int insertedRows = statement.executeUpdate("insert into jdbc_ok_packet_no_auto (id) values (1)",
                    Statement.RETURN_GENERATED_KEYS);
                long insertAfterErrorId = generatedKey(statement);
                if (insertedRows != 1 || insertAfterErrorId != 0) {
                    throw new AssertionError("Plain INSERT after error expected 1 row and generated key 0, got " +
                        insertedRows + "/" + insertAfterErrorId);
                }
                System.out.println("plain insert after error generated key: 0");
            } finally {
                try {
                    statement.executeUpdate("drop table if exists jdbc_ok_packet_no_auto");
                } finally {
                    try {
                        statement.executeUpdate("drop table if exists jdbc_ok_packet_second");
                    } finally {
                        statement.executeUpdate("drop table if exists jdbc_ok_packet");
                    }
                }
            }
        }
    }
}
