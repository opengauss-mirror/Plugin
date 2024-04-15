create schema keyword_test;
set search_path to keyword_test;
/* cast */
create table cast(cast int);
insert into cast values(1);
select cast from cast;

/* last_day */
create table last_day(last_day int);
insert into last_day values(1);
select last_day from last_day;

/* less */
create table less(less int);
insert into less values(1);
select less from less;

/* modify */
create table modify(modify int);
insert into modify values(1);
select modify from modify;

/* modify */
create table modify(modify int);
insert into modify values(1);
select modify from modify;

/* notnull */
create table notnull(notnull int);
insert into notnull values(1);
select notnull from notnull;

/* recyclebin */
create table recyclebin(recyclebin int);
insert into recyclebin values(1);
select recyclebin from recyclebin;

/* analyse */
create table analyse(analyse int);
insert into analyse values(1);
select analyse from analyse;
-- analyse unsupported
analyse;
explain analyse select 1;
analyse verbose;
alter table analyse analyse /* unsupported analyse keyword*/ verbose partition all;

/* buckets */
create table buckets(buckets int);
insert into buckets values(1);
select buckets from buckets;
select * from buckets buckets(1,2,3,4) /* unsupported buckets keyword*/;
alter node group test copy buckets from test2 /* unsupported */;
create node group mergegroup1 with (datanode1, datanode3, datanode5); /* unsupported */

/* compact */
create table compact(compact int);
insert into compact values(1);
select compact from compact;
analyze compact test/* table name */;
-- compact unsupported
create table test(id int) with (row_format=compact);

/* rownum */
create table rownum(rownum int);
insert into rownum values(1);
select rownum from rownum;
select rownum rownum from rownum; /* rownum can be alias name */

/* user */
create table user(user int);
insert into user values(1);
select user from user;

select user(); --success
select user; --failed
create user user identified by 'xxxx@xx1xx'; -- username user can be created
create user mapping for user server mot_server; -- username: user
create user mapping for current_user server mot_server; -- username: current_user
select count(distinct usename) from pg_user_mappings where usename in ('user', current_user()); -- expected: 2
alter table user disable trigger user; -- success: disable user trigger
alter table user disable trigger "user"; -- failed: cant not find trigger(named user)

/* sysdate */
create table sysdate(sysdate int);
insert into sysdate values(1);
select sysdate from sysdate;

/* body */
create table body(body int);
insert into body values(1);
select body from body;

/* collation */
create table collation(collation int);
insert into collation values(1);
select collation from collation;

/* minus */
create table minus(minus int);
insert into minus values(1);
select minus from minus;

/* any */
create table any(any int);
insert into any values(1);
select any from any;

/* do */
create table do(do int);
insert into do values(1);
select do from do;
-- binary/prior do 
create rule "test" as on select to t1 where binary do instead select * from t2; -- unsupported
create rule "test" as on select to t1 where prior do instead select * from t2; -- unsupported
create rule "test" as on select to t1 where binary do do instead select * from t2; -- t1 no exists
create rule "test" as on select to t1 where prior do do instead select * from t2; -- t1 no exists
create rule "test" as on select to t1 where (binary) do instead select * from t2; -- t1 no exists
create rule "test" as on select to t1 where (prior) do instead select * from t2; -- t1 no exists
create rule "test" as on select to t1 where test do instead select * from t2; -- t1 no exists
create rule "test" as on select to t1 where 1=1 do instead select * from t2; -- t1 no exists

/* end */
create table end(end int);
insert into end values(1);
select end from end;
select case when end then binary else binary end from end; -- unsupported
select case when end then binary else (binary) end from end; -- binary un exists
select case when end then binary end else binary end end from end;

/* only */
create table only(only int);
insert into only values(1);
select only from only;
select * from only test; -- scan only(alias: test) table
select * from only (test); -- scan test(scan InhOption: INH_NO) table 

/* verbose */
create table verbose(verbose int);
insert into verbose values(1);
select verbose from verbose;
-- cluster
cluster verbose verbose; --unsupported
cluster verbose; --supported
cluster (verbose) verbose; --supported
cluster verbose partition (test); --supported
cluster (verbose) verbose partition (test); --supported
cluster; --supported
cluster (verbose); --supported
cluster (verbose) verbose on verbose; --supported
cluster verbose on verbose; --supported
-- vacuum
vacuum full verbose partition (test); --supported
vacuum full verbose subpartition (test); --supported
VACUUM full analyze verbose; --supported
vacuum (full,freeze,verbose) verbose; --supported
vacuum verbose;
vacuum full verbose;
vacuum full freeze verbose; --unsupported
vacuum full freeze verbose compact; --unsupported
-- analyse
analyze verbose; --supported
analyze verbose verbose; --unsupported
analyze (verbose) verbose; --supported
analyze verbose; --supported
analyze (verbose) verbose; --supported
analyze foreign tables; --supported
analyze (verbose) foreign tables; --supported
analyze verbose foreign tables; --unsupported

create index verbose_index on verbose(verbose);

/* excluded */
create table excluded(excluded int);
insert into excluded values(1);
select excluded from excluded;
insert into excluded values (1) on duplicate key update excluded = excluded.excluded;
insert into excluded select 1 on duplicate key update excluded = excluded + 1;

-- name test
CREATE TABLE x (id int);
CREATE FUNCTION fn_x_before () RETURNS TRIGGER AS '
  BEGIN
		NEW.e := ''before trigger fired''::text;
		return NEW;
	END;
' LANGUAGE plpgsql;
CREATE TRIGGER "user" AFTER INSERT ON x FOR EACH ROW EXECUTE PROCEDURE fn_x_before(); -- unsupported name
CREATE TRIGGER test AFTER INSERT ON x FOR EACH ROW EXECUTE PROCEDURE fn_x_before();
ALTER TRIGGER test ON x RENAME TO user; -- unsupported name
       
reset search_path;
drop schema keyword_test cascade;
drop user user cascade;