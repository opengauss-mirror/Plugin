create schema uint_auto_increment;
set current_schema to 'uint_auto_increment';
-- test CREATE TABLE with AUTO_INCREMENT
-- syntax error
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment key, name varchar(200),a int unsigned);
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment unique key, name varchar(200),a int unsigned);
drop table test_create_autoinc_err;
-- constraint error
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment, name varchar(200),a int unsigned, primary key(name, id));
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment, name varchar(200),a int unsigned, unique(name, id));
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment primary key CHECK (id < 500), name varchar(200),a int unsigned);
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment primary key, name varchar(200),a int unsigned CHECK ((id + a) < 500));
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment primary key DEFAULT 100, name varchar(200),a int unsigned);
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment primary key GENERATED ALWAYS AS (a+1) STORED, name varchar(200),a int unsigned);

--auto_increment value error
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment primary key, name varchar(200),a int unsigned) auto_increment=1;
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment primary key, name varchar(200),a int unsigned) auto_increment=0; 
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment primary key, name varchar(200),a int unsigned) auto_increment=170141183460469231731687303715884105728;
CREATE TABLE test_create_autoinc_err(id int unsigned auto_increment primary key, name varchar(200),a int unsigned) auto_increment=1.1;
CREATE TEMPORARY TABLE test_create_autoinc_err(id int unsigned auto_increment primary key, name varchar(200),a int unsigned) auto_increment=1;
CREATE TEMPORARY TABLE test_create_autoinc_err(id int unsigned auto_increment primary key, name varchar(200),a int unsigned) auto_increment=0; 
CREATE TEMPORARY TABLE test_create_autoinc_err(id int unsigned auto_increment primary key, name varchar(200),a int unsigned) auto_increment=170141183460469231731687303715884105728;
CREATE TEMPORARY TABLE test_create_autoinc_err(id int unsigned auto_increment primary key, name varchar(200),a int unsigned) auto_increment=1.1;
-- table type error
CREATE TABLE test_create_autoinc_err(id int4 unsigned auto_increment, name varchar(200),a int unsigned, primary key(id)) with (ORIENTATION=column);
CREATE TABLE test_create_autoinc_err(id int2 unsigned auto_increment, name varchar(200),a int unsigned, primary key(id)) with (ORIENTATION=orc);
CREATE TABLE test_create_autoinc_err(id int1 unsigned auto_increment, name varchar(200),a int unsigned, primary key(id)) with (ORIENTATION=timeseries);

--test CREATE TABLE LIKE with AUTO_INCREMENT
CREATE TABLE test_create_autoinc_source(id int unsigned auto_increment primary key) AUTO_INCREMENT = 100;
INSERT INTO test_create_autoinc_source VALUES(DEFAULT);
INSERT INTO test_create_autoinc_source VALUES(DEFAULT);
SELECT id FROM test_create_autoinc_source ORDER BY 1;

--local temp table
CREATE TEMPORARY TABLE test_create_autoinc_like_temp(LIKE test_create_autoinc_source INCLUDING INDEXES);
INSERT INTO test_create_autoinc_like_temp VALUES(0);
SELECT LAST_INSERT_ID();
INSERT INTO test_create_autoinc_like_temp VALUES(DEFAULT);
ALTER TABLE test_create_autoinc_like_temp AUTO_INCREMENT=200;
INSERT INTO test_create_autoinc_like_temp VALUES(DEFAULT);
SELECT id FROM test_create_autoinc_like_temp ORDER BY 1;
TRUNCATE TABLE test_create_autoinc_like_temp;
INSERT INTO test_create_autoinc_like_temp VALUES(DEFAULT);
SELECT id FROM test_create_autoinc_like_temp ORDER BY 1;
DROP TABLE test_create_autoinc_like_temp;
CREATE TEMPORARY TABLE test_create_autoinc_like_temp(LIKE test_create_autoinc_source INCLUDING INDEXES) AUTO_INCREMENT=100;
INSERT INTO test_create_autoinc_like_temp VALUES(0);
SELECT LAST_INSERT_ID();
INSERT INTO test_create_autoinc_like_temp VALUES(DEFAULT);
ALTER TABLE test_create_autoinc_like_temp AUTO_INCREMENT=200;
INSERT INTO test_create_autoinc_like_temp VALUES(DEFAULT);
SELECT id FROM test_create_autoinc_like_temp ORDER BY 1;
TRUNCATE TABLE test_create_autoinc_like_temp;
INSERT INTO test_create_autoinc_like_temp VALUES(DEFAULT);
SELECT id FROM test_create_autoinc_like_temp ORDER BY 1;
DROP TABLE test_create_autoinc_like_temp;

DROP TABLE test_create_autoinc_source;

--test ALTER TABLE ADD COLUMN AUTO_INCREMENT
--cstore is not supported
CREATE TABLE test_alter_autoinc_col(col int unsigned) with (ORIENTATION=column);
INSERT INTO test_alter_autoinc_col VALUES(1);
ALTER TABLE test_alter_autoinc_col ADD COLUMN id int unsigned AUTO_INCREMENT primary key;
DROP TABLE test_alter_autoinc_col;
--astore with data
CREATE TABLE test_alter_autoinc(col int unsigned);
INSERT INTO test_alter_autoinc VALUES(1);
INSERT INTO test_alter_autoinc VALUES(2);

