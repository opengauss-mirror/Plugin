create schema test_lag;
set current_schema to 'test_lag';
set dolphin.b_compatibility_mode to on;

CREATE TABLE test_lag_table (d DOUBLE, id INT, sex CHAR(1), n INT NOT NULL AUTO_INCREMENT, PRIMARY KEY(n));
SELECT n, id, LAG(id, 0, n*n) OVER (ORDER BY id DESC, n ASC ROWS BETWEEN CURRENT ROW AND 2 FOLLOWING) L FROM test_lag_table;
INSERT INTO test_lag_table values(1.0, 1, 'F', 1);
INSERT INTO test_lag_table values(2.0, 2, 'M', 2);
SELECT n, id, LAG(id, 1, n*n) OVER (ORDER BY id DESC, n ASC ROWS BETWEEN CURRENT ROW AND 2 FOLLOWING) L FROM test_lag_table;

drop table test_lag_table;
set dolphin.b_compatibility_mode to off;
drop schema test_lag cascade;
reset current_schema;
