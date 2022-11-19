drop database if exists test_if_not_exists;
create database test_if_not_exists dbcompatibility 'B';
\c test_if_not_exists

CREATE USER ZZZ WITH PASSWORD 'openGauss@123';
CREATE USER ZZZ WITH PASSWORD 'openGauss@123';
CREATE USER IF NOT EXISTS ZZZ WITH PASSWORD 'openGauss@123';
DROP USER ZZZ;
CREATE USER IF NOT EXISTS ZZZ WITH PASSWORD 'openGauss@123';

\c postgres
drop database test_if_not_exists;
