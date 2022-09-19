drop database if exists show_variables;
create database show_variables dbcompatibility 'b';
\c show_variables
SET datestyle TO postgres, dmy;
show variables like 'DateSty%';
show variables where variable_name like 'DateSty%';
show variables where variable_name = 'DateStyle';
show session variables like 'DateSty%';
show session variables where variable_name like 'DateSty%';
show session variables where variable_name = 'DateStyle';
show global variables like 'DateSty%';
show global variables where variable_name like 'DateSty%';
show global variables where variable_name = 'DateStyle';
SET datestyle TO ISO, MDY;
show session variables where variable_name = 'DateStyle';
show global variables where variable_name = 'DateStyle';
RESET datestyle;
\c postgres
drop database if exists show_variables;