ALTER TABLE test_alter_autoinc ADD COLUMN id int unsigned AUTO_INCREMENT primary key;
SELECT id, col FROM test_alter_autoinc ORDER BY 1, 2;
insert into test_alter_autoinc(col) values (3),(4),(5);
SELECT id, col FROM test_alter_autoinc ORDER BY 1, 2;
insert into test_alter_autoinc values (1, 1);
insert into test_alter_autoinc values (6, 0);
select last_insert_id();
ALTER TABLE test_alter_autoinc AUTO_INCREMENT = 1000;
INSERT INTO test_alter_autoinc VALUES(3,DEFAULT);
SELECT id, col FROM test_alter_autoinc ORDER BY 1, 2;
TRUNCATE TABLE test_alter_autoinc;
INSERT INTO test_alter_autoinc VALUES(4,DEFAULT);
SELECT id, col FROM test_alter_autoinc ORDER BY 1, 2;
ALTER TABLE test_alter_autoinc DROP COLUMN id;
SELECT col FROM test_alter_autoinc ORDER BY 1;

ALTER TABLE test_alter_autoinc ADD id int unsigned AUTO_INCREMENT, ADD primary key(id), AUTO_INCREMENT = 100;
INSERT INTO test_alter_autoinc VALUES(5,DEFAULT);
SELECT id, col FROM test_alter_autoinc ORDER BY 1, 2;
ALTER TABLE test_alter_autoinc DROP COLUMN id, ADD id int unsigned AUTO_INCREMENT, ADD primary key(id), AUTO_INCREMENT = 200;
INSERT INTO test_alter_autoinc VALUES(6,DEFAULT);
SELECT id, col FROM test_alter_autoinc ORDER BY 1, 2;
SELECT pg_catalog.pg_get_tabledef('test_alter_autoinc');

-- supplementary tests
ALTER TABLE test_alter_autoinc AUTO_INCREMENT = 1000;
INSERT INTO test_alter_autoinc VALUES(7,DEFAULT);
SELECT id, col FROM test_alter_autoinc ORDER BY 1, 2;
ALTER TABLE test_alter_autoinc DROP COLUMN id, ADD id int unsigned AUTO_INCREMENT UNIQUE;
INSERT INTO test_alter_autoinc VALUES(8,DEFAULT);
SELECT id, col FROM test_alter_autoinc ORDER BY 1, 2;
DROP TABLE test_alter_autoinc;
--test alter table add AUTO_INCREMENT NULL UNIQUE
CREATE TABLE test_alter_autoinc(col int1 unsigned);
INSERT INTO test_alter_autoinc VALUES(1);
INSERT INTO test_alter_autoinc VALUES(2);
ALTER TABLE test_alter_autoinc ADD COLUMN id int unsigned AUTO_INCREMENT NULL UNIQUE;
INSERT INTO test_alter_autoinc VALUES(3,0);
SELECT id, col FROM test_alter_autoinc ORDER BY 1, 2;
DROP TABLE test_alter_autoinc;

--part table with data
CREATE TABLE test_alter_partition_autoinc (
    col2 INT NOT NULL,
    col3 INT NOT NULL
) PARTITION BY RANGE (col2) SUBPARTITION BY HASH (col3) (
    PARTITION col1_less_1000 VALUES LESS THAN(1000)
    (
        SUBPARTITION p1_col2_hash1,
        SUBPARTITION p1_col2_hash2
    ),
    PARTITION col1_mid_1000 VALUES LESS THAN(2000)
    (
        SUBPARTITION p2_col2_hash1,
        SUBPARTITION p2_col2_hash2
    ),
    PARTITION col1_greater_2000 VALUES LESS THAN (MAXVALUE)
    (
        SUBPARTITION p3_col2_hash1,
        SUBPARTITION p3_col2_hash2
    )
);
INSERT INTO test_alter_partition_autoinc VALUES(1,1);
INSERT INTO test_alter_partition_autoinc VALUES(2001,2001);
ALTER TABLE test_alter_partition_autoinc ADD col1 int unsigned AUTO_INCREMENT, ADD UNIQUE(col1, col2, col3), AUTO_INCREMENT = 100;
SELECT col1, col2, col3 FROM test_alter_partition_autoinc ORDER BY 1, 2;
ALTER TABLE test_alter_partition_autoinc AUTO_INCREMENT = 1000;
INSERT INTO test_alter_partition_autoinc VALUES(3001,3001,DEFAULT);
SELECT col1, col2, col3 FROM test_alter_partition_autoinc ORDER BY 1, 2;
TRUNCATE TABLE test_alter_partition_autoinc;
INSERT INTO test_alter_partition_autoinc VALUES(1,1,DEFAULT);
SELECT col1, col2, col3 FROM test_alter_partition_autoinc ORDER BY 1, 2;
DROP TABLE test_alter_partition_autoinc;

