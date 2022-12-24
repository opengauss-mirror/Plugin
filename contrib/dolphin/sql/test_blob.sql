create schema test_blob;
set current_schema to 'test_blob';
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

truncate test_tiny;
truncate test_blob;
truncate test_medium;
truncate test_long;

insert into test_tiny values('ppp');
insert into test_blob values('ppp');
insert into test_medium values('ppp');
insert into test_long values('ppp');

set dolphin.b_compatibility_mode = true;
set bytea_output = 'escape';

insert into test_blob values('ppp');

select * from test_tiny;
select * from test_blob;
select * from test_medium;
select * from test_long;

set dolphin.b_compatibility_mode = false;
select * from test_blob;

drop table test_template;
drop table test_template2;
drop table test_tiny;
drop table test_blob;
drop table test_medium;
drop table test_long;
drop schema test_blob cascade;
reset current_schema;