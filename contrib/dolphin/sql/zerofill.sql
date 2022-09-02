drop database if exists db_zerofill;
create database db_zerofill dbcompatibility 'b';
\c db_zerofill

create table t1_zerofill (
    a int(5) zerofill,
    b integer(5) zerofill unsigned,
    c tinyint(2) unsigned zerofill,
    d smallint(2) zerofill zerofill,
    e mediumint(2) unsigned zerofill zerofill,
    f bigint(2) zerofill,
    g int1 zerofill,
    h int2 zerofill unsigned,
    i int4 unsigned zerofill,
    j int8 zerofill);

\d+ t1_zerofill

-- some unspported types
create table t2_zerofill (a float zerofill);
create table t2_zerofill (a double precision zerofill);

\c postgres
drop database if exists db_zerofill;