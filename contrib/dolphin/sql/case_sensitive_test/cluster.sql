\c table_name_test_db;
SET lower_case_table_names TO 0;
--
--  CLUSTER
--

CREATE TABLE Clstr_tst_s (rf_a SERIAL PRIMARY KEY,
	b INT);

CREATE TABLE Clstr_tst (a SERIAL PRIMARY KEY,
	b INT,
	c TEXT,
	d TEXT
	);

CREATE INDEX Clstr_tst_b ON Clstr_tst (b);
CREATE INDEX Clstr_tst_c ON Clstr_tst (c);
CREATE INDEX Clstr_tst_c_b ON Clstr_tst (c,b);
CREATE INDEX Clstr_tst_b_c ON Clstr_tst (b,c);

INSERT INTO Clstr_tst_s (b) VALUES (0);
INSERT INTO Clstr_tst_s (b) SELECT b FROM Clstr_tst_s;
INSERT INTO Clstr_tst_s (b) SELECT b FROM Clstr_tst_s;
INSERT INTO Clstr_tst_s (b) SELECT b FROM Clstr_tst_s;
INSERT INTO Clstr_tst_s (b) SELECT b FROM Clstr_tst_s;
INSERT INTO Clstr_tst_s (b) SELECT b FROM Clstr_tst_s;

-- CREATE TABLE Clstr_tst_inh () INHERITS (Clstr_tst);

INSERT INTO Clstr_tst (b, c) VALUES (11, 'once');
INSERT INTO Clstr_tst (b, c) VALUES (10, 'diez');
INSERT INTO Clstr_tst (b, c) VALUES (31, 'treinta y uno');
INSERT INTO Clstr_tst (b, c) VALUES (22, 'veintidos');
INSERT INTO Clstr_tst (b, c) VALUES (3, 'tres');
INSERT INTO Clstr_tst (b, c) VALUES (20, 'veinte');
INSERT INTO Clstr_tst (b, c) VALUES (23, 'veintitres');
INSERT INTO Clstr_tst (b, c) VALUES (21, 'veintiuno');
INSERT INTO Clstr_tst (b, c) VALUES (4, 'cuatro');
INSERT INTO Clstr_tst (b, c) VALUES (14, 'catorce');
INSERT INTO Clstr_tst (b, c) VALUES (2, 'dos');
INSERT INTO Clstr_tst (b, c) VALUES (18, 'dieciocho');
INSERT INTO Clstr_tst (b, c) VALUES (27, 'veintisiete');
INSERT INTO Clstr_tst (b, c) VALUES (25, 'veinticinco');
INSERT INTO Clstr_tst (b, c) VALUES (13, 'trece');
INSERT INTO Clstr_tst (b, c) VALUES (28, 'veintiocho');
INSERT INTO Clstr_tst (b, c) VALUES (32, 'treinta y dos');
INSERT INTO Clstr_tst (b, c) VALUES (5, 'cinco');
INSERT INTO Clstr_tst (b, c) VALUES (29, 'veintinueve');
INSERT INTO Clstr_tst (b, c) VALUES (1, 'uno');
INSERT INTO Clstr_tst (b, c) VALUES (24, 'veinticuatro');
INSERT INTO Clstr_tst (b, c) VALUES (30, 'treinta');
INSERT INTO Clstr_tst (b, c) VALUES (12, 'doce');
INSERT INTO Clstr_tst (b, c) VALUES (17, 'diecisiete');
INSERT INTO Clstr_tst (b, c) VALUES (9, 'nueve');
INSERT INTO Clstr_tst (b, c) VALUES (19, 'diecinueve');
INSERT INTO Clstr_tst (b, c) VALUES (26, 'veintiseis');
INSERT INTO Clstr_tst (b, c) VALUES (15, 'quince');
INSERT INTO Clstr_tst (b, c) VALUES (7, 'siete');
INSERT INTO Clstr_tst (b, c) VALUES (16, 'dieciseis');
INSERT INTO Clstr_tst (b, c) VALUES (8, 'ocho');
-- This entry is needed to test that TOASTED values are copied correctly.
INSERT INTO Clstr_tst (b, c, d) VALUES (6, 'seis', repeat('xyzzy', 100000));

CLUSTER Clstr_tst_c ON Clstr_tst;

SELECT a,b,c,substring(d for 30), length(d) from Clstr_tst ORDER BY a, b, c;
SELECT a,b,c,substring(d for 30), length(d) from Clstr_tst ORDER BY a;
SELECT a,b,c,substring(d for 30), length(d) from Clstr_tst ORDER BY b;
SELECT a,b,c,substring(d for 30), length(d) from Clstr_tst ORDER BY c;

-- Verify that inheritance link still works
-- INSERT INTO Clstr_tst_inh VALUES (0, 100, 'in child table');
SELECT a,b,c,substring(d for 30), length(d) from Clstr_tst ORDER BY a, b, c;

-- Verify that foreign key link still works
INSERT INTO Clstr_tst (b, c) VALUES (1111, 'this should fail');

SELECT conname FROM pg_constraint WHERE conrelid = 'Clstr_tst'::regclass
ORDER BY 1;


SELECT relname, relkind,
    EXISTS(SELECT 1 FROM pg_class WHERE oid = c.reltoastrelid) AS hastoast
FROM pg_class c WHERE relname LIKE 'Clstr_tst%' ORDER BY relname;

