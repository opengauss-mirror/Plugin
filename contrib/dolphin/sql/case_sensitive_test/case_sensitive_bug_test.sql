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