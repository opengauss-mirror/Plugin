\! gs_ktool -d all
\! gs_ktool -g

DROP CLIENT MASTER KEY IF EXISTS input_cmk CASCADE;
CREATE CLIENT MASTER KEY input_cmk WITH ( KEY_STORE = gs_ktool , KEY_PATH = "gs_ktool/1" , ALGORITHM = AES_256_CBC);
CREATE COLUMN ENCRYPTION KEY input_cek WITH VALUES (CLIENT_MASTER_KEY = input_cmk, ALGORITHM = AEAD_AES_256_CBC_HMAC_SHA256);

CREATE TABLE t_processed (name text, val INT ENCRYPTED WITH (COLUMN_ENCRYPTION_KEY = input_cek, ENCRYPTION_TYPE = DETERMINISTIC), val2 INT);
insert into t_processed values('one',1,10),('two',2,20),('three',3,30),('four',4,40),('five',5,50),('six',6,60),('seven',7,70),('eight',8,80),('nine',9,90),('ten',10,100);

CREATE FUNCTION f_processed_in_sql(int, int) RETURNS int AS 'SELECT val2 from t_processed where val=$1 or val2=$2 LIMIT 1' LANGUAGE SQL; 
\sf f_processed_in_sql

select f_processed_in_sql(1,2);

call f_processed_in_sql(1,2);

select * from f_processed_in_sql(1,2);

CREATE FUNCTION f_processed_in_sql_named (val_param int, val2_param int) RETURNS int AS 'SELECT val2 from t_processed where val=val_param or val2=val2_param LIMIT 1' LANGUAGE SQL;

\sf f_processed_in_sql_named

select f_processed_in_sql_named (100,val2_param => 30 );

select * from t_processed where val2 = f_processed_in_sql_named (val_param := 7,val2_param => 300 );

delete t_processed where val2 = f_processed_in_sql_named (val_param => 6,val2_param := 500 );

create table accounts (
    id serial,
    name varchar(100) not null ENCRYPTED WITH (COLUMN_ENCRYPTION_KEY = input_cek, ENCRYPTION_TYPE = DETERMINISTIC),
    balance dec(15,2) not null ENCRYPTED WITH (COLUMN_ENCRYPTION_KEY = input_cek, ENCRYPTION_TYPE = DETERMINISTIC),
    primary key(id)
);

CREATE OR REPLACE FUNCTION insert_func_2(name varchar(100), balance dec(15,2)) RETURNS VOID AS 'INSERT INTO accounts(name,balance) VALUES($1, $2);' LANGUAGE SQL;

call regression.public.insert_func_2('Bob', 101.30);
call insert_func_2('George', 505.70);
select insert_func_2('Joe', 710.00);
select  * from insert_func_2('Donald', 1214.88);
select * from accounts order by id;

drop FUNCTION f_processed_in_sql(int, int);
drop FUNCTION f_processed_in_sql_named;
drop FUNCTION insert_func_2;

drop table t_processed;
drop table accounts;
DROP COLUMN ENCRYPTION KEY input_cek;
DROP CLIENT MASTER KEY input_cmk;
\! gs_ktool -d all