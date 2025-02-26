-- test Case 1: Basic SUM() test with numeric values
-- Create a table with a numeric column
CREATE TABLE test_sum_numeric (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value INT
);

-- Insert test data
INSERT INTO test_sum_numeric (value) VALUES (1), (2), (3), (4), (5);

-- Calculate the sum of all values in the column
SELECT SUM(value) AS total_sum FROM test_sum_numeric;

-- test Case 2: SUM() with DISTINCT for numeric values
-- Calculate the sum of distinct values in the column
SELECT SUM(DISTINCT value) AS distinct_sum FROM test_sum_numeric;

-- test Case 3: Empty table test for numeric values
-- Create an empty table with a numeric column
CREATE TABLE test_sum_numeric_empty (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value INT
);

-- Calculate the sum of values in the empty table
SELECT SUM(value) AS total_sum FROM test_sum_numeric_empty;

-- test Case 4: Single - value test for numeric values
-- Create a table with a single numeric value
CREATE TABLE test_sum_numeric_single (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value INT
);

-- Insert a single value
INSERT INTO test_sum_numeric_single (value) VALUES (10);

-- Calculate the sum of the single value
SELECT SUM(value) AS total_sum FROM test_sum_numeric_single;

-- test Case 5: test with string values that can be converted to numbers
-- Create a table with a string column
CREATE TABLE test_sum_string_convertible (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value VARCHAR(20)
);

-- Insert test data
INSERT INTO test_sum_string_convertible (value) VALUES ('1'), ('2'), ('3');

-- Calculate the sum of string values that can be converted to numbers
SELECT SUM(value) AS total_sum FROM test_sum_string_convertible;

-- Calculate the sum of distinct string values that can be converted to numbers
SELECT SUM(DISTINCT value) AS distinct_sum FROM test_sum_string_convertible;

-- test Case 6: test with non - convertible string values
-- Create a table with a string column
CREATE TABLE test_sum_string_non_convertible (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value VARCHAR(20)
);

-- Insert test data
INSERT INTO test_sum_string_non_convertible (value) VALUES ('abc'), ('def'), ('ghi');

-- Calculate the sum of non - convertible string values
SELECT SUM(value) AS total_sum FROM test_sum_string_non_convertible;

-- test Case 7: Mixed numeric and non - numeric values
-- Create a table with a mixed column
CREATE TABLE test_sum_mixed (
    id INT AUTO_INCREMENT PRIMARY KEY,
    value VARCHAR(20)
);

-- Insert test data
INSERT INTO test_sum_mixed (value) VALUES ('1'), ('2'), ('abc');

-- Calculate the sum of mixed values
SELECT SUM(value) AS total_sum FROM test_sum_mixed;

-- Calculate the sum of distinct mixed values
SELECT SUM(DISTINCT value) AS distinct_sum FROM test_sum_mixed;


DROP TABLE IF EXISTS test_sum_numeric;
DROP TABLE IF EXISTS test_sum_numeric_empty;
DROP TABLE IF EXISTS test_sum_numeric_single;
DROP TABLE IF EXISTS test_sum_string_convertible;
DROP TABLE IF EXISTS test_sum_string_non_convertible;
DROP TABLE IF EXISTS test_sum_mixed;