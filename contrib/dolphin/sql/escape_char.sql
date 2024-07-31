create schema escape_char;
set current_schema to 'escape_char';
set dolphin.b_compatibility to on;

-- default no escaping, all as normal strings
SELECT 'moon\tlight';
SELECT 'moon\nlight';
SELECT 'moon\flight';
SELECT 'moon\%light';
SELECT 'moon\_light';
SELECT 'moon\Zlight';

-- set standard_conforming_strings to off
set standard_conforming_strings to off;
SELECT 'moon\tlight';
SELECT 'moon\flight';
SELECT 'moon\%light';
SELECT 'moon\_light';
SELECT 'moon\Zlight';

-- result same with mysql
-- cancel wanring
set standard_conforming_strings to off;
set escape_string_warning to off;
SELECT 'moon\tlight';
SELECT 'moon\flight';
SELECT 'moon\%light';
SELECT 'moon\_light';
SELECT 'moon\Zlight';

--  result same with mysql
-- set standard_conforming_strings to on and use E'..'
set standard_conforming_strings to on;
set escape_string_warning to on;
SELECT E'moon\tlight';
SELECT E'moon\nlight';
SELECT E'moon\flight';
SELECT E'moon\%light';
SELECT E'moon\_light';
SELECT E'moon\Zlight';


-- test insert escape
set standard_conforming_strings to on;
CREATE TABLE t1 (id int, c2 varchar(20));
INSERT INTO t1 VALUES (1, 'moon\tlight');
INSERT INTO t1 VALUES (2, 'moon\nlight');
INSERT INTO t1 VALUES (3, 'moon\flight');
INSERT INTO t1 VALUES (4, 'moon\%light');
INSERT INTO t1 VALUES (5, 'moon\_light');
INSERT INTO t1 VALUES (6, 'moon\Zlight');
SELECT * FROM t1 ORDER BY id;

set standard_conforming_strings to off;
CREATE TABLE t2 (id int, c2 varchar(20));
INSERT INTO t2 VALUES (1, 'moon\tlight');
INSERT INTO t2 VALUES (2, 'moon\nlight');
INSERT INTO t2 VALUES (3, 'moon\flight');
INSERT INTO t2 VALUES (4, 'moon\%light');
INSERT INTO t2 VALUES (5, 'moon\_light');
INSERT INTO t2 VALUES (6, 'moon\Zlight');
SELECT * FROM t2 ORDER BY id;

--  result same with mysql
set standard_conforming_strings to off;
set escape_string_warning to off;
CREATE TABLE t3 (id int, c2 varchar(20));
INSERT INTO t3 VALUES (1, 'moon\tlight');
INSERT INTO t3 VALUES (2, 'moon\nlight');
INSERT INTO t3 VALUES (3, 'moon\flight');
INSERT INTO t3 VALUES (4, 'moon\%light');
INSERT INTO t3 VALUES (5, 'moon\_light');
INSERT INTO t3 VALUES (6, 'moon\Zlight');
SELECT * FROM t3 ORDER BY id;

--  result same with mysql
set standard_conforming_strings to on;
set escape_string_warning to on;
CREATE TABLE t4 (id int, c2 varchar(20));
INSERT INTO t4 VALUES (1, E'moon\tlight');
INSERT INTO t4 VALUES (2, E'moon\nlight');
INSERT INTO t4 VALUES (3, E'moon\flight');
INSERT INTO t4 VALUES (4, E'moon\%light');
INSERT INTO t4 VALUES (5, E'moon\_light');
INSERT INTO t4 VALUES (6, E'moon\Zlight');
SELECT * FROM t4 ORDER BY id;

drop table t1, t2, t3, t4;

-- test insert json
set standard_conforming_strings to off;
set escape_string_warning to off;
CREATE TABLE t_json (ID INT PRIMARY KEY, C2 JSON DEFAULT NULL);
INSERT INTO t_json VALUES (1, '{"id":"110", "name":"aaa\baa"}');
INSERT INTO t_json VALUES (2, '{"id":"110", "name":"aaa\faa"}');
SELECT * FROM t_json ORDER BY id;

drop table t_json;

drop schema escape_char cascade;
reset current_schema;
