create schema test_lag_lead;
set current_schema to 'test_lag_lead';
set dolphin.b_compatibility_mode to on;
-- 1、3参数类型兼容
CREATE TABLE test_lag_lead_table (d DOUBLE, id INT, sex CHAR(1), n INT NOT NULL AUTO_INCREMENT, PRIMARY KEY(n));
SELECT n, id, LAG(id, 0, n*n) OVER (ORDER BY id DESC, n ASC ROWS BETWEEN CURRENT ROW AND 2 FOLLOWING) L FROM test_lag_lead_table;
INSERT INTO test_lag_lead_table values(1.0, 1, 'F', 1);
INSERT INTO test_lag_lead_table values(2.0, 2, 'M', 2);
SELECT n, id, LAG(id, 1, n*n) OVER (ORDER BY id DESC, n ASC ROWS BETWEEN CURRENT ROW AND 2 FOLLOWING) L FROM test_lag_lead_table;
-- 1参数类型unknown、NULL兼容
SELECT LEAD(NULL) OVER ();
SELECT LAG(NULL) OVER ();
SELECT LEAD(NULL, 0) OVER ();
SELECT LAG(NULL, 0) OVER ();
SELECT LEAD(NULL, 1, 'test') OVER ();
SELECT LAG(NULL, 1, 'test') OVER ();
SELECT LEAD('] .| /= ') OVER ();
SELECT LAG('] .| /= ') OVER ();
SELECT LEAD('] .| /= ', 0) OVER ();
SELECT LAG('] .| /= ', 0) OVER ();
SELECT LEAD('] .| /= ', 1, 'test') OVER ();
SELECT LAG('] .| /= ', 1, 'test') OVER ();

drop table test_lag_lead_table;
set dolphin.b_compatibility_mode to off;
drop schema test_lag_lead cascade;
reset current_schema;
