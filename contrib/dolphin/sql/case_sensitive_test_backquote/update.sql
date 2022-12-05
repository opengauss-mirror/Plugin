\c table_name_test_db;
SET dolphin.lower_case_table_names TO 0;
--
-- UPDATE syntax tests
--

CREATE TABLE `Update_Test` (
    `a`   INT DEFAULT 10,
    `b`   INT,
    `c`   TEXT
);

INSERT INTO `Update_Test` VALUES (5, 10, 'foo');
INSERT INTO `Update_Test`(`b`, `a`) VALUES (15, 10);

SELECT * FROM `Update_Test` ORDER BY `a`, `b`, `c`;

UPDATE `Update_Test` SET `a` = DEFAULT, `b` = DEFAULT;

SELECT * FROM `Update_Test`  ORDER BY `a`, `b`, `c`;

-- aliases for the UPDATE target table
UPDATE `Update_Test` AS `t` SET `b` = 10 WHERE `t`.`a` = 10;

SELECT * FROM `Update_Test`  ORDER BY `a`, `b`, `c`;

UPDATE `Update_Test` `t` SET `b` = `t`.`b` + 10 WHERE `t`.`a` = 10;

SELECT * FROM `Update_Test`  ORDER BY `a`, `b`, `c`;

--
-- Test VALUES in FROM
--

UPDATE `Update_Test` SET `a`=`v`.`i` FROM (VALUES(100, 20)) AS `v`(`i`, `j`)
  WHERE `Update_Test`.`b` = `v`.`j`;

SELECT * FROM `Update_Test`  ORDER BY `a`, `b`, `c`;

--
-- Test multiple-set-clause syntax
--

UPDATE `Update_Test` SET (`c`,`b`,`a`) = ('bugle', `b`+11, DEFAULT) WHERE `c` = 'foo';
SELECT * FROM `Update_Test`  ORDER BY `a`, `b`, `c`;
UPDATE `Update_Test` SET (`c`,`b`) = ('car', `a`+`b`), `a` = `a` + 1 WHERE `a` = 10;
SELECT * FROM `Update_Test`  ORDER BY `a`, `b`, `c`;
-- fail, multi assignment to same column:
UPDATE `Update_Test` SET (`c`,`b`) = ('car', `a`+`b`), `b` = `a` + 1 WHERE `a` = 10;

-- XXX this should work, but doesn'`t` yet:
UPDATE `Update_Test` SET (`a`,`b`) = (select `a`,`b` FROM `Update_Test` where `c` = 'foo')
  WHERE `a` = 10;

-- if an alias for the target table is specified, don'`t` allow references
-- to the original table name
UPDATE `Update_Test` AS `t` SET `b` = `Update_Test`.`b` + 10 WHERE `t`.`a` = 10;

-- Make sure that we can update to `a` TOASTed value.
UPDATE `Update_Test` SET `c` = repeat('x', 10000) WHERE `c` = 'car';
SELECT `a`, `b`, char_length(`c`) FROM `Update_Test` ORDER BY `a`;

DROP TABLE `Update_Test`;

--Test "update tablename AS aliasname SET aliasname.colname = colvalue;"
CREATE TABLE `Update_Test_c`(
    `a`    INT DEFAULT 10
);
CREATE TABLE `Update_Test_d`(
    `a`    INT DEFAULT 10,
    `b`    INT
);

INSERT INTO `Update_Test_c` (`a`) VALUES (1);
SELECT * FROM `Update_Test_c`;
UPDATE `Update_Test_c` AS `test_c` SET `test_c`.`a` = 2;
SELECT * FROM `Update_Test_c`;
UPDATE `Update_Test_c` AS `test_c` SET `test_c`.`a` = 3 WHERE `test_c`.`a` = 2;
SELECT * FROM `Update_Test_c`;
UPDATE `Update_Test_c` `test_c` SET `test_c`.`a` = 4;
SELECT * FROM `Update_Test_c`;
UPDATE `Update_Test_c` AS `test_c` SET `test_c`.`a` = 5 WHERE `test_c`.`a` = 4;
SELECT * FROM `Update_Test_c`;
UPDATE `Update_Test_c` AS `test_c` SET `test_a`.`a` = 6;
SELECT * FROM `Update_Test_c`;
UPDATE `Update_Test_c` `test_c` SET `test_a`.`a` = 7;
SELECT * FROM `Update_Test_c`;

