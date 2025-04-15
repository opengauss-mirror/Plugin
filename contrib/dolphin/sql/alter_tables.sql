-- ------ normal mode -------
-- test ALTER TABLE ADD COLUMN for a single column
CREATE TABLE IF NOT EXISTS altertbl_t (
    id INT PRIMARY KEY,
    name VARCHAR(50)
);

INSERT INTO altertbl_t (id, name) VALUES (1, 'vertifyname');
INSERT INTO altertbl_t (id, name) VALUES (2, 'test');
ALTER TABLE altertbl_t ADD COLUMN age INT;
\d altertbl_t

-- test ALTER TABLE ADD COLUMN for multiple columns
ALTER TABLE altertbl_t ADD COLUMN salary DECIMAL(10, 2), ADD COLUMN department VARCHAR(50);
\d altertbl_t

-- test ALTER TABLE ADD CHECK for a single constraint
ALTER TABLE altertbl_t ADD CHECK (age > 0);
INSERT INTO altertbl_t (id, name, age) VALUES (3, 'xugo', -1);
ALTER TABLE altertbl_t ADD CHECK (salary > 0), ADD CHECK (LENGTH(department) > 0);
INSERT INTO altertbl_t (id, name, age, salary, department) VALUES (4, 'wangzi', 25, -1000.00, '');

-- test ALTER TABLE ALTER COLUMN SET DEFAULT
ALTER TABLE altertbl_t ALTER COLUMN age SET DEFAULT 18;
INSERT INTO altertbl_t (id, name) VALUES (5, 'litzz');
SELECT id, name, age FROM altertbl_t WHERE id = 5;

-- test ALTER TABLE ALTER COLUMN SET DEFAULT with an expression
ALTER TABLE altertbl_t ALTER COLUMN salary SET DEFAULT (1500.00 * 1.1);
INSERT INTO altertbl_t (id, name, age) VALUES (6, 'gg', 30);
SELECT id, name, salary FROM altertbl_t WHERE id = 6;

-- test ALTER TABLE ALTER COLUMN DROP DEFAULT
ALTER TABLE altertbl_t ALTER COLUMN age DROP DEFAULT;
INSERT INTO altertbl_t (id, name) VALUES (7, 'xiaoming');
SELECT id, name, age FROM altertbl_t WHERE id = 7;

-- test ALTER TABLE DROP COLUMN for a single column
ALTER TABLE altertbl_t DROP COLUMN age;
\d altertbl_t
ALTER TABLE altertbl_t DROP COLUMN salary, DROP COLUMN department;
\d altertbl_t

-- test CREATE TABLE (basic form)
CREATE TABLE IF NOT EXISTS atnt_t (
    product_id INT PRIMARY KEY,
    product_name VARCHAR(100)
);
\d atnt_t
INSERT INTO atnt_t (product_id, product_name) VALUES (1, 'good');
SELECT * FROM atnt_t;

-- test CREATE TABLE (including CHECK)
CREATE TABLE IF NOT EXISTS checktbl_t (
    price DECIMAL(10, 2),
    quantity INT,
    CHECK (price > 0 AND quantity > 0)
);
INSERT INTO checktbl_t (price, quantity) VALUES (-10.00, 5);
INSERT INTO checktbl_t (price, quantity) VALUES (20.00, -1);
INSERT INTO checktbl_t (price, quantity) VALUES (25.00, 10);
SELECT * FROM checktbl_t;

-- test CREATE TABLE (specifying COLLATE)
CREATE TABLE IF NOT EXISTS collatetbl_t (
    code CHAR(5)
) DEFAULT COLLATE = utf8mb4_unicode_ci;
\d collatetbl_t
INSERT INTO collatetbl_t (code) VALUES ('ABCDE');
SELECT * FROM collatetbl_t;

-- test CREATE TABLE LIKE
CREATE TABLE IF NOT EXISTS liketbl_t LIKE altertbl_t;
\d liketbl_t
INSERT INTO liketbl_t (id, name) VALUES (8, 'hello');
SELECT * FROM liketbl_t;

drop table if exists atnt_t cascade;
drop table if exists checktbl_t cascade;
drop table if exists collatetbl_t cascade;
drop table if exists liketbl_t cascade;
drop table if exists altertbl_t cascade;

-- ------ temporary mode -------
-- test ALTER TABLE ADD COLUMN for a single column
CREATE TEMPORARY TABLE IF NOT EXISTS altertbl_t (
    id INT PRIMARY KEY,
    name VARCHAR(50)
);

INSERT INTO altertbl_t (id, name) VALUES (1, 'vertifyname');
INSERT INTO altertbl_t (id, name) VALUES (2, 'test');
ALTER TABLE altertbl_t ADD COLUMN age INT;
\d altertbl_t

-- test ALTER TABLE ADD COLUMN for multiple columns
ALTER TABLE altertbl_t ADD COLUMN salary DECIMAL(10, 2), ADD COLUMN department VARCHAR(50);
\d altertbl_t

-- test ALTER TABLE ADD CHECK for a single constraint
ALTER TABLE altertbl_t ADD CHECK (age > 0);
INSERT INTO altertbl_t (id, name, age) VALUES (3, 'xugo', -1);
ALTER TABLE altertbl_t ADD CHECK (salary > 0), ADD CHECK (LENGTH(department) > 0);
INSERT INTO altertbl_t (id, name, age, salary, department) VALUES (4, 'wangzi', 25, -1000.00, '');

-- test ALTER TABLE ALTER COLUMN SET DEFAULT
ALTER TABLE altertbl_t ALTER COLUMN age SET DEFAULT 18;
INSERT INTO altertbl_t (id, name) VALUES (5, 'litzz');
SELECT id, name, age FROM altertbl_t WHERE id = 5;

-- test ALTER TABLE ALTER COLUMN SET DEFAULT with an expression
ALTER TABLE altertbl_t ALTER COLUMN salary SET DEFAULT (1500.00 * 1.1);
INSERT INTO altertbl_t (id, name, age) VALUES (6, 'gg', 30);
SELECT id, name, salary FROM altertbl_t WHERE id = 6;

-- test ALTER TABLE ALTER COLUMN DROP DEFAULT
ALTER TABLE altertbl_t ALTER COLUMN age DROP DEFAULT;
INSERT INTO altertbl_t (id, name) VALUES (7, 'xiaoming');
SELECT id, name, age FROM altertbl_t WHERE id = 7;

-- test ALTER TABLE DROP COLUMN for a single column
ALTER TABLE altertbl_t DROP COLUMN age;
\d altertbl_t
ALTER TABLE altertbl_t DROP COLUMN salary, DROP COLUMN department;
\d altertbl_t

-- test CREATE TABLE (basic form)
CREATE TEMPORARY TABLE IF NOT EXISTS atnt_t (
    product_id INT PRIMARY KEY,
    product_name VARCHAR(100)
);
\d atnt_t
INSERT INTO atnt_t (product_id, product_name) VALUES (1, 'good');
SELECT * FROM atnt_t;

-- test CREATE TABLE (including CHECK)
CREATE TEMPORARY TABLE IF NOT EXISTS checktbl_t (
    price DECIMAL(10, 2),
    quantity INT,
    CHECK (price > 0 AND quantity > 0)
);
INSERT INTO checktbl_t (price, quantity) VALUES (-10.00, 5);
INSERT INTO checktbl_t (price, quantity) VALUES (20.00, -1);
INSERT INTO checktbl_t (price, quantity) VALUES (25.00, 10);
SELECT * FROM checktbl_t;

-- test CREATE TABLE (specifying COLLATE)
CREATE TEMPORARY TABLE IF NOT EXISTS collatetbl_t (
    code CHAR(5)
) DEFAULT COLLATE = utf8mb4_unicode_ci;
\d collatetbl_t
INSERT INTO collatetbl_t (code) VALUES ('ABCDE');
SELECT * FROM collatetbl_t;

-- test CREATE TABLE LIKE
CREATE TEMPORARY TABLE IF NOT EXISTS liketbl_t LIKE altertbl_t;
\d liketbl_t
INSERT INTO liketbl_t (id, name) VALUES (8, 'hello');
SELECT * FROM liketbl_t;

drop table if exists atnt_t cascade;
drop table if exists checktbl_t cascade;
drop table if exists collatetbl_t cascade;
drop table if exists liketbl_t cascade;
drop table if exists altertbl_t cascade;
