create schema greatest_least;
set current_schema to 'greatest_least';
--return null if input include null
select GREATEST(null,1,2), GREATEST(null,1,2) is null;
select GREATEST(1,2);
--return null if input include null
select LEAST(null,1,2), LEAST(null,1,2) is null;
select LEAST(1,2);

-- test: expect no errors
select GREATEST(20010101, date '2005-05-05');
SELECT GREATEST(date '2005-05-05', 20010101, 20040404, 20030303);
SELECT GREATEST(date '2005-05-05', 20060101, 20040404, 20030303);
SELECT GREATEST(time '20:00:00', 120000);
CREATE TABLE t1 (a DATETIME, b BLOB);
SELECT GREATEST(a, b) FROM t1;
DROP TABLE t1;
DO GREATEST(RIGHT('11',1),1);
select LEAST(20010101, date '2005-05-05');
SELECT LEAST(date '2005-05-05', 20010101, 20040404, 20030303);
SELECT LEAST(date '2000-05-05', 20010101, 20040404, 20030303);
SELECT LEAST(time '20:00:00', 120000);
CREATE TABLE t1 (a DATETIME, b BLOB);
SELECT LEAST(a, b) FROM t1;
DROP TABLE t1;
DO LEAST(RIGHT('11',1),1);
SELECT LEAST(date '2005-05-05', time '20:00:00');
SELECT GREATEST(date '2005-05-05', time '20:00:00');
select GREATEST(20010101, timestamp '99991230121212');

drop schema greatest_least cascade;
reset current_schema;
