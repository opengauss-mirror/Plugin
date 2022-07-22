create database second_microsecond dbcompatibility = 'b';
\c second_microsecond
select microsecond(timestamp(6) '2021-11-4 16:30:44.3411');
select second(timestamp(6) '2021-11-4 16:30:44.3411');
select microsecond(datetime(6) '2021-11-4 16:30:44.3411');
select second(datetime(6) '2021-11-4 16:30:44.3411');
select microsecond(timestamptz(6) '2021-11-4 16:30:44.3411');
select second(timestamptz(6) '2021-11-4 16:30:44.3411');
select microsecond(time(6) '2021-11-4 16:30:44.3411');
select second(time(6) '2021-11-4 16:30:44.3411');
select microsecond(timetz(6) '2021-11-4 16:30:44.3411');
select second(timetz(6) '2021-11-4 16:30:44.3411');

\c postgres
drop database second_microsecond;