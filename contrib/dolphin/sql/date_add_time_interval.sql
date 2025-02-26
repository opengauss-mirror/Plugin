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

SELECT SUBDATE(time'23:59:59.9', '3 years 2 mons 5 days 14:25:26');
SELECT SUBDATE(time'23:59:59','08:00:2b:01:02:03');
SELECT SUBDATE(time'838:59:59.9', '3 years 2 mons 5 days 14:25:26');
SELECT SUBDATE(time'838:59:59','08:00:2b:01:02:03');
SELECT ADDDATE(time'23:59:59.9', '3 years 2 mons 5 days 14:25:26');
SELECT ADDDATE(time'23:59:59','08:00:2b:01:02:03');
SELECT ADDDATE(time'838:59:59.9', '3 years 2 mons 5 days 14:25:26');
SELECT ADDDATE(time'838:59:59','08:00:2b:01:02:03');

set dolphin.cmpt_version = '8.0';
select date_add(time'838:59:59', interval -1 day);
select date_sub(time'838:59:59', interval 1 day);
select date_add(time'838:59:59', interval 1 day);
select date_sub(time'838:59:59', interval -1 day);
select date_add(time'838:59:59', interval -1 month);
select date_sub(time'838:59:59', interval 1 month);
select date_add(time'838:59:59', interval -1 year);
select date_sub(time'838:59:59', interval 1 year);

SELECT SUBDATE(time'23:59:59.9', '3 years 2 mons 5 days 14:25:26');
SELECT SUBDATE(time'23:59:59','08:00:2b:01:02:03');
SELECT SUBDATE(time'838:59:59.9', '3 years 2 mons 5 days 14:25:26');
SELECT SUBDATE(time'838:59:59','08:00:2b:01:02:03');
SELECT ADDDATE(time'23:59:59.9', '3 years 2 mons 5 days 14:25:26');
SELECT ADDDATE(time'23:59:59','08:00:2b:01:02:03');
SELECT ADDDATE(time'838:59:59.9', '3 years 2 mons 5 days 14:25:26');
SELECT ADDDATE(time'838:59:59','08:00:2b:01:02:03');

reset current_schema;
drop schema date_add_time_interval;
