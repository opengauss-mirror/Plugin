create schema foreign_key_checks_test;
set current_schema to 'foreign_key_checks_test';

-- test FOREIGN_KEY_CHECKS
create table parent_table(id INT PRIMARY KEY);
CREATE TABLE child_table(
id INT PRIMARY KEY,
parent_id INT,
FOREIGN KEY (parent_id) REFERENCES parent_table(id)
);

insert into parent_table values (1),(2),(3);
insert into child_table values (11, 1);
insert into child_table values (15, 5); -- error expectly
SET FOREIGN_KEY_CHECKS=0;
insert into child_table values (15, 5); -- success expectly

SET FOREIGN_KEY_CHECKS=1; 
update child_table set parent_id = 6 where parent_id = 1;   -- error expectly
SET FOREIGN_KEY_CHECKS=0;
update child_table set parent_id = 6 where parent_id = 1;   -- success expectly


drop table child_table;
drop table parent_table;


SET FOREIGN_KEY_CHECKS=1;
create table parent_table(id INT PRIMARY KEY);
CREATE TABLE child_table(
id INT PRIMARY KEY,
parent_id INT,
FOREIGN KEY (parent_id) REFERENCES parent_table(id)
);

insert into parent_table values (1),(2),(3);
insert into child_table values (11, 1);

delete from parent_table where id = 1; --error expectly 
SET FOREIGN_KEY_CHECKS=0;
delete from parent_table where id = 1; --success expectly 

SET FOREIGN_KEY_CHECKS=1;
alter table parent_table drop column id; --error expectly 
SET FOREIGN_KEY_CHECKS=0;
alter table parent_table drop column id;  --error expectly

SET FOREIGN_KEY_CHECKS=1;
drop table parent_table; -- error expectly
drop table child_table;
drop table parent_table; -- success expectly

drop schema foreign_key_checks_test cascade;
reset current_schema;
