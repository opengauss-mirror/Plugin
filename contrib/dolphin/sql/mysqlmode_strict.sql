create schema sql_mode_strict;
set current_schema to 'sql_mode_strict';
set dolphin.sql_mode = '';

create table test_tint(a tinyint);
create table test_sint(a smallint);
create table test_int(a int);
create table test_bint(a bigint);
create table test_float4(a float4);
create table test_float8(a float8);
create table test_char(a char);
create table test_varchar7(a varchar(7));

--insert into table values ...
insert into test_tint(a) values(-55);
insert into test_tint(a) values(55);
insert into test_tint(a) values(355);
insert into test_tint(a) values(-355);
insert into test_tint(a) values('a888');
insert into test_tint(a) values('888aa');
insert into test_tint(a) values('-88aa');
insert into test_tint(a) values(123423.55);
insert into test_tint(a) values(12.55);
insert into test_tint(a) values(-12.55);
insert into test_tint(a) values(-12123451.55);
insert into test_tint(a) values(1.4e300);
insert into test_tint(a) values(-1.4e300);
select * from test_tint;

insert into test_sint(a) values(-55);
insert into test_sint(a) values(55);
insert into test_sint(a) values(35589898923423423423423423);
insert into test_sint(a) values(-3553434343434343432434);
insert into test_sint(a) values('a888');
insert into test_sint(a) values('88123423433834343423434aa');
insert into test_sint(a) values('-88aa');
insert into test_sint(a) values(1234233434343434343434343434343.55);
insert into test_sint(a) values(12.55);
insert into test_sint(a) values(-12.55);
insert into test_sint(a) values(-12123451343434343434343434343434.55);
insert into test_sint(a) values(1.4e300);
insert into test_sint(a) values(-1.4e300);
select * from test_sint;

insert into test_int(a) values(-55);
insert into test_int(a) values(55);
insert into test_int(a) values(35589898923423423423423423);
insert into test_int(a) values(-3553434343434343432434);
insert into test_int(a) values('a888');
insert into test_int(a) values('88123423433834343423434aa');
insert into test_int(a) values('-88aa');
insert into test_int(a) values(1234233434343434343434343434343.55);
insert into test_int(a) values(12.55);
insert into test_int(a) values(-12.55);
insert into test_int(a) values(-12123451343434343434343434343434.55);
insert into test_int(a) values(1.4e300);
insert into test_int(a) values(-1.4e300);
select * from test_int;

insert into test_bint(a) values(-55);
insert into test_bint(a) values(55);
insert into test_bint(a) values(35589898923423423423423423);
insert into test_bint(a) values(-3553434343434343432434);
insert into test_bint(a) values('a888');
insert into test_bint(a) values('88123423433834343423434aa');
insert into test_bint(a) values('-88aa');
insert into test_bint(a) values(1234233434343434343434343434343.55);
insert into test_bint(a) values(12.55);
insert into test_bint(a) values(-12.55);
insert into test_bint(a) values(-12123451343434343434343434343434.55);
insert into test_bint(a) values(1.4e300);
insert into test_bint(a) values(-1.4e300);
select * from test_bint;

insert into test_float4(a) values(3.4e37);
insert into test_float4(a) values(-3.4e37);
select * from test_float4;

insert into test_float8(a) values(3.4e307);
insert into test_float8(a) values(-3.4e307);
select * from test_float8;

insert into test_char(a) values('adsfsdaf');
insert into test_char(a) values(-1234998.55);
insert into test_char(a) values(13434.77);
insert into test_char(a) values(13);
select * from test_char;

insert into test_varchar7(a) values('adsfsdaf');
insert into test_varchar7(a) values('-1234998.55');
insert into test_varchar7(a) values(13434.77);
insert into test_varchar7(a) values(13434.8);
select * from test_varchar7;



--insert into table select ...
create table test_tint1(a tinyint);
create table test_sint1(a smallint);
create table test_int1(a int);
create table test_bint1(a bigint);
create table test_char1(a char);
create table test_varchar7_1(a varchar(7));

insert into test_tint1 select * from test_sint;
select * from test_tint1;

delete from test_tint1;
insert into test_tint1 select * from test_int;
select * from test_tint1;

delete from test_tint1;
insert into test_tint1 select * from test_bint;
select * from test_tint1;

