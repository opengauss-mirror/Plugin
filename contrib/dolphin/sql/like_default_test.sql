-- b compatibility case
drop database if exists like_default_test;
-- create database like_default_test dbcompatibility 'b';
create database like_default_test with DBCOMPATIBILITY = 'B';
\c like_default_test

create table test_nv (name national varchar(10));
\d test_nv
drop table if exists test_nv;
create table test_nv (id int, name national varchar(10));
\d test_nv
drop table if exists test_nv;
create table test_nv (id int, name nvarchar(10));
\d test_nv
drop table if exists test_nv;


create table test_non(id int, name varchar(25));
create table test_part(id int, name varchar(25))
partition by range(id)
(
    partition p1 values less than(100),
    partition p2 values less than(200),
    partition p3 values less than(300),
    partition p4 values less than(400),
    partition p5 values less than(MAXVALUE)
);
create table test_non_like1 (like test_non);
\d test_non_like1
create table test_non_like2 like test_non;
\d test_non_like2
create table test_part_like1 (like test_part);
\d test_part_like1
create table test_part_like2 like test_part;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table if not exists test_non_like1 (like test_non);
\d test_non_like1
create table if not exists test_non_like2 like test_non;
\d test_non_like2
create table if not exists test_part_like1 (like test_part);
\d test_part_like1
create table if not exists test_part_like2 like test_part;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including indexes);
\d test_non_like1
create table test_non_like2 like test_non including indexes;
\d test_non_like2
create table test_part_like1 (like test_part including indexes);
\d test_part_like1
create table test_part_like2 like test_part including indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including partition);
\d test_non_like1
create table test_non_like2 like test_non including partition;
\d test_non_like2
create table test_part_like1 (like test_part including partition);
\d test_part_like1
create table test_part_like2 like test_part including partition;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including partition including indexes);
\d test_non_like1
create table test_non_like2 like test_non including partition including indexes;
\d test_non_like2
create table test_part_like1 (like test_part including partition including indexes);
\d test_part_like1
create table test_part_like2 like test_part including partition including indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including all);
\d test_non_like1
create table test_non_like2 like test_non including all;
\d test_non_like2
create table test_part_like1 (like test_part including all);
\d test_part_like1
create table test_part_like2 like test_part including all;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including all excluding indexes);
\d test_non_like1
create table test_non_like2 like test_non including all excluding indexes;
\d test_non_like2
create table test_part_like1 (like test_part including all excluding indexes);
\d test_part_like1
create table test_part_like2 like test_part including all excluding indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including all excluding partition);
\d test_non_like1
create table test_non_like2 like test_non including all excluding partition;
\d test_non_like2
create table test_part_like1 (like test_part including all excluding partition);
\d test_part_like1
create table test_part_like2 like test_part including all excluding partition;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including all excluding partition excluding indexes);
\d test_non_like1
create table test_non_like2 like test_non including all excluding partition excluding indexes;
\d test_non_like2
create table test_part_like1 (like test_part including all excluding partition excluding indexes);
\d test_part_like1
create table test_part_like2 like test_part including all excluding partition excluding indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non excluding indexes);
\d test_non_like1
create table test_non_like2 like test_non excluding indexes;
\d test_non_like2
create table test_part_like1 (like test_part excluding indexes);
\d test_part_like1
create table test_part_like2 like test_part excluding indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non excluding partition);
\d test_non_like1
create table test_non_like2 like test_non excluding partition;
\d test_non_like2
create table test_part_like1 (like test_part excluding partition);
\d test_part_like1
create table test_part_like2 like test_part excluding partition;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non excluding reloptions);
\d test_non_like1
create table test_non_like2 like test_non excluding reloptions;
\d test_non_like2
create table test_part_like1 (like test_part excluding reloptions);
\d test_part_like1
create table test_part_like2 like test_part excluding reloptions;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non excluding partition excluding indexes);
\d test_non_like1
create table test_non_like2 like test_non excluding partition excluding indexes;
\d test_non_like2
create table test_part_like1 (like test_part excluding partition excluding indexes);
\d test_part_like1
create table test_part_like2 like test_part excluding partition excluding indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create index test_non_id_idx on test_non(id);
create index test_part_id_idx on test_part(id);