-- auto_increment in table with single column PRIMARY KEY
CREATE TABLE single_autoinc_pk(col int unsigned auto_increment PRIMARY KEY) AUTO_INCREMENT = 10;
INSERT INTO single_autoinc_pk VALUES(NULL);
SELECT LAST_INSERT_ID();
INSERT INTO single_autoinc_pk VALUES(1 - 1);
SELECT LAST_INSERT_ID();
INSERT INTO single_autoinc_pk VALUES(100);
SELECT LAST_INSERT_ID();
INSERT INTO single_autoinc_pk VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
ALTER TABLE single_autoinc_pk AUTO_INCREMENT = 1000;
SELECT LAST_INSERT_ID();
INSERT INTO single_autoinc_pk VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_pk ORDER BY 1;
UPDATE single_autoinc_pk SET col=NULL WHERE col=10;
UPDATE single_autoinc_pk SET col=0 WHERE col=11;
SELECT col FROM single_autoinc_pk ORDER BY 1;
UPDATE single_autoinc_pk SET col=DEFAULT WHERE col=100;
UPDATE single_autoinc_pk SET col=3000 WHERE col=0;
SELECT LAST_INSERT_ID();
INSERT INTO single_autoinc_pk VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_pk ORDER BY 1;
SELECT pg_catalog.pg_get_tabledef('single_autoinc_pk');
ALTER TABLE single_autoinc_pk ADD CONSTRAINT test_alter_single_autoinc_pk_u1 UNIQUE(col);
ALTER TABLE single_autoinc_pk ADD CONSTRAINT test_alter_single_autoinc_pk_u2 UNIQUE(col);
SELECT pg_catalog.pg_get_tabledef('single_autoinc_pk');
ALTER TABLE single_autoinc_pk DROP CONSTRAINT test_alter_single_autoinc_pk_u2;
SELECT pg_catalog.pg_get_tabledef('single_autoinc_pk');
DROP TABLE single_autoinc_pk;

-- auto_increment in table with single column NULL UNIQUE
CREATE TABLE single_autoinc_uk(col int unsigned auto_increment NULL UNIQUE) AUTO_INCREMENT = 10;
INSERT INTO single_autoinc_uk VALUES(NULL);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
INSERT INTO single_autoinc_uk VALUES(1 - 1);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
INSERT INTO single_autoinc_uk VALUES(100);
SELECT col FROM single_autoinc_uk ORDER BY 1;
INSERT INTO single_autoinc_uk VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
ALTER TABLE single_autoinc_uk AUTO_INCREMENT = 1000;
INSERT INTO single_autoinc_uk VALUES(0);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
UPDATE single_autoinc_uk SET col=NULL WHERE col=11;
SELECT col FROM single_autoinc_uk ORDER BY 1;
UPDATE single_autoinc_uk SET col=0 WHERE col=100;
SELECT col FROM single_autoinc_uk ORDER BY 1;
UPDATE single_autoinc_uk SET col=DEFAULT WHERE col=1000;
SELECT col FROM single_autoinc_uk ORDER BY 1;
UPDATE single_autoinc_uk SET col=3000 WHERE col=0;
INSERT INTO single_autoinc_uk VALUES(0);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
SELECT pg_catalog.pg_get_tabledef('single_autoinc_uk');
DROP TABLE single_autoinc_uk;

-- test auto_increment with rollback
CREATE TABLE single_autoinc_rollback(col int unsigned auto_increment PRIMARY KEY) AUTO_INCREMENT = 10;

begin transaction;
INSERT INTO single_autoinc_rollback VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_rollback ORDER BY 1;
rollback;
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_rollback ORDER BY 1;

begin transaction;
INSERT INTO single_autoinc_rollback VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_rollback ORDER BY 1;
commit;

begin transaction;
INSERT INTO single_autoinc_rollback VALUES(4000);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_rollback ORDER BY 1;
rollback;
SELECT col FROM single_autoinc_rollback ORDER BY 1;

begin transaction;
INSERT INTO single_autoinc_rollback VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_rollback ORDER BY 1;
commit;

DROP TABLE single_autoinc_rollback;

--test auto_increment if duplcate key error
create table test_autoinc_duplicate_err
(
    id     int unsigned auto_increment primary key,
    gender int unsigned         null,
    name   varchar(50) null,
    unique (gender,name)
);
insert into test_autoinc_duplicate_err (gender,name) values (1,'Gauss');
SELECT LAST_INSERT_ID();
insert into test_autoinc_duplicate_err (gender,name) values (1,'Gauss');
SELECT LAST_INSERT_ID();
insert into test_autoinc_duplicate_err (gender,name) values (1,'Euler');
SELECT LAST_INSERT_ID();
select * from test_autoinc_duplicate_err order by id;
insert into test_autoinc_duplicate_err (id,gender,name) values (10,1,'Gauss');
select * from test_autoinc_duplicate_err order by id;
insert into test_autoinc_duplicate_err (gender,name) values (1,'Newton');
select * from test_autoinc_duplicate_err order by id;

drop table test_autoinc_duplicate_err;

--test auto_increment with check error
CREATE TABLE test_autoinc_with_check(
    col1 int unsigned auto_increment primary key, col2 int unsigned, CHECK (col2 >0)
);
INSERT INTO test_autoinc_with_check VALUES(DEFAULT, 1);
INSERT INTO test_autoinc_with_check VALUES(DEFAULT, 1);
INSERT INTO test_autoinc_with_check VALUES(DEFAULT, 2);
SELECT col1,col2 FROM test_autoinc_with_check ORDER BY 1;
DROP TABLE test_autoinc_with_check;

