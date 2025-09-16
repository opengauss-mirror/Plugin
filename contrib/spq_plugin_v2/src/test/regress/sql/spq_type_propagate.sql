----------------------- test 1: enum related -----------------------------
-- create enum
CREATE TYPE bugstatus AS ENUM ('create', 'modify', 'closed');
\dT+ bugstatus

\c - - - :worker_1_port
\dT+ bugstatus

\c - - - :worker_2_port
\dT+ bugstatus

\c - - - :master_port
-- alter enum add one value
ALTER TYPE bugstatus ADD VALUE IF NOT EXISTS 'regress' BEFORE 'closed';
\dT+ bugstatus

\c - - - :worker_1_port
\dT+ bugstatus

\c - - - :worker_2_port
\dT+ bugstatus

\c - - - :master_port
-- rename enum value
ALTER TYPE bugstatus RENAME VALUE 'create' TO 'new';
\dT+ bugstatus

\c - - - :worker_1_port
\dT+ bugstatus

\c - - - :worker_2_port
\dT+ bugstatus

\c - - - :master_port
-- rename enum
alter type bugstatus rename to bs;
\dT+ bugstatus
\dT+ bs

\c - - - :worker_1_port
\dT+ bugstatus
\dT+ bs

\c - - - :worker_2_port
\dT+ bugstatus
\dT+ bs

\c - - - :master_port
CREATE TABLE bug_reports (
    id int PRIMARY KEY,
    title TEXT NOT NULL,
    status bs NOT NULL
);
SELECT create_distributed_table('bug_reports', 'id', shard_count:=4);

INSERT INTO bug_reports VALUES (1, '页面错误1', 'new');
INSERT INTO bug_reports VALUES (2, '页面错误2', 'modify');
INSERT INTO bug_reports VALUES (3, '页面错误3', 'regress');
INSERT INTO bug_reports VALUES (4, '页面错误4', 'closed');

-- error
INSERT INTO bug_reports VALUES (5, '接口问题5', 'open');

select * from bug_reports order by id;
-- drop enum error
drop type bs;

drop table bug_reports;
-- success
drop type bs;
\dT+ bs

\c - - - :worker_1_port
\dT+ bs

\c - - - :worker_2_port
\dT+ bs

----------------------- test 2: composite type -----------------------------
\c - - - :master_port
create type address_type AS (
    street varchar(100),
    city varchar(50),
    country varchar(50)
);

\c - - - :worker_1_port
select count(1) from pg_type where typname = 'address_type';

\c - - - :worker_2_port
select count(1) from pg_type where typname = 'address_type';

\c - - - :master_port
create table users (
    id int primary key,
    name varchar(100),
    home_address address_type
);

SELECT create_distributed_table('users', 'id', shard_count:=4);
insert into users values (1, 'BOB', ('456 Elm St', 'London', 'UK')::address_type);
insert into users values (2, 'Tom', ('789 Elm St', 'New York', 'USA')::address_type);
select * from users order by id;

alter type address_type add attribute c float;
insert into users values (3, 'ZS', ('000 Elm St', 'Beijing', 'PRC', 8.8)::address_type);
select * from users order by id;

-- error
drop type address_type;
drop type address_type CASCADE;
select * from users order by id;

\c - - - :worker_1_port
select count(1) from pg_type where typname = 'address_type';

\c - - - :worker_2_port
select count(1) from pg_type where typname = 'address_type';

\c - - - :master_port
drop table users;

-- test type alter schema
\c - - - :master_port
create type co AS (
    x int,
    y int
);
create schema s1;
\c - - - :worker_1_port
-- expect 0
select count(1) from pg_type where typname = 'co' and typnamespace in (select oid from pg_namespace where nspname = 's1');

\c - - - :master_port
alter type co set schema s1;
\c - - - :worker_1_port
-- expect 1
select count(1) from pg_type where typname = 'co' and typnamespace in (select oid from pg_namespace where nspname = 's1');

\c - - - :master_port
drop type s1.co;
drop schema s1;
