\c table_name_test_db;
SET lower_case_table_names TO 0;

--
--FOR BLACKLIST FEATURE: SEQUENCE、EXCLUDE、InheRITS is not supported.
--

/* Test Inheritance of structure (LIKE) */
CREATE TABLE `Inhx` (`xx` text DEFAULT 'text');

/*
 * Test double Inheritance
 *
 * Ensure that defaults are NOT included unless
 * INCLUDING DEFAULTS is specified
 */
CREATE TABLE `Ctla` (`aa` TEXT);
CREATE TABLE `Ctlb` (`bb` TEXT) InheRITS (`Ctla`);

CREATE TABLE `Foo` (LIKE `nonexistent`);

CREATE TABLE `Inhe` (`ee` text, LIKE `Inhx`) Inherits (`Ctlb`);
INSERT INTO `Inhe` VALUES ('ee-col1', 'ee-col2', DEFAULT, 'ee-col4');
SELECT * FROM `Inhe`; /* Columns aa, bb, xx value NULL, ee */
SELECT * FROM `Inhx`; /* Empty set since LIKE Inherits structure only */
SELECT * FROM `Ctlb`; /* Has ee entry */
SELECT * FROM `Ctla`; /* Has ee entry */

CREATE TABLE `inhf` (LIKE `Inhx`, LIKE `Inhx`); /* Throw error */

CREATE TABLE `inhf` (LIKE `Inhx` INCLUDING DEFAULTS INCLUDING CONSTRAINTS);
INSERT INTO `inhf` DEFAULT VALUES;
SELECT * FROM `inhf`; /* Single entry with value 'text' */

ALTER TABLE `Inhx` add constraint `Foo` CHECK (`xx` = 'text');
ALTER TABLE `Inhx` ADD PRIMARY KEY (`xx`);
CREATE TABLE `Inhg` (LIKE `Inhx`); /* Doesn't copy constraint */
INSERT INTO `Inhg` VALUES ('foo');
DROP TABLE `Inhg`;
CREATE TABLE `Inhg` (`x` text, LIKE `Inhx` INCLUDING CONSTRAINTS, `y` text); /* Copies constraints */
INSERT INTO `Inhg` VALUES ('x', 'text', 'y'); /* Succeeds */
INSERT INTO `Inhg` VALUES ('x', 'text', 'y'); /* Succeeds -- Unique constraints not copied */
INSERT INTO `Inhg` VALUES ('x', 'foo',  'y');  /* fails due to constraint */
SELECT * FROM `Inhg`; /* Two records with three columns in order x=x, xx=text, y=y */
DROP TABLE `Inhg`;

CREATE TABLE `Inhg` (`x` text, LIKE `Inhx` INCLUDING INDEXES, `y` text) DISTRIBUTE BY REPLICATION; /* copies indexes */
INSERT INTO `Inhg` VALUES (5, 10);
INSERT INTO `Inhg` VALUES (20, 10); -- should fail
DROP TABLE `Inhg`;
/* Multiple primary keys creation should fail */
CREATE TABLE `Inhg` (`x` text, LIKE `Inhx` INCLUDING INDEXES, PRIMARY KEY(`x`)); /* fails */
CREATE TABLE `Inhz` (`xx` text DEFAULT 'text', `yy` int UNIQUE) DISTRIBUTE BY REPLICATION;
CREATE UNIQUE INDEX `Inhz_xx_idx` on `Inhz` (`xx`) WHERE `xx` <> 'Test';
/* Ok to create multiple unique indexes */
CREATE TABLE `Inhg` (x text UNIQUE, LIKE `Inhz` INCLUDING INDEXES) DISTRIBUTE BY REPLICATION;
INSERT INTO `Inhg` (`xx`, `yy`, `x`) VALUES ('Test', 5, 10);
INSERT INTO `Inhg` (`xx`, `yy`, `x`) VALUES ('Test', 10, 15);
INSERT INTO `Inhg` (`xx`, `yy`, `x`) VALUES ('foo', 10, 15); -- should fail
DROP TABLE `Inhg`;
DROP TABLE `Inhz`;

