-- test insert
create table t1 (c1 int) with (storage_type=USTORE);
start transaction;
	insert into t1 values (1);
	insert into t1 values (2);
	select * from t1;
rollback;
select * from t1;

drop table t1;
