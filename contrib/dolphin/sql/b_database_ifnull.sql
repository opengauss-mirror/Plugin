-- Test Case 1: TINYINT type
CREATE TABLE test_nullif_tinyint (
    id INT AUTO_INCREMENT PRIMARY KEY,
    tiny1 TINYINT,
    tiny2 TINYINT
);
INSERT INTO test_nullif_tinyint (tiny1, tiny2) VALUES (10, 10), (20, 30);
SELECT 
    tiny1,
    tiny2,
    NULLIF(tiny1, tiny2) AS nullif_result
FROM test_nullif_tinyint;

-- Test Case 2: SMALLINT type
CREATE TABLE test_nullif_smallint (
    id INT AUTO_INCREMENT PRIMARY KEY,
    small1 SMALLINT,
    small2 SMALLINT
);
INSERT INTO test_nullif_smallint (small1, small2) VALUES (100, 100), (200, 300);
SELECT 
    small1,
    small2,
    NULLIF(small1, small2) AS nullif_result
FROM test_nullif_smallint;

-- Test Case 3: MEDIUMINT type
CREATE TABLE test_nullif_mediumint (
    id INT AUTO_INCREMENT PRIMARY KEY,
    medium1 MEDIUMINT,
    medium2 MEDIUMINT
);
INSERT INTO test_nullif_mediumint (medium1, medium2) VALUES (1000, 1000), (2000, 3000);
SELECT 
    medium1,
    medium2,
    NULLIF(medium1, medium2) AS nullif_result
FROM test_nullif_mediumint;

-- Test Case 4: INT type
CREATE TABLE test_nullif_int (
    id INT AUTO_INCREMENT PRIMARY KEY,
    int1 INT,
    int2 INT
);
INSERT INTO test_nullif_int (int1, int2) VALUES (10000, 10000), (20000, 30000);
SELECT 
    int1,
    int2,
    NULLIF(int1, int2) AS nullif_result
FROM test_nullif_int;

-- Test Case 5: BIGINT type
CREATE TABLE test_nullif_bigint (
    id INT AUTO_INCREMENT PRIMARY KEY,
    big1 BIGINT,
    big2 BIGINT
);
INSERT INTO test_nullif_bigint (big1, big2) VALUES (10000000000, 10000000000), (20000000000, 30000000000);
SELECT 
    big1,
    big2,
    NULLIF(big1, big2) AS nullif_result
FROM test_nullif_bigint;

-- Test Case 6: FLOAT type
CREATE TABLE test_nullif_float (
    id INT AUTO_INCREMENT PRIMARY KEY,
    float1 FLOAT,
    float2 FLOAT
);
INSERT INTO test_nullif_float (float1, float2) VALUES (1.5, 1.5), (2.5, 3.5);
SELECT 
    float1,
    float2,
    NULLIF(float1, float2) AS nullif_result
FROM test_nullif_float;

-- Test Case 7: DOUBLE type
CREATE TABLE test_nullif_double (
    id INT AUTO_INCREMENT PRIMARY KEY,
    double1 DOUBLE,
    double2 DOUBLE
);
INSERT INTO test_nullif_double (double1, double2) VALUES (1.123456789, 1.123456789), (2.123456789, 3.123456789);
SELECT 
    double1,
    double2,
    NULLIF(double1, double2) AS nullif_result
FROM test_nullif_double;

-- Test Case 8: DECIMAL type
CREATE TABLE test_nullif_decimal (
    id INT AUTO_INCREMENT PRIMARY KEY,
    dec1 DECIMAL(5, 2),
    dec2 DECIMAL(5, 2)
);
INSERT INTO test_nullif_decimal (dec1, dec2) VALUES (10.25, 10.25), (20.25, 30.25);
SELECT 
    dec1,
    dec2,
    NULLIF(dec1, dec2) AS nullif_result
FROM test_nullif_decimal;

