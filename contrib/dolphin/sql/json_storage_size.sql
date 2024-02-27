create schema test_json_storage_size;
set current_schema to 'test_json_storage_size';

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


set dolphin.b_compatibility_mode = on;
create table test_json_table
(
   `char` char(100),
   `varchar` varchar(100),
   `json` json,
   `int1` tinyint,
   `binary` binary(100)
);

insert into test_json_table values ('[1,2,3,4,5]', '[1,2,3,4,5]', json_object('a', 1, 'b', 2), 1, '[1,2,3,4,5]');

select json_storage_size(`char`), json_storage_size(`varchar`), json_storage_size(`json`), pg_typeof(json_storage_size(`char`)), pg_typeof(json_storage_size(`varchar`)), pg_typeof(json_storage_size(`json`)) from test_json_table;

create table test1 as select json_storage_size(`char`) as c1, json_storage_size(`varchar`) as c2, json_storage_size(`json`) as c3 from test_json_table;

-- expect error
select json_storage_size(`int1`) from test_json_table;
select json_storage_size(`binary`) from test_json_table;

drop table test1;
drop table test_json_table;

drop schema test_json_storage_size cascade;
reset current_schema;