INSERT INTO `Update_Test_d` (`a`,`b`) VALUES (1,2);
SELECT * FROM `Update_Test_d`;
UPDATE `Update_Test_d` AS `test_D` SET `test_D`.`a` = 3, `test_D`.`b` = 4;
SELECT * FROM `Update_Test_d`;
UPDATE `Update_Test_d` AS `test_D` SET `test_D`.`a` = 5, `test_D`.`b` = 6 WHERE `test_D`.`a` = 3 AND `test_D`.`b` = 4;
SELECT * FROM `Update_Test_d`;
UPDATE `Update_Test_d` `test_D` SET `test_D`.`a` = 7, `test_D`.`b` = 8;
SELECT * FROM `Update_Test_d`;
UPDATE `Update_Test_d` `test_D` SET `test_D`.`a` = 9, `test_D`.`b` = 10  WHERE `test_D`.`a` = 7 AND `test_D`.`b` = 8;
SELECT * FROM `Update_Test_d`;
UPDATE `Update_Test_d` AS `test_D` SET `test_D`.`a` = 11, `test_b`.`b` = 12;
SELECT * FROM `Update_Test_d`;
UPDATE `Update_Test_d` `test_D` SET `test_D`.`a` = 11, `test_b`.`b` = 12;
SELECT * FROM `Update_Test_d`;

DROP TABLE `Update_Test_c`;
DROP TABLE `Update_Test_d`;
DROP TABLE `Update_Test_d`;

create table `tbl_Update`(`a1` int,`a2` varchar2(100));
ALTER TABLE `tbl_Update` ADD PRIMARY KEY(`a1`);
delete from `tbl_Update`;
insert into `tbl_Update` values(1,'`a`');
insert into `tbl_Update` values(2,'`b`');
insert into `tbl_Update` values(3,'`c`');
insert into `tbl_Update` values(4,'d');
insert into `tbl_Update` values(11,'AA');
select * from `tbl_Update` order by `a1`;
create table `sub_tab`(`T1` int,`t2` varchar2(100));
insert into `sub_tab` values(11,'AA');
select * from `sub_tab`;
update `tbl_Update` `a` set (`a1`,`a2`)=(100,'hello') from `sub_tab` `t` where `t`.`T1`=`a`.`a1`;
select * from `tbl_Update` order by `a1`;
update `tbl_Update` `a1` set (`a1`,`a2`)=(101,'hello world') from `sub_tab` `t` where `t`.`T1`=`a1`.`a1`;
select * from `tbl_Update` order by `a1`;
drop table `tbl_Update`;
drop table `sub_tab`;

create table `test_tbl_A`(`a` int);
insert into `test_tbl_A` values(1);
select * from `test_tbl_A`;
update `test_tbl_A` `a` set `a`=2;
select * from `test_tbl_A`;
update `test_tbl_A` `a` set `a`=3 where `a`.`a`=2;
select * from `test_tbl_A`;
drop table `test_tbl_A`;

create table `test_tbl_B`(`a` int, `b` int);
insert into `test_tbl_B` values(1,2);
select * from `test_tbl_B`;
update `test_tbl_B` as `a` set (`a`,`b`)=(3,4);
update `test_tbl_B` set `c` = 100;
select * from `test_tbl_B`;
update `test_tbl_B` as `a` set (`a`,`b`)=(5,6) where `a`.`a`=3 and `a`.`b`=4;
select * from `test_tbl_B`;
update `test_tbl_B` as `a` set (`a`.`a`, `a`.`b`)=(7,8) where `a`.`a`=5 and `a`.`b`=6;
select * from `test_tbl_B`;
drop table `test_tbl_B`;

CREATE TYPE `complex` AS (`b` int,`c` int);
CREATE TYPE `complex` AS (`b` int,`c` int);
create table `test_tbl_C`(`a` `complex`);
ALTER TABLE `test_tbl_C` ADD PRIMARY KEY(`a`);
insert into `test_tbl_C` values((1,2));
select * from `test_tbl_C`;
update `test_tbl_C` `col` set `col`.`a`.`b`=(100);
select * from `test_tbl_C`;
drop table `test_tbl_C`;
drop type `complex`;

-- Test multiple column set with GROUP BY of UPDATE
CREATE TABLE `Update_Multiple_Set_01`(`a` INT, `b` INT, `c` INT);
CREATE TABLE `Update_Multiple_Set_02`(`a` INT, `b` INT, `c` INT);
UPDATE `Update_Multiple_Set_02` `t2` SET (`b`, `c`) = (SELECT `b`, `c` FROM `Update_Multiple_Set_01` `T1` WHERE `T1`.`a`=`t2`.`a` GROUP BY 1, 2);
DROP TABLE `Update_Multiple_Set_01`;
DROP TABLE `Update_Multiple_Set_02`;

