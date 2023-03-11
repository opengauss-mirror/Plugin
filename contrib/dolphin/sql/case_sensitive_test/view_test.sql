\c table_name_test_db;
set dolphin.lower_case_table_names TO 0;

CREATE SCHEMA test_view_schema_1;
CREATE SCHEMA Test_View_Schema_1;

CREATE VIEW Test1 AS SELECT 1 + 1 a;
CREATE VIEW test_view_schema_1.Test2 AS SELECT 1 + 1 a;
CREATE VIEW Test_View_Schema_1.Test3 AS SELECT 1 + 1 a;

SELECT * FROM Test1;                                               -- expected: success
SELECT * FROM test_view_schema_1.Test2;                            -- expected: success
SELECT * FROM Test_View_Schema_1.Test3;                            -- expected: success
SELECT * FROM table_name_test_db.test_view_schema_1.Test2;         -- expected: success
SELECT * FROM table_name_test_db.Test_View_Schema_1.Test3;         -- expected: success

SELECT t1.a a1, t2.a a2, t3.a a3 FROM                              -- expected: success
Test1 t1,
test_view_schema_1.Test2 t2,
Test_View_Schema_1.Test3 t3;

SHOW CREATE VIEW Test1;                                            -- expected: success
SHOW CREATE VIEW test_view_schema_1.Test2;                         -- expected: success
SHOW CREATE VIEW Test_View_Schema_1.Test3;                         -- expected: success
SHOW CREATE VIEW test1;                                            -- expected: failed
SHOW CREATE VIEW Test_View_Schema_1.Test2;                         -- expected: failed
SHOW CREATE VIEW test_view_schema_1.Test3;                         -- expected: failed
SHOW CREATE VIEW test_view_schema_1.test2;                         -- expected: failed
SHOW CREATE VIEW Test_View_Schema_1.test3;                         -- expected: failed

SELECT * FROM test1;                            -- expected: failed
SELECT * FROM Test_View_Schema_1.Test2;         -- expected: failed
SELECT * FROM test_view_schema_1.Test3;         -- expected: failed

SELECT * FROM test_view_schema_1.test2;         -- expected: failed
SELECT * FROM Test_View_Schema_1.test3;         -- expected: failed

ALTER VIEW Test1 AS SELECT 2 + 2 a, 3 + 3 b;
ALTER VIEW test_view_schema_1.Test2 AS SELECT 2 + 2 a, 3 + 3 b;
ALTER VIEW Test_View_Schema_1.Test3 AS SELECT 2 + 2 a, 3 + 3 b;

ALTER VIEW test_view_schema_1.Test2 RENAME TO Test_view_schema_1.test2;        --- expected: failed
ALTER VIEW Test_View_Schema_1.Test3 RENAME TO test_view_schema_1.test3;        --- expected: failed

ALTER VIEW test_view_schema_1.Test2 RENAME TO test2;        --- expected: success
ALTER VIEW Test_View_Schema_1.Test3 RENAME TO test3;        --- expected: success
ALTER VIEW test_view_schema_1.test2 RENAME TO Test2;        --- expected: success
ALTER VIEW Test_View_Schema_1.test3 RENAME TO Test3;        --- expected: success


SELECT * FROM test_view_schema_1.Test2;        --- expected: success
SELECT * FROM Test_View_Schema_1.Test3;        --- expected: success

SELECT * FROM test_view_schema_1.test2;        --- expected: failed
SELECT * FROM Test_View_Schema_1.test3;        --- expected: failed

DROP VIEW Test1;                                -- expected: success
DROP VIEW test_view_schema_1.Test2;             -- expected: success
DROP VIEW Test_View_Schema_1.Test3;             -- expected: success
DROP VIEW test_view_schema_1.test2;             -- expected: failed
DROP VIEW Test_View_Schema_1.test3;             -- expected: failed
DROP SCHEMA test_view_schema_1 CASCADE;
DROP SCHEMA Test_View_Schema_1 CASCADE;