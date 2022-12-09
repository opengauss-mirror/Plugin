drop database if exists test_json_storage_size;
create database test_json_storage_size dbcompatibility'B';
\c test_json_storage_size

set enable_set_variable_b_format to on;

SELECT JSON_STORAGE_SIZE('0');

SELECT JSON_STORAGE_SIZE('1');

SELECT JSON_STORAGE_SIZE('1000');

SELECT JSON_STORAGE_SIZE('100000');

SELECT JSON_STORAGE_SIZE('"a"');

SELECT JSON_STORAGE_SIZE('"Hello World"');

SELECT JSON_STORAGE_SIZE('true');

SELECT JSON_STORAGE_SIZE('false');

SELECT JSON_STORAGE_SIZE('null');

SELECT JSON_STORAGE_SIZE('[1, "abc", null, true, "10:27:06.000000", {"id": 1}]');

SELECT JSON_STORAGE_SIZE('{"db": ["mysql", "oracle"], "id": 123, "info": {"age": 20}}');

SELECT JSON_STORAGE_SIZE('[1, 2]');

SELECT JSON_STORAGE_SIZE('[1, 2, 3]');

SELECT JSON_STORAGE_SIZE('{"x": 1}');

SELECT JSON_STORAGE_SIZE('{"x": 1, "y": 2}');

SET @data = '{
  "Person": {
    "Age": 10,
    "Name": "Bart",
    "Friends": [
      "Bart",
      "Milhouse"
    ]
  }
}';

SELECT JSON_STORAGE_SIZE(@data);

DROP TABLE IF EXISTS test_json_storage_size;
CREATE TABLE test_json_storage_size (
    json_col JSON NOT NULL
);

INSERT INTO test_json_storage_size
VALUES ('{"x": 1, "y": 2}');

SELECT
    json_col,
    JSON_STORAGE_SIZE(json_col)
FROM
    test_json_storage_size;

UPDATE test_json_storage_size
SET json_col = '{"x": 1, "y": 2, "z": 3}';

SELECT
    json_col,
    JSON_STORAGE_SIZE(json_col)
FROM
    test_json_storage_size;

SELECT JSON_STORAGE_SIZE('{0,1}');

\c postgres

drop database test_json_storage_size