create table test_non_like1 (like test_non);
\d test_non_like1
create table test_non_like2 like test_non;
\d test_non_like2
create table test_part_like1 (like test_part);
\d test_part_like1
create table test_part_like2 like test_part;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table if not exists test_non_like1 (like test_non);
\d test_non_like1
create table if not exists test_non_like2 like test_non;
\d test_non_like2
create table if not exists test_part_like1 (like test_part);
\d test_part_like1
create table if not exists test_part_like2 like test_part;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including indexes);
\d test_non_like1
create table test_non_like2 like test_non including indexes;
\d test_non_like2
create table test_part_like1 (like test_part including indexes);
\d test_part_like1
create table test_part_like2 like test_part including indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including partition);
\d test_non_like1
create table test_non_like2 like test_non including partition;
\d test_non_like2
create table test_part_like1 (like test_part including partition);
\d test_part_like1
create table test_part_like2 like test_part including partition;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including partition including indexes);
\d test_non_like1
create table test_non_like2 like test_non including partition including indexes;
\d test_non_like2
create table test_part_like1 (like test_part including partition including indexes);
\d test_part_like1
create table test_part_like2 like test_part including partition including indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including all);
\d test_non_like1
create table test_non_like2 like test_non including all;
\d test_non_like2
create table test_part_like1 (like test_part including all);
\d test_part_like1
create table test_part_like2 like test_part including all;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including all excluding indexes);
\d test_non_like1
create table test_non_like2 like test_non including all excluding indexes;
\d test_non_like2
create table test_part_like1 (like test_part including all excluding indexes);
\d test_part_like1
create table test_part_like2 like test_part including all excluding indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including all excluding partition);
\d test_non_like1
create table test_non_like2 like test_non including all excluding partition;
\d test_non_like2
create table test_part_like1 (like test_part including all excluding partition);
\d test_part_like1
create table test_part_like2 like test_part including all excluding partition;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non including all excluding partition excluding indexes);
\d test_non_like1
create table test_non_like2 like test_non including all excluding partition excluding indexes;
\d test_non_like2
create table test_part_like1 (like test_part including all excluding partition excluding indexes);
\d test_part_like1
create table test_part_like2 like test_part including all excluding partition excluding indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non excluding indexes);
\d test_non_like1
create table test_non_like2 like test_non excluding indexes;
\d test_non_like2
create table test_part_like1 (like test_part excluding indexes);
\d test_part_like1
create table test_part_like2 like test_part excluding indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non excluding partition);
\d test_non_like1
create table test_non_like2 like test_non excluding partition;
\d test_non_like2
create table test_part_like1 (like test_part excluding partition);
\d test_part_like1
create table test_part_like2 like test_part excluding partition;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

create table test_non_like1 (like test_non excluding partition excluding indexes);
\d test_non_like1
create table test_non_like2 like test_non excluding partition excluding indexes;
\d test_non_like2
create table test_part_like1 (like test_part excluding partition excluding indexes);
\d test_part_like1
create table test_part_like2 like test_part excluding partition excluding indexes;
\d test_part_like2
drop table if exists test_non_like1, test_non_like2, test_part_like1, test_part_like2;

drop table if exists test_non, test_part;

create table test_insert
(
    c1 int default 666,
    c2 float default 666.6,
    c3 char default 's',
    c4 text
);
insert into test_insert default values;

insert into test_insert values();
insert into test_insert values(1, 1.0);
insert into test_insert values(null, 1.0);
insert into test_insert values(2, 2.1, 'b', 'asdf');
insert into test_insert values(2, null, 'b', null);

insert into test_insert() values();
insert into test_insert() values(3, 3.2);
insert into test_insert() values(null, 3.2);
insert into test_insert() values(4, 4.3, 'c', 'sdf');
insert into test_insert() values(4, null, 'c', null);

insert into test_insert(c1, c2) values();
insert into test_insert(c1, c2) values(5, 5.4);
insert into test_insert(c1, c2) values(null, 5.4);
insert into test_insert(c1, c2) values(6, 6.5, 'd', 'dfas');
insert into test_insert(c1, c2) values(6, null, 'd', null);

insert into test_insert(c1, c2, c3, c4) values();
insert into test_insert(c1, c2, c3, c4) values(7, 7.6);
insert into test_insert(c1, c2, c3, c4) values(null, 7.6);
insert into test_insert(c1, c2, c3, c4) values(8, 8.7, 'e', 'dfa');
insert into test_insert(c1, c2, c3, c4) values(8, null, 'e', null);

select * from test_insert;

\c postgres
drop database if exists like_default_test;






