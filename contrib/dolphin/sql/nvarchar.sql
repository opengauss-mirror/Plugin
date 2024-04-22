create schema db_nvarchar;
set current_schema to 'db_nvarchar';
--
-- VARCHAR
--

CREATE TABLE NVARCHAR_TBL(f1 nvarchar(1));

INSERT INTO NVARCHAR_TBL (f1) VALUES ('a');

INSERT INTO NVARCHAR_TBL (f1) VALUES ('A');

-- any of the following three input formats are acceptable
INSERT INTO NVARCHAR_TBL (f1) VALUES ('1');

INSERT INTO NVARCHAR_TBL (f1) VALUES (2);

INSERT INTO NVARCHAR_TBL (f1) VALUES ('3');

-- zero-length char
INSERT INTO NVARCHAR_TBL (f1) VALUES ('');

-- try varchar's of greater than 1 length
INSERT INTO NVARCHAR_TBL (f1) VALUES ('cd');
INSERT INTO NVARCHAR_TBL (f1) VALUES ('c     ');


SELECT '' AS seven, * FROM NVARCHAR_TBL;

SELECT '' AS six, c.*
   FROM NVARCHAR_TBL c
   WHERE c.f1 <> 'a';

SELECT '' AS one, c.*
   FROM NVARCHAR_TBL c
   WHERE c.f1 = 'a';

SELECT '' AS five, c.*
   FROM NVARCHAR_TBL c
   WHERE c.f1 < 'a';

SELECT '' AS six, c.*
   FROM NVARCHAR_TBL c
   WHERE c.f1 <= 'a';

SELECT '' AS one, c.*
   FROM NVARCHAR_TBL c
   WHERE c.f1 > 'a';

SELECT '' AS two, c.*
   FROM NVARCHAR_TBL c
   WHERE c.f1 >= 'a';

DROP TABLE NVARCHAR_TBL;

--
-- Now test longer arrays of char
--

CREATE TABLE NVARCHAR_TBL(f1 nvarchar(4));

INSERT INTO NVARCHAR_TBL (f1) VALUES ('a');
INSERT INTO NVARCHAR_TBL (f1) VALUES ('ab');
INSERT INTO NVARCHAR_TBL (f1) VALUES ('abcd');
INSERT INTO NVARCHAR_TBL (f1) VALUES ('abcde');
INSERT INTO NVARCHAR_TBL (f1) VALUES ('abcd    ');

SELECT '' AS four, * FROM NVARCHAR_TBL;

set dolphin.b_compatibility_mode to on;
set b_format_behavior_compat_options = 'enable_set_variables';
SET extra_float_digits = 3;

create table t_text0001(
    c1 int,
    c2 national varchar(1),
    c3 national varchar(10),
    c4 national varchar(255),
    c5 text,
    c6 text(1),
    c7 text(10),
    c8 text(255),
    c9 tinytext,
    c10 mediumtext,
    c11 longtext,
    c12 varchar(1),
    c13 varchar(255),
    c14 char(10)) charset utf8;
	
set @val = 3.14159265;

insert into t_text0001 values (1, substr(@val, 1, 1), @val, @val, @val, substr(@val, 1, 1), @val, @val, @val, @val, @val, substr(@val, 1, 1), @val, @val);

insert into t_text0001 values (1, substr(@val, 1, 1), @val::float4, @val::float4, @val::float4, substr(@val, 1, 1), @val::float4, @val::float4, @val::float4, @val::float4, @val::float4, substr(@val, 1, 1), @val::float4, @val::float4);


set @val = -12.1314;
insert into t_text0001 values (1, substr(@val, 1, 1), @val, @val, @val, substr(@val, 1, 1), @val, @val, @val, @val, @val, substr(@val, 1, 1), @val, @val);
insert into t_text0001 values (1, substr(@val, 1, 1), @val::float4, @val::float4, @val::float4, substr(@val, 1, 1), @val::float4, @val::float4, @val::float4, @val::float4, @val::float4, substr(@val, 1, 1), @val::float4, @val::float4);

select * from t_text0001;
drop table t_text0001;

--test extra_float_digits
reset extra_float_digits;
select 1.234::float4;
select 1.234::float8;
select 3.14159265358979323846::float4;
select 3.14159265358979323846::float8;
select concat(1.234::float4,'a');
select concat(1.234::float8,'a');
set extra_float_digits = 2;
select 1.234::float4;
select 1.234::float8;
select 3.14159265358979323846::float4;
select 3.14159265358979323846::float8;
select concat(1.234::float4,'a');
select concat(1.234::float8,'a');
set extra_float_digits = 3;
select 1.234::float4;
select 1.234::float8;
select 3.14159265358979323846::float4;
select 3.14159265358979323846::float8;
select concat(1.234::float4,'a');
select concat(1.234::float8,'a');

drop schema db_nvarchar cascade;
reset current_schema;