-- Test multiple column set with GROUP BY alias of UPDATE
drop table `Usview08t`;
drop table `Offers_20050701`;
create table `Usview08t`(`location_id` int, `on_hand_unit_qty` int, `on_order_qty` int);
create table `Offers_20050701`(`location_id` int null, visits int null);
insert into `Usview08t` values(1,3,5);
insert into `Offers_20050701` values(2,4);
UPDATE `Usview08t` `Table_008` SET (`on_hand_unit_qty`,`on_order_qty`) = (SELECT AVG(VISITS),154 `c2` FROM `Offers_20050701` GROUP BY `c2`);
select * from `Usview08t`;
UPDATE `Usview08t` `t2` SET (`t2`.`on_hand_unit_qty`, `t2`.`on_order_qty`) = (SELECT AVG(VISITS),154 FROM `Offers_20050701`);
UPDATE `Usview08t` `Table_008` SET (`on_hand_unit_qty`,`on_hand_unit_qty`) = (SELECT AVG(VISITS),154 `c2` FROM `Offers_20050701` GROUP BY `c2`);
drop table `Usview08t`;
drop table `Offers_20050701`;

--Test table name reference or alias reference
create table `Test` (`b` int, `a` int);
insert into `Test` values(1,2);
update `Test` set `Test`.`a`=10;
update `Test` `t` set `t`.`b`=20;
select * from `Test`;
drop table `Test`;

create table `Test`(`a` int[3],`b` int);
insert into `Test` values('{1,2,3}',4);
update `Test` set `Test`.`a`='{10,20,30}';
select * from `Test`;
update `Test` `t` set `t`.`a`='{11,21,31}';
select * from `Test`;
update `Test` set `a`='{12,22,32}';
select * from `Test`;
update `Test` set `a`[1,2]='{13,23}';
select * from `Test`;
--must compatible with previous features, though not perfect
update `Test` set `Test`.`a`[1,2]='{14,24}';
select * from `Test`;
update `Test` `t` set `t`.`a`[1,2]='{15,25}';
select * from `Test`;
drop table `Test`;

create type `newtype` as(`a` int, `b` int);
create table `Test`(`a` `newtype`,`b` int);
insert into `Test` values(ROW(1,2),3);
update `Test` set `Test`.`a`=ROW(10,20);
select * from `Test`;
update `Test` `t` set `t`.`a`=ROW(11,21);
select * from `Test`;
--Ambiguous scene
--update field `a` of column `a` rather than column `a` of table `a`
update `Test` `a` set `a`.`a`=12;
--update field `b` of column `a` rather than column `b` of table `a`
update `Test` `a` set `a`.`b`=22;
select * from `Test`;
--fail
update `Test` `a` set `a`.`a`=ROW(13,23);
update `Test` `a` set `a`.`c`=10;
update `Test` `b` set `b`.`c`=10;
--must compatible with previous features, though not perfect
update `Test` `a` set `a`.`a`.`a`=12;
select * from `Test`;
drop table `Test`;
drop type `newtype`;

--Test update in merge into
create table `test_D` (`a` int, `b` int);
create table `test_S` (`a` int, `b` int);
insert into `test_D` values(generate_series(6,10),1);
insert into `test_S` values(generate_series(1,10),2);
merge into `test_D` using `test_S` on(`test_D`.`a`=`test_S`.`a`) when matched then update set `test_D`.`b`=`test_S`.`b`;
select * from `test_D` order by `a`;
truncate table `test_S`;
insert into `test_S` values(generate_series(1,10),20);
merge into `test_D` `d` using `test_S` on(`d`.`a`=`test_S`.`a`) when matched then update set `d`.`b`=`test_S`.`b`;
select * from `test_D` order by `a`;
drop table `test_D`;
drop table `test_S`;

create table `test_D`(`a` int[3],`b` int);
create table `test_S`(`a` int[3],`b` int);
insert into `test_D` values('{1,2,3}',4);
insert into `test_S` values('{10,20,30}',4);
merge into `test_D` using `test_S` on(`test_D`.`b`=`test_S`.`b`) when matched then update set `test_D`.`a`=`test_S`.`a`;
select * from `test_D`;
truncate table `test_S`;
insert into `test_S` values('{11,21,31}',4);
merge into `test_D` `d` using `test_S` on(`d`.`b`=`test_S`.`b`) when matched then update set `d`.`a`=`test_S`.`a`;
select * from `test_D`;
--must compatible with previous features, though not perfect
merge into `test_D` using `test_S` on(`test_D`.`b`=`test_S`.`b`) when matched then update set `test_D`.`a`[1,3]=`test_S`.`a`[1,3];
select * from `test_D`;
merge into `test_D` `d` using `test_S` on(`d`.`b`=`test_S`.`b`) when matched then update set `d`.`a`[1,3]=`test_S`.`a`[1,3];
select * from `test_D`;
drop table `test_D`;
drop table `test_S`;

