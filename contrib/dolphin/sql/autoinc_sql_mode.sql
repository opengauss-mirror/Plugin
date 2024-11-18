
create schema autoinc_sql_mode;
set current_schema to 'autoinc_sql_mode';

set dolphin.sql_mode = '';
create table auto_inc1(id int);
insert into auto_inc1 values(0),(1),(2);
select * from auto_inc1;
alter table auto_inc1 modify id INT AUTO_INCREMENT PRIMARY KEY;
set dolphin.sql_mode='no_auto_value_on_zero';
alter table auto_inc1 modify id INT AUTO_INCREMENT PRIMARY KEY;
select * from auto_inc1;

set dolphin.sql_mode='';
insert into auto_inc1 values(0);
insert into auto_inc1 values(NULL);
select * from auto_inc1;
set dolphin.sql_mode='no_auto_value_on_zero';
insert into auto_inc1 values(0);
insert into auto_inc1 values(NULL);
select * from auto_inc1;

drop schema autoinc_sql_mode cascade;
reset current_schema;
