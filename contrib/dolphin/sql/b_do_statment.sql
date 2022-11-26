create database db_do_stmt dbcompatibility = 'B';
\c db_do_stmt

create table t1 (a int);
insert into t1 values(1),(4),(7);
select a from t1;

--simple expr;
do 1;
do 1+1;
do 1+3;
do 2*4;


--multi expr
do 1,1,1,1;
do 1+1,2,3+3;
do 1+3,6;
do 2*4,9;

-- function
do sin(1);
do pg_sleep(1);
do exp(x);

-- function multi
do sin(1),exp(1);
do pg_sleep(1),sin(1);

--error stmt
do sin(1) limit 1;

--origin do stmt
DO $$
BEGIN
  perform  2,3,4;
END;
$$
;

DO $$
BEGIN
  perform  2;
END;
$$
LANGUAGE SQL;

-- error when has table
do sin(a) from t1;


\c regress
drop database db_do_stmt;
