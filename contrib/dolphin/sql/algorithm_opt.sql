create user algorithm with password 'algorithm@123';
grant usage on schema public to algorithm;
grant create on schema public to algorithm;

-- -------- test create view cases --------
create table va_1_t(
	c1 int,
    c2 varchar(20),
    c3 date
);
insert into va_1_t values(1, 'hello', '2022-01-02');
insert into va_1_t values(2, 'hello2', '2022-01-02');
insert into va_1_t values(3, 'hello3', '2022-01-02');
select * from va_1_t;

-- test basic create
create algorithm=UNDEFINED view algo_c_v1 as select * from va_1_t;
select * from algo_c_v1;
create algorithm=MERGE view algo_c_v2 as select * from va_1_t;
select * from algo_c_v2;
create algorithm=TEMPTABLE view algo_c_v3 as select * from va_1_t;
select * from algo_c_v3;

-- test replace view
create   Or  ReplacE algorithm=UNDEFINED view algo_c_v1 as select * from va_1_t;
select  * from algo_c_v1;

-- test irregular space
create    Or                        
    RepLacE ALgorIthM   =   MERGE LOCAL TEMP view algo_c_v2 as select * from va_1_t;
select * from algo_c_v2;

   create  		  OR  REPLACE    alGoRithM		

    =
        TEmPtABlE view algo_c_v3 as select * from va_1_t;
select * from algo_c_v3;

-- test definer case
create or replace algorithm=TEMPTABLE definer= algorithm TEMPORARY view algo_c_v4 as select c1, c2 from va_1_t;
\d algo_c_v4

-- test should error
create algorithm TEMPTABLE view algo_c_v5 as select * from va_1_t;
create algorithm='TEMPTABLE' view algo_c_v6 as select * from va_1_t;
create view algorithm=MERge algo_c_v7 as select * from va_1_t;
create algorithm='hello' view algo_c_v8 as select * from va_1_t;

-- test coincident
create table algorithm (
    algorithm int default 100,
    UNDEFINED int default 100,
    MERGE int not null,
    TEMPTABLE int
);
insert into algorithm values(1, 1, 1, 1);
insert into algorithm values(2, 2, 2, 2);
select * from algorithm t;
select algorithm=UNDEFINED, algorithm=MERGE, algorithm = TEMPTABLE from algorithm;

create algorithm=merge view algorithm_v as 
    select algorithm=UNDEFINED as algorithm, 
           algorithm=merge as UNDEFINED,
           algorithm = temptable as temptable 
        from algorithm;
select * from algorithm_v;

create table UNDEFINED(
    algorithm int
);
create table MERGE(
    algorithm int
);
create table TEMPTABLE(
    algorithm int
);

-- ----- test alter view cases -------------
-- test basic case
alter algorithm=UNDEFINED view algo_c_v1 as select * from va_1_t;
alter algorithm=MERGE view algo_c_v2 as select * from va_1_t;
select * from algo_c_v2;
-- test irregular space
	  			alter 			  
			
		
		algorithm			 =temptable view algo_c_v1 as select * from va_1_t;
select * from algo_c_v1;
ALTER algorithm=UNDEFINED VIEW  algo_c_v1 RENAME TO algo_c_v1_2;

alter algorithm=MERGE view algo_c_v2 as select * from va_1_t;
alter algorithm=MERGE definer = algorithm view algo_c_v2 as select * from va_1_t;

-- test should error
alter algorithm=TEMPTABLE view algo_c_v3 as select c3 from va_1_t; -- openGauss does not support drop columns currently
alter algorithm TEMPTABLE view algo_c_v3 as select c3 from algo_t;
alter algorithm='TEMPTABLE' view algo_c_v3 as select c3 from algo_t;
alter algorithm='' view algo_c_v3 as select c3 from algo_t;
alter view algorithm=TEMPTABLE algo_c_v3 as select c3 from algo_t;

