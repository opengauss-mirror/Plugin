create schema db_zerofill;
set current_schema to 'db_zerofill';

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

drop schema db_zerofill cascade;
reset current_schema;