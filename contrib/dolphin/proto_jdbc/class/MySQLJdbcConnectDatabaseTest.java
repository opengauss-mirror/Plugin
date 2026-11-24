import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

public class MySQLJdbcConnectDatabaseTest {
    private static Properties connectionProperties(Properties base, String database, String password) {
        Properties info = new Properties();
        info.putAll(base);
        if (database != null) {
            info.setProperty("DBNAME", database);
        }
        info.setProperty("password", password);
        return info;
    }

    private static SQLException connectExpectingError(Properties info) throws SQLException {
        try (Connection connection = DriverManager.getConnection("jdbc:mysql://", info)) {
            throw new AssertionError("connection unexpectedly succeeded: " + info.getProperty("DBNAME"));
        } catch (SQLException exception) {
            return exception;
        }
    }

    private static void checkSchema(Properties info, String expectedSchema) throws SQLException {
        try (Connection connection = DriverManager.getConnection("jdbc:mysql://", info);
             Statement statement = connection.createStatement();
             ResultSet result = statement.executeQuery("select current_schema(), database()")) {
            if (!result.next() || !expectedSchema.equals(result.getString(1))
                || !expectedSchema.equals(result.getString(2))) {
                throw new AssertionError("connection did not select schema " + expectedSchema);
            }
        }
    }

    private static void assertSchema(String label, Properties info, String expectedSchema) throws SQLException {
        checkSchema(info, expectedSchema);
        System.out.println(label + ": " + expectedSchema);
    }

    private static String quoteIdentifier(String identifier) {
        return "`" + identifier.replace("`", "``") + "`";
    }

    private static void assertSpecialSchemas(Properties base, String database, String password) throws SQLException {
        String[] schemas = {"jdbc_connect space", "jdbc_connect,comma", "jdbc_connect\"quote"};
        List<String> createdSchemas = new ArrayList<>();
        try (Connection connection = DriverManager.getConnection(
                 "jdbc:mysql://", connectionProperties(base, database, password));
             Statement statement = connection.createStatement()) {
            try {
                for (String schema : schemas) {
                    statement.executeUpdate("create schema " + quoteIdentifier(schema));
                    createdSchemas.add(schema);
                    checkSchema(connectionProperties(base, schema, password), schema);
                }
            } finally {
                for (String schema : createdSchemas) {
                    statement.executeUpdate("drop schema " + quoteIdentifier(schema) + " cascade");
                }
            }
        }
        System.out.println("special database names: passed");
    }

    public static void main(String[] args) throws Exception {
        Properties base = new Properties();
        base.setProperty("HOST", args[0]);
        base.setProperty("PORT", args[1]);
        base.setProperty("user", args[3]);
        base.setProperty("useSSL", "false");

        String database = args[2];
        String password = args[4];
        String missingDatabase = "jdbc_missing_database_1639";
        Properties valid = connectionProperties(base, database, password);
        Properties missing = connectionProperties(base, missingDatabase, password);

        assertSchema("valid database", valid, database);
        for (int attempt = 0; attempt < 3; attempt++) {
            SQLException exception = connectExpectingError(missing);
            if (exception.getErrorCode() != 1049 || !"42000".equals(exception.getSQLState())) {
                throw new AssertionError("missing database expected 1049/42000 but got "
                    + exception.getErrorCode() + "/" + exception.getSQLState(), exception);
            }
        }
        System.out.println("missing database rejected during connect 3 times: 1049/42000");
        assertSchema("valid database after failed connections", valid, database);
        assertSpecialSchemas(base, database, password);
        assertSchema("omitted database", connectionProperties(base, null, password), args[3]);
        assertSchema("empty database", connectionProperties(base, "", password), args[3]);

        String wrongPassword = password + "_wrong";
        SQLException authentication = connectExpectingError(connectionProperties(base, database, wrongPassword));
        SQLException missingWithWrongPassword = connectExpectingError(
            connectionProperties(base, missingDatabase, wrongPassword));
        if (authentication.getErrorCode() == 1049
            || authentication.getErrorCode() != missingWithWrongPassword.getErrorCode()
            || !authentication.getSQLState().equals(missingWithWrongPassword.getSQLState())) {
            throw new AssertionError("missing database changed the authentication error", missingWithWrongPassword);
        }
        System.out.println("authentication checked before database");
    }
}