-- auto_increment in ustore table PRIMARY KEY
CREATE TABLE ustore_single_autoinc(col int unsigned auto_increment PRIMARY KEY) WITH (STORAGE_TYPE=USTORE);
INSERT INTO ustore_single_autoinc VALUES(NULL);
SELECT LAST_INSERT_ID();
INSERT INTO ustore_single_autoinc VALUES(1 - 1);
SELECT LAST_INSERT_ID();
INSERT INTO ustore_single_autoinc VALUES(100);
SELECT LAST_INSERT_ID();
INSERT INTO ustore_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
ALTER TABLE ustore_single_autoinc AUTO_INCREMENT = 1000;
SELECT LAST_INSERT_ID();
INSERT INTO ustore_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM ustore_single_autoinc ORDER BY 1;
INSERT INTO ustore_single_autoinc select col-col from ustore_single_autoinc;
SELECT LAST_INSERT_ID();
SELECT col FROM ustore_single_autoinc ORDER BY 1;
INSERT INTO ustore_single_autoinc VALUES(500);
SELECT LAST_INSERT_ID();
INSERT INTO ustore_single_autoinc VALUES(1);
SELECT LAST_INSERT_ID();
INSERT INTO ustore_single_autoinc VALUES(2100), (NULL), (2000), (DEFAULT), (2200), (NULL);
SELECT LAST_INSERT_ID();
SELECT col FROM ustore_single_autoinc ORDER BY 1;
UPDATE ustore_single_autoinc SET col=NULL WHERE col=2201;
SELECT col FROM ustore_single_autoinc ORDER BY 1;
UPDATE ustore_single_autoinc SET col=0 WHERE col=2200;
SELECT col FROM ustore_single_autoinc ORDER BY 1;
UPDATE ustore_single_autoinc SET col=DEFAULT WHERE col=2101;
UPDATE ustore_single_autoinc SET col=3000 WHERE col=1;
SELECT LAST_INSERT_ID();
INSERT INTO ustore_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM ustore_single_autoinc ORDER BY 1;
SELECT LAST_INSERT_ID();
SELECT col FROM ustore_single_autoinc ORDER BY 1;
SELECT pg_catalog.pg_get_tabledef('ustore_single_autoinc');
TRUNCATE TABLE ustore_single_autoinc;
INSERT INTO ustore_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM ustore_single_autoinc ORDER BY 1;
DROP TABLE ustore_single_autoinc;

-- auto_increment in local temp table PRIMARY KEY
CREATE TEMPORARY TABLE ltemp_single_autoinc(col int unsigned auto_increment PRIMARY KEY);
INSERT INTO ltemp_single_autoinc VALUES(NULL);
SELECT LAST_INSERT_ID();
INSERT INTO ltemp_single_autoinc VALUES(1 - 1);
SELECT LAST_INSERT_ID();
INSERT INTO ltemp_single_autoinc VALUES(100);
SELECT LAST_INSERT_ID();
INSERT INTO ltemp_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
ALTER TABLE ltemp_single_autoinc AUTO_INCREMENT = 1000;
SELECT LAST_INSERT_ID();
INSERT INTO ltemp_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM ltemp_single_autoinc ORDER BY 1;
INSERT INTO ltemp_single_autoinc select col-col from ltemp_single_autoinc;
SELECT LAST_INSERT_ID();
SELECT col FROM ltemp_single_autoinc ORDER BY 1;
INSERT INTO ltemp_single_autoinc VALUES(500);
SELECT LAST_INSERT_ID();
INSERT INTO ltemp_single_autoinc VALUES(1);
SELECT LAST_INSERT_ID();
INSERT INTO ltemp_single_autoinc VALUES(2100), (NULL), (2000), (DEFAULT), (2200), (NULL);
SELECT LAST_INSERT_ID();
SELECT col FROM ltemp_single_autoinc ORDER BY 1;
UPDATE ltemp_single_autoinc SET col=NULL WHERE col=2201;
SELECT col FROM ltemp_single_autoinc ORDER BY 1;
UPDATE ltemp_single_autoinc SET col=0 WHERE col=2200;
SELECT col FROM ltemp_single_autoinc ORDER BY 1;
UPDATE ltemp_single_autoinc SET col=DEFAULT WHERE col=2101;
UPDATE ltemp_single_autoinc SET col=3000 WHERE col=1;
SELECT LAST_INSERT_ID();
INSERT INTO ltemp_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM ltemp_single_autoinc ORDER BY 1;
SELECT LAST_INSERT_ID();
SELECT col FROM ltemp_single_autoinc ORDER BY 1;
SELECT pg_catalog.pg_get_tabledef('ltemp_single_autoinc');
TRUNCATE TABLE ltemp_single_autoinc;
INSERT INTO ltemp_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM ltemp_single_autoinc ORDER BY 1;
DROP TABLE ltemp_single_autoinc;

