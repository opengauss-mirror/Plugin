create schema test_if_not_exists;
set current_schema to 'test_if_not_exists';

CREATE USER ZZZ WITH PASSWORD 'openGauss@123';
CREATE USER ZZZ WITH PASSWORD 'openGauss@123';
CREATE USER IF NOT EXISTS ZZZ WITH PASSWORD 'openGauss@123';
DROP USER ZZZ;
CREATE USER IF NOT EXISTS ZZZ WITH PASSWORD 'openGauss@123';

drop schema test_if_not_exists cascade;
reset current_schema;
