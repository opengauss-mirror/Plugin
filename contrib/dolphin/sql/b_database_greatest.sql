-- Test Case 1: TINYINT type
CREATE TABLE test_greatest_tinyint (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 TINYINT,
    val2 TINYINT,
    val3 TINYINT
);
INSERT INTO test_greatest_tinyint (val1, val2, val3) VALUES (10, 20, 30), (5, 3, 8);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_tinyint;

-- Test Case 2: SMALLINT type
CREATE TABLE test_greatest_smallint (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 SMALLINT,
    val2 SMALLINT,
    val3 SMALLINT
);
INSERT INTO test_greatest_smallint (val1, val2, val3) VALUES (100, 200, 300), (50, 30, 80);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_smallint;

-- Test Case 3: MEDIUMINT type
CREATE TABLE test_greatest_mediumint (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 MEDIUMINT,
    val2 MEDIUMINT,
    val3 MEDIUMINT
);
INSERT INTO test_greatest_mediumint (val1, val2, val3) VALUES (1000, 2000, 3000), (500, 300, 800);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_mediumint;

-- Test Case 4: INT type
CREATE TABLE test_greatest_int (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 INT,
    val2 INT,
    val3 INT
);
INSERT INTO test_greatest_int (val1, val2, val3) VALUES (10000, 20000, 30000), (5000, 3000, 8000);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_int;

-- Test Case 5: BIGINT type
CREATE TABLE test_greatest_bigint (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 BIGINT,
    val2 BIGINT,
    val3 BIGINT
);
INSERT INTO test_greatest_bigint (val1, val2, val3) VALUES (10000000000, 20000000000, 30000000000), (5000000000, 3000000000, 8000000000);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_bigint;

-- Test Case 6: FLOAT type
CREATE TABLE test_greatest_float (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 FLOAT,
    val2 FLOAT,
    val3 FLOAT
);
INSERT INTO test_greatest_float (val1, val2, val3) VALUES (1.5, 2.5, 3.5), (0.5, 0.3, 0.8);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_float;

-- Test Case 7: DOUBLE type
CREATE TABLE test_greatest_double (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 DOUBLE,
    val2 DOUBLE,
    val3 DOUBLE
);
INSERT INTO test_greatest_double (val1, val2, val3) VALUES (1.123456789, 2.123456789, 3.123456789), (0.123, 0.023, 0.089);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_double;

-- Test Case 8: DECIMAL type
CREATE TABLE test_greatest_decimal (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 DECIMAL(5, 2),
    val2 DECIMAL(5, 2),
    val3 DECIMAL(5, 2)
);
INSERT INTO test_greatest_decimal (val1, val2, val3) VALUES (10.25, 20.25, 30.25), (5.25, 3.25, 8.25);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_decimal;

-- Test Case 9: CHAR type
CREATE TABLE test_greatest_char (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 CHAR(10),
    val2 CHAR(10),
    val3 CHAR(10)
);
INSERT INTO test_greatest_char (val1, val2, val3) VALUES ('apple', 'banana', 'cherry'), ('dog', 'cat', 'elephant');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_char;

-- Test Case 10: VARCHAR type
CREATE TABLE test_greatest_varchar (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 VARCHAR(50),
    val2 VARCHAR(50),
    val3 VARCHAR(50)
);
INSERT INTO test_greatest_varchar (val1, val2, val3) VALUES ('hello', 'world', 'mysql'), ('abc', 'def', 'ghi');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_varchar;

-- Test Case 11: TEXT type
CREATE TABLE test_greatest_text (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 TEXT,
    val2 TEXT,
    val3 TEXT
);
INSERT INTO test_greatest_text (val1, val2, val3) VALUES ('This is a long text', 'Another long text', 'Yet another text'), ('Short', 'Shorter', 'Shortest');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_text;

-- Test Case 12: DATE type
CREATE TABLE test_greatest_date (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 DATE,
    val2 DATE,
    val3 DATE
);
INSERT INTO test_greatest_date (val1, val2, val3) VALUES ('2023-01-01', '2023-02-01', '2023-03-01'), ('2023-05-01', '2023-04-01', '2023-06-01');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_date;

-- Test Case 13: TIME type
CREATE TABLE test_greatest_time (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 TIME,
    val2 TIME,
    val3 TIME
);
INSERT INTO test_greatest_time (val1, val2, val3) VALUES ('12:00:00', '13:00:00', '14:00:00'), ('09:30:00', '10:15:00', '08:45:00');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_time;

