----------------------- test 1: role propagate -----------------------------
\c - - - :master_port
create role dev_role with login password 'spq_test1';
select count(1) from pg_roles where rolname = 'dev_role';
\c - - - :worker_1_port
select count(1) from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
select count(1) from pg_roles where rolname = 'dev_role';

----------------------- test 1: alter role propagate -----------------------------
\c - - - :master_port
-- error same password
alter role dev_role password 'spq_test1';
-- expect success
alter role dev_role password 'spq_test12';

\c - - - :worker_1_port
select rolname, rolcanlogin from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
select rolname, rolcanlogin from pg_roles where rolname = 'dev_role';

\c - - - :master_port
alter role dev_role nologin;

\c - - - :worker_1_port
select rolname, rolcanlogin from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
select rolname, rolcanlogin from pg_roles where rolname = 'dev_role';

\c - - - :master_port
drop role dev_role;
\c - - - :worker_1_port
select count(1) from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
select count(1) from pg_roles where rolname = 'dev_role';

----------------------- test 2: alter role set propagate -----------------------------
\c - - - :master_port
create role dev_role with login password 'spq_test1';
\c - - - :worker_1_port
select rolname, rolconfig from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
select rolname, rolconfig from pg_roles where rolname = 'dev_role';

\c - - - :master_port
alter role dev_role set work_mem = '16MB';
\c - - - :worker_1_port
select rolname, rolconfig from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
select rolname, rolconfig from pg_roles where rolname = 'dev_role';

\c - - - :master_port
alter role dev_role set session_timeout = '10min';
\c - - - :worker_1_port
select rolname, rolconfig from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
select rolname, rolconfig from pg_roles where rolname = 'dev_role';


-- rename role
\c - - - :master_port
alter role dev_role rename to vip_role;
\c - - - :worker_1_port
select count(1) from pg_roles where rolname = 'dev_role';
select count(1) from pg_roles where rolname = 'vip_role';

\c - - - :worker_2_port
select count(1) from pg_roles where rolname = 'dev_role';
select count(1) from pg_roles where rolname = 'vip_role';

----------------------- test 3: grant role propagate -----------------------------
\c - - - :worker_1_port
select rolname, rolcreatedb, rolcreaterole, rolreplication from pg_roles where rolname = 'vip_role';
\c - - - :worker_2_port
select rolname, rolcreatedb, rolcreaterole, rolreplication from pg_roles where rolname = 'vip_role';

\c - - - :master_port
alter role vip_role CREATEDB;
alter role vip_role CREATEROLE;
alter role vip_role REPLICATION;

\c - - - :worker_1_port
select rolname, rolcreatedb, rolcreaterole, rolreplication from pg_roles where rolname = 'vip_role';
\c - - - :worker_2_port
select rolname, rolcreatedb, rolcreaterole, rolreplication from pg_roles where rolname = 'vip_role';

\c - - - :master_port
create table t1 (a int);
insert into t1 values (1), (2), (3), (4);
SELECT create_distributed_table('t1', 'a', shard_count:=4);

grant select, insert on t1 to vip_role;
\c - - - :worker_1_port
select grantee, table_name, privilege_type from information_schema.table_privileges where table_name like 't1%' and grantee = 'vip_role';
\c - - - :worker_2_port
select grantee, table_name, privilege_type from information_schema.table_privileges where table_name like 't1%' and grantee = 'vip_role';

\c - - - :master_port
revoke insert on t1 from vip_role;
\c - - - :worker_1_port
select grantee, table_name, privilege_type from information_schema.table_privileges where table_name like 't1%' and grantee = 'vip_role';
\c - - - :worker_2_port
select grantee, table_name, privilege_type from information_schema.table_privileges where table_name like 't1%' and grantee = 'vip_role';

\c - - - :master_port
grant ALL on t1 to vip_role;
\c - - - :worker_1_port
select grantee, table_name, privilege_type from information_schema.table_privileges where table_name like 't1%' and grantee = 'vip_role' order by privilege_type;
\c - - - :worker_2_port
select grantee, table_name, privilege_type from information_schema.table_privileges where table_name like 't1%' and grantee = 'vip_role' order by privilege_type;


\c - - - :master_port
create schema s1;

