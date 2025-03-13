create schema db_elt;
set current_schema to 'db_elt';
select elt(1,1);
select elt(-1,1);
select elt(1.2,'a');
select elt(1,1.2);
select elt(1,'1.2');
select elt('1a','1.2a');
select elt('1.6a','1.2a',23);
select elt('1.6测','1.2a',23);
select elt('1.6测','1.2a测试',23);
select elt(9,'1.2a测试',23);
select elt(-1,'1.2a测试',23);
select elt(2,null,'1.2a测试',23);
select elt(1,null,'1.2a测试',23);
select elt(true,12,null,'1.2a测试',23);
select elt('1',1);
select elt('999999999999999999999999999999999999999999999999999999999999999', '12345');
select elt('1',1.2);
select elt(1,'a','b');
select elt(1,'a',2);
select elt('1.0', 0);
select elt('1.0', 2);
create table t_elt(a int);
insert into t_elt select elt('1.0', 0); -- error
insert ignore into t_elt select elt('1.0', 0); -- warning
set dolphin.sql_mode='';
insert into t_elt select elt('1.0', 0); -- warning
drop table t_elt;
drop schema db_elt cascade;
reset current_schema;