-- Test Case 9: CHAR type
CREATE TABLE test_nullif_char (
    id INT AUTO_INCREMENT PRIMARY KEY,
    char1 CHAR(10),
    char2 CHAR(10)
);
INSERT INTO test_nullif_char (char1, char2) VALUES ('hello   ', 'hello   '), ('world   ', 'universe');
SELECT 
    char1,
    char2,
    NULLIF(char1, char2) AS nullif_result
FROM test_nullif_char;

-- Test Case 10: VARCHAR type
CREATE TABLE test_nullif_varchar (
    id INT AUTO_INCREMENT PRIMARY KEY,
    varchar1 VARCHAR(50),
    varchar2 VARCHAR(50)
);
INSERT INTO test_nullif_varchar (varchar1, varchar2) VALUES ('hello', 'hello'), ('world', 'universe');
SELECT 
    varchar1,
    varchar2,
    NULLIF(varchar1, varchar2) AS nullif_result
FROM test_nullif_varchar;

-- Test Case 11: TEXT type
CREATE TABLE test_nullif_text (
    id INT AUTO_INCREMENT PRIMARY KEY,
    text1 TEXT,
    text2 TEXT
);
INSERT INTO test_nullif_text (text1, text2) VALUES ('This is a test', 'This is a test'), ('Another test', 'Different test');
SELECT 
    text1,
    text2,
    NULLIF(text1, text2) AS nullif_result
FROM test_nullif_text;

-- Test Case 12: DATE type
CREATE TABLE test_nullif_date (
    id INT AUTO_INCREMENT PRIMARY KEY,
    date1 DATE,
    date2 DATE
);
INSERT INTO test_nullif_date (date1, date2) VALUES ('2023-01-01', '2023-01-01'), ('2023-02-01', '2023-03-01');
SELECT 
    date1,
    date2,
    NULLIF(date1, date2) AS nullif_result
FROM test_nullif_date;

-- Test Case 13: TIME type
CREATE TABLE test_nullif_time (
    id INT AUTO_INCREMENT PRIMARY KEY,
    time1 TIME,
    time2 TIME
);
INSERT INTO test_nullif_time (time1, time2) VALUES ('12:00:00', '12:00:00'), ('13:00:00', '14:00:00');
SELECT 
    time1,
    time2,
    NULLIF(time1, time2) AS nullif_result
FROM test_nullif_time;

-- Test Case 14: DATETIME type
CREATE TABLE test_nullif_datetime (
    id INT AUTO_INCREMENT PRIMARY KEY,
    datetime1 DATETIME,
    datetime2 DATETIME
);
INSERT INTO test_nullif_datetime (datetime1, datetime2) VALUES ('2023-01-01 12:00:00', '2023-01-01 12:00:00'), ('2023-02-01 13:00:00', '2023-03-01 14:00:00');
SELECT 
    datetime1,
    datetime2,
    NULLIF(datetime1, datetime2) AS nullif_result
FROM test_nullif_datetime;

-- Test Case 15: TIMESTAMP type
CREATE TABLE test_nullif_timestamp (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp1 TIMESTAMP,
    timestamp2 TIMESTAMP
);
INSERT INTO test_nullif_timestamp (timestamp1, timestamp2) VALUES ('2023-01-01 12:00:00', '2023-01-01 12:00:00'), ('2023-02-01 13:00:00', '2023-03-01 14:00:00');
SELECT 
    timestamp1,
    timestamp2,
    NULLIF(timestamp1, timestamp2) AS nullif_result
FROM test_nullif_timestamp;

-- Test Case 16: YEAR type
CREATE TABLE test_nullif_year (
    id INT AUTO_INCREMENT PRIMARY KEY,
    year1 YEAR,
    year2 YEAR
);
INSERT INTO test_nullif_year (year1, year2) VALUES (2023, 2023), (2024, 2025);
SELECT 
    year1,
    year2,
    NULLIF(year1, year2) AS nullif_result
FROM test_nullif_year;

