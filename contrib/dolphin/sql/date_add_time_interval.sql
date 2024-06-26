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

reset current_schema;
drop schema date_add_time_interval;
