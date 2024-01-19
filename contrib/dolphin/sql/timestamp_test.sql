select '101111101'::datetime;
select '2010117.5678'::datetime;
select '2010117.1234'::datetime;

select '101111101'::timestamp;
select '2010117.5678'::timestamp;
select '2010117.1234'::timestamp;

set dolphin.b_compatibility_mode to on;

set dolphin.sql_mode='';
-- true
select '2022-1-12 12:23:23'::timestamp and  '20220112122324':: timestamp;
select '2023-1-12 12:23:23'::datetime and  '20230112122324':: datetime;
select '2022-1-12 12:23:23'::timestamp and  '20220112122324':: datetime;
select '2022-1-12 12:23:23'::datetime and  '20220112122324':: timestamp;
select timestamp '2022-1-12 12:23:23' and  timestamp '20220112122324';
select datetime '2023-1-12 12:23:23' and  datetime '20230112122324';
select timestamp '2022-1-12 12:23:23' and  datetime '20220112122324';
select datetime '2022-1-12 12:23:23' and  timestamp '20220112122324';

-- false
select '0000-00-00 00:00:00'::timestamp and  '20220112122324':: timestamp;
select '2023-1-12 12:23:23'::datetime and  '00000000000000':: datetime;
select '2022-1-12 12:23:23'::timestamp and  '00000000000000':: datetime;
select '2022-1-12 12:23:23'::datetime and  '00000000000000':: timestamp;
select timestamp '2022-1-12 12:23:23' and  timestamp '00000000000000';
select datetime '2023-1-12 12:23:23' and  datetime '00000000000000';
select timestamp '2022-1-12 12:23:23' and  datetime '00000000000000';
select datetime '2022-1-12 12:23:23' and  timestamp '00000000000000';

-- true
select '2022-1-12 12:23:23'::timestamp or  '20220112122324':: timestamp;
select '2023-1-12 12:23:23'::datetime or  '20230112122324':: datetime;
select '2022-1-12 12:23:23'::timestamp or  '20220112122324':: datetime;
select '2022-1-12 12:23:23'::datetime or  '20220112122324':: timestamp;
select timestamp '2022-1-12 12:23:23' or  timestamp '20220112122324';
select datetime '2023-1-12 12:23:23' or  datetime '20230112122324';
select timestamp '2022-1-12 12:23:23' or  datetime '20220112122324';
select datetime '2022-1-12 12:23:23' or  timestamp '20220112122324';
select '0000-00-00 00:00:00'::timestamp or  '20220112122324':: timestamp;
select '2023-1-12 12:23:23'::datetime or  '00000000000000':: datetime;
select '2022-1-12 12:23:23'::timestamp or  '00000000000000':: datetime;
select '2022-1-12 12:23:23'::datetime or  '00000000000000':: timestamp;
select timestamp '2022-1-12 12:23:23' or  timestamp '00000000000000';
select datetime '2023-1-12 12:23:23' or  datetime '00000000000000';
select timestamp '2022-1-12 12:23:23' or  datetime '00000000000000';
select datetime '2022-1-12 12:23:23' or  timestamp '00000000000000';

reset dolphin.sql_mode;
create table t_set0004(c1 int not null auto_increment primary key, c2 set('2011-11-11', '2023-02-28 11:23:00', '2024-01', '2025/01/01') default null, c3 set('red', 'yellow', 'blue') not null, c4 set('0', '1', '1.01314'));
insert into t_set0004(c2, c3, c4) values ('2025/01/01', 'blue', '0');
insert into t_set0004(c2, c3, c4) values ('2011-11-11,2023-02-28 11:23:00', 'red,yellow', '0,1');
insert into t_set0004(c2, c3, c4) values ('2024-01,2011-11-11,2025/01/01', 'red,blue', '0,1.01314');
insert into t_set0004(c2, c3) values ('2023-02-28 11:23:00', 'red');
insert into t_set0004(c2, c3) values ('2023-02-28 11:23:00,2025/01/01,2025/01/01', 'blue,blue,yellow');
insert into t_set0004(c3) values ('yellow');
insert into t_set0004(c3) values ('yellow,yellow,yellow,yellow');
insert into t_set0004(c3) values ('blue,yellow,red,red');
insert into t_set0004(c3) values ('blue,red');
insert into t_set0004(c3, c4) values ('red', '1');
insert into t_set0004(c3, c4) values ('red,red', '1.01314,1.01314');
insert into t_set0004(c3, c4) values ('red,blue', '0,1,1.01314');

select cast(c1 as date), cast(c2 as date), cast(c3 as date), cast(c4 as date) from t_set0004 order by 1,2,3,4;
select cast(c1 as datetime), cast(c2 as datetime), cast(c3 as datetime), cast(c4 as datetime) from t_set0004 order by 1,2,3,4;
