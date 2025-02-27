-- test Case 1: Basic functionality test for numeric values
-- Create a table with a numeric column
CREATE TABLE test_stddev_numeric (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value INT
);

-- Insert test data
INSERT INTO test_stddev_numeric (value) VALUES (1), (2), (3), (4), (5);

-- Calculate standard deviation using STDDEV(), STDDEV_POP(), and STDDEV_SAMP()
SELECT 
    STDDEV(value) AS stddev_result,
    STDDEV_POP(value) AS stddev_pop_result,
    STDDEV_SAMP(value) AS stddev_samp_result
FROM test_stddev_numeric;

-- test Case 2: Empty table test for numeric values
-- Create an empty table with a numeric column
CREATE TABLE test_stddev_numeric_empty (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value INT
);

-- Calculate standard deviation using STDDEV(), STDDEV_POP(), and STDDEV_SAMP()
SELECT 
    STDDEV(value) AS stddev_result,
    STDDEV_POP(value) AS stddev_pop_result,
    STDDEV_SAMP(value) AS stddev_samp_result
FROM test_stddev_numeric_empty;

-- test Case 3: Single - value test for numeric values
-- Create a table with a single numeric value
CREATE TABLE test_stddev_numeric_single (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value INT
);

-- Insert a single value
INSERT INTO test_stddev_numeric_single (value) VALUES (10);

-- Calculate standard deviation using STDDEV(), STDDEV_POP(), and STDDEV_SAMP()
SELECT 
    STDDEV(value) AS stddev_result,
    STDDEV_POP(value) AS stddev_pop_result,
    STDDEV_SAMP(value) AS stddev_samp_result
FROM test_stddev_numeric_single;

-- test Case 4: test with string values
-- Create a table with a string column
CREATE TABLE test_stddev_string (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value VARCHAR(20)
);

-- Insert test data
INSERT INTO test_stddev_string (value) VALUES ('abc'), ('def'), ('ghi');

-- Try to calculate standard deviation using STDDEV(), STDDEV_POP(), and STDDEV_SAMP()
-- Expected result: NULL since the input is non - numeric
SELECT 
    STDDEV(value) AS stddev_result,
    STDDEV_POP(value) AS stddev_pop_result,
    STDDEV_SAMP(value) AS stddev_samp_result
FROM test_stddev_string;

-- test Case 5: Mixed numeric and non - numeric values
-- Create a table with a mixed column
CREATE TABLE test_stddev_mixed (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value VARCHAR(20)
);

-- Insert test data
INSERT INTO test_stddev_mixed (value) VALUES ('1'), ('2'), ('abc');

-- Try to calculate standard deviation using STDDEV(), STDDEV_POP(), and STDDEV_SAMP()
SELECT 
    STDDEV(value) AS stddev_result,
    STDDEV_POP(value) AS stddev_pop_result,
    STDDEV_SAMP(value) AS stddev_samp_result
FROM test_stddev_mixed;

-- Clean up the test data
DROP TABLE IF EXISTS test_stddev_numeric;
DROP TABLE IF EXISTS test_stddev_numeric_empty;
DROP TABLE IF EXISTS test_stddev_numeric_single;
DROP TABLE IF EXISTS test_stddev_string;
DROP TABLE IF EXISTS test_stddev_mixed;