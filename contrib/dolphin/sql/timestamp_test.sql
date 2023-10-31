select '101111101'::datetime;
select '2010117.5678'::datetime;
select '2010117.1234'::datetime;

select '101111101'::timestamp;
select '2010117.5678'::timestamp;
select '2010117.1234'::timestamp;

set dolphin.b_compatibility_mode to on;

select '2022-01-01'::bit(64)::datetime;
select '2022-01-01'::bit(64)::timestamp;
select 8385958.999999::bit(64)::time;

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