-- auto_increment in local temp table NULL UNIQUE
CREATE TEMPORARY TABLE single_autoinc_uk(col int unsigned auto_increment NULL UNIQUE) AUTO_INCREMENT = 10;
INSERT INTO single_autoinc_uk VALUES(NULL);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
INSERT INTO single_autoinc_uk VALUES(1 - 1);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
INSERT INTO single_autoinc_uk VALUES(100);
SELECT col FROM single_autoinc_uk ORDER BY 1;
INSERT INTO single_autoinc_uk VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
ALTER TABLE single_autoinc_uk AUTO_INCREMENT = 1000;
INSERT INTO single_autoinc_uk VALUES(0);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
UPDATE single_autoinc_uk SET col=NULL WHERE col=11;
SELECT col FROM single_autoinc_uk ORDER BY 1;
UPDATE single_autoinc_uk SET col=0 WHERE col=100;
SELECT col FROM single_autoinc_uk ORDER BY 1;
UPDATE single_autoinc_uk SET col=DEFAULT WHERE col=1000;
SELECT col FROM single_autoinc_uk ORDER BY 1;
UPDATE single_autoinc_uk SET col=3000 WHERE col=0;
INSERT INTO single_autoinc_uk VALUES(0);
SELECT LAST_INSERT_ID();
SELECT col FROM single_autoinc_uk ORDER BY 1;
SELECT pg_catalog.pg_get_tabledef('single_autoinc_uk');
DROP TABLE single_autoinc_uk;

--auto_increment in global temp table
CREATE GLOBAL TEMPORARY TABLE gtemp_single_autoinc(col int unsigned auto_increment PRIMARY KEY);
INSERT INTO gtemp_single_autoinc VALUES(NULL);
SELECT LAST_INSERT_ID();
INSERT INTO gtemp_single_autoinc VALUES(1 - 1);
SELECT LAST_INSERT_ID();
INSERT INTO gtemp_single_autoinc VALUES(100);
SELECT LAST_INSERT_ID();
INSERT INTO gtemp_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
ALTER TABLE gtemp_single_autoinc AUTO_INCREMENT = 1000;
SELECT LAST_INSERT_ID();
INSERT INTO gtemp_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM gtemp_single_autoinc ORDER BY 1;
INSERT INTO gtemp_single_autoinc select col-col from gtemp_single_autoinc;
SELECT LAST_INSERT_ID();
SELECT col FROM gtemp_single_autoinc ORDER BY 1;
INSERT INTO gtemp_single_autoinc VALUES(500);
SELECT LAST_INSERT_ID();
INSERT INTO gtemp_single_autoinc VALUES(1);
SELECT LAST_INSERT_ID();
INSERT INTO gtemp_single_autoinc VALUES(2100), (NULL), (2000), (DEFAULT), (2200), (NULL);
SELECT LAST_INSERT_ID();
SELECT col FROM gtemp_single_autoinc ORDER BY 1;
UPDATE gtemp_single_autoinc SET col=NULL WHERE col=2201;
SELECT col FROM gtemp_single_autoinc ORDER BY 1;
UPDATE gtemp_single_autoinc SET col=0 WHERE col=2200;
SELECT col FROM gtemp_single_autoinc ORDER BY 1;
UPDATE gtemp_single_autoinc SET col=DEFAULT WHERE col=2101;
UPDATE gtemp_single_autoinc SET col=3000 WHERE col=1;
SELECT LAST_INSERT_ID();
INSERT INTO gtemp_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM gtemp_single_autoinc ORDER BY 1;
SELECT LAST_INSERT_ID();
SELECT col FROM gtemp_single_autoinc ORDER BY 1;
SELECT pg_catalog.pg_get_tabledef('gtemp_single_autoinc');
TRUNCATE TABLE gtemp_single_autoinc;
INSERT INTO gtemp_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM gtemp_single_autoinc ORDER BY 1;
DROP TABLE gtemp_single_autoinc;

--auto_increment in unlogged table
CREATE UNLOGGED TABLE unlog_single_autoinc(col int unsigned auto_increment PRIMARY KEY);
INSERT INTO unlog_single_autoinc VALUES(NULL);
SELECT LAST_INSERT_ID();
INSERT INTO unlog_single_autoinc VALUES(1 - 1);
SELECT LAST_INSERT_ID();
INSERT INTO unlog_single_autoinc VALUES(100);
SELECT LAST_INSERT_ID();
INSERT INTO unlog_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
ALTER TABLE unlog_single_autoinc AUTO_INCREMENT = 1000;
SELECT LAST_INSERT_ID();
INSERT INTO unlog_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM unlog_single_autoinc ORDER BY 1;
INSERT INTO unlog_single_autoinc select col-col from unlog_single_autoinc;
SELECT LAST_INSERT_ID();
SELECT col FROM unlog_single_autoinc ORDER BY 1;
INSERT INTO unlog_single_autoinc VALUES(500);
SELECT LAST_INSERT_ID();
INSERT INTO unlog_single_autoinc VALUES(1);
SELECT LAST_INSERT_ID();
INSERT INTO unlog_single_autoinc VALUES(2100), (NULL), (2000), (DEFAULT), (2200), (NULL);
SELECT LAST_INSERT_ID();
SELECT col FROM unlog_single_autoinc ORDER BY 1;
UPDATE unlog_single_autoinc SET col=NULL WHERE col=2201;
SELECT col FROM unlog_single_autoinc ORDER BY 1;
UPDATE unlog_single_autoinc SET col=0 WHERE col=2200;
SELECT col FROM unlog_single_autoinc ORDER BY 1;
UPDATE unlog_single_autoinc SET col=DEFAULT WHERE col=2101;
UPDATE unlog_single_autoinc SET col=3000 WHERE col=1;
SELECT LAST_INSERT_ID();
INSERT INTO unlog_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM unlog_single_autoinc ORDER BY 1;
SELECT LAST_INSERT_ID();
SELECT col FROM unlog_single_autoinc ORDER BY 1;
SELECT pg_catalog.pg_get_tabledef('unlog_single_autoinc');
TRUNCATE TABLE unlog_single_autoinc;
INSERT INTO unlog_single_autoinc VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
SELECT col FROM unlog_single_autoinc ORDER BY 1;
DROP TABLE unlog_single_autoinc;

