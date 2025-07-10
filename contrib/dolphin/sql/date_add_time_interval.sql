create schema date_add_time_interval;
set current_schema = date_add_time_interval;

set dolphin.cmpt_version = '5.7';
select date_add(time'838:59:59', interval -1 day);
select date_sub(time'838:59:59', interval 1 day);
select date_add(time'838:59:59', interval 1 day);
select date_sub(time'838:59:59', interval -1 day);
select date_add(time'838:59:59', interval -1 month);
select date_sub(time'838:59:59', interval 1 month);
select date_add(time'838:59:59', interval -1 year);
select date_sub(time'838:59:59', interval 1 year);

set dolphin.cmpt_version = '8.0';
select date_add(time'838:59:59', interval -1 day);
select date_sub(time'838:59:59', interval 1 day);
select date_add(time'838:59:59', interval 1 day);
select date_sub(time'838:59:59', interval -1 day);
select date_add(time'838:59:59', interval -1 month);
select date_sub(time'838:59:59', interval 1 month);
select date_add(time'838:59:59', interval -1 year);
select date_sub(time'838:59:59', interval 1 year);

create table test1(c1 date);
create table x1 as select * from test1 where c1 >= date_sub(curdate(), interval 3 month);
create table x2 as select * from test1 where c1 >= date_add(curdate(), interval 3 month);

drop table test1;
drop table x1;
drop table x2;

reset current_schema;
drop schema date_add_time_interval;