-- Test Case 14: DATETIME type
CREATE TABLE test_greatest_datetime (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 DATETIME,
    val2 DATETIME,
    val3 DATETIME
);
INSERT INTO test_greatest_datetime (val1, val2, val3) VALUES ('2023-01-01 12:00:00', '2023-02-01 13:00:00', '2023-03-01 14:00:00'), ('2023-05-01 09:30:00', '2023-04-01 10:15:00', '2023-06-01 08:45:00');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_datetime;

-- Test Case 15: TIMESTAMP type
CREATE TABLE test_greatest_timestamp (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 TIMESTAMP,
    val2 TIMESTAMP,
    val3 TIMESTAMP
);
INSERT INTO test_greatest_timestamp (val1, val2, val3) VALUES ('2023-01-01 12:00:00', '2023-02-01 13:00:00', '2023-03-01 14:00:00'), ('2023-05-01 09:30:00', '2023-04-01 10:15:00', '2023-06-01 08:45:00');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_timestamp;

-- Test Case 16: YEAR type
CREATE TABLE test_greatest_year (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 YEAR,
    val2 YEAR,
    val3 YEAR
);
INSERT INTO test_greatest_year (val1, val2, val3) VALUES (2023, 2024, 2025), (2022, 2026, 2021);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_year;

-- Test Case 17: ENUM type
CREATE TABLE test_greatest_enum (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 ENUM('red', 'green', 'blue'),
    val2 ENUM('red', 'green', 'blue'),
    val3 ENUM('red', 'green', 'blue')
);
INSERT INTO test_greatest_enum (val1, val2, val3) VALUES ('red', 'green', 'blue'), ('green', 'blue', 'red');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_enum;

-- Test Case 18: SET type
CREATE TABLE test_greatest_set (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 SET('apple', 'banana', 'cherry'),
    val2 SET('apple', 'banana', 'cherry'),
    val3 SET('apple', 'banana', 'cherry')
);
INSERT INTO test_greatest_set (val1, val2, val3) VALUES ('apple', 'banana', 'cherry'), ('banana,cherry', 'apple', 'cherry');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_set;

-- Test Case 19: BINARY type
CREATE TABLE test_greatest_binary (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 BINARY(5),
    val2 BINARY(5),
    val3 BINARY(5)
);
INSERT INTO test_greatest_binary (val1, val2, val3) VALUES (0x6162636465, 0x6263646566, 0x6364656667), (0x666768696a, 0x6768696a6b, 0x68696a6b6c);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_binary;

-- Test Case 20: VARBINARY type
CREATE TABLE test_greatest_varbinary (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 VARBINARY(50),
    val2 VARBINARY(50),
    val3 VARBINARY(50)
);
INSERT INTO test_greatest_varbinary (val1, val2, val3) VALUES (0x616263, 0x626364, 0x636465), (0x646566, 0x656667, 0x666768);
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_varbinary;

-- Test Case 21: BIT type
CREATE TABLE test_greatest_bit (
    id INT AUTO_INCREMENT PRIMARY KEY,
    val1 BIT(8),
    val2 BIT(8),
    val3 BIT(8)
);
INSERT INTO test_greatest_bit (val1, val2, val3) VALUES (b'00000001', b'00000010', b'00000011'), (b'00000100', b'00000011', b'00000101');
SELECT 
    val1,
    val2,
    val3,
    GREATEST(val1, val2, val3) AS greatest_result
FROM test_greatest_bit;

-- Clean up the test environment
DROP TABLE IF EXISTS test_greatest_tinyint;
DROP TABLE IF EXISTS test_greatest_smallint;
DROP TABLE IF EXISTS test_greatest_mediumint;
DROP TABLE IF EXISTS test_greatest_int;
DROP TABLE IF EXISTS test_greatest_bigint;
DROP TABLE IF EXISTS test_greatest_float;
DROP TABLE IF EXISTS test_greatest_double;
DROP TABLE IF EXISTS test_greatest_decimal;
DROP TABLE IF EXISTS test_greatest_char;
DROP TABLE IF EXISTS test_greatest_varchar;
DROP TABLE IF EXISTS test_greatest_text;
DROP TABLE IF EXISTS test_greatest_date;
DROP TABLE IF EXISTS test_greatest_time;
DROP TABLE IF EXISTS test_greatest_datetime;
DROP TABLE IF EXISTS test_greatest_timestamp;
DROP TABLE IF EXISTS test_greatest_year;
DROP TABLE IF EXISTS test_greatest_enum;
DROP TABLE IF EXISTS test_greatest_set;
DROP TABLE IF EXISTS test_greatest_binary;
DROP TABLE IF EXISTS test_greatest_varbinary;
DROP TABLE IF EXISTS test_greatest_bit;
