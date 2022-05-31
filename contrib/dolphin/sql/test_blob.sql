drop database if exists test;
create database test dbcompatibility 'B';
\c test
create table test_template (t tinyblob, b blob, m mediumblob, l longblob);
insert into test_template values('aaaaaaaaa', 'aaaaaaaaa', 'aaaaaaaaa', 'aaaaaaaaa');
create table test_tiny (t tinyblob);
create table test_blob (b blob);
create table test_medium (m mediumblob);
create table test_long (l longblob);

insert into test_tiny select t from test_template;
insert into test_tiny select b from test_template;
insert into test_tiny select m from test_template;
insert into test_tiny select l from test_template;

insert into test_blob select t from test_template;
insert into test_blob select b from test_template;
insert into test_blob select m from test_template;
insert into test_blob select l from test_template;

insert into test_medium select t from test_template;
insert into test_medium select b from test_template;
insert into test_medium select m from test_template;
insert into test_medium select l from test_template;

insert into test_long select t from test_template;
insert into test_long select b from test_template;
insert into test_long select m from test_template;
insert into test_long select l from test_template;

\COPY test_template to './test_template.data';
create table test_template2 (t tinyblob, b blob, m mediumblob, l longblob);
\COPY test_template2 from './test_template.data';

drop table test_template;
drop table test_template2;
drop table test_tiny;
drop table test_blob;
drop table test_medium;
drop table test_long;
\c postgres
drop database test;