delete from test_tint1;
insert into test_tint1 select * from test_float4;
select * from test_tint1;

delete from test_tint1;
insert into test_tint1 select * from test_float8;
select * from test_tint1;

delete from test_tint1;
insert into test_tint1 select * from test_varchar7;
select * from test_tint1;

delete from test_tint1;
insert into test_tint1 select * from test_char;
select * from test_tint1;


insert into test_sint1 select * from test_tint;
select * from test_sint1;

delete from test_sint1;
insert into test_sint1 select * from test_int;
select * from test_sint1;

delete from test_sint1;
insert into test_sint1 select * from test_bint;
select * from test_sint1;

delete from test_sint1;
insert into test_sint1 select * from test_float4;
select * from test_sint1;

delete from test_sint1;
insert into test_sint1 select * from test_float8;
select * from test_sint1;

delete from test_sint1;
insert into test_sint1 select * from test_varchar7;
select * from test_sint1;

delete from test_sint1;
insert into test_sint1 select * from test_char;
select * from test_sint1;


insert into test_int1 select * from test_tint;
select * from test_int1;

delete from test_int1;
insert into test_int1 select * from test_sint;
select * from test_int1;

delete from test_int1;
insert into test_int1 select * from test_bint;
select * from test_int1;

delete from test_int1;
insert into test_int1 select * from test_float4;
select * from test_int1;

delete from test_int1;
insert into test_int1 select * from test_float8;
select * from test_int1;

delete from test_int1;
insert into test_int1 select * from test_varchar7;
select * from test_int1;

delete from test_int1;
insert into test_int1 select * from test_char;
select * from test_int1;


insert into test_bint1 select * from test_tint;
select * from test_bint1;

delete from test_bint1;
insert into test_bint1 select * from test_sint;
select * from test_bint1;

delete from test_bint1;
insert into test_bint1 select * from test_int;
select * from test_bint1;

delete from test_bint1;
insert into test_bint1 select * from test_float4;
select * from test_bint1;

delete from test_bint1;
insert into test_bint1 select * from test_float8;
select * from test_bint1;

delete from test_bint1;
insert into test_bint1 select * from test_varchar7;
select * from test_bint1;

delete from test_bint1;
insert into test_bint1 select * from test_char;
select * from test_bint1;


insert into test_char1 select * from test_tint;
select * from test_char1;

delete from test_char1;
insert into test_char1 select * from test_sint;
select * from test_char1;

delete from test_char1;
insert into test_char1 select * from test_int;
select * from test_char1;

delete from test_char1;
insert into test_char1 select * from test_bint;
select * from test_char1;

delete from test_char1;
insert into test_char1 select * from test_varchar7;
select * from test_char1;


insert into test_varchar7_1 select * from test_tint;
select * from test_varchar7_1;

delete from test_varchar7_1;
insert into test_varchar7_1 select * from test_sint;
select * from test_varchar7_1;

delete from test_varchar7_1;
insert into test_varchar7_1 select * from test_int;
select * from test_varchar7_1;

delete from test_varchar7_1;
insert into test_varchar7_1 select * from test_bint;
select * from test_varchar7_1;

delete from test_varchar7_1;
insert into test_varchar7_1 select * from test_char;
select * from test_varchar7_1;

--A non-null column is not inserted when executing insert into clause, and this non-null column has no default value
create table test_notnull_tint(a tinyint, b tinyint not null);
create table test_notnull_sint(a smallint, b smallint not null);
create table test_notnull_int(a int, b int not null);
create table test_notnull_bint(a bigint, b bigint not null);
create table test_notnull_char(a char, b char not null);
create table test_notnull_varchar(a varchar(5), b varchar(5) not null);
create table test_notnull_clob(a clob, b clob not null);
create table test_notnull_text(a text, b text not null);
create table test_notnull_real(a real, b real not null);
create table test_notnull_double(a double, b double not null);
create table test_notnull_numeric(a numeric(5,2), b numeric(5,2) not null);

insert into test_notnull_tint(a) values(1);
select * from test_notnull_tint;

insert into test_notnull_sint(a) values(1);
select * from test_notnull_sint;

insert into test_notnull_int(a) values(1);
select * from test_notnull_int;

insert into test_notnull_bint(a) values(1);
select * from test_notnull_bint;