--auto_increment column as partkey primary key
CREATE TABLE test_part_autoinc_pk (
	col1 INT AUTO_INCREMENT,
    col2 INT NOT NULL,
    col3 INT NOT NULL,
    PRIMARY KEY(col1, col2)
) PARTITION BY RANGE (col1) SUBPARTITION BY HASH (col2) (
    PARTITION col1_less_1000 VALUES LESS THAN(1000)
    (
        SUBPARTITION p1_col2_hash1,
        SUBPARTITION p1_col2_hash2
    ),
    PARTITION col1_mid_1000 VALUES LESS THAN(2000)
    (
        SUBPARTITION p2_col2_hash1,
        SUBPARTITION p2_col2_hash2
    ),
    PARTITION col1_greater_2000 VALUES LESS THAN (MAXVALUE)
    (
        SUBPARTITION p3_col2_hash1,
        SUBPARTITION p3_col2_hash2
    )
);
INSERT INTO test_part_autoinc_pk VALUES(NULL, 1, 1);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_pk VALUES(1 - 1, 1, 1);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_pk VALUES(100, 1, 1);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_pk VALUES(DEFAULT, 1, 1);
SELECT LAST_INSERT_ID();
ALTER TABLE test_part_autoinc_pk AUTO_INCREMENT = 1000;
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_pk VALUES(DEFAULT, 1, 1);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_pk ORDER BY 1;
INSERT INTO test_part_autoinc_pk select col1-col1, col2, col3 from test_part_autoinc_pk;
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_pk ORDER BY 1;
INSERT INTO test_part_autoinc_pk VALUES(500, 1, 1);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_pk VALUES(1, 1, 1);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_pk VALUES(2100, 1, 1), (NULL, 1, 1), (2000, 1, 1), (DEFAULT, 1, 1), (2200, 1, 1), (NULL, 1, 1);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_pk ORDER BY 1;
UPDATE test_part_autoinc_pk SET col1=NULL WHERE col1=2201;
SELECT col1 FROM test_part_autoinc_pk ORDER BY 1;
UPDATE test_part_autoinc_pk SET col1=0 WHERE col1=2200;
SELECT col1 FROM test_part_autoinc_pk ORDER BY 1;
UPDATE test_part_autoinc_pk SET col1=DEFAULT WHERE col1=2101;
UPDATE test_part_autoinc_pk SET col1=3000 WHERE col1=1;
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_pk VALUES(DEFAULT, 1, 1);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_pk ORDER BY 1;
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_pk ORDER BY 1;
SELECT pg_catalog.pg_get_tabledef('test_part_autoinc_pk');
TRUNCATE TABLE test_part_autoinc_pk;
INSERT INTO test_part_autoinc_pk VALUES(DEFAULT, 1, 1);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_pk ORDER BY 1;
DROP TABLE test_part_autoinc_pk;

--auto_increment column as unique key not partkey
CREATE TABLE test_part_autoinc_unique (
	col1 INT AUTO_INCREMENT NULL,
    col2 INT NOT NULL,
    col3 INT NOT NULL,
    UNIQUE(col1, col2, col3)
) PARTITION BY RANGE (col2) SUBPARTITION BY HASH (col3) (
    PARTITION col1_less_1000 VALUES LESS THAN(1000)
    (
        SUBPARTITION p1_col2_hash1,
        SUBPARTITION p1_col2_hash2
    ),
    PARTITION col1_mid_1000 VALUES LESS THAN(2000)
    (
        SUBPARTITION p2_col2_hash1,
        SUBPARTITION p2_col2_hash2
    ),
    PARTITION col1_greater_2000 VALUES LESS THAN (MAXVALUE)
    (
        SUBPARTITION p3_col2_hash1,
        SUBPARTITION p3_col2_hash2
    )
);
INSERT INTO test_part_autoinc_unique VALUES(NULL, 1, 1);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_unique VALUES(1 - 1, 1001, 1001);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_unique VALUES(100, 2001, 2001);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_unique(col2,col3) VALUES(2001, 2001);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
ALTER TABLE test_part_autoinc_unique AUTO_INCREMENT = 1000;
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_unique VALUES(0, 1, 1);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
INSERT INTO test_part_autoinc_unique select col1-col1, col2, col3 from test_part_autoinc_unique order by 1 NULLS FIRST;
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
INSERT INTO test_part_autoinc_unique VALUES(500, 1001, 1001);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_unique VALUES(1, 2001, 2001);
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_unique VALUES(2100, 1, 1), (0, 1001, 1001), (2000, 2001, 2001), (NULL, 1, 1), (2200, 1001, 1001), (0, 2001, 2001);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
UPDATE test_part_autoinc_unique SET col1=NULL WHERE col1=2201;
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
UPDATE test_part_autoinc_unique SET col1=0 WHERE col1=2200;
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
UPDATE test_part_autoinc_unique SET col1=DEFAULT WHERE col1=2101;
SELECT LAST_INSERT_ID();
INSERT INTO test_part_autoinc_unique VALUES(0, 1, 1);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
SELECT pg_catalog.pg_get_tabledef('test_part_autoinc_unique');
TRUNCATE TABLE test_part_autoinc_unique;
INSERT INTO test_part_autoinc_unique VALUES(DEFAULT, 1, 1);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
INSERT INTO test_part_autoinc_unique VALUES(0, 1, 1);
SELECT LAST_INSERT_ID();
SELECT col1 FROM test_part_autoinc_unique ORDER BY 1;
DROP TABLE test_part_autoinc_unique;

