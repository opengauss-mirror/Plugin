DROP USER IF EXISTS native_user;
CREATE USER native_user WITH PASSWORD 'Gauss@123';
GRANT ALL PRIVILEGES TO native_user;
GRANT ALL ON SCHEMA dbe_perf TO native_user;
SET ROLE native_user PASSWORD 'Gauss@123';

-- set password for navtive_password_plugin authentication first
select set_native_password('native_user', 'Gauss@123', '');

-- ERROR set password for navtive_password_plugin authentication
select set_native_password('native_user', 'Gauss@123', 'Gauss');

-- set password for navtive_password_plugin authentication
select set_native_password('native_user', 'Gauss@admin', 'Gauss@123');

DROP USER IF EXISTS caching_user;
CREATE USER caching_user WITH PASSWORD 'Gauss@123';
GRANT ALL PRIVILEGES TO caching_user;
GRANT ALL ON SCHEMA dbe_perf TO caching_user;
SET ROLE caching_user PASSWORD 'Gauss@123';

-- set password for caching_sha2_password authentication first
select replace(set_caching_sha2_password('caching_user', 'Gauss@123', ''),E'\n','X');

-- ERROR set password for caching_sha2_password authentication
select set_caching_sha2_password('caching_user', 'Gauss@123', 'Gauss');

-- set password for caching_sha2_password authentication
select replace(set_caching_sha2_password('caching_user', 'Gauss@admin', 'Gauss@123'),E'\n','X');