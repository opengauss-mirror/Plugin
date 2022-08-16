drop database if exists uint_partition;
create database uint_partition dbcompatibility 'b';
\c uint_partition

CREATE TABLE t1
(
    col1 uint4 NOT NULL,
    col2 character varying(60)
) PARTITION BY RANGE (col1)
(
    PARTITION P1 VALUES LESS THAN(5000),
    PARTITION P2 VALUES LESS THAN(10000),
    PARTITION P3 VALUES LESS THAN(15000)
)
ENABLE ROW MOVEMENT;
insert into t1 values(3000);
insert into t1 values(11000);
insert into t1 values(9000);
insert into t1 values(16000);

select *from t1 partition(p1);
select *from t1 partition(p2);
select *from t1 partition(p3);

CREATE TABLE t2
(
    col1 uint4 NOT NULL,
    col2 character varying(60)
) PARTITION BY LIST (col1)
(
    PARTITION P1 VALUES IN(1, 2, 3),
    PARTITION P2 VALUES IN(4, 5, 6),
    PARTITION P3 VALUES IN(7, 8, 9, 10)
)
DISABLE ROW MOVEMENT;

insert into t2 values(1);
insert into t2 values(4);
insert into t2 values(10);
insert into t2 values(11);


-- CREATE TABLE t3
-- (
--     col1 uint4 NOT NULL,
--     col2 character varying(60)
-- ) PARTITION BY hash (col1)
-- (
--     PARTITION P1,
--     PARTITION P2,
--     PARTITION P3
-- );

-- insert into t3 values(1);
-- insert into t3 values(4);
-- insert into t3 values(10);
-- insert into t3 values(11);

--join and index
create table a1(a uint2, b uint4);
create table a2(a uint4, b uint8);
insert into a1 values(1, 3);
insert into a1 values(1, 4);
insert into a2 values(1, 5);
insert into a2 values(2, 3);


create index idx1 on a1(a, b);
create unique index idx2 on a1(a, b);
create index idx3 on a1 using hash (b);

\c postgres
drop database uint_partition;