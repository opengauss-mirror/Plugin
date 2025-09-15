---------------------------------- simple sequece testcases ---------------------------

---------------------------------- TEST 1: SIMPLE SEQUENCE  ------------------------------------
CREATE TABLE t1 (id SERIAL PRIMARY KEY, name TEXT);
SELECT create_distributed_table('t1', 'id', shard_count:=4);
\d t1;
INSERT INTO t1(name) VALUES('aaaaaaaaaa'), ('bbbbbbbbb'), ('ccccccccccccc'), ('dddddddddddddd');
SELECT * FROM t1 ORDER BY id;
-- error duplicated primary key
INSERT INTO t1 VALUES(1, 'eeeeeeeeeeeeeeee');
SELECT * FROM t1 ORDER BY id;

INSERT INTO t1 VALUES(5, 'eeeeeeeeeeeeeeee');
SELECT * FROM t1 ORDER BY id;

INSERT INTO t1(name) VALUES('aaaaaaaaaa'), ('bbbbbbbbb'), ('ccccccccccccc'), ('dddddddddddddd');
INSERT INTO t1(name) VALUES('aaaaaaaaaa'), ('bbbbbbbbb'), ('ccccccccccccc'), ('dddddddddddddd');
SELECT * FROM t1 ORDER BY id;
DROP TABLE t1;
SELECT table_schema AS "Schema", table_name AS "Name", table_type AS "Type"
  FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_schema, table_name;

------------------------------ TEST 2: alter sequence owned to table --------------------------
CREATE SEQUENCE seq1;
CREATE SEQUENCE seq2 START WITH 1000;
CREATE TABLE t1 (id INT DEFAULT nextval('seq1'), name TEXT, id2 INT DEFAULT nextval('seq2'));
SELECT create_distributed_table('t1', 'id', shard_count:=4);
\d t1;
INSERT INTO t1(name) VALUES('aaaaaaaaaa'), ('bbbbbbbbb'), ('ccccccccccccc'), ('dddddddddddddd');
SELECT * FROM t1 ORDER BY id, id2;
--Note: no error
INSERT INTO t1 VALUES(1, 'eeeeeeeeeeeeeeee');
SELECT * FROM t1 ORDER BY id, id2;

INSERT INTO t1 VALUES(5, 'eeeeeeeeeeeeeeee');
INSERT INTO t1(name) VALUES('aaaaaaaaaa'), ('bbbbbbbbb'), ('ccccccccccccc'), ('dddddddddddddd');
SELECT * FROM t1 ORDER BY id, id2;
ALTER SEQUENCE seq1 OWNED BY t1.id;
\d t1
SELECT table_schema AS "Schema", table_name AS "Name", table_type AS "Type"
  FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_schema, table_name;
DROP TABLE t1;
SELECT table_schema AS "Schema", table_name AS "Name", table_type AS "Type"
  FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_schema, table_name;
DROP SEQUENCE seq2;
SELECT table_schema AS "Schema", table_name AS "Name", table_type AS "Type"
  FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_schema, table_name;

------------------------------ TEST 3: add new sequece to distributed table ----------------
CREATE TABLE t1 (id INT, name TEXT);
INSERT INTO t1 VALUES(1, 'aaaaaaaa'),(2, 'bbbbbbbbbb'),(3, 'cccccccccc'),(4, 'ddddddddddddddd');
SELECT create_distributed_table('t1', 'id', shard_count:=4);
\d t1;
CREATE SEQUENCE seq1;
ALTER TABLE t1 ALTER COLUMN id SET DEFAULT nextval('seq1');
ALTER TABLE t1 add COLUMN id_new int not null DEFAULT nextval('seq1');

ALTER TABLE t1 add COLUMN id_new integer;
UPDATE t1 SET id_new = nextval('seq1');
ALTER TABLE t1 ALTER COLUMN id_new SET DEFAULT nextval('seq1');
\d t1;
ALTER SEQUENCE seq1 OWNED BY t1.id_new;

SELECT alter_distributed_table('t1','id_new', shard_count:=4);
INSERT INTO t1(id, name) VALUES (5, 'aaaaaaaa');
SELECT * FROM t1 ORDER BY id;

UPDATE t1 SET id_new=id;
SELECT * FROM t1 ORDER BY id;

SELECT alter_distributed_table('t1','id_new', shard_count:=4);
\d t1;
INSERT INTO t1(id, name) VALUES (5, 'aaaaaaaa');
SELECT * FROM t1 ORDER BY id;

SELECT setval('seq1', 100);
INSERT INTO t1(id, name) VALUES (5, 'aaaaaaaa');
SELECT * FROM t1 ORDER BY id_new;
DROP TABLE t1;
SELECT table_schema AS "Schema", table_name AS "Name", table_type AS "Type"
  FROM information_schema.tables WHERE table_schema = 'public' ORDER BY table_schema, table_name;
