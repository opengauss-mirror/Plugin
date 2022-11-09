create database show_test dbcompatibility 'b';
\c show_test
create user grant_test identified by 'H&*#^DH85@#(J';
set search_path = 'grant_test';
create table test(id int);
grant select on table test to grant_test;
grant update on table test to grant_test with grant option;
grant alter on foreign server log_srv to grant_test;
grant comment on foreign server log_srv to grant_test with grant option;
grant usage on foreign data wrapper file_fdw to grant_test with grant option;
/* column */
create table test2(id1 int,id2 int, id3 int);
grant comment(id1, id2) on test2 to grant_test ;
grant comment(id3) on test2 to grant_test with grant option;
/* encrypt key */
CREATE CLIENT MASTER KEY proc_cmk2 WITH ( KEY_STORE = localkms , KEY_PATH = "gs_ktool" , ALGORITHM = RSA_2048);
CREATE COLUMN ENCRYPTION KEY proc_cek2 WITH VALUES (CLIENT_MASTER_KEY = proc_cmk2, ALGORITHM = AEAD_AES_256_CBC_HMAC_SHA256);
grant usage on column_encryption_key proc_cek2 to grant_test with grant option;
grant usage on client_master_key  proc_cmk2 to grant_test;
create or replace procedure proc1() 
as
declare
genre_rec record; --声明记录类型
begin
for genre_rec in (select e1.ename from public.emp1 e1 join public.emp1mot e1m on e1.mgr = e1m.mgr)
loop
raise notice '%', genre_rec."ename"; --打印
end loop;
end;
/
grant alter on procedure proc1() to grant_test;
grant comment on procedure proc1() to grant_test with grant option;
CREATE FUNCTION func_add_sql(integer, integer) 
RETURNS integer AS 'select $1 + $2;' 
    LANGUAGE SQL IMMUTABLE 
    RETURNS NULL ON NULL INPUT;
grant alter on function func_add_sql(integer, integer) to grant_test;
grant comment on function func_add_sql(integer, integer) to grant_test with grant option;
grant update any table to grant_test;
grant select any table to grant_test with admin option;
alter role grant_test with createdb;
show grants for grant_test;
alter role grant_test with monadmin;

/* trigger */
CREATE TABLE test_trigger_src_tbl(id1 INT, id2 INT, id3 INT);
CREATE TABLE test_trigger_des_tbl(id1 INT, id2 INT, id3 INT);
CREATE OR REPLACE FUNCTION tri_insert_func() RETURNS TRIGGER AS $$ DECLARE BEGIN INSERT INTO test_trigger_des_tbl VALUES(NEW.id1, NEW.id2, NEW.id3); RETURN NEW; END $$ LANGUAGE PLPGSQL;
CREATE OR REPLACE FUNCTION tri_update_func() RETURNS TRIGGER AS $$ DECLARE BEGIN UPDATE test_trigger_des_tbl SET id3 = NEW.id3 WHERE id1=OLD.id1; RETURN OLD; END $$ LANGUAGE PLPGSQL;
CREATE OR REPLACE FUNCTION TRI_DELETE_FUNC() RETURNS TRIGGER AS $$ DECLARE BEGIN DELETE FROM test_trigger_des_tbl WHERE id1=OLD.id1; RETURN OLD; END $$ LANGUAGE PLPGSQL;
create or replace function tri_truncate_func0010() returns trigger as $$ declare begin truncate table test_trigger_src_tbl; return new; end $$ language plpgsql;
CREATE TRIGGER insert_trigger BEFORE INSERT ON test_trigger_src_tbl FOR EACH ROW EXECUTE PROCEDURE tri_insert_func();
CREATE TRIGGER update_trigger AFTER UPDATE ON test_trigger_src_tbl FOR EACH ROW EXECUTE PROCEDURE tri_update_func();
CREATE TRIGGER delete_trigger BEFORE DELETE ON test_trigger_src_tbl FOR EACH ROW EXECUTE PROCEDURE tri_delete_func();
create trigger truncate_trigger0010 before truncate on test_trigger_src_tbl for each statement execute procedure tri_truncate_func0010();

show function status like 'func_add_s%';
show procedure status like 'proc%';
show triggers in grant_test;

/* show character set */
SHOW CHARSET;
SHOW CHARACTER SET LIKE 'a%';
SHOW CHARACTER SET WHERE charset LIKE 'i%';
/* show collation */
SHOW COLLATION LIKE 'aa%';
SHOW COLLATION WHERE charset = 'win1251';

reset search_path;
drop user grant_test cascade;
\c postgres
drop database show_test;