grant USAGE, CREATE ON schema s1 to vip_role;
\c - - - :worker_1_port
select nspname, rolname from pg_namespace join pg_roles on true where nspname = 's1' and rolname = 'vip_role';
\c - - - :worker_2_port
select nspname, rolname from pg_namespace join pg_roles on true where nspname = 's1' and rolname = 'vip_role';

\c - - - :master_port
drop schema s1 CASCADE;
drop table t1 CASCADE;
drop role vip_role;

create role dev_role with login password 'spq_test1';
\c - - - :worker_1_port
-- expect f
select rolname, rolsystemadmin from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
-- expect f
select rolname, rolsystemadmin from pg_roles where rolname = 'dev_role';

\c - - - :master_port
grant all privileges to dev_role;
\c - - - :worker_1_port
-- expect t
select rolname, rolsystemadmin from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
-- expect t
select rolname, rolsystemadmin from pg_roles where rolname = 'dev_role';

\c - - - :master_port
revoke all privileges from dev_role;
\c - - - :worker_1_port
-- expect f
select rolname, rolsystemadmin from pg_roles where rolname = 'dev_role';
\c - - - :worker_2_port
-- expect f
select rolname, rolsystemadmin from pg_roles where rolname = 'dev_role';

drop role dev_role;

----------------------- test 3: user propagate -----------------------------
\c - - - :master_port
-- create user will create role and schema with the username
create user dev_user with login password 'spq_test1';
select count(1) from pg_roles where rolname = 'dev_user';
select count(1) from pg_namespace where nspname = 'dev_user';

\c - - - :worker_1_port
-- expect 1
select count(1) from pg_roles where rolname = 'dev_user';
-- expect 1
select count(1) from pg_namespace where nspname = 'dev_user';

\c - - - :worker_2_port
-- expect 1
select count(1) from pg_roles where rolname = 'dev_user';
-- expect 1
select count(1) from pg_namespace where nspname = 'dev_user';

\c - - - :master_port
grant create on database regression to dev_user;
-- expect 't'
select has_database_privilege('dev_user','regression', 'CREATE');
\c - - - :worker_1_port
-- expect 't'
select has_database_privilege('dev_user','regression', 'CREATE');
\c - - - :worker_2_port
-- expect 't'
select has_database_privilege('dev_user','regression', 'CREATE');

\c - - - :master_port
revoke create on database regression from dev_user;
-- expect 'f'
select has_database_privilege('dev_user','regression', 'CREATE');
\c - - - :worker_1_port
-- expect 'f'
select has_database_privilege('dev_user','regression', 'CREATE');
\c - - - :worker_2_port
-- expect 'f'
select has_database_privilege('dev_user','regression', 'CREATE');

\c - - - :master_port
create schema test_grant_on_shcema;
grant create on schema test_grant_on_shcema to dev_user;
-- expect 't'
select has_schema_privilege('dev_user','test_grant_on_shcema', 'CREATE');
\c - - - :worker_1_port
-- expect 't'
select has_schema_privilege('dev_user','test_grant_on_shcema', 'CREATE');
\c - - - :worker_2_port
-- expect 't'
select has_schema_privilege('dev_user','test_grant_on_shcema', 'CREATE');

\c - - - :master_port
revoke create on schema test_grant_on_shcema from dev_user;
-- expect 'f'
select has_schema_privilege('dev_user','test_grant_on_shcema', 'CREATE');
\c - - - :worker_1_port
-- expect 'f'
select has_schema_privilege('dev_user','test_grant_on_shcema', 'CREATE');
\c - - - :worker_2_port
-- expect 'f'
select has_schema_privilege('dev_user','test_grant_on_shcema', 'CREATE');

\c - - - :master_port
drop schema test_grant_on_shcema;

-- drop user will drop role and schema with the username
drop user dev_user;
-- expect 0
select count(1) from pg_roles where rolname = 'dev_user';
-- expect 0
select count(1) from pg_namespace where nspname = 'dev_user';

\c - - - :worker_1_port
-- expect 0
select count(1) from pg_roles where rolname = 'dev_user';
-- expect 0
select count(1) from pg_namespace where nspname = 'dev_user';

\c - - - :worker_2_port
-- expect 0
select count(1) from pg_roles where rolname = 'dev_user';
-- expect 0
select count(1) from pg_namespace where nspname = 'dev_user';