-- test schema/set/reset/error place
create table vv_t (c1 int, c2 varchar(30));
create algorithm=UNDEFINED view vv_v as select * from vv_t;
alter view vv_v alter c1 set default 1;
alter view vv_v alter c1 set default 1 algorithm default;
alter algorithm =merge  view vv_v alter c1 set default 1;
alter algorithm =merge  view vv_v alter c1 drop default;

alter algorithm =  view vv_v alter c1 set default 1;
alter algorithm = 1 view vv_v alter c1 set default 1;

create user nu with password 'newUser@123';
create or replace algorithm = bad view vv_v2 as select * from vv_t;
create or replace algorithm = merge view vv_v2 as select * from vv_t;

alter algorithm =merge  view vv_v2 owner to nu;
alter algorithm =TEMPTABLE  view vv_v2 rename to vv_nv;
create schema vs;
alter algorithm =undifined  view vv_nv set schema vs;
alter algorithm =undefined  view vv_nv set schema vs;
alter view if exists vv_v set (check_option=local);
alter algorithm=temptable view if exists vv_v set (check_option=local);
alter algorithm=temptable view if exists vv_v reset (check_option);

create table v_test(a int);
CREATE ALGORITHM = UNDEFINED DEFINER = `algorithm` SQL SECURITY DEFINER VIEW alg_definer_sql AS SELECT * FROM v_test;
CREATE ALGORITHM = UNDEFINED SQL SECURITY DEFINER VIEW alg_sql AS SELECT * FROM v_test;
ALTER ALGORITHM = UNDEFINED DEFINER = `algorithm` SQL SECURITY DEFINER VIEW alg_sql as SELECT * FROM v_test;
ALTER ALGORITHM = UNDEFINED SQL SECURITY DEFINER VIEW alg_definer_sql as SELECT * FROM v_test;
drop view alg_sql;
drop view alg_definer_sql;
drop table v_test;

drop table vv_t cascade;
drop user nu cascade;
drop schema vs cascade;
drop table UNDEFINED, MERGE, TEMPTABLE cascade;
drop table va_1_t, algorithm cascade;

-- ---------- test create/drop index cases -----------------
create table algo_idx_t(
	c1 int,
    c2 varchar(20),
    c3 date,
    c4 int,
    c5 int
);
insert into algo_idx_t values(1, 'dsfdsg', '2022-01-02');
insert into algo_idx_t values(2, 'dsfds', '2022-01-04');
insert into algo_idx_t values(3, 'sdgsdf', '2022-01-06');

-- basic cases
create index algo_idx_c1 on algo_idx_t(c1) algorithm COPY;
\d algo_idx_c1
create index CONCURRENTLY algo_idx_c1_2 on algo_idx_t(c1) algorithm=COPY;
\d algo_idx_c1_2
create index algo_idx_c1_3 on algo_idx_t(c1) algorithm DEFAULT;
create index algo_idx_c1_4 on algo_idx_t(c1) algorithm = DEFAULT;
create index algo_idx_c1_5 on algo_idx_t(c1) algorithm INPLACE;
create index algo_idx_c1_6 on algo_idx_t(c1) algorithm = INPLACE;
create index on algo_idx_t(c1) algorithm = INPLACE;
create unique index CONCURRENTLY algo_idx_c2c1 using btree on algo_idx_t(c2, c1);
create unique index CONCURRENTLY algo_idx_c3 on algo_idx_t using btree(c3) algorithm copy;
create index CONCURRENTLY algo_idx_c4 on algo_idx_t using btree (c4) algorithm INPLACE;

drop index algo_idx_c1_3 on algo_idx_t algorithm DEFAULT;
drop index algo_idx_c1_4 on algo_idx_t algorithm = copy;
drop index algo_idx_c1_5 on algo_idx_t algorithm INPLACE;
drop index algo_idx_c1_6 on algo_idx_t algorithm = INPLACE;

-- if not exists
create index if not exists algo_idx_c1_3 on algo_idx_t(c1) algorithm DEFAULT;
create index if not exists algo_idx_c1_4 on algo_idx_t(c1) algorithm = copy;
create index if not exists algo_idx_c1_5 on algo_idx_t(c1) algorithm INPLACE;
create index if not exists algo_idx_c1_6 on algo_idx_t(c1) algorithm = INPLACE;