-- Verify that indisclustered is correctly set
SELECT pg_class.relname FROM pg_index, pg_class, pg_class AS pg_class_2
WHERE pg_class.oid=indexrelid
	AND indrelid=pg_class_2.oid
	AND pg_class_2.relname = 'Clstr_tst'
	AND indisclustered;

-- Try changing indisclustered
ALTER TABLE Clstr_tst CLUSTER ON Clstr_tst_b_c;
SELECT pg_class.relname FROM pg_index, pg_class, pg_class AS pg_class_2
WHERE pg_class.oid=indexrelid
	AND indrelid=pg_class_2.oid
	AND pg_class_2.relname = 'Clstr_tst'
	AND indisclustered;

-- Try turning off all clustering
ALTER TABLE Clstr_tst SET WITHOUT CLUSTER;
SELECT pg_class.relname FROM pg_index, pg_class, pg_class AS pg_class_2
WHERE pg_class.oid=indexrelid
	AND indrelid=pg_class_2.oid
	AND pg_class_2.relname = 'Clstr_tst'
	AND indisclustered;

-- Verify that clustering all tables does in fact cluster the right ones
CREATE USER Clstr_user PASSWORD 'ttest@123';
CREATE TABLE Clstr_1 (a INT PRIMARY KEY);
CREATE TABLE Clstr_2 (a INT PRIMARY KEY);
CREATE TABLE Clstr_3 (a INT PRIMARY KEY);
ALTER TABLE Clstr_1 OWNER TO Clstr_user;
ALTER TABLE Clstr_3 OWNER TO Clstr_user;
GRANT SELECT ON Clstr_2 TO Clstr_user;
INSERT INTO Clstr_1 VALUES (2);
INSERT INTO Clstr_1 VALUES (1);
INSERT INTO Clstr_2 VALUES (2);
INSERT INTO Clstr_2 VALUES (1);
INSERT INTO Clstr_3 VALUES (2);
INSERT INTO Clstr_3 VALUES (1);

-- "CLUSTER <tablename>" on a table that hasn't been clustered
CLUSTER Clstr_2;

CLUSTER Clstr_1_pkey ON Clstr_1;
CLUSTER Clstr_2 USING Clstr_2_pkey;
SELECT * FROM Clstr_1 UNION ALL
  SELECT * FROM Clstr_2 UNION ALL
  SELECT * FROM Clstr_3
  ORDER BY 1;

-- revert to the original state
DELETE FROM Clstr_1;
DELETE FROM Clstr_2;
DELETE FROM Clstr_3;
INSERT INTO Clstr_1 VALUES (2);
INSERT INTO Clstr_1 VALUES (1);
INSERT INTO Clstr_2 VALUES (2);
INSERT INTO Clstr_2 VALUES (1);
INSERT INTO Clstr_3 VALUES (2);
INSERT INTO Clstr_3 VALUES (1);

-- this user can only cluster Clstr_1 and Clstr_3, but the latter
-- has not been clustered
SET SESSION AUTHORIZATION Clstr_user PASSWORD 'ttest@123';
CLUSTER;
SELECT * FROM Clstr_1 UNION ALL
  SELECT * FROM Clstr_2 UNION ALL
  SELECT * FROM Clstr_3
  ORDER BY 1;

-- cluster a single table using the indisclustered bit previously set
DELETE FROM Clstr_1;
INSERT INTO Clstr_1 VALUES (2);
INSERT INTO Clstr_1 VALUES (1);
CLUSTER Clstr_1;
SELECT * FROM Clstr_1
ORDER BY 1;

-- Test MVCC-safety of cluster. There isn't much we can do to verify the
-- results with a single backend...

CREATE TABLE clustertest (key_a int PRIMARY KEY);

INSERT INTO clustertest VALUES (10);
INSERT INTO clustertest VALUES (20);
INSERT INTO clustertest VALUES (30);
INSERT INTO clustertest VALUES (40);
INSERT INTO clustertest VALUES (50);

-- Use a transaction so that updates are not committed when CLUSTER sees 'em
START TRANSACTION;

-- Test update where the old row version is found first in the scan
UPDATE clustertest SET key_a = 100 WHERE key_a = 10;

-- Test update where the new row version is found first in the scan
UPDATE clustertest SET key_a = 35 WHERE key_a = 40;

-- Test longer update chain
UPDATE clustertest SET key_a = 60 WHERE key_a = 50;
UPDATE clustertest SET key_a = 70 WHERE key_a = 60;
UPDATE clustertest SET key_a = 80 WHERE key_a = 70;

SELECT * FROM clustertest ORDER BY 1;
CLUSTER clustertest_pkey ON clustertest;
SELECT * FROM clustertest ORDER BY 1;

COMMIT;

SELECT * FROM clustertest ORDER BY 1;

-- check that temp tables can be clustered
-- Enforce use of COMMIT instead of 2PC for temporary objects
RESET SESSION AUTHORIZATION;
SET SESSION AUTHORIZATION Clstr_user PASSWORD 'ttest@123';

create temp table Clstr_temp (col1 int primary key, col2 text);
insert into Clstr_temp values (2, 'two'), (1, 'one');
cluster Clstr_temp using Clstr_temp_pkey;
select * from Clstr_temp order by 1;
drop table Clstr_temp;
DROP TABLE Clstr_2;
DROP TABLE Clstr_tst_s;

-- clean up
\c -
RESET SESSION AUTHORIZATION;
DROP USER "Clstr_user" CASCADE;
