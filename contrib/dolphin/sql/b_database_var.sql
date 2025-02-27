-- test Case 1: Basic functionality test_var_var for numeric values
-- Create a table with a numeric column
CREATE TABLE test_var_numeric (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value INT
);

-- Insert test data
INSERT INTO test_var_numeric (value) VALUES (1), (2), (3), (4), (5);

-- Calculate variance using VAR_POP() and VAR_SAMP()
SELECT 
    VAR_POP(value) AS var_pop_result,
    VAR_SAMP(value) AS var_samp_result
FROM test_var_numeric;

-- test Case 2: Empty table test for numeric values
-- Create an empty table with a numeric column
CREATE TABLE test_var_numeric_empty (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value INT
);

-- Calculate variance using VAR_POP() and VAR_SAMP()
SELECT 
    VAR_POP(value) AS var_pop_result,
    VAR_SAMP(value) AS var_samp_result
FROM test_var_numeric_empty;

-- test Case 3: Single - value test for numeric values
-- Create a table with a single numeric value
CREATE TABLE test_var_numeric_single (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value INT
);

-- Insert a single value
INSERT INTO test_var_numeric_single (value) VALUES (10);

-- Calculate variance using VAR_POP() and VAR_SAMP()
SELECT 
    VAR_POP(value) AS var_pop_result,
    VAR_SAMP(value) AS var_samp_result
FROM test_var_numeric_single;

-- test Case 4: test with string values
-- Create a table with a string column
CREATE TABLE test_var_string (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value VARCHAR(20)
);

-- Insert test data
INSERT INTO test_var_string (value) VALUES ('abc'), ('def'), ('ghi');

-- Try to calculate variance using VAR_POP() and VAR_SAMP()
-- Expected result: NULL since the input is non - numeric
SELECT 
    VAR_POP(value) AS var_pop_result,
    VAR_SAMP(value) AS var_samp_result
FROM test_var_string;

-- test Case 5: Mixed numeric and non - numeric values
-- Create a table with a mixed column
CREATE TABLE test_var_mixed (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value VARCHAR(20)
);

-- Insert test data
INSERT INTO test_var_mixed (value) VALUES ('1'), ('2'), ('abc');

-- Try to calculate variance using VAR_POP() and VAR_SAMP()
-- MySQL will try to convert strings to numbers if possible and ignore non - convertible values
SELECT 
    VAR_POP(value) AS var_pop_result,
    VAR_SAMP(value) AS var_samp_result
FROM test_var_mixed;

-- Clean up the test data
DROP TABLE IF EXISTS test_var_numeric;
DROP TABLE IF EXISTS test_var_numeric_empty;
DROP TABLE IF EXISTS test_var_numeric_single;
DROP TABLE IF EXISTS test_var_string;
DROP TABLE IF EXISTS test_var_mixed;