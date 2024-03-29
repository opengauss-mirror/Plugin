\c table_name_test_db;
SET dolphin.lower_case_table_names TO 0;
--
-- CREATE_INDEX
-- Create ancillary data structures (i.e. indices)
--

--
-- BTREE
--
CREATE INDEX `onek_unique1` ON `Onek` USING btree(`unique1` `int4_ops`);

CREATE INDEX `onek_unique2` ON `Onek` USING btree(`unique2` `int4_ops`);

CREATE INDEX `onek_hundred` ON `Onek` USING btree(`hundred` `int4_ops`);

CREATE INDEX `onek_stringu1` ON `Onek` USING btree(`stringu1` `name_ops`);

CREATE INDEX `tenk1_unique1` ON `Tenk1` USING btree(`unique1` `int4_ops`);

CREATE INDEX `tenk1_unique2` ON `Tenk1` USING btree(`unique2` `int4_ops`);

CREATE INDEX `tenk1_hundred` ON `Tenk1` USING btree(`hundred` `int4_ops`);

CREATE INDEX `tenk1_thous_tenthous` ON `Tenk1` (`thousand`, `tenthous`);

CREATE INDEX `tenk2_unique1` ON `Tenk2` USING btree(`unique1` `int4_ops`);

CREATE INDEX `tenk2_unique2` ON `Tenk2` USING btree(`unique2` `int4_ops`);

CREATE INDEX `tenk2_hundred` ON `Tenk2` USING btree(`hundred` `int4_ops`);

CREATE INDEX `rix` ON `Road` USING btree (`name` `text_ops`);

CREATE INDEX `iix` ON `iHighway` USING btree (`name` `text_ops`);

CREATE INDEX `six` ON `sHighway` USING btree (`name` `text_ops`);

-- test comments
COMMENT ON INDEX six_wrong IS 'bad index';
COMMENT ON INDEX six IS 'good index';
COMMENT ON INDEX six IS NULL;

--
-- BTREE ascending/descending cases
--
-- we load int4/text from pure descending data (each key is a new
-- low key) and name/f8 from pure ascending data (each key is a new
-- high key).  we had a bug where new low keys would sometimes be
-- "lost".
--
CREATE INDEX `bt_i4_index` ON `BT_i4_heap` USING btree (`seqno` `int4_ops`);

CREATE INDEX `bt_name_index` ON `BT_name_heap` USING btree (`seqno` `name_ops`);

CREATE INDEX `bt_txt_index` ON `BT_txt_heap` USING btree (`seqno` `text_ops`);

CREATE INDEX `bt_f8_index` ON `BT_f8_heap` USING btree (`seqno` `float8_ops`);

--
-- BTREE partial indices
--
CREATE INDEX `onek2_u1_prtl` ON `Onek2` USING btree(`unique1` `int4_ops`)
	where `unique1` < 20 or `unique1` > 980;

CREATE INDEX `onek2_u2_prtl` ON `Onek2` USING btree(`unique2` `int4_ops`)
	where `stringu1` < 'B';

CREATE INDEX `onek2_stu1_prtl` ON `Onek2` USING btree(`stringu1` `name_ops`)
	where `Onek2`.`stringu1` >= 'J' and `Onek2`.`stringu1` < 'K';

--
-- GiST (rtree-equivalent opclasses only)
--
CREATE INDEX `grect2ind` ON `Fast_emp4000` USING gist (`home_base`);

CREATE INDEX `gpolygonind` ON `Polygon_tbl` USING gist (`f1`);

CREATE INDEX `gcircleind` ON `Circle_tbl` USING gist (`f1`);

INSERT INTO `point_tbl`(`f1`) VALUES (NULL);

CREATE INDEX `gpointind` ON `point_tbl` USING gist (`f1`);


--CREATE TABLE gPolygon_tbl AS
--    SELECT polygon(home_base) AS f1 FROM Slow_emp4000;
--INSERT INTO gPolygon_tbl VALUES ( '(1000,0,0,1000)' );
--INSERT INTO gPolygon_tbl VALUES ( '(0,1000,1000,1000)' );

--CREATE TABLE gCircle_tbl AS
--    SELECT circle(home_base) AS f1 FROM Slow_emp4000;

--CREATE INDEX ggpolygonind ON gPolygon_tbl USING gist (f1);