insert into test_notnull_char(a) values(1);
select * from test_notnull_char;

insert into test_notnull_varchar(a) values(1);
select * from test_notnull_varchar;

insert into test_notnull_clob(a) values(1);
select * from test_notnull_clob;

insert into test_notnull_text(a) values(1);
select * from test_notnull_text;

insert into test_notnull_real(a) values(1);
select * from test_notnull_real;

insert into test_notnull_double(a) values(1);
select * from test_notnull_double;

insert into test_notnull_numeric(a) values(1);
select * from test_notnull_numeric;



--insert into table values(null)
insert into test_notnull_tint(b) values(null);

insert into test_notnull_sint(b) values(null);

insert into test_notnull_int(b) values(null);

insert into test_notnull_bint(b) values(null);

insert into test_notnull_char(b) values(null);

insert into test_notnull_varchar(b) values(null);

insert into test_notnull_clob(b) values(null);

insert into test_notnull_text(b) values(null);

insert into test_notnull_real(b) values(null);

insert into test_notnull_double(b) values(null);

insert into test_notnull_numeric(b) values(null);


--insert into table(a) values(...,...)
insert into test_notnull_tint(a) values(2,2);
insert into test_notnull_sint(a) values(2,2);
insert into test_notnull_int(a) values(2,2);
insert into test_notnull_bint(a) values(2,2);
insert into test_notnull_char(a) values(2,2);
insert into test_notnull_varchar(a) values(2,2);
insert into test_notnull_clob(a) values(2,2);
insert into test_notnull_text(a) values(2,2);
insert into test_notnull_real(a) values(2,2);
insert into test_notnull_double(a) values(2,2);
insert into test_notnull_numeric(a) values(2,2);


--insert into table values(...,default,...)
insert into test_notnull_tint values(2,default);
select * from test_notnull_tint;

insert into test_notnull_sint values(2,default);
select * from test_notnull_sint;

insert into test_notnull_int values(2,default);
select * from test_notnull_int;

insert into test_notnull_bint values(2,default);
select * from test_notnull_bint;

insert into test_notnull_char values(2,default);
select * from test_notnull_char;

insert into test_notnull_varchar values(2,default);
select * from test_notnull_varchar;

insert into test_notnull_clob values(2,default);
select * from test_notnull_clob;

insert into test_notnull_text values(2,default);
select * from test_notnull_text;

insert into test_notnull_real values(2,default);
select * from test_notnull_real;

insert into test_notnull_double values(2,default);
select * from test_notnull_double;

insert into test_notnull_numeric values(2,default);
select * from test_notnull_numeric;

create table test_multi_default(a numeric not null,b decimal not null,c real not null,d double precision not null, e float not null, f varchar(20) not null);
insert into test_multi_default values(default,default,default,default,default,default);
select * from test_multi_default;

--insert into table values(...),(...)
insert into test_notnull_tint(a) values(3),(4),(5);
select * from test_notnull_tint;

insert into test_notnull_sint(a) values(3),(4),(5);
select * from test_notnull_sint;

insert into test_notnull_int(a) values(3),(4),(5);
select * from test_notnull_int;

insert into test_notnull_bint(a) values(3),(4),(5);
select * from test_notnull_bint;

insert into test_notnull_char(a) values(3),(4),(5);
select * from test_notnull_char;

insert into test_notnull_varchar(a) values(3),(4),(5);
select * from test_notnull_varchar;

insert into test_notnull_clob(a) values(3),(4),(5);
select * from test_notnull_clob;

insert into test_notnull_text(a) values(3),(4),(5);
select * from test_notnull_text;

insert into test_notnull_real(a) values(3),(4),(5);
select * from test_notnull_real;

insert into test_notnull_double(a) values(3),(4),(5);
select * from test_notnull_double;

insert into test_notnull_numeric(a) values(3),(4),(5);
select * from test_notnull_numeric;

create table mysql_strictsqlmode_t1(c1 int,c2 double precision);
insert into mysql_strictsqlmode_t1 values(1,0);
insert into mysql_strictsqlmode_t1 values(2,-99);
insert into mysql_strictsqlmode_t1 values(3,-999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999);
insert into mysql_strictsqlmode_t1 values(4,99);
insert into mysql_strictsqlmode_t1 values(5,999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999);
select * from mysql_strictsqlmode_t1 order by c1;
drop table mysql_strictsqlmode_t1;

