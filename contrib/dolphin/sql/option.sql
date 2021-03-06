drop database if exists option;
create database option dbcompatibility = 'b';
\c option

create global temp table test1(a int primary key, b text) on commit delete rows engine = InnoDB with(STORAGE_TYPE = USTORE);

create global temp table test2(id int,vname varchar(48),remark text) engine = InnoDB on commit PRESERVE rows ;

create table test3(a int) FOR MATERIALIZED VIEW nocompress compression = pglz;

create table test4(a int) FOR MATERIALIZED VIEW nocompress compression = pglz FOR MATERIALIZED VIEW compression = zstd;

create table test5 nocompress with(orientation = column) as select * from test4;

create table test6 with(orientation = column) nocompress  with(orientation = row) as table test5;

create table test7 (a int primary key, b text);

CREATE TABLE if not exists test8
(
    month_code VARCHAR2 ( 30 ) NOT NULL ,
    dept_code  VARCHAR2 ( 30 ) NOT NULL ,
    user_no    VARCHAR2 ( 30 ) NOT NULL ,
    sales_amt  int
)
compress
with(FILLFACTOR = 40)
PARTITION BY LIST (month_code) SUBPARTITION BY LIST (dept_code)
(
  PARTITION p_201901 VALUES ( '201902' )
  (
    SUBPARTITION p_201901_a VALUES ( '1' ),
    SUBPARTITION p_201901_b VALUES ( '2' )
  ),
  PARTITION p_201902 VALUES ( '201903' )
  (
    SUBPARTITION p_201902_a VALUES ( '1' ),
    SUBPARTITION p_201902_b VALUES ( '2' )
  )
) nocompress;

CREATE TABLE if not exists test9
(
    month_code VARCHAR2 ( 30 ) NOT NULL ,
    dept_code  VARCHAR2 ( 30 ) NOT NULL ,
    user_no    VARCHAR2 ( 30 ) NOT NULL ,
    sales_amt  int
)
compress
with(FILLFACTOR = 40, STORAGE_TYPE = USTORE)
PARTITION BY LIST (month_code) SUBPARTITION BY LIST (dept_code)
(
  PARTITION p_201901 VALUES ( '201902' )
  (
    SUBPARTITION p_201901_a VALUES ( '1' ),
    SUBPARTITION p_201901_b VALUES ( '2' )
  ),
  PARTITION p_201902 VALUES ( '201903' )
  (
    SUBPARTITION p_201902_a VALUES ( '1' ),
    SUBPARTITION p_201902_b VALUES ( '2' )
  )
) nocompress;

CREATE TABLE test10
(
  I_INDEX        NUMBER(38) NOT NULL,
  SV_FEATURENAME VARCHAR2(64) NOT NULL,
  SV_ITEMNAME    VARCHAR2(64) NOT NULL,
  I_ITEMTYPE     NUMBER(38) NOT NULL,
  I_ITEMVALUEMIN NUMBER(38) NOT NULL,
  I_ITEMVALUEMAX NUMBER(38) NOT NULL,
  I_RESERVED1    NUMBER(38) DEFAULT 0,
  I_RESERVED2    NUMBER(38) DEFAULT 0,
  I_RESERVED3    NUMBER(38) DEFAULT 0,
  I_RESERVED4    NUMBER(38) DEFAULT 0,
  I_RESERVED5    NUMBER(38) DEFAULT 0,
  I_RESERVED6    NUMBER(38) DEFAULT 0,
  I_RESERVED7    NUMBER(38) DEFAULT 0,
  SV_RESERVED8   VARCHAR2(32) DEFAULT '',
  SV_RESERVED9   VARCHAR2(32) DEFAULT '',
  SV_RESERVED10  VARCHAR2(32) DEFAULT '',
  I_STATUS       NUMBER(38) NOT NULL
)
  PCTFREE 10
  INITRANS 1
  MAXTRANS 255
  STORAGE
  (
    INITIAL 64K
    MINEXTENTS 1
    MAXEXTENTS UNLIMITED
  );

create table test11 nocompress as select * from test4;

create table test12() inherits (test11);

create table if not exists test13(a int);

create index idx1 on test1 using ubtree(a) with(fillfactor = 30) INCLUDE(b);

create index idx2 on test8(user_no) LOCAL;

drop index idx2;

create index idx3 on test8(user_no) GLOBAL;

drop index idx3;

create index idx4 on test8(user_no) with(fillfactor = 30) with(fillfactor = 30);

drop index idx4;

create index idx5 on test9(user_no) LOCAL with(fillfactor = 30) include(dept_code) with(fillfactor = 30) include(month_code);

drop index idx5;

create index idx6 on test8(user_no) GLOBAL with(fillfactor = 30) with(fillfactor = 30);

drop index idx6;

CREATE UNIQUE INDEX idx7 ON test10 (I_INDEX DESC, I_STATUS)
  PCTFREE 10
  INITRANS 2
  MAXTRANS 255
  STORAGE
  (
    INITIAL 64K
    MINEXTENTS 1
    MAXEXTENTS UNLIMITED
  );

drop index idx7;

drop table test1;
drop table test2;
drop table test3;
drop table test4;
drop table test5;
drop table test6;
drop table test7;
drop table test8;
drop table test9;
drop table test10;

\c postgres

drop database option;