create unique index CONCURRENTLY if not exists algo_idx_c2c1 using btree on algo_idx_t(c2, c1) ;
create unique index CONCURRENTLY if not exists algo_idx_c3 on algo_idx_t using btree (c3) algorithm copy;
create index CONCURRENTLY if not exists algo_idx_c4 on algo_idx_t using btree (c4) algorithm INPLACE;

drop index algo_idx_c1_3 on algo_idx_t algorithm DEFAULT;
drop index algo_idx_c1_4 on algo_idx_t algorithm = DEFAULT;
drop index algo_idx_c1_5 on algo_idx_t algorithm INPLACE;
drop index algo_idx_c1_6 on algo_idx_t algorithm = INPLACE;
drop index algo_idx_c2c1 on algo_idx_t algorithm = INPLACE;
drop index algo_idx_c3 on algo_idx_t algorithm = coPy;
drop index algo_idx_c4 on algo_idx_t algorithm coPy;

-- test unique
create unique index algo_idx_c1c2_1 on algo_idx_t(c1, c2, c3) algorithm = INPLACE;
\d algo_idx_c1c2_1
create unique index CONCURRENTLY algo_idx_c1c2c3 on algo_idx_t(c1, c2, c3) algorithm = COPY;
\d algo_idx_c1c2c3

drop index algo_idx_c1c2_1 on algo_idx_t algorithm = COPY;
drop index algo_idx_c1c2c3 algorithm  default;

-- test comment
create index algo_idx_c1c2 on algo_idx_t(c1, c2) comment 'hello index' algorithm inplace;
\d algo_idx_c1c2

-- test global、local、if not cases
create table algo_idx_gl_t(
	c1 int,
    c2 varchar(20),
    c3 date,
    c4 int,
    c5 int
) PARTITION BY RANGE(c5)
(
    PARTITION p0 VALUES LESS THAN (1000),
    PARTITION p1 VALUES LESS THAN (2000),
    PARTITION p2 VALUES LESS THAN MAXVALUE
);

insert into algo_idx_gl_t values(1, 'dsfdsg', '2022-01-02');
insert into algo_idx_gl_t values(2, 'dsfds', '2022-01-04');
insert into algo_idx_gl_t values(3, 'sdgsdf', '2022-01-06');

create index algo_idx_c4_2 on algo_idx_gl_t using btree (c4) LOCAL COMMENT='algorithm' algorithm INPLACE;
create index algo_idx_c4_3 on algo_idx_gl_t (c4) LOCAL invisible algorithm default;
create index algo_idx_c4_4 on algo_idx_gl_t using btree (c4) LOCAL algorithm =  DEFAULT;
create index algo_idx_c3_5 on algo_idx_gl_t using btree (c3) algorithm INPLACE;
create index algo_idx_c4_6 on algo_idx_gl_t(c4) LOCAL algorithm = copy;
\d+ algo_idx_c4_6

create index algo_idx_c5_2 on algo_idx_gl_t using btree (c5) GLOBAL COMMENT='algorithm' algorithm = INPLACE;
create index algo_idx_c5_3 on algo_idx_gl_t (c5) GLOBAL invisible algorithm default;
create index algo_idx_c5_4 on algo_idx_gl_t using btree (c5) GLOBAL algorithm = DEFAULT;
create index algo_idx_c5_5 on algo_idx_gl_t using btree (c5) algorithm INPLACE;
create unique index algo_idx_c5_6 on algo_idx_gl_t(c5) GLOBAL algorithm copy;
\d+ algo_idx_c5_2

drop index algo_idx_c4_2 on algo_idx_gl_t algorithm coPy;
drop index algo_idx_c4_3 on algo_idx_gl_t algorithm default;
drop index algo_idx_c4_4 on algo_idx_gl_t algorithm = INPLACE;
drop index algo_idx_c3_5 on algo_idx_gl_t algorithm INPLACE;
drop index algo_idx_c4_6 on algo_idx_gl_t algorithm =default;