-- including storage and comments
CREATE TABLE `Ctlt1` (`a` text CHECK (length(`a`) > 2) PRIMARY KEY, `b` text);
CREATE INDEX `ctlt1_b_key` ON `Ctlt1` (`b`);
CREATE INDEX `ctlt1_fnidx` ON `Ctlt1` ((`a` || `b`));
COMMENT ON COLUMN `Ctlt1`.a IS 'A';
COMMENT ON COLUMN `Ctlt1`.b IS 'B';
COMMENT ON CONSTRAINT ctlt1_a_check ON `Ctlt1` IS 't1_a_check';
COMMENT ON INDEX ctlt1_pkey IS 'index pkey';
COMMENT ON INDEX `ctlt1_b_key` IS 'index b_key';
ALTER TABLE `Ctlt1` ALTER COLUMN `a` SET STORAGE MAIN;

CREATE TABLE `Ctlt2` (`c` text);
ALTER TABLE `Ctlt2` ALTER COLUMN `c` SET STORAGE EXTERNAL;
COMMENT ON COLUMN `Ctlt2`.`c` IS C;

CREATE TABLE `Ctlt3` (`a` text CHECK (length(`a`) < 5), `c` text);
ALTER TABLE `Ctlt3` ALTER COLUMN `c` SET STORAGE EXTERNAL;
ALTER TABLE `Ctlt3` ALTER COLUMN `a` SET STORAGE MAIN;
COMMENT ON COLUMN `Ctlt3`.`a` IS 'A3';
COMMENT ON COLUMN `Ctlt3`.`c` IS 'C';
COMMENT ON CONSTRAINT `ctlt3_a_check` ON `Ctlt3` IS 't3_a_check';

CREATE TABLE `Ctlt4` (`a` text, `c` text);
ALTER TABLE `Ctlt4` ALTER COLUMN `c` SET STORAGE EXTERNAL;

CREATE TABLE `Ctlt12_storage` (LIKE `Ctlt1` INCLUDING STORAGE, LIKE `Ctlt2` INCLUDING STORAGE);
\d+ "Ctlt12_storage"
CREATE TABLE `Ctlt12_comments` (LIKE `Ctlt1` INCLUDING COMMENTS, LIKE `Ctlt2` INCLUDING COMMENTS);
\d+ "Ctlt12_comments"
CREATE TABLE `Ctlt1_inh` (LIKE `Ctlt1` INCLUDING CONSTRAINTS INCLUDING COMMENTS) InheRITS (`Ctlt1`);
\d+ "Ctlt1_inh"
SELECT `description` FROM `pg_description`, `pg_constraint` c WHERE `classoid` = 'pg_constraint'::regclass AND `objoid` = `c`.`oid` AND `c`.`conrelid` = 'Ctlt1_inh'::regclass;
CREATE TABLE `Ctlt13_inh` () InheRITS (`Ctlt1`, `Ctlt3`);
\d+ "Ctlt13_inh"
CREATE TABLE `ctlt13_like` (LIKE `Ctlt3` INCLUDING CONSTRAINTS INCLUDING COMMENTS INCLUDING STORAGE) InheRITS (`Ctlt1`);
\d+ "ctlt13_like"
SELECT `description` FROM `pg_description`, `pg_constraint` c WHERE `classoid` = 'pg_constraint'::regclass AND `objoid` = `c`.`oid` AND `c`.`conrelid` = 'ctlt13_like'::regclass;

CREATE TABLE `Ctlt_all` (LIKE `Ctlt1` INCLUDING DEFAULTS  INCLUDING CONSTRAINTS  INCLUDING INDEXES  INCLUDING STORAGE  INCLUDING COMMENTS);
\d+ "Ctlt_all"
SELECT `c`.`relname`, `objsubid`, `description` FROM `pg_description`, `pg_index` i, `pg_class` c WHERE `classoid` = 'pg_class'::regclass AND `objoid` = `i`.`indexrelid` AND `c`.`oid` = `i`.`indexrelid` AND `i`.`indrelid` = 'Ctlt_all'::regclass ORDER BY `c`.`relname`, `objsubid`;

CREATE TABLE `Inh_error1` () InheRITS (`Ctlt1`, `Ctlt4`);
CREATE TABLE `Inh_error2` (LIKE `Ctlt4` INCLUDING STORAGE) InheRITS (`Ctlt1`);

DROP TABLE if exists `Ctlt1`, `Ctlt2`, `Ctlt3`, `Ctlt4`, `Ctlt12_storage`, `Ctlt12_comments`, `Ctlt1_inh`, `Ctlt13_inh`, `ctlt13_like`, `Ctlt_all`, `Ctla`, `Ctlb` CASCADE;


/* LIKE with other relation kinds */

CREATE TABLE `Ctlt4` (`a` int, `b` text);

