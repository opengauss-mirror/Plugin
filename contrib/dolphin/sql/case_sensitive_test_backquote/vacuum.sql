\c table_name_test_db;
SET lower_case_table_names TO 0;
--
-- VACUUM
--
CREATE TABLE `Vactst` (`i` INT);
INSERT INTO `Vactst` VALUES (1);
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` VALUES (0);
SELECT count(*) FROM `Vactst`;
DELETE FROM `Vactst` WHERE `i` != 0;
SELECT * FROM `Vactst`;
VACUUM FULL `Vactst`;
UPDATE `Vactst` SET `i` = `i` + 1;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` SELECT * FROM `Vactst`;
INSERT INTO `Vactst` VALUES (0);
SELECT count(*) FROM `Vactst`;
DELETE FROM `Vactst` WHERE `i` != 0;
VACUUM (FULL) `Vactst`;
DELETE FROM `Vactst`;
SELECT * FROM `Vactst`;

VACUUM (FULL, FREEZE) `Vactst`;
VACUUM (ANALYZE, FULL) `Vactst`;

CREATE TABLE `Vaccluster` (`i` INT PRIMARY KEY);
ALTER TABLE `Vaccluster` CLUSTER ON Vaccluster_pkey;
INSERT INTO `Vaccluster` SELECT * FROM `Vactst`;
CLUSTER `Vaccluster`;

set xc_maintenance_mode = on;
VACUUM FULL pg_am;
VACUUM FULL pg_class;
VACUUM FULL pg_database;
set xc_maintenance_mode = off;
VACUUM FULL `Vaccluster`;
VACUUM FULL `Vactst`;

DROP TABLE `Vaccluster`;
DROP TABLE `Vactst`;

CREATE TABLE "~!@#$%^&*+-=`,./\';:{}[]|0(>_<)0"(A TEXT);
VACUUM  "~!@#$%^&*+-=`,./\';:{}[]|0(>_<)0";
DROP TABLE "~!@#$%^&*+-=`,./\';:{}[]|0(>_<)0";
