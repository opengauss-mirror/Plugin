create schema b_datetime_func_test1;
set current_schema to 'b_datetime_func_test1';
set datestyle = 'ISO,ymd';
set time zone "Asia/Shanghai";
-- test part-one function
-- test curdate()
select curdate();

-- test current_time(precision)
select current_time;
select current_time();
select current_time(0);
select current_time(1);
select current_time(2);
select current_time(3);
select current_time(4);
select current_time(5);
select current_time(6);
select current_time(7);

-- test curtime(precision)
select curtime;
select curtime();
select curtime(0);
select curtime(1);
select curtime(2);
select curtime(3);
select curtime(4);
select curtime(5);
select curtime(6);
select curtime(7);

-- test current_timestamp(precision)

select current_timestamp;
select current_timestamp();
select current_timestamp(0);
select current_timestamp(1);
select current_timestamp(2);
select current_timestamp(3);
select current_timestamp(4);
select current_timestamp(5);
select current_timestamp(6);
select current_timestamp(7);

-- test localtime(precision)
select localtime;
select localtime();
select localtime(0);
select localtime(1);
select localtime(2);
select localtime(3);
select localtime(4);
select localtime(5);
select localtime(6);
select localtime(7);

-- test localtimestamp(precision)
select localtimestamp;
select localtimestamp();
select localtimestamp(0);
select localtimestamp(1);
select localtimestamp(2);
select localtimestamp(3);
select localtimestamp(4);
select localtimestamp(5);
select localtimestamp(6);
select localtimestamp(7);

-- test now(precision)
-- when the GUC variable b_db_timestamp = 0, now returns the current date and time
show dolphin.b_db_timestamp;
select now();
select now(0);
select now(1);
select now(2);
select now(3);
select now(4);
select now(5);
select now(6);
select now(7);

-- when the GUC variable b_db_timestamp != 0, now returns the timestamp according to b_db_timestamp
set dolphin.b_db_timestamp = 0.1;
set dolphin.b_db_timestamp = 2147483647.1;
set dolphin.b_db_timestamp = 1.1234;
select now();
select now(0);
select now(1);
select now(2);
select now(3);
select now(4);
select now(5);
select now(6);
select now(7);
set dolphin.b_db_timestamp = 0.0;

-- test sysdate(precision)
select sysdate();
select sysdate(0);
select sysdate(1);
select sysdate(2);
select sysdate(3);
select sysdate(4);
select sysdate(5);
select sysdate(6);
select sysdate(7);

-- test insert
create table test_date(col date);
insert into test_date values(curdate());
select * from test_date;
drop table test_date;
create table test_time(col time);
insert into test_time values(current_time);
insert into test_time values(current_time());
insert into test_time values(current_time(0));
insert into test_time values(current_time(6));
select * from test_time;
delete from test_time;
insert into test_time values(curtime());
insert into test_time values(curtime(0));
insert into test_time values(curtime(6));
select * from test_time;
drop table test_time;
create table test_datetime(col datetime);
insert into test_datetime values(current_timestamp);
insert into test_datetime values(current_timestamp());
insert into test_datetime values(current_timestamp(0));
insert into test_datetime values(current_timestamp(6));
select * from test_datetime;
delete from test_datetime;
insert into test_datetime values(localtime);
insert into test_datetime values(localtime());
insert into test_datetime values(localtime(0));
insert into test_datetime values(localtime(6));
select * from test_datetime;
delete from test_datetime;
insert into test_datetime values(localtimestamp);
insert into test_datetime values(localtimestamp());
insert into test_datetime values(localtimestamp(0));
insert into test_datetime values(localtimestamp(6));
select * from test_datetime;
delete from test_datetime;
insert into test_datetime values(now());
insert into test_datetime values(now(0));
insert into test_datetime values(now(6));
select * from test_datetime;
delete from test_datetime;
insert into test_datetime values(sysdate());
insert into test_datetime values(sysdate(0));
insert into test_datetime values(sysdate(6));
select * from test_datetime;
drop table test_datetime;

drop table if exists t_dump_007 cascade;
create table t_dump_007(id int, name varchar(200), age int, tel int, birth date primary key)
partition by range(BIRTH)
interval ('1 year')
(partition p1 values less than('1970-01-01'),
partition p2 values less than('1980-01-01'),
partition p3 values less than('1990-01-01'),
partition p4 values less than('2000-01-01'),
partition p5 values less than('2010-01-01'),
partition p6 values less than('2020-01-01'));

declare
    i integer;
begin
    for i in 1..10 loop
    insert into t_dump_007 values(i,'name_'||i,random()*100,random()*1000000,
    to_timestamp(to_char(current_date - random()*365*interval '1 year','YYYY-MM-DD'),'YYYY-MM-DD'));
    end loop;
end;
/


drop table if exists t_dump_007 cascade;
create table t_dump_007(birth date primary key, id int)
partition by range(BIRTH)
interval ('1 year')
(partition p1 values less than('1970-01-01'),
partition p2 values less than('1980-01-01'),
partition p3 values less than('1990-01-01'),
partition p4 values less than('2000-01-01'),
partition p5 values less than('2010-01-01'),
partition p6 values less than('2020-01-01'));

declare
    i integer;
begin
    for i in 1..10 loop
    insert into t_dump_007 values(to_timestamp(to_char(current_date - random()*365*interval '1 year','YYYY-MM-DD'),'YYYY-MM-DD'), i);
    end loop;
end;
/


drop table if exists t_dump_007 cascade;

drop schema b_datetime_func_test1 cascade;
reset current_schema;