create type `newtype` as(`a` int,`b` int);
create table `test_D`(`a` `newtype`, `b` int);
create table `test_S`(`a` `newtype`, `b` int);
insert into `test_D` values(ROW(1,2),3);
insert into `test_S` values(ROW(10,20),3);
merge into `test_D` using `test_S` on(`test_D`.`b`=`test_S`.`b`) when matched then update set `test_D`.`a`=`test_S`.`a`;
select * from `test_D`;
truncate table `test_S`;
insert into `test_S` values(ROW(11,12),3);
merge into `test_D` `d` using `test_S` on(`d`.`b`=`test_S`.`b`) when matched then update set `d`.`a`=`test_S`.`a`;
select * from `test_D`;
truncate table `test_S`;
insert into `test_S` values(ROW(22,22),3);
merge into `test_D` `a` using `test_S` on(`a`.`b`=`test_S`.`b`) when matched then update set `a`.`a`=21;
merge into `test_D` `a` using `test_S` on(`a`.`b`=`test_S`.`b`) when matched then update set `a`.`b`=22;
select * from `test_D`;
--fail
merge into `test_D` `a` using `test_S` on(`a`.`b`=`test_S`.`b`) when matched then update set `a`.`a`=`test_S`.`a`;
--must compatible with previous features, though not perfect
merge into `test_D` using `test_S` on(`test_D`.`b`=`test_S`.`b`) when matched then update set `test_D`.`a`.`a`=`test_S`.`b`;
select * from `test_D`;
merge into `test_D` `d` using `test_S` on(`d`.`b`=`test_S`.`b`) when matched then update set `d`.`a`.`a`=`test_S`.`b`;
select * from `test_D`;
drop table `test_S`;
drop table `test_D`;
drop type `newtype`;

-- Test update multiple entries for the same column with subselect
create table `Test` (`a` int[2], `b` int);
insert into `Test` values('{1,2}',3);
update `Test` set (`a`[1],`a`[2])=(select 10,20);
select * from `Test`;
drop table `Test`;
create type `nt` as(`a` int,`b` int);
create table `Test`(`a` `nt`,`b` `nt`,`c` int);
insert into `Test` values(row(1,2),row(3,4),5);
update `Test` set (`a`.`b`,`b`.`b`)=(select 20,40);
select * from `Test`;
drop table `Test`;
drop type `nt`;

-- Test comment in subselect of update
create table `Test`(`a` int,`b` int);
insert into `Test` values(1,2);
update `Test` set (`a`)=(select /*comment*/10);
select * from `Test`;
update `Test` set (`a`)=(select /*+comment*/20);
select * from `Test`;
drop table `Test`;

--Test update multiple fields of column which using composite type at once
create type `nt` as(`a` int,`b` int);
create table `AA` (`a` `nt`, `b` int,`c` char);
explain (verbose on, costs off) insert into `AA` values(ROW(1,2),3,'4');
insert into `AA` values(ROW(1,2),3,'4');
explain (verbose on, costs off) update `AA` set `a`.`a`=10,`a`.`b`=20 where `c`='4';
update `AA` set `a`.`a`=10,`a`.`b`=20 where `c`='4';
select * from `AA`;
drop table `AA`;
drop type `nt`;

--Test update multiple values of of an array at once
create table `Test` (`a` int[2], `b` int,`c` char);
insert into `Test` values('{1,2}',3,'4');
explain (verbose on, costs off) update `Test` set `a`[1]=100,`a`[2]=200 where `c`='4';
update `Test` set `a`[1]=100,`a`[2]=200 where `c`='4';
select * from `Test`;
explain (verbose on, costs off) update `Test` set `a`[1,2]='{101,201}' where `c`='4';
update `Test` set `a`[1,2]='{101,201}' where `c`='4';
select * from `Test`;
explain (verbose on, costs off) insert into `Test`  (`a`[1,2],`b`,`c`) values('{113,114}',4,'5');
insert into `Test`  (`a`[1,2],`b`,`c`) values('{113,114}',4,'5');
select * from `Test` order by 3;
select `a`[1,2] from `Test` where `c`='4';
explain (verbose on, costs off) insert into `Test` (`a`[1],`a`[2],`b`,`c`)values(1,2,3,'6');
insert into `Test` (`a`[1],`a`[2],`b`,`c`)values(1,2,3,'6');
select * from `Test` order by 3;
explain (verbose on, costs off) insert into `Test` (`a`[1:2],`b`,`c`)values('{1,2}',3,'7');
insert into `Test` (`a`[1:2],`b`,`c`)values('{1,2}',3,'7');
select * from `Test` order by 3;
explain (verbose on, costs off) update `Test` set `a`[1:2]='{10,20}' where `c`='7';
update `Test` set `a`[1:2]='{10,20}' where `c`='7';
select * from `Test` order by 3;
drop table `Test`;