drop index algo_idx_c5_2 on algo_idx_gl_t algorithm = coPy;
drop index algo_idx_c5_3 on algo_idx_gl_t algorithm default;
drop index algo_idx_c5_4 on algo_idx_gl_t algorithm INPLACE;
drop index algo_idx_c5_5 algorithm = INPLACE;
drop index algo_idx_c5_6 algorithm default;

-- if not exists 2
create index if not exists algo_idx_c5_2 on algo_idx_gl_t using btree (c5) GLOBAL COMMENT='algorithm' algorithm INPLACE;
create index if not exists algo_idx_c5_3 on algo_idx_gl_t (c5) GLOBAL invisible algorithm = default;
create index if not exists algo_idx_c5_4 on algo_idx_gl_t using btree (c5) GLOBAL algorithm DEFAULT;
create index if not exists algo_idx_c5_5 on algo_idx_gl_t using btree (c5) algorithm =copy;
create index if not exists algo_idx_c5_6 on algo_idx_gl_t(c5) GLOBAL algorithm DEFAULT;

drop index algo_idx_c5_2 on algo_idx_gl_t algorithm;  -- should error
drop index algo_idx_c5_2 on algo_idx_gl_t algorithm 'COPy';  -- should error
drop index algo_idx_c5_2 on algo_idx_gl_t algorithm coPy;
drop index algo_idx_c5_3 on algo_idx_gl_t algorithm merge; -- should error
drop index algo_idx_c5_3 on algo_idx_gl_t algorithm=default;
drop index algo_idx_c5_4 on algo_idx_gl_t algorithm INPLACE;
drop index algo_idx_c5_5 algorithm INPLACE;
drop index algo_idx_c5_6 algorithm default;

-- should error
create index if not exists algo_idx_c5_6 on algo_idx_gl_t(c5) GLOBAL algorithm temptable;
create index if not exists algo_idx_c5_6 on algo_idx_gl_t(c5) GLOBAL algorithm=merge;
create index if not exists algo_idx_c5_6 on algo_idx_gl_t(c5) algorithm=INPLACE GLOBAL;
create index if not exists algo_idx_c5_6 on algo_idx_gl_t(c5) GLOBAL algorithm=INPLACE algorithm=default;
create index if not exists algo_idx_c5_6 on algo_idx_gl_t(c5) GLOBAL algorithm;
alter table algo_idx_t  drop index algo_idx_c1c2 ALGORITHM = COPY; -- not support

-- passed
drop index algo_idx_c1c2 ALGORITHM = COPY;

drop table algo_idx_t cascade;
drop table algo_idx_gl_t cascade;

-- test where case & coincidence
create table algorithm (
    algorithm varchar(20),
    INPLACE varchar(20),
    COPY varchar(20)
);
insert into algorithm values('algorithm', 'INPLACE', 'COPY');
insert into algorithm values('INPLACE', 'INPLACE', 'COPY');
insert into algorithm values('COPY', 'INPLACE', 'COPY');
insert into algorithm values('default', 'INPLACE', 'COPY');

create unique index CONCURRENTLY algorithm_idx on algorithm (algorithm) where algorithm=INPLACE;
\d+ algorithm_idx
create unique index CONCURRENTLY algorithm_idx_2 on algorithm (algorithm, INPLACE)
    where algorithm=INPLACE
    and algorithm= 'default'
    or algorithm=COPY;
\d+ algorithm_idx_2
create unique index CONCURRENTLY algorithm_idx_3 on algorithm (algorithm, COPY) algorithm copy;
\d+ algorithm_idx_3

drop index algorithm_idx algorithm=copy;
drop index algorithm_idx_2 on algorithm;
drop index algorithm_idx_3 on algorithm algorithm default;

-- test should error:cannot spesify where clause and algorithm at the same time;
create unique index CONCURRENTLY algorithm_idx_88 on algorithm (algorithm) where algorithm=INPLACE algorithm copy;

drop table algorithm cascade;
drop user algorithm cascade;