-- Test Case 17: ENUM type
CREATE TABLE test_nullif_enum (
    id INT AUTO_INCREMENT PRIMARY KEY,
    enum1 ENUM('red', 'green', 'blue'),
    enum2 ENUM('red', 'green', 'blue')
);
INSERT INTO test_nullif_enum (enum1, enum2) VALUES ('red', 'red'), ('green', 'blue');
SELECT 
    enum1,
    enum2,
    NULLIF(enum1, enum2) AS nullif_result
FROM test_nullif_enum;

-- Test Case 18: SET type
CREATE TABLE test_nullif_set (
    id INT AUTO_INCREMENT PRIMARY KEY,
    set1 SET('apple', 'banana', 'cherry'),
    set2 SET('apple', 'banana', 'cherry')
);
INSERT INTO test_nullif_set (set1, set2) VALUES ('apple', 'apple'), ('apple,banana', 'banana,cherry');
SELECT 
    set1,
    set2,
    NULLIF(set1, set2) AS nullif_result
FROM test_nullif_set;

-- Test Case 19: BINARY type
CREATE TABLE test_nullif_binary (
    id INT AUTO_INCREMENT PRIMARY KEY,
    bin1 BINARY(5),
    bin2 BINARY(5)
);
INSERT INTO test_nullif_binary (bin1, bin2) VALUES (0x6162636465, 0x6162636465), (0x666768696a, 0x6b6c6d6e6f);
SELECT 
    bin1,
    bin2,
    NULLIF(bin1, bin2) AS nullif_result
FROM test_nullif_binary;

-- Test Case 20: VARBINARY type
CREATE TABLE test_nullif_varbinary (
    id INT AUTO_INCREMENT PRIMARY KEY,
    varbin1 VARBINARY(50),
    varbin2 VARBINARY(50)
);
INSERT INTO test_nullif_varbinary (varbin1, varbin2) VALUES (0x616263, 0x616263), (0x646566, 0x676869);
SELECT 
    varbin1,
    varbin2,
    NULLIF(varbin1, varbin2) AS nullif_result
FROM test_nullif_varbinary;

-- Test Case 21: BIT type
CREATE TABLE test_nullif_bit (
    id INT AUTO_INCREMENT PRIMARY KEY,
    bit1 BIT(8),
    bit2 BIT(8)
);
INSERT INTO test_nullif_bit (bit1, bit2) VALUES (b'00000001', b'00000001'), (b'00000010', b'00000011');
SELECT 
    bit1,
    bit2,
    NULLIF(bit1, bit2) AS nullif_result
FROM test_nullif_bit;

-- Clean up the test environment
DROP TABLE IF EXISTS test_nullif_tinyint;
DROP TABLE IF EXISTS test_nullif_smallint;
DROP TABLE IF EXISTS test_nullif_mediumint;
DROP TABLE IF EXISTS test_nullif_int;
DROP TABLE IF EXISTS test_nullif_bigint;
DROP TABLE IF EXISTS test_nullif_float;
DROP TABLE IF EXISTS test_nullif_double;
DROP TABLE IF EXISTS test_nullif_decimal;
DROP TABLE IF EXISTS test_nullif_char;
DROP TABLE IF EXISTS test_nullif_varchar;
DROP TABLE IF EXISTS test_nullif_text;
DROP TABLE IF EXISTS test_nullif_date;
DROP TABLE IF EXISTS test_nullif_time;
DROP TABLE IF EXISTS test_nullif_datetime;
DROP TABLE IF EXISTS test_nullif_timestamp;
DROP TABLE IF EXISTS test_nullif_year;
DROP TABLE IF EXISTS test_nullif_enum;
DROP TABLE IF EXISTS test_nullif_set;
DROP TABLE IF EXISTS test_nullif_binary;
DROP TABLE IF EXISTS test_nullif_varbinary;
DROP TABLE IF EXISTS test_nullif_bit;