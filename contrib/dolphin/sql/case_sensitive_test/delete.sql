\c table_name_test_db;
SET lower_case_table_names TO 0;
CREATE TABLE Delete_test (
    id int PRIMARY KEY,
    a INT,
    b text
);

INSERT INTO Delete_test (a) VALUES (10);
INSERT INTO Delete_test (a, b) VALUES (50, repeat('x', 10000));
INSERT INTO Delete_test (a) VALUES (100);

-- allow an alias to be specified for DELETE's target table
DELETE FROM Delete_test AS dt WHERE dt.a > 75;

-- if an alias is specified, don't allow the original table name
-- to be referenced
DELETE FROM Delete_test dt WHERE Delete_test.a > 25;

SELECT id, a, char_length(b) FROM Delete_test ORDER BY id;

-- delete a row with a TOASTed value
DELETE FROM Delete_test WHERE a > 25;

SELECT id, a, char_length(b) FROM Delete_test ORDER BY id;

DROP TABLE Delete_test;
