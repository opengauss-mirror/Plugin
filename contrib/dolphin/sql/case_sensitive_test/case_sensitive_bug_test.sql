\c table_name_test_db
set dolphin.lower_case_table_names = 0;
DROP TABLE IF EXISTS PUBLIC.Test;
CREATE TABLE PUBLIC.Test (a int);
SELECT PUBLIC.Test.A FROM PUBLIC.Test WHERE PUBLIC.Test.A <> 2;

INSERT INTO PUBLIC.Test(A) VALUES(1), (2), (3);
UPDATE PUBLIC.Test SET Test.A = (PUBLIC.Test.A + 1) WHERE PUBLIC.Test.A > 1;
SELECT PUBLIC.Test.A FROM PUBLIC.Test;
DELETE FROM PUBLIC.Test WHERE PUBLIC.Test.A <= 1;
SELECT PUBLIC.Test.A FROM PUBLIC.Test;

create table T1(AA int);
insert into T1 values(1);
update T1 set aa=2 where aa=1;
insert into T1 set aa=1;
select * from T1;

create table T2(AA int);
insert into T2 values(1);
update T2 set aa=2 where aa=1;
insert into T2 set aa=1;
select * from T2;

update T1,T2 set T1.aa=1, T2.aa=2;
DELETE FROM T1, T2 USING T1, T2 WHERE T1.aa = T2.aa;
select count(*) from pg_catalog.PG_Class where relname='pg_class';
select count(*) from table_name_test_db.pg_catalog.PG_ClaSS where relname='pg_class';
select count(*) from pg_cATAlog.pg_class where relname='pg_class';
select count(*) from table_NAME_test_DB.pg_cATAloG.pg_class where relname='pg_class';
select PG_cataLOG.TIMESTAMPTZ_out('2022-12-31 23:11:11');
select table_NAME_TEST_DB.PG_CATALOg.TIMESTAMptz_OUT('2022-12-31 23:11:11');
select '2022-12-31 23:59:59'::timestamp::smalldatetime;

set dolphin.lower_case_table_names = 1;
select count(*) from pg_cATAlog.PG_Class where relname='pg_class';
select count(*) from table_NAME_test_DB.pg_cATAloG.PG_ClaSS where relname='pg_class';
select PG_cataLOG.TIMESTAMPTZ_out('2022-12-31 23:11:11');
select table_NAME_TEST_DB.PG_CATALOg.TIMESTAMptz_OUT('2022-12-31 23:11:11');
select '2022-12-31 23:59:59'::timestamp::smalldatetime;