--CREATE INDEX ggcircleind ON gCircle_tbl USING gist (f1);

--
-- SP-GiST
--

CREATE TABLE `Quad_point_tbl` AS
    SELECT point(`unique1`,`unique2`) AS `p` FROM `Tenk1`;

INSERT INTO `Quad_point_tbl`
    SELECT '(333.0,400.0)'::point FROM generate_series(1,1000);

INSERT INTO `Quad_point_tbl` VALUES (NULL), (NULL), (NULL);

CREATE INDEX `sp_quad_ind` ON `Quad_point_tbl` USING spgist (`p`);

CREATE TABLE `KD_point_tbl` AS SELECT * FROM `Quad_point_tbl`;

CREATE INDEX `SP_kd_ind` ON `KD_point_tbl` USING spgist (`p` kd_point_ops);

CREATE TABLE `Suffix_text_tbl` AS
    SELECT `name` AS `t` FROM `Road` WHERE `name` !~ '^[0-9]';

INSERT INTO `Suffix_text_tbl`
    SELECT 'P0123456789abcdef' FROM generate_series(1,1000);
INSERT INTO `Suffix_text_tbl` VALUES ('P0123456789abcde');
INSERT INTO `Suffix_text_tbl` VALUES ('P0123456789abcdefF');

CREATE INDEX `sp_suff_ind` ON `Suffix_text_tbl` USING spgist (`t`);

--
-- Test GiST and SP-GiST indexes
--

-- get non-indexed results for comparison purposes

SET enable_seqscan = ON;
SET enable_indexscan = OFF;
SET enable_bitmapscan = OFF;

SELECT * FROM `Fast_emp4000`
    WHERE `home_base` @ '(200,200),(2000,1000)'::box
    ORDER BY (`home_base`[0])[0];

SELECT count(*) FROM `Fast_emp4000` WHERE `home_base` && '(1000,1000,0,0)'::box;

SELECT count(*) FROM `Fast_emp4000` WHERE `home_base` IS NULL;

SELECT * FROM `Polygon_tbl` WHERE `f1` ~ '((1,1),(2,2),(2,1))'::polygon
    ORDER BY (`poly_center`(`f1`))[0];

SELECT * FROM `Circle_tbl` WHERE `f1` && circle(point(1,-2), 1)
    ORDER BY `area`(`f1`);

--SELECT count(*) FROM gPolygon_tbl WHERE f1 && '(1000,1000,0,0)'::polygon;

--SELECT count(*) FROM gCircle_tbl WHERE f1 && '<(500,500),500>'::circle;

SELECT count(*) FROM `point_tbl` WHERE `f1` <@ box '(0,0,100,100)';

SELECT count(*) FROM `point_tbl` WHERE box '(0,0,100,100)' @> `f1`;

SELECT count(*) FROM `point_tbl` WHERE `f1` <@ polygon '(0,0),(0,100),(100,100),(50,50),(100,0),(0,0)';

SELECT count(*) FROM `point_tbl` WHERE `f1` <@ circle '<(50,50),50>';

SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` << '(0.0, 0.0)';

SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` >> '(0.0, 0.0)';

SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` <^ '(0.0, 0.0)';

SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` >^ '(0.0, 0.0)';

SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` ~= '(-5, -12)';

SELECT * FROM `point_tbl` ORDER BY `f1` <-> '0,1';

SELECT * FROM `point_tbl` WHERE `f1` IS NULL;

SELECT * FROM `point_tbl` WHERE `f1` IS NOT NULL ORDER BY `f1` <-> '0,1';

SELECT * FROM `point_tbl` WHERE `f1` <@ '(-10,-10),(10,10)':: box ORDER BY `f1` <-> '0,1';

SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NULL;

SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NOT NULL;

SELECT count(*) FROM `Quad_point_tbl`;

SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <@ box '(200,200,1000,1000)';

SELECT count(*) FROM `Quad_point_tbl` WHERE box '(200,200,1000,1000)' @> `p`;

SELECT count(*) FROM `Quad_point_tbl` WHERE `p` << '(5000, 4000)';

SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >> '(5000, 4000)';

SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <^ '(5000, 4000)';

SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >^ '(5000, 4000)';

SELECT count(*) FROM `Quad_point_tbl` WHERE `p` ~= '(4585, 365)';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdef';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcde';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdefF';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <    'Aztec                         Ct  ';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<~  'Aztec                         Ct  ';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <=   'Aztec                         Ct  ';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<=~ 'Aztec                         Ct  ';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Aztec                         Ct  ';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Worth                         St  ';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >=   'Worth                         St  ';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>=~ 'Worth                         St  ';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >    'Worth                         St  ';

SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>~  'Worth                         St  ';

-- Now check the results from plain indexscan
SET enable_seqscan = OFF;
SET enable_indexscan = ON;
SET enable_bitmapscan = OFF;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `Fast_emp4000`
    WHERE `home_base` @ '(200,200),(2000,1000)'::box
    ORDER BY (`home_base`[0])[0];
SELECT * FROM `Fast_emp4000`
    WHERE `home_base` @ '(200,200),(2000,1000)'::box
    ORDER BY (`home_base`[0])[0];

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Fast_emp4000` WHERE `home_base` && '(1000,1000,0,0)'::box;
SELECT count(*) FROM `Fast_emp4000` WHERE `home_base` && '(1000,1000,0,0)'::box;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Fast_emp4000` WHERE `home_base` IS NULL;
SELECT count(*) FROM `Fast_emp4000` WHERE `home_base` IS NULL;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `Polygon_tbl` WHERE `f1` ~ '((1,1),(2,2),(2,1))'::polygon
    ORDER BY (`poly_center`(`f1`))[0];
SELECT * FROM `Polygon_tbl` WHERE `f1` ~ '((1,1),(2,2),(2,1))'::polygon
    ORDER BY (`poly_center`(`f1`))[0];

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `Circle_tbl` WHERE `f1` && circle(point(1,-2), 1)
    ORDER BY area(`f1`);
SELECT * FROM `Circle_tbl` WHERE `f1` && circle(point(1,-2), 1)
    ORDER BY area(`f1`);

--EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
--SELECT count(*) FROM gPolygon_tbl WHERE f1 && '(1000,1000,0,0)'::polygon;
--SELECT count(*) FROM gPolygon_tbl WHERE f1 && '(1000,1000,0,0)'::polygon;

--EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
--SELECT count(*) FROM gCircle_tbl WHERE f1 && '<(500,500),500>'::circle;
--SELECT count(*) FROM gCircle_tbl WHERE f1 && '<(500,500),500>'::circle;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `point_tbl` WHERE `f1` <@ box '(0,0,100,100)';
SELECT count(*) FROM `point_tbl` WHERE `f1` <@ box '(0,0,100,100)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `point_tbl` WHERE box '(0,0,100,100)' @> `f1`;
SELECT count(*) FROM `point_tbl` WHERE box '(0,0,100,100)' @> `f1`;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `point_tbl` WHERE `f1` <@ polygon '(0,0),(0,100),(100,100),(50,50),(100,0),(0,0)';
SELECT count(*) FROM `point_tbl` WHERE `f1` <@ polygon '(0,0),(0,100),(100,100),(50,50),(100,0),(0,0)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `point_tbl` WHERE `f1` <@ circle '<(50,50),50>';
SELECT count(*) FROM `point_tbl` WHERE `f1` <@ circle '<(50,50),50>';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` << '(0.0, 0.0)';
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` << '(0.0, 0.0)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` >> '(0.0, 0.0)';
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` >> '(0.0, 0.0)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` <^ '(0.0, 0.0)';
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` <^ '(0.0, 0.0)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` >^ '(0.0, 0.0)';
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` >^ '(0.0, 0.0)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` ~= '(-5, -12)';
SELECT count(*) FROM `point_tbl` `p` WHERE `p`.`f1` ~= '(-5, -12)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `point_tbl` ORDER BY `f1` <-> '0,1';
SELECT * FROM `point_tbl` ORDER BY `f1` <-> '0,1';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `point_tbl` WHERE `f1` IS NULL;
SELECT * FROM `point_tbl` WHERE `f1` IS NULL;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `point_tbl` WHERE `f1` IS NOT NULL ORDER BY `f1` <-> '0,1';
SELECT * FROM `point_tbl` WHERE `f1` IS NOT NULL ORDER BY `f1` <-> '0,1';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `point_tbl` WHERE `f1` <@ '(-10,-10),(10,10)':: box ORDER BY `f1` <-> '0,1';
SELECT * FROM `point_tbl` WHERE `f1` <@ '(-10,-10),(10,10)':: box ORDER BY `f1` <-> '0,1';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NULL;
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NULL;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NOT NULL;
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NOT NULL;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl`;
SELECT count(*) FROM `Quad_point_tbl`;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <@ box '(200,200,1000,1000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <@ box '(200,200,1000,1000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE box '(200,200,1000,1000)' @> `p`;
SELECT count(*) FROM `Quad_point_tbl` WHERE box '(200,200,1000,1000)' @> `p`;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` << '(5000, 4000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` << '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >> '(5000, 4000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >> '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <^ '(5000, 4000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <^ '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >^ '(5000, 4000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >^ '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` ~= '(4585, 365)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` ~= '(4585, 365)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` <@ box '(200,200,1000,1000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` <@ box '(200,200,1000,1000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE box '(200,200,1000,1000)' @> `p`;
SELECT count(*) FROM `KD_point_tbl` WHERE box '(200,200,1000,1000)' @> `p`;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` << '(5000, 4000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` << '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` >> '(5000, 4000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` >> '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` <^ '(5000, 4000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` <^ '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` >^ '(5000, 4000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` >^ '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` ~= '(4585, 365)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` ~= '(4585, 365)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdef';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdef';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcde';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcde';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdefF';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdefF';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <    'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <    'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<~  'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<~  'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <=   'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <=   'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<=~ 'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<=~ 'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Worth                         St  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >=   'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >=   'Worth                         St  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>=~ 'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>=~ 'Worth                         St  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >    'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >    'Worth                         St  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>~  'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>~  'Worth                         St  ';

-- Now check the results from bitmap indexscan
SET enable_seqscan = OFF;
SET enable_indexscan = OFF;
SET enable_bitmapscan = ON;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `point_tbl` WHERE `f1` <@ '(-10,-10),(10,10)':: box ORDER BY `f1` <-> '0,1';
SELECT * FROM `point_tbl` WHERE `f1` <@ '(-10,-10),(10,10)':: box ORDER BY `f1` <-> '0,1';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NULL;
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NULL;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NOT NULL;
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` IS NOT NULL;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl`;
SELECT count(*) FROM `Quad_point_tbl`;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <@ box '(200,200,1000,1000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <@ box '(200,200,1000,1000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE box '(200,200,1000,1000)' @> `p`;
SELECT count(*) FROM `Quad_point_tbl` WHERE box '(200,200,1000,1000)' @> `p`;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` << '(5000, 4000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` << '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >> '(5000, 4000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >> '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <^ '(5000, 4000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` <^ '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >^ '(5000, 4000)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` >^ '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` ~= '(4585, 365)';
SELECT count(*) FROM `Quad_point_tbl` WHERE `p` ~= '(4585, 365)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` <@ box '(200,200,1000,1000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` <@ box '(200,200,1000,1000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE box '(200,200,1000,1000)' @> `p`;
SELECT count(*) FROM `KD_point_tbl` WHERE box '(200,200,1000,1000)' @> `p`;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` << '(5000, 4000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` << '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` >> '(5000, 4000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` >> '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` <^ '(5000, 4000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` <^ '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` >^ '(5000, 4000)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` >^ '(5000, 4000)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `KD_point_tbl` WHERE `p` ~= '(4585, 365)';
SELECT count(*) FROM `KD_point_tbl` WHERE `p` ~= '(4585, 365)';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdef';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdef';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcde';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcde';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdefF';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` = 'P0123456789abcdefF';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <    'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <    'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<~  'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<~  'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <=   'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` <=   'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<=~ 'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~<=~ 'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Aztec                         Ct  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Aztec                         Ct  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` =    'Worth                         St  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >=   'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >=   'Worth                         St  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>=~ 'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>=~ 'Worth                         St  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >    'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` >    'Worth                         St  ';

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>~  'Worth                         St  ';
SELECT count(*) FROM `Suffix_text_tbl` WHERE `t` ~>~  'Worth                         St  ';

RESET enable_seqscan;
RESET enable_indexscan;
RESET enable_bitmapscan;

--
-- GIN over int[] and text[]
--
-- Note: GIN currently supports only bitmap scans, not plain indexscans
--

SET enable_seqscan = OFF;
SET enable_indexscan = OFF;
SET enable_bitmapscan = ON;

CREATE INDEX `intarrayidx` ON `Array_index_op_test` USING gin (i);

explain (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `Array_index_op_test` WHERE `i` @> '{32}' ORDER BY `seqno`;

SELECT * FROM `Array_index_op_test` WHERE `i` @> '{32}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` && '{32}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` @> '{17}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` && '{17}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` @> '{32,17}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` && '{32,17}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` <@ '{38,34,32,89}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` = '{47,77}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` = '{}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` @> '{}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` && '{}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` <@ '{}' ORDER BY `seqno`;
SELECT * FROM `Array_op_test` WHERE `i` = '{NULL}' ORDER BY `seqno`;
SELECT * FROM `Array_op_test` WHERE `i` @> '{NULL}' ORDER BY `seqno`;
SELECT * FROM `Array_op_test` WHERE `i` && '{NULL}' ORDER BY `seqno`;
SELECT * FROM `Array_op_test` WHERE `i` <@ '{NULL}' ORDER BY `seqno`;

CREATE INDEX `textarrayidx` ON `Array_index_op_test` USING gin (t);

explain (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `Array_index_op_test` WHERE `t` @> '{AAAAAAAA72908}' ORDER BY `seqno`;

SELECT * FROM `Array_index_op_test` WHERE `t` @> '{AAAAAAAA72908}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` && '{AAAAAAAA72908}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` @> '{AAAAAAAAAA646}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` && '{AAAAAAAAAA646}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` @> '{AAAAAAAA72908,AAAAAAAAAA646}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` && '{AAAAAAAA72908,AAAAAAAAAA646}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` <@ '{AAAAAAAA72908,AAAAAAAAAAAAAAAAAAA17075,AA88409,AAAAAAAAAAAAAAAAAA36842,AAAAAAA48038,AAAAAAAAAAAAAA10611}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` = '{AAAAAAAAAA646,A87088}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` = '{}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` @> '{}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` && '{}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` <@ '{}' ORDER BY `seqno`;

-- And try it with a multicolumn GIN index

DROP INDEX `intarrayidx`, `textarrayidx`;

CREATE INDEX `botharrayidx` ON `Array_index_op_test` USING gin (`i`, `t`);

SELECT * FROM `Array_index_op_test` WHERE `i` @> '{32}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` && '{32}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` @> '{AAAAAAA80240}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` && '{AAAAAAA80240}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` @> '{32}' AND `t` && '{AAAAAAA80240}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `i` && '{32}' AND `t` @> '{AAAAAAA80240}' ORDER BY `seqno`;
SELECT * FROM `Array_index_op_test` WHERE `t` = '{}' ORDER BY `seqno`;
SELECT * FROM `Array_op_test` WHERE `i` = '{NULL}' ORDER BY `seqno`;
SELECT * FROM `Array_op_test` WHERE `i` <@ '{NULL}' ORDER BY `seqno`;

RESET enable_seqscan;
RESET enable_indexscan;
RESET enable_bitmapscan;

--
-- Test GIN index's reloptions
--
CREATE INDEX `gin_relopts_test` ON `Array_index_op_test` USING gin (`i`)
  WITH (FASTUPDATE=on, GIN_PENDING_LIST_LIMIT=128);
\d+ "gin_relopts_test"

--
-- HASH
--
CREATE INDEX `hash_i4_index` ON `Hash_i4_heap` USING hash (random `int4_ops`);

CREATE INDEX `hash_name_index` ON `Hash_name_heap` USING hash (random `name_ops`);

CREATE INDEX `hash_txt_index` ON `Hash_txt_heap` USING hash (random `text_ops`);

CREATE INDEX `hash_f8_index` ON `Hash_f8_heap` USING hash (random `float8_ops`);

-- CREATE INDEX hash_ovfl_index ON hash_ovfl_heap USING hash (x `int4_ops`);


--
-- Test functional index
--
-- PGXC: Here replication is used to ensure correct index creation
-- when a non-shippable expression is used.
-- PGXCTODO: this should be removed once global constraints are supported
CREATE TABLE `func_index_heap` (`f1` text, `f2` text)  ;
CREATE UNIQUE INDEX `func_index_index` on `func_index_heap` (textcat(`f1`,`f2`));

INSERT INTO `func_index_heap` VALUES('ABC','DEF');
INSERT INTO `func_index_heap` VALUES('AB','CDEFG');
INSERT INTO `func_index_heap` VALUES('QWE','RTY');
-- this should fail because of unique index:
INSERT INTO `func_index_heap` VALUES('ABCD', 'EF');
-- but this shouldn't:
INSERT INTO `func_index_heap` VALUES('QWERTY');


--
-- Same test, expressional index
--
DROP TABLE `func_index_heap`;
-- PGXC: Here replication is used to ensure correct index creation
-- when a non-shippable expression is used.
-- PGXCTODO: this should be removed once global constraints are supported
CREATE TABLE `func_index_heap` (`f1` text, `f2` text)  ;
CREATE UNIQUE INDEX `func_index_index` on `func_index_heap` ((`f1` || `f2`) `text_ops`);

INSERT INTO `func_index_heap` VALUES('ABC','DEF');
INSERT INTO `func_index_heap` VALUES('AB','CDEFG');
INSERT INTO `func_index_heap` VALUES('QWE','RTY');
-- this should fail because of unique index:
INSERT INTO `func_index_heap` VALUES('ABCD', 'EF');
-- but this shouldn't:
INSERT INTO `func_index_heap` VALUES('QWERTY');

--
-- Also try building functional, expressional, and partial indexes on
-- tables that already contain data.
--
create unique index `hash_f8_index_1` on `Hash_f8_heap`(abs(random));
create unique index `hash_f8_index_2` on `Hash_f8_heap`((`seqno` + 1), random);
create unique index `hash_f8_index_3` on `Hash_f8_heap`(random) where `seqno` > 1000;

--
-- Try some concurrent index builds
--
-- Unfortunately this only tests about half the code paths because there are
-- no concurrent updates happening to the table at the same time.

CREATE TABLE `concur_heap` (`f1` text, `f2` text);
-- empty table
CREATE INDEX CONCURRENTLY `concur_index1` ON `concur_heap`(`f2`,`f1`);
INSERT INTO `concur_heap` VALUES  ('a','b');
INSERT INTO `concur_heap` VALUES  ('b','b');
-- unique index
CREATE UNIQUE INDEX CONCURRENTLY `concur_index2` ON `concur_heap`(`f1`);
-- check if constraint is set up properly to be enforced
INSERT INTO `concur_heap` VALUES ('b','x');
-- check if constraint is enforced properly at build time
CREATE UNIQUE INDEX CONCURRENTLY `concur_index3` ON `concur_heap`(`f2`);
-- test that expression indexes and partial indexes work concurrently
CREATE INDEX CONCURRENTLY `concur_index4` on `concur_heap`(`f2`) WHERE `f1`='a';
CREATE INDEX CONCURRENTLY `concur_index5` on `concur_heap`(`f2`) WHERE `f1`='x';
-- here we also check that you can default the index `name`
CREATE INDEX CONCURRENTLY on `concur_heap`((`f2`||`f1`));

-- You can't do a concurrent index build in a transaction
START TRANSACTION;
CREATE INDEX CONCURRENTLY `concur_index7` ON `concur_heap`(`f1`);
COMMIT;

-- But you can do a regular index build in a transaction
START TRANSACTION;
CREATE INDEX `std_index` on `concur_heap`(`f2`);
COMMIT;

-- check to make sure that the failed indexes were cleaned up properly and the
-- successful indexes are created properly. Notably that they do NOT have the
-- "invalid" flag set.

\d "concur_heap"

--
-- Try some concurrent index drops
--
DROP INDEX CONCURRENTLY "concur_index2";				-- works
DROP INDEX CONCURRENTLY IF EXISTS "concur_index2";		-- notice

-- failures
DROP INDEX CONCURRENTLY "concur_index2", "concur_index3";
START TRANSACTION;
DROP INDEX CONCURRENTLY "concur_index5";
ROLLBACK;

-- successes
DROP INDEX CONCURRENTLY IF EXISTS "concur_index3";
DROP INDEX CONCURRENTLY "concur_index4";
DROP INDEX CONCURRENTLY "concur_index5";
DROP INDEX CONCURRENTLY "concur_index1";
DROP INDEX CONCURRENTLY "concur_heap_expr_idx";

\d "concur_heap"

DROP TABLE `concur_heap`;

--
-- Test ADD CONSTRAINT USING INDEX
--

CREATE TABLE `cwi_test`( `a` int , `b` varchar(10), `c` char);

-- add some data so that all tests have something to work with.

INSERT INTO `cwi_test` VALUES(1, 2), (3, 4), (5, 6);

CREATE UNIQUE INDEX `cwi_uniq_idx` ON `cwi_test`(`a` , `b`);
ALTER TABLE `cwi_test` ADD primary key USING INDEX `cwi_uniq_idx`;

\d "cwi_test"

CREATE UNIQUE INDEX `cwi_uniq2_idx` ON `cwi_test`(`b` , `a`);
ALTER TABLE `cwi_test` DROP CONSTRAINT `cwi_uniq_idx`,
	ADD CONSTRAINT `cwi_replaced_pkey` PRIMARY KEY
		USING INDEX `cwi_uniq2_idx`;

\d "cwi_test"

DROP INDEX `cwi_replaced_pkey`;	-- Should fail; a constraint depends on it

DROP TABLE `cwi_test`;

--
-- Tests for IS NULL/IS NOT NULL with b-tree indexes
--

SELECT `unique1`, `unique2` INTO `onek_with_null` FROM Onek;
INSERT INTO `onek_with_null` (`unique1`,`unique2`) VALUES (NULL, -1), (NULL, NULL);
CREATE UNIQUE INDEX `onek_nulltest` ON `onek_with_null` (`unique2`,`unique1`);

SET enable_seqscan = OFF;
SET enable_indexscan = ON;
SET enable_bitmapscan = ON;

SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique2` IS NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NOT NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique2` IS NOT NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NOT NULL AND `unique1` > 500;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique1` > 500;

DROP INDEX `onek_nulltest`;

CREATE UNIQUE INDEX `onek_nulltest` ON `onek_with_null` (`unique2` desc,`unique1`);

SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique2` IS NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NOT NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique2` IS NOT NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NOT NULL AND `unique1` > 500;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique1` > 500;

DROP INDEX `onek_nulltest`;

CREATE UNIQUE INDEX `onek_nulltest` ON `onek_with_null` (`unique2` desc nulls last,`unique1`);

SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique2` IS NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NOT NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique2` IS NOT NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NOT NULL AND `unique1` > 500;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique1` > 500;

DROP INDEX `onek_nulltest`;

CREATE UNIQUE INDEX `onek_nulltest` ON `onek_with_null` (`unique2`  nulls first,`unique1`);

SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique2` IS NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NOT NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique2` IS NOT NULL;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NOT NULL AND `unique1` > 500;
SELECT count(*) FROM `onek_with_null` WHERE `unique1` IS NULL AND `unique1` > 500;

DROP INDEX `onek_nulltest`;

-- Check initial-positioning logic too

CREATE UNIQUE INDEX `onek_nulltest` ON `onek_with_null` (`unique2`);

SET enable_seqscan = OFF;
SET enable_indexscan = ON;
SET enable_bitmapscan = OFF;

SELECT `unique1`, `unique2` FROM `onek_with_null`
  ORDER BY `unique2` LIMIT 2;
SELECT `unique1`, `unique2` FROM `onek_with_null` WHERE `unique2` >= -1
  ORDER BY `unique2` LIMIT 2;
SELECT `unique1`, `unique2` FROM `onek_with_null` WHERE `unique2` >= 0
  ORDER BY `unique2` LIMIT 2;

SELECT `unique1`, `unique2` FROM `onek_with_null`
  ORDER BY `unique2` DESC LIMIT 2;
SELECT `unique1`, `unique2` FROM `onek_with_null` WHERE `unique2` >= -1
  ORDER BY `unique2` DESC LIMIT 2;
SELECT `unique1`, `unique2` FROM `onek_with_null` WHERE `unique2` < 999
  ORDER BY `unique2` DESC LIMIT 2;

RESET enable_seqscan;
RESET enable_indexscan;
RESET enable_bitmapscan;

DROP TABLE `onek_with_null`;

--
-- Check bitmap index path planning
--

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT * FROM `Tenk1`
  WHERE `thousand` = 42 AND (`tenthous` = 1 OR `tenthous` = 3 OR `tenthous` = 42);
SELECT * FROM `Tenk1`
  WHERE `thousand` = 42 AND (`tenthous` = 1 OR `tenthous` = 3 OR `tenthous` = 42);

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT count(*) FROM `Tenk1`
  WHERE `hundred` = 42 AND (`thousand` = 42 OR `thousand` = 99);
SELECT count(*) FROM `Tenk1`
  WHERE `hundred` = 42 AND (`thousand` = 42 OR `thousand` = 99);

--
-- Check behavior with duplicate index column contents
--

CREATE TABLE `dupindexcols` AS
  SELECT `unique1` as `id`, stringu2::text as `f1` FROM `Tenk1`;
CREATE INDEX `dupindexcols_i` ON `dupindexcols` (`f1`, `id`, `f1` `text_pattern_ops`);
ANALYZE `dupindexcols`;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
  SELECT count(*) FROM `dupindexcols`
    WHERE `f1` > 'WA' and `id` < 1000 and `f1` ~<~ 'YX';
SELECT count(*) FROM `dupindexcols`
  WHERE `f1` > 'WA' and `id` < 1000 and `f1` ~<~ 'YX';

--
-- Check ordering of =ANY indexqual results (bug in 9.2.0)
--

vacuum analyze `Tenk1`;		-- ensure we get consistent plans here

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT `unique1` FROM `Tenk1`
WHERE `unique1` IN (1,42,7)
ORDER BY `unique1`;

SELECT `unique1` FROM `Tenk1`
WHERE `unique1` IN (1,42,7)
ORDER BY `unique1`;

EXPLAIN (NUM_NODES OFF, NODES OFF, COSTS OFF)
SELECT `thousand`, `tenthous` FROM `Tenk1`
WHERE `thousand` < 2 AND `tenthous` IN (1001,3000)
ORDER BY `thousand`;

SELECT `thousand`, `tenthous` FROM `Tenk1`
WHERE `thousand` < 2 AND `tenthous` IN (1001,3000)
ORDER BY `thousand`;

--test for relpages and reltuples of pg_class
create table `test_table`(`a` int, `b` int);
create table `test_table_col`(`a` int, `b` int) with (orientation=column);
insert into `test_table` select generate_series(1,100),generate_series(1,100);
insert into `test_table_col` select * from `test_table`;
analyze `test_table`;
analyze `test_table_col`;

select `relpages`>0 as `relpages`,`reltuples`>0 as `reltuples` from `pg_class` where `relname`='test_table';
create index `test_table_idx1` on `test_table`(`a`,`b`);
select `relpages`>0 as `relpages`,`reltuples`>0 as `reltuples` from `pg_class` where `relname`='test_table';
analyze `test_table`;
select `relpages`>0 as `relpages`,`reltuples`>0 as `reltuples` from `pg_class` where `relname`='test_table_idx1';

select `relpages`>0 as `relpages`,`reltuples`>0 as `reltuples` from `pg_class` where `relname`='test_table_col';
create index `test_table_col_idx1` on `test_table_col`(`a`,`b`);
select `relpages`>0 as `relpages`,`reltuples`>0 as `reltuples` from `pg_class` where `relname`='test_table_col';
analyze `test_table_col`;
select `relpages`>0 as `relpages`,`reltuples`>0 as `reltuples` from `pg_class` where `relname`='test_table_col_idx1';

--test nestloop index param path
create table `t_hash_table` (`a` int, `b` int);
create table `t_rep_table` (`a` int, `b` int)  ;
create index `idx_rep_table` on `t_rep_table`(`a`);

explain (costs off) select  /*+ rows(t_rep_table #100000) */  * from `t_hash_table` where `t_hash_table`.`a` in (select `a` from `t_rep_table`);
explain (costs off) select  /*+ rows(t_rep_table #100000) */  * from `t_hash_table` where '1' = '0' or `t_hash_table`.`a` in (select `a` from `t_rep_table`);

create index `test0` on `t_rep_table`(`rownum`);
create index `test0` on `t_rep_table`(sin(`a`), sin(`rownum`));
create index `test0` on `t_rep_table`(sin(`a`), sin(`rownum`+1));

drop index `idx_rep_table`;
drop table `t_hash_table`;
drop table `t_rep_table`;

drop index `test_table_idx1`;
drop index `test_table_col_idx1`;
drop table `test_table`;
drop table `test_table_col`;
