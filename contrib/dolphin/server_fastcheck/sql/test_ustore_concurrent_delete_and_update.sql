-- for concurrent delete and update
-- test case: commit, delete first and then update the same records
create table z1 (c1 int) with (storage_type=USTORE);

insert into z1 values (1);
insert into z1 values (2);
insert into z1 values (3);
insert into z1 values (4);
insert into z1 values (5);
insert into z1 values (6);

\parallel on 2

declare
	cnt int;
begin
	delete from z1 where c1=1;
	delete from z1 where c1=2;
	perform pg_sleep(2);
	select count(*) into cnt from z1;
	if cnt != 4 then
		raise notice 'ERROR: worker 1 expect cnt = %, but %', 4, cnt;
	end if;
end;
/

declare
	cnt int;
begin
	perform pg_sleep(1);
	update z1 set c1=11 where c1=1;
	update z1 set c1=12 where c1=2;
	select count(*) into cnt from z1;
	if cnt != 4 then
		raise notice 'ERROR: worker 2 expect cnt = %, but %', 4, cnt;
	end if;
end;
/

\parallel off

select * from z1; -- should be 3, 4, 5, 6 

drop table z1;

-- test case: commit, update first and then delete the same records
create table z1 (c1 int) with (storage_type=USTORE);

insert into z1 values (1);
insert into z1 values (2);
insert into z1 values (3);
insert into z1 values (4);
insert into z1 values (5);
insert into z1 values (6);

\parallel on 2

declare
	cnt int;
begin
	update z1 set c1=11 where c1=1;
	update z1 set c1=12 where c1=2;
	perform pg_sleep(2);
	select count(*) into cnt from z1;
	if cnt != 6 then
		raise notice 'ERROR: worker 1 expect cnt = %, but %', 6, cnt;
	end if;
end;
/

declare
	cnt int;
begin
	perform pg_sleep(1);
	delete from z1 where c1=1;
	delete from z1 where c1=2;
	select count(*) into cnt from z1;
	if cnt != 6 then
		raise notice 'ERROR: worker 2 expect cnt = %, but %', 6, cnt;
	end if;
end;
/

\parallel off

select * from z1; -- should be 11, 12, 3, 4, 5, 6 

drop table z1;

-- test case: commit, delete first and then update different records
create table z1 (c1 int) with (storage_type=USTORE);

insert into z1 values (1);
insert into z1 values (2);
insert into z1 values (3);
insert into z1 values (4);
insert into z1 values (5);
insert into z1 values (6);

\parallel on 2
declare
	cnt int;
begin
	delete from z1 where c1=1;
	perform pg_sleep(1);
	delete from z1 where c1=2;
	perform pg_sleep(1);
	select count(*) from z1 into cnt;
	if cnt != 4 then
		raise notice 'ERROR: worker 1 expect cnt = %, but %', 4, cnt;
	end if;
end;
/

declare
	cnt int;
begin
	perform pg_sleep(1);
	update z1 set c1=13 where c1=3;
	perform pg_sleep(2);
	update z1 set c1=14 where c1=4;
	select count(*) from z1 into cnt;
	if cnt != 4 then
		raise notice 'ERROR: worker 2 expect cnt = %, but %', 4, cnt;
	end if;
end;
/

\parallel off

select * from z1; -- should be 13, 14, 5, 6

drop table z1;

-- test case: commit, update first and then delete different records
create table z1 (c1 int) with (storage_type=USTORE);

insert into z1 values (1);
insert into z1 values (2);
insert into z1 values (3);
insert into z1 values (4);
insert into z1 values (5);
insert into z1 values (6);

\parallel on 2
declare
	cnt int;
begin
	update z1 set c1=11 where c1=1;
	update z1 set c1=12 where c1=2;
	perform pg_sleep(1);
	select count(*) from z1 into cnt;
	if cnt != 6 then
		raise notice 'ERROR: worker 1 expect cnt = %, but %', 6, cnt;
	end if;
end;
/

declare
	cnt int;
begin
	perform pg_sleep(1);
	delete from z1 where c1=3;
	perform pg_sleep(1);
	delete from z1 where c1=4;
	select count(*) from z1 into cnt;
	if cnt != 4 then
		raise notice 'ERROR: worker 2 expect cnt = %, but %', 4, cnt;
	end if;
end;
/

\parallel off

select * from z1; -- should be 11, 12, 5, 6

drop table z1;

-- test case: commit, mixed delete and update the same records in one worker
create table z1 (c1 int) with (storage_type=USTORE);

insert into z1 values (1);
insert into z1 values (2);
insert into z1 values (3);
insert into z1 values (4);
insert into z1 values (5);
insert into z1 values (6);

\parallel on 2

declare
	cnt int;
begin
	delete from z1 where c1=1;
	perform pg_sleep(1);
	update z1 set c1=12 where c1=2;
	perform pg_sleep(2);
	select count(*) into cnt from z1;
	if cnt != 5 then
		raise notice 'ERROR: worker 1 expect cnt = %, but %', 5, cnt;
	end if;
end;
/

declare
	cnt int;
begin
	perform pg_sleep(1);
	update z1 set c1=11 where c1=1;
	perform pg_sleep(2);
	delete from z1 where c1=2;
	select count(*) into cnt from z1;
	if cnt != 5 then
		raise notice 'ERROR: worker 2 expect cnt = %, but %', 5, cnt;
	end if;
end;
/

\parallel off

select * from z1; -- should be 12, 3, 4, 5, 6 

drop table z1;

create table z1 (c1 int) with (storage_type=ustore);

insert into z1 values (1);

\parallel on 2

begin
   update z1 set c1 = 11 where c1 = 1;
   perform pg_sleep(2);
   end;
/

begin
    perform pg_sleep(1);
    -- select c1 from z1 returns old value (1) instead of the updated value (11)
    -- so the delete should not be able to delete the tuple
    delete from z1 where c1 in (select c1 from z1 order by c1 limit 1);
    end;
/

\parallel off

select * from z1; -- should be 11

drop table z1;