--test insert into on duplicate AND merge into
CREATE TABLE test_autoinc_upsert(col int unsigned auto_increment UNIQUE, col1 INT);
CREATE TABLE test_autoinc_merge(col int unsigned auto_increment UNIQUE, col1 INT);
--insert test_autoinc_upsert
INSERT INTO test_autoinc_upsert VALUES(DEFAULT,0) ON DUPLICATE KEY UPDATE col1=col;
INSERT INTO test_autoinc_upsert VALUES(1,0) ON DUPLICATE KEY UPDATE col1=col;
INSERT INTO test_autoinc_upsert VALUES(2,0) ON DUPLICATE KEY UPDATE col1=col;
INSERT INTO test_autoinc_upsert VALUES(100,0) ON DUPLICATE KEY UPDATE col1=col;
SELECT LAST_INSERT_ID();
SELECT col,col1 FROM test_autoinc_upsert ORDER BY 1;
--all insert test_autoinc_merge
MERGE INTO test_autoinc_merge m USING test_autoinc_upsert u 
  ON m.col = u.col
WHEN MATCHED THEN
  UPDATE SET m.col1=u.col
WHEN NOT MATCHED THEN  
  INSERT VALUES (u.col, u.col1);
SELECT LAST_INSERT_ID();
SELECT col,col1 FROM test_autoinc_merge ORDER BY 1;
--all update test_autoinc_merge
MERGE INTO test_autoinc_merge m USING test_autoinc_upsert u 
  ON m.col = u.col
WHEN MATCHED THEN
  UPDATE SET m.col1=u.col
WHEN NOT MATCHED THEN  
  INSERT VALUES (u.col, u.col1);
SELECT LAST_INSERT_ID();
SELECT col,col1 FROM test_autoinc_merge ORDER BY 1;
SELECT LAST_INSERT_ID();
SELECT col,col1 FROM test_autoinc_merge ORDER BY 1;
--insert test_autoinc_upsert 
INSERT INTO test_autoinc_upsert VALUES(100,NULL) ON DUPLICATE KEY UPDATE col1=col;
INSERT INTO test_autoinc_upsert VALUES(DEFAULT,NULL) ON DUPLICATE KEY UPDATE col1=col;
INSERT INTO test_autoinc_upsert VALUES(50,NULL) ON DUPLICATE KEY UPDATE col1=col;
SELECT LAST_INSERT_ID();
SELECT col,col1 FROM test_autoinc_upsert ORDER BY 1;
--insert and update test_autoinc_merge
MERGE INTO test_autoinc_merge m USING test_autoinc_upsert u 
  ON m.col = u.col
WHEN MATCHED THEN
  UPDATE SET m.col1=u.col1
WHEN NOT MATCHED THEN  
  INSERT VALUES (u.col, u.col1);
SELECT LAST_INSERT_ID();
SELECT col,col1 FROM test_autoinc_merge ORDER BY 1;
--insert null/0 to test_autoinc_merge.col
MERGE INTO test_autoinc_merge m USING test_autoinc_upsert u 
  ON m.col = u.col1
WHEN MATCHED THEN
  UPDATE SET m.col1=u.col1
WHEN NOT MATCHED THEN  
  INSERT VALUES (u.col1, u.col1);
SELECT LAST_INSERT_ID();
SELECT col,col1 FROM test_autoinc_merge ORDER BY 1;
--iinsert default to test_autoinc_merge.col
MERGE INTO test_autoinc_merge m USING test_autoinc_upsert u 
  ON m.col = u.col1
WHEN MATCHED THEN
  UPDATE SET m.col1=u.col1
WHEN NOT MATCHED THEN  
  INSERT VALUES (default, u.col1);
SELECT LAST_INSERT_ID();
SELECT col,col1 FROM test_autoinc_merge ORDER BY 1;

DROP TABLE test_autoinc_upsert;
DROP TABLE test_autoinc_merge;

--ERROR test
CREATE TABLE test_autoinc_err (
    col1 int2 unsigned auto_increment primary key,
    col2 int unsigned unique
);
INSERT INTO test_autoinc_err VALUES(NULL, 1);
SELECT LAST_INSERT_ID();
INSERT INTO test_autoinc_err VALUES(DEFAULT, 1);
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_err ORDER BY 1;
INSERT INTO test_autoinc_err VALUES(DEFAULT, 3);
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_err ORDER BY 1;
INSERT INTO test_autoinc_err VALUES(0,1), (100,5);
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_err ORDER BY 1;
INSERT INTO test_autoinc_err VALUES(0,5), (6,6), (0,1), (7,7);
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_err ORDER BY 1;
INSERT INTO test_autoinc_err VALUES(DEFAULT, 5);
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_err ORDER BY 1;

DROP TABLE test_autoinc_err;