CREATE SEQUENCE `ctlseq1`;
CREATE TABLE `ctlt10` (LIKE `ctlseq1`);  -- fail

CREATE VIEW `ctlv1` AS SELECT * FROM `Ctlt4`;
CREATE TABLE `Ctlt11` (LIKE `ctlv1`);
CREATE TABLE `ctlt11a` (LIKE `ctlv1` INCLUDING DEFAULTS  INCLUDING CONSTRAINTS  INCLUDING INDEXES  INCLUDING STORAGE  INCLUDING COMMENTS);

CREATE TYPE `ctlty1` AS (`a` int, `b` text);
CREATE TABLE `Ctlt12` (LIKE `ctlty1`);

-- including all, including all excluding some option(s)
CREATE TABLE `Ctlt13` (LIKE `Ctlt4` INCLUDING ALL);
CREATE TABLE `Ctlt14` (LIKE `Ctlt4` INCLUDING ALL EXCLUDING RELOPTIONS) WITH (ORIENTATION = COLUMN);
-- should fail, syntax error
CREATE TABLE `Ctlt15` (LIKE `Ctlt4` INCLUDING ALL INCLUDING RELOPTIONS);
CREATE TABLE `Ctlt16` (LIKE `Ctlt4` INCLUDING ALL EXCLUDING ALL);
CREATE TABLE `Ctlt17` (LIKE `Ctlt4` INCLUDING DEFAULTS INCLUDING CONSTRAINTS EXCLUDING ALL);
CREATE TABLE `Ctlt18` (LIKE `Ctlt4` EXCLUDING ALL);

DROP SEQUENCE `ctlseq1`;
DROP TYPE `ctlty1`;
DROP VIEW `ctlv1`;
DROP TABLE IF EXISTS `Ctlt4`, `ctlt10`, `Ctlt11`, `ctlt11a`, `Ctlt12`, `Ctlt13`, `Ctlt14`, `Ctlt16`, `Ctlt17`, `Ctlt18`;

create table `Ctltcol`(`id1` integer, `id2` integer, `id3` integer, `partial` cluster key(`id1`,`id2`))with(orientation = column);
create table `Ctltcollike`(like `Ctltcol` including all);
\d+ "Ctltcollike"
drop table `Ctltcol`;
drop table `Ctltcollike`;

create table `Test1`(`a` int, `b` int, `c` int)distribute by hash(`a`, `b`);
create table `Test` (like `Test1` including distribution);
\d+ "Test"
drop table `Test`;
drop table `Test1`;

-- including all, with oids /without oids
create table `Ctltesta`(`a1` int, `a2` int) with oids;
\d+ "Ctltesta"
create table `Ctltestb`(like `Ctltesta` including all);
\d+ "Ctltestb"
create table `Ctltestc`(like `Ctltesta` including all excluding oids);
\d+ "Ctltestc"
create table `Ctltestd`(`a1` int, `a2` int, constraint `firstkey` primary key(`a1`))with oids distribute by hash(`a1`);
\d+ "Ctltestd"
create table `Ctlteste`(like `Ctltestd` including all);
\d+ "Ctlteste"
drop table if exists `Ctltesta`, `Ctltestb`, `Ctltestc`, `Ctltestd`, `Ctlteste`;
create table `Ctltestf`(`a1` int, `a2` int, constraint `firstkey` primary key(oid)) distribute by hash(`a1`);
\d+ "Ctltestf"
create table `Ctltestg`(`a1` int, `a2` int, constraint `firstkey` primary key(oid))with oids distribute by hash(`a1`);
\d+ "Ctltestg"
drop table if exists `Ctltestf`, `Ctltestg`;

create schema `testschema`;
CREATE OR REPLACE FUNCTION `testschema`.`func_increment_plsql`(`i` integer) RETURNS integer AS $$
        BEGIN
                RETURN `i` + 1;
        END;
$$ LANGUAGE plpgsql IMMUTABLE  ;
create table `testschema`.`Test1` (`a` int , `b` int default `testschema`.`func_increment_plsql`(1));
alter schema `testschema` rename to `TESTTABLE_bak`;
create table `TESTTABLE_bak`.`Test2` (like `TESTTABLE_bak`.`Test1` including all);

drop table `TESTTABLE_bak`.`Test2`;
drop table `TESTTABLE_bak`.`Test1`;
drop function `TESTTABLE_bak`.`func_increment_plsql`();
