\c table_name_test_db;
SET dolphin.lower_case_table_names TO 0;
CREATE TABLE `Test_trigger_src_tbl`(`id1` INT, `id2` INT, `id3` INT);
CREATE TABLE `Test_trigger_des_tbl`(`id1` INT, `id2` INT, `id3` INT);

CREATE OR REPLACE FUNCTION `tri_insert_func`() RETURNS TRIGGER AS
$$
DECLARE
BEGIN
        INSERT INTO `Test_trigger_des_tbl` VALUES(`NEW`.`id1`, `NEW`.`id2`, `NEW`.`id3`);
        RETURN `NEW`;
END
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION `tri_update_func`() RETURNS TRIGGER AS
$$
DECLARE
BEGIN
        UPDATE `Test_trigger_des_tbl` SET `id3` = `NEW`.`id3` WHERE `id1`=`OLD`.`id1`;
        RETURN `OLD`;
END
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION `TRI_DELETE_FUNC`() RETURNS TRIGGER AS
$$
DECLARE
BEGIN
        DELETE FROM `Test_trigger_des_tbl` WHERE `id1`=`OLD`.`id1`;
        RETURN `OLD`;
END
$$ LANGUAGE PLPGSQL;

CREATE TRIGGER `insert_trigger`
BEFORE INSERT ON `Test_trigger_src_tbl`
FOR EACH ROW
EXECUTE PROCEDURE `tri_insert_func`();

CREATE TRIGGER `update_trigger`
AFTER UPDATE ON `Test_trigger_src_tbl`  
FOR EACH ROW
EXECUTE PROCEDURE `tri_update_func`();

CREATE TRIGGER `delete_trigger`
BEFORE DELETE ON `Test_trigger_src_tbl`
FOR EACH ROW
EXECUTE PROCEDURE `tri_delete_func`();

INSERT INTO `Test_trigger_src_tbl` VALUES(100,200,300);
SELECT * FROM `Test_trigger_src_tbl`;
SELECT * FROM `Test_trigger_des_tbl`;

UPDATE `Test_trigger_src_tbl` SET `id3`=400 WHERE `id1`=100;
SELECT * FROM `Test_trigger_src_tbl`;
SELECT * FROM `Test_trigger_des_tbl`;

DELETE FROM `Test_trigger_src_tbl` WHERE `id1`=100;
SELECT * FROM `Test_trigger_src_tbl`;
SELECT * FROM `Test_trigger_des_tbl`;

ALTER TRIGGER `delete_trigger` ON `Test_trigger_src_tbl` RENAME TO `delete_trigger_renamed`;
ALTER TABLE `Test_trigger_src_tbl` DISABLE TRIGGER `insert_trigger`;
ALTER TABLE `Test_trigger_src_tbl` DISABLE TRIGGER ALL;

DROP TRIGGER `insert_trigger` ON `Test_trigger_src_tbl`;
DROP TRIGGER `update_trigger` ON `Test_trigger_src_tbl`;
DROP TRIGGER `delete_trigger_renamed` ON `Test_trigger_src_tbl`;