-- last_insert_id test
CREATE TABLE test_last_insert_id(col int8 unsigned auto_increment PRIMARY KEY) AUTO_INCREMENT = 10;
INSERT INTO test_last_insert_id VALUES(DEFAULT);
SELECT LAST_INSERT_ID();
INSERT INTO test_last_insert_id VALUES(0), (NULL), (50), (DEFAULT);
SELECT LAST_INSERT_ID();
SELECT LAST_INSERT_ID(100 + 100);
SELECT LAST_INSERT_ID();
SELECT LAST_INSERT_ID(NULL);
SELECT LAST_INSERT_ID();
SELECT LAST_INSERT_ID(1.1);
SELECT LAST_INSERT_ID();
DROP TABLE test_last_insert_id;

-- test auto_increment insert multi values
create table test_autoinc_batch_insert (
    col1 int8 unsigned auto_increment primary key,
    col2 int unsigned 
);

TRUNCATE TABLE test_autoinc_batch_insert;
INSERT INTO test_autoinc_batch_insert VALUES(DEFAULT, 1), (DEFAULT, 1), (DEFAULT, 1), (DEFAULT, 1), (DEFAULT, 1);
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_batch_insert ORDER BY 1;
INSERT INTO test_autoinc_batch_insert VALUES(DEFAULT, 1);
SELECT col1,col2 FROM test_autoinc_batch_insert ORDER BY 1;

TRUNCATE TABLE test_autoinc_batch_insert;
INSERT INTO test_autoinc_batch_insert VALUES(5, 1), (8, 1), (7, 1), (9, 1), (20, 1);
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_batch_insert ORDER BY 1;
INSERT INTO test_autoinc_batch_insert VALUES(DEFAULT, 1);
SELECT col1,col2 FROM test_autoinc_batch_insert ORDER BY 1;

TRUNCATE TABLE test_autoinc_batch_insert;
INSERT INTO test_autoinc_batch_insert VALUES(5, 1), (NULL, 1), (NULL, 1), (NULL, 1), (NULL, 1);
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_batch_insert ORDER BY 1;
INSERT INTO test_autoinc_batch_insert VALUES(DEFAULT, 1);
SELECT col1,col2 FROM test_autoinc_batch_insert ORDER BY 1;

TRUNCATE TABLE test_autoinc_batch_insert;
INSERT INTO test_autoinc_batch_insert VALUES(10, 1);
INSERT INTO test_autoinc_batch_insert VALUES(NULL, 1), (NULL, 1), (15, 1), (NULL, 1), (NULL, 1);
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_batch_insert ORDER BY 1;
INSERT INTO test_autoinc_batch_insert VALUES(DEFAULT, 1);
SELECT col1,col2 FROM test_autoinc_batch_insert ORDER BY 1;

drop table test_autoinc_batch_insert;

-- test auto_increment insert select
create table test_autoinc_insert_select (
    col1 int2 unsigned auto_increment primary key,
    col2 int unsigned 
);
create table test_autoinc_source (
    col1 int unsigned,
    col2 int unsigned
);

TRUNCATE TABLE test_autoinc_source;
TRUNCATE TABLE test_autoinc_insert_select;
INSERT INTO test_autoinc_source VALUES(DEFAULT, 1), (DEFAULT, 2), (DEFAULT, 3), (DEFAULT, 4), (DEFAULT, 5);
INSERT INTO test_autoinc_insert_select SELECT * FROM test_autoinc_source order by col2;
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_insert_select ORDER BY 1;
INSERT INTO test_autoinc_insert_select VALUES(DEFAULT, 1);
SELECT col1,col2 FROM test_autoinc_insert_select ORDER BY 1;

TRUNCATE TABLE test_autoinc_source;
TRUNCATE TABLE test_autoinc_insert_select;
INSERT INTO test_autoinc_source VALUES(5, 1), (8, 2), (7, 3), (9, 4), (20, 5);
INSERT INTO test_autoinc_insert_select SELECT * FROM test_autoinc_source order by col2;
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_insert_select ORDER BY 1;
INSERT INTO test_autoinc_insert_select VALUES(DEFAULT, 1);
SELECT col1,col2 FROM test_autoinc_insert_select ORDER BY 1;

TRUNCATE TABLE test_autoinc_source;
TRUNCATE TABLE test_autoinc_insert_select;
INSERT INTO test_autoinc_source VALUES(5, 1), (NULL, 2), (NULL, 3), (NULL, 4), (NULL, 5);
INSERT INTO test_autoinc_insert_select SELECT * FROM test_autoinc_source order by col2;
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_insert_select ORDER BY 1;
INSERT INTO test_autoinc_insert_select VALUES(DEFAULT, 1);
SELECT col1,col2 FROM test_autoinc_insert_select ORDER BY 1;

TRUNCATE TABLE test_autoinc_source;
TRUNCATE TABLE test_autoinc_insert_select;
INSERT INTO test_autoinc_source VALUES(5, 1), (NULL, 2), (NULL, 3), (4, 4), (NULL, 5);
INSERT INTO test_autoinc_insert_select SELECT * FROM test_autoinc_source order by col2;
SELECT LAST_INSERT_ID();
SELECT col1,col2 FROM test_autoinc_insert_select ORDER BY 1;
INSERT INTO test_autoinc_insert_select VALUES(DEFAULT, 1);
SELECT col1,col2 FROM test_autoinc_insert_select ORDER BY 1;

drop table test_autoinc_source;
drop table test_autoinc_insert_select;

drop schema uint_auto_increment cascade;
reset current_schema;