--test sql_mode = 'sql_mode_strict'
set dolphin.sql_mode = 'sql_mode_strict';
create table test_tint_strict(a tinyint);
create table test_sint_strict(a smallint);
create table test_int_strict(a int);
create table test_bint_strict(a bigint);
create table test_float4_strict(a float4);
create table test_float8_strict(a float8);
create table test_char_strict(a char);
create table test_varchar7_strict(a varchar(7));
    
--insert into table values ...
insert into test_tint_strict(a) values(-55);
insert into test_tint_strict(a) values(55);
insert into test_tint_strict(a) values(355);
insert into test_tint_strict(a) values(-355);
insert into test_tint_strict(a) values('a888');
insert into test_tint_strict(a) values('888aa');
insert into test_tint_strict(a) values('-88aa');
insert into test_tint_strict(a) values(123423.55);
insert into test_tint_strict(a) values(12.55);
insert into test_tint_strict(a) values(-12.55);
insert into test_tint_strict(a) values(-12123451.55);
insert into test_tint_strict(a) values(1.4e300);
insert into test_tint_strict(a) values(-1.4e300);
select * from test_tint_strict;
    
insert into test_sint_strict(a) values(-55);
insert into test_sint_strict(a) values(55);
insert into test_sint_strict(a) values(35589898923423423423423423);
insert into test_sint_strict(a) values(-3553434343434343432434);
insert into test_sint_strict(a) values('a888');
insert into test_sint_strict(a) values('88123423433834343423434aa');
insert into test_sint_strict(a) values('-88aa');
insert into test_sint_strict(a) values(1234233434343434343434343434343.55);
insert into test_sint_strict(a) values(12.55);
insert into test_sint_strict(a) values(-12.55);
insert into test_sint_strict(a) values(-12123451343434343434343434343434.55);
insert into test_sint_strict(a) values(1.4e300);
insert into test_sint_strict(a) values(-1.4e300);
select * from test_sint_strict;
    
insert into test_int_strict(a) values(-55);
insert into test_int_strict(a) values(55);
insert into test_int_strict(a) values(35589898923423423423423423);
insert into test_int_strict(a) values(-3553434343434343432434);
insert into test_int_strict(a) values('a888');
insert into test_int_strict(a) values('88123423433834343423434aa');
insert into test_int_strict(a) values('-88aa');
insert into test_int_strict(a) values(1234233434343434343434343434343.55);
insert into test_int_strict(a) values(12.55);
insert into test_int_strict(a) values(-12.55);
insert into test_int_strict(a) values(-12123451343434343434343434343434.55);
insert into test_int_strict(a) values(1.4e300);
insert into test_int_strict(a) values(-1.4e300);
select * from test_int_strict;
    
insert into test_bint_strict(a) values(-55);
insert into test_bint_strict(a) values(55);
insert into test_bint_strict(a) values(35589898923423423423423423);
insert into test_bint_strict(a) values(-3553434343434343432434);
insert into test_bint_strict(a) values('a888');
insert into test_bint_strict(a) values('88123423433834343423434aa');
insert into test_bint_strict(a) values('-88aa');
insert into test_bint_strict(a) values(1234233434343434343434343434343.55);
insert into test_bint_strict(a) values(12.55);
insert into test_bint_strict(a) values(-12.55);
insert into test_bint_strict(a) values(-12123451343434343434343434343434.55);
insert into test_bint_strict(a) values(1.4e300);
insert into test_bint_strict(a) values(-1.4e300);
select * from test_bint_strict;
    
insert into test_float4_strict(a) values(3.4e37);
insert into test_float4_strict(a) values(-3.4e37);
select * from test_float4_strict;
    
insert into test_float8_strict(a) values(3.4e307);
insert into test_float8_strict(a) values(-3.4e307);
select * from test_float8_strict;
    
insert into test_char_strict(a) values('adsfsdaf');
insert into test_char_strict(a) values(-1234998.55);
insert into test_char_strict(a) values(13434.77);
insert into test_char_strict(a) values(13);
select * from test_char_strict;
    
insert into test_varchar7_strict(a) values('adsfsdaf');
insert into test_varchar7_strict(a) values('-1234998.55');
insert into test_varchar7_strict(a) values(13434.77);
insert into test_varchar7_strict(a) values(13434.8);
select * from test_varchar7_strict;
    
    
    
--insert into table select ...
create table test_tint_strict1(a tinyint);
create table test_sint_strict1(a smallint);
create table test_int_strict1(a int);
create table test_bint_strict1(a bigint);
create table test_char_strict1(a char);
create table test_varchar7_strict_1(a varchar(7));
    
insert into test_tint_strict1 select * from test_sint_strict;
select * from test_tint_strict1;
    
delete from test_tint_strict1;
insert into test_tint_strict1 select * from test_int_strict;
select * from test_tint_strict1;
    
delete from test_tint_strict1;
insert into test_tint_strict1 select * from test_bint_strict;
select * from test_tint_strict1;
    
delete from test_tint_strict1;
insert into test_tint_strict1 select * from test_float4_strict;
select * from test_tint_strict1;
    
delete from test_tint_strict1;
insert into test_tint_strict1 select * from test_float8_strict;
select * from test_tint_strict1;
    
delete from test_tint_strict1;
insert into test_tint_strict1 select * from test_varchar7_strict;
select * from test_tint_strict1;
    
delete from test_tint_strict1;
insert into test_tint_strict1 select * from test_char_strict;
select * from test_tint_strict1;
    
    
insert into test_sint_strict1 select * from test_tint_strict;
select * from test_sint_strict1;
    
delete from test_sint_strict1;
insert into test_sint_strict1 select * from test_int_strict;
select * from test_sint_strict1;
    
delete from test_sint_strict1;
insert into test_sint_strict1 select * from test_bint_strict;
select * from test_sint_strict1;
    
delete from test_sint_strict1;
insert into test_sint_strict1 select * from test_float4_strict;
select * from test_sint_strict1;
    
delete from test_sint_strict1;
insert into test_sint_strict1 select * from test_float8_strict;
select * from test_sint_strict1;
    
delete from test_sint_strict1;
insert into test_sint_strict1 select * from test_varchar7_strict;
select * from test_sint_strict1;
    
delete from test_sint_strict1;
insert into test_sint_strict1 select * from test_char_strict;
select * from test_sint_strict1;
    
    
insert into test_int_strict1 select * from test_tint_strict;
select * from test_int_strict1;
    
delete from test_int_strict1;
insert into test_int_strict1 select * from test_sint_strict;
select * from test_int_strict1;
    
delete from test_int_strict1;
insert into test_int_strict1 select * from test_bint_strict;
select * from test_int_strict1;
    
delete from test_int_strict1;
insert into test_int_strict1 select * from test_float4_strict;
select * from test_int_strict1;
    
delete from test_int_strict1;
insert into test_int_strict1 select * from test_float8_strict;
select * from test_int_strict1;
    
delete from test_int_strict1;
insert into test_int_strict1 select * from test_varchar7_strict;
select * from test_int_strict1;
    
delete from test_int_strict1;
insert into test_int_strict1 select * from test_char_strict;
select * from test_int_strict1;
    
    
insert into test_bint_strict1 select * from test_tint_strict;
select * from test_bint_strict1;
    
delete from test_bint_strict1;
insert into test_bint_strict1 select * from test_sint_strict;
select * from test_bint_strict1;
    
delete from test_bint_strict1;
insert into test_bint_strict1 select * from test_int_strict;
select * from test_bint_strict1;
    
delete from test_bint_strict1;
insert into test_bint_strict1 select * from test_float4_strict;
select * from test_bint_strict1;
    
delete from test_bint_strict1;
insert into test_bint_strict1 select * from test_float8_strict;
select * from test_bint_strict1;
    
delete from test_bint_strict1;
insert into test_bint_strict1 select * from test_varchar7_strict;
select * from test_bint_strict1;
    
delete from test_bint_strict1;
insert into test_bint_strict1 select * from test_char_strict;
select * from test_bint_strict1;
    
    
insert into test_char_strict1 select * from test_tint_strict;
select * from test_char_strict1;
    
delete from test_char_strict1;
insert into test_char_strict1 select * from test_sint_strict;
select * from test_char_strict1;
    
delete from test_char_strict1;
insert into test_char_strict1 select * from test_int_strict;
select * from test_char_strict1;
    
delete from test_char_strict1;
insert into test_char_strict1 select * from test_bint_strict;
select * from test_char_strict1;
    
delete from test_char_strict1;
insert into test_char_strict1 select * from test_varchar7_strict;
select * from test_char_strict1;
    
    
insert into test_varchar7_strict_1 select * from test_tint_strict;
select * from test_varchar7_strict_1;
    
delete from test_varchar7_strict_1;
insert into test_varchar7_strict_1 select * from test_sint_strict;
select * from test_varchar7_strict_1;
    
delete from test_varchar7_strict_1;
insert into test_varchar7_strict_1 select * from test_int_strict;
select * from test_varchar7_strict_1;
    
delete from test_varchar7_strict_1;
insert into test_varchar7_strict_1 select * from test_bint_strict;
select * from test_varchar7_strict_1;
    
delete from test_varchar7_strict_1;
insert into test_varchar7_strict_1 select * from test_char_strict;
select * from test_varchar7_strict_1;
    
--A non-null column is not inserted when executing insert into clause, and this non-null column has no default value
create table test_notnull_tint_strict(a tinyint, b tinyint not null);
create table test_notnull_sint_strict(a smallint, b smallint not null);
create table test_notnull_int_strict(a int, b int not null);
create table test_notnull_bint_strict(a bigint, b bigint not null);
create table test_notnull_char_strict(a char, b char not null);
create table test_notnull_varchar_strict(a varchar(5), b varchar(5) not null);
create table test_notnull_clob_strict(a clob, b clob not null);
create table test_notnull_text_strict(a text, b text not null);
create table test_notnull_real_strict(a real, b real not null);
create table test_notnull_double_strict(a double, b double not null);
create table test_notnull_numeric_strict(a numeric(5,2), b numeric(5,2) not null);
    
insert into test_notnull_tint_strict(a) values(1);
select * from test_notnull_tint_strict;
    
insert into test_notnull_sint_strict(a) values(1);
select * from test_notnull_sint_strict;
    
insert into test_notnull_int_strict(a) values(1);
select * from test_notnull_int_strict;
    
insert into test_notnull_bint_strict(a) values(1);
select * from test_notnull_bint_strict;
    
insert into test_notnull_char_strict(a) values(1);
select * from test_notnull_char_strict;
    
insert into test_notnull_varchar_strict(a) values(1);
select * from test_notnull_varchar_strict;
    
insert into test_notnull_clob_strict(a) values(1);
select * from test_notnull_clob_strict;
    
insert into test_notnull_text_strict(a) values(1);
select * from test_notnull_text_strict;
    
insert into test_notnull_real_strict(a) values(1);
select * from test_notnull_real_strict;
    
insert into test_notnull_double_strict(a) values(1);
select * from test_notnull_double_strict;
    
insert into test_notnull_numeric_strict(a) values(1);
select * from test_notnull_numeric_strict;
    
    
    
--insert into table values(null)
insert into test_notnull_tint_strict(b) values(null);
    
insert into test_notnull_sint_strict(b) values(null);
    
insert into test_notnull_int_strict(b) values(null);
    
insert into test_notnull_bint_strict(b) values(null);
    
insert into test_notnull_char_strict(b) values(null);
    
insert into test_notnull_varchar_strict(b) values(null);
    
insert into test_notnull_clob_strict(b) values(null);
    
insert into test_notnull_text_strict(b) values(null);
    
insert into test_notnull_real_strict(b) values(null);
    
insert into test_notnull_double_strict(b) values(null);
    
insert into test_notnull_numeric_strict(b) values(null);


--test some bug fix
create table test_space_to_int1(a tinyint);
insert into test_space_to_int1 values('34 55');
create table test_space_to_int2(a smallint);
insert into test_space_to_int2 values('34 55');
create table test_space_to_int4(a int);
insert into test_space_to_int4 values('34 55');
create table test_space_to_int8(a bigint);
insert into test_space_to_int8 values('34 55');
set dolphin.sql_mode = '';
insert into test_space_to_int1 values('34 55');
insert into test_space_to_int2 values('34 55');
insert into test_space_to_int4 values('34 55');
insert into test_space_to_int8 values('34 55');
select * from test_space_to_int1;
select * from test_space_to_int2;
select * from test_space_to_int4;
select * from test_space_to_int8;

drop schema sql_mode_strict cascade;
reset current_schema;
