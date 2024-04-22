drop schema if exists time_operator_test_schema cascade;
create schema time_operator_test_schema;
set current_schema to 'time_operator_test_schema';
---------- head ----------
set dolphin.b_compatibility_mode to on;
drop table if exists test_time_table;
CREATE TABLE test_time_table
(
    `date` date,
    `time` time,
    `time(4)` time(4),
    `datetime` datetime,
    `datetime(4)` datetime(4),
    `timestamp` timestamp,
    `timestamp(4)` timestamp(4),
    `year` year
);

-- type test
drop table if exists test_time_type;
CREATE TABLE test_time_type AS SELECT
`date` + `date` AS `date+date`,
`date` - `date` AS `date-date`,
`date` * `date` AS `date*date`,
`date` / `date` AS `date/date`,
`date` + `time` AS `date+time`,
`date` - `time` AS `date-time`,
`date` * `time` AS `date*time`,
`date` / `time` AS `date/time`,
`date` + `time(4)` AS `date+time(4)`,
`date` - `time(4)` AS `date-time(4)`,
`date` * `time(4)` AS `date*time(4)`,
`date` / `time(4)` AS `date/time(4)`,
`date` + `datetime` AS `date+datetime`,
`date` - `datetime` AS `date-datetime`,
`date` * `datetime` AS `date*datetime`,
`date` / `datetime` AS `date/datetime`,
`date` + `datetime(4)` AS `date+datetime(4)`,
`date` - `datetime(4)` AS `date-datetime(4)`,
`date` * `datetime(4)` AS `date*datetime(4)`,
`date` / `datetime(4)` AS `date/datetime(4)`,
`date` + `timestamp` AS `date+timestamp`,
`date` - `timestamp` AS `date-timestamp`,
`date` * `timestamp` AS `date*timestamp`,
`date` / `timestamp` AS `date/timestamp`,
`date` + `timestamp(4)` AS `date+timestamp(4)`,
`date` - `timestamp(4)` AS `date-timestamp(4)`,
`date` * `timestamp(4)` AS `date*timestamp(4)`,
`date` / `timestamp(4)` AS `date/timestamp(4)`,
`date` + `year` AS `date+year`,
`date` - `year` AS `date-year`,
`date` * `year` AS `date*year`,
`date` / `year` AS `date/year`,
`time` + `date` AS `time+date`,
`time` - `date` AS `time-date`,
`time` * `date` AS `time*date`,
`time` / `date` AS `time/date`,
`time` + `time` AS `time+time`,
`time` - `time` AS `time-time`,
`time` * `time` AS `time*time`,
`time` / `time` AS `time/time`,
`time` + `time(4)` AS `time+time(4)`,
`time` - `time(4)` AS `time-time(4)`,
`time` * `time(4)` AS `time*time(4)`,
`time` / `time(4)` AS `time/time(4)`,
`time` + `datetime` AS `time+datetime`,
`time` - `datetime` AS `time-datetime`,
`time` * `datetime` AS `time*datetime`,
`time` / `datetime` AS `time/datetime`,
`time` + `datetime(4)` AS `time+datetime(4)`,
`time` - `datetime(4)` AS `time-datetime(4)`,
`time` * `datetime(4)` AS `time*datetime(4)`,
`time` / `datetime(4)` AS `time/datetime(4)`,
`time` + `timestamp` AS `time+timestamp`,
`time` - `timestamp` AS `time-timestamp`,
`time` * `timestamp` AS `time*timestamp`,
`time` / `timestamp` AS `time/timestamp`,
`time` + `timestamp(4)` AS `time+timestamp(4)`,
`time` - `timestamp(4)` AS `time-timestamp(4)`,
`time` * `timestamp(4)` AS `time*timestamp(4)`,
`time` / `timestamp(4)` AS `time/timestamp(4)`,
`time` + `year` AS `time+year`,
`time` - `year` AS `time-year`,
`time` * `year` AS `time*year`,
`time` / `year` AS `time/year`,
`time(4)` + `date` AS `time(4)+date`,
`time(4)` - `date` AS `time(4)-date`,
`time(4)` * `date` AS `time(4)*date`,
`time(4)` / `date` AS `time(4)/date`,
`time(4)` + `time` AS `time(4)+time`,
`time(4)` - `time` AS `time(4)-time`,
`time(4)` * `time` AS `time(4)*time`,
`time(4)` / `time` AS `time(4)/time`,
`time(4)` + `time(4)` AS `time(4)+time(4)`,
`time(4)` - `time(4)` AS `time(4)-time(4)`,
`time(4)` * `time(4)` AS `time(4)*time(4)`,
`time(4)` / `time(4)` AS `time(4)/time(4)`,
`time(4)` + `datetime` AS `time(4)+datetime`,
`time(4)` - `datetime` AS `time(4)-datetime`,
`time(4)` * `datetime` AS `time(4)*datetime`,
`time(4)` / `datetime` AS `time(4)/datetime`,
`time(4)` + `datetime(4)` AS `time(4)+datetime(4)`,
`time(4)` - `datetime(4)` AS `time(4)-datetime(4)`,
`time(4)` * `datetime(4)` AS `time(4)*datetime(4)`,
`time(4)` / `datetime(4)` AS `time(4)/datetime(4)`,
`time(4)` + `timestamp` AS `time(4)+timestamp`,
`time(4)` - `timestamp` AS `time(4)-timestamp`,
`time(4)` * `timestamp` AS `time(4)*timestamp`,
`time(4)` / `timestamp` AS `time(4)/timestamp`,
`time(4)` + `timestamp(4)` AS `time(4)+timestamp(4)`,
`time(4)` - `timestamp(4)` AS `time(4)-timestamp(4)`,
`time(4)` * `timestamp(4)` AS `time(4)*timestamp(4)`,
`time(4)` / `timestamp(4)` AS `time(4)/timestamp(4)`,
`time(4)` + `year` AS `time(4)+year`,
`time(4)` - `year` AS `time(4)-year`,
`time(4)` * `year` AS `time(4)*year`,
`time(4)` / `year` AS `time(4)/year`,
`datetime` + `date` AS `datetime+date`,
`datetime` - `date` AS `datetime-date`,
`datetime` * `date` AS `datetime*date`,
`datetime` / `date` AS `datetime/date`,
`datetime` + `time` AS `datetime+time`,
`datetime` - `time` AS `datetime-time`,
`datetime` * `time` AS `datetime*time`,
`datetime` / `time` AS `datetime/time`,
`datetime` + `time(4)` AS `datetime+time(4)`,
`datetime` - `time(4)` AS `datetime-time(4)`,
`datetime` * `time(4)` AS `datetime*time(4)`,
`datetime` / `time(4)` AS `datetime/time(4)`,
`datetime` + `datetime` AS `datetime+datetime`,
`datetime` - `datetime` AS `datetime-datetime`,
`datetime` * `datetime` AS `datetime*datetime`,
`datetime` / `datetime` AS `datetime/datetime`,
`datetime` + `datetime(4)` AS `datetime+datetime(4)`,
`datetime` - `datetime(4)` AS `datetime-datetime(4)`,
`datetime` * `datetime(4)` AS `datetime*datetime(4)`,
`datetime` / `datetime(4)` AS `datetime/datetime(4)`,
`datetime` + `timestamp` AS `datetime+timestamp`,
`datetime` - `timestamp` AS `datetime-timestamp`,
`datetime` * `timestamp` AS `datetime*timestamp`,
`datetime` / `timestamp` AS `datetime/timestamp`,
`datetime` + `timestamp(4)` AS `datetime+timestamp(4)`,
`datetime` - `timestamp(4)` AS `datetime-timestamp(4)`,
`datetime` * `timestamp(4)` AS `datetime*timestamp(4)`,
`datetime` / `timestamp(4)` AS `datetime/timestamp(4)`,
`datetime` + `year` AS `datetime+year`,
`datetime` - `year` AS `datetime-year`,
`datetime` * `year` AS `datetime*year`,
`datetime` / `year` AS `datetime/year`,
`datetime(4)` + `date` AS `datetime(4)+date`,
`datetime(4)` - `date` AS `datetime(4)-date`,
`datetime(4)` * `date` AS `datetime(4)*date`,
`datetime(4)` / `date` AS `datetime(4)/date`,
`datetime(4)` + `time` AS `datetime(4)+time`,
`datetime(4)` - `time` AS `datetime(4)-time`,
`datetime(4)` * `time` AS `datetime(4)*time`,
`datetime(4)` / `time` AS `datetime(4)/time`,
`datetime(4)` + `time(4)` AS `datetime(4)+time(4)`,
`datetime(4)` - `time(4)` AS `datetime(4)-time(4)`,
`datetime(4)` * `time(4)` AS `datetime(4)*time(4)`,
`datetime(4)` / `time(4)` AS `datetime(4)/time(4)`,
`datetime(4)` + `datetime` AS `datetime(4)+datetime`,
`datetime(4)` - `datetime` AS `datetime(4)-datetime`,
`datetime(4)` * `datetime` AS `datetime(4)*datetime`,
`datetime(4)` / `datetime` AS `datetime(4)/datetime`,
`datetime(4)` + `datetime(4)` AS `datetime(4)+datetime(4)`,
`datetime(4)` - `datetime(4)` AS `datetime(4)-datetime(4)`,
`datetime(4)` * `datetime(4)` AS `datetime(4)*datetime(4)`,
`datetime(4)` / `datetime(4)` AS `datetime(4)/datetime(4)`,
`datetime(4)` + `timestamp` AS `datetime(4)+timestamp`,
`datetime(4)` - `timestamp` AS `datetime(4)-timestamp`,
`datetime(4)` * `timestamp` AS `datetime(4)*timestamp`,
`datetime(4)` / `timestamp` AS `datetime(4)/timestamp`,
`datetime(4)` + `timestamp(4)` AS `datetime(4)+timestamp(4)`,
`datetime(4)` - `timestamp(4)` AS `datetime(4)-timestamp(4)`,
`datetime(4)` * `timestamp(4)` AS `datetime(4)*timestamp(4)`,
`datetime(4)` / `timestamp(4)` AS `datetime(4)/timestamp(4)`,
`datetime(4)` + `year` AS `datetime(4)+year`,
`datetime(4)` - `year` AS `datetime(4)-year`,
`datetime(4)` * `year` AS `datetime(4)*year`,
`datetime(4)` / `year` AS `datetime(4)/year`,
`timestamp` + `date` AS `timestamp+date`,
`timestamp` - `date` AS `timestamp-date`,
`timestamp` * `date` AS `timestamp*date`,
`timestamp` / `date` AS `timestamp/date`,
`timestamp` + `time` AS `timestamp+time`,
`timestamp` - `time` AS `timestamp-time`,
`timestamp` * `time` AS `timestamp*time`,
`timestamp` / `time` AS `timestamp/time`,
`timestamp` + `time(4)` AS `timestamp+time(4)`,
`timestamp` - `time(4)` AS `timestamp-time(4)`,
`timestamp` * `time(4)` AS `timestamp*time(4)`,
`timestamp` / `time(4)` AS `timestamp/time(4)`,
`timestamp` + `datetime` AS `timestamp+datetime`,
`timestamp` - `datetime` AS `timestamp-datetime`,
`timestamp` * `datetime` AS `timestamp*datetime`,
`timestamp` / `datetime` AS `timestamp/datetime`,
`timestamp` + `datetime(4)` AS `timestamp+datetime(4)`,
`timestamp` - `datetime(4)` AS `timestamp-datetime(4)`,
`timestamp` * `datetime(4)` AS `timestamp*datetime(4)`,
`timestamp` / `datetime(4)` AS `timestamp/datetime(4)`,
`timestamp` + `timestamp` AS `timestamp+timestamp`,
`timestamp` - `timestamp` AS `timestamp-timestamp`,
`timestamp` * `timestamp` AS `timestamp*timestamp`,
`timestamp` / `timestamp` AS `timestamp/timestamp`,
`timestamp` + `timestamp(4)` AS `timestamp+timestamp(4)`,
`timestamp` - `timestamp(4)` AS `timestamp-timestamp(4)`,
`timestamp` * `timestamp(4)` AS `timestamp*timestamp(4)`,
`timestamp` / `timestamp(4)` AS `timestamp/timestamp(4)`,
`timestamp` + `year` AS `timestamp+year`,
`timestamp` - `year` AS `timestamp-year`,
`timestamp` * `year` AS `timestamp*year`,
`timestamp` / `year` AS `timestamp/year`,
`timestamp(4)` + `date` AS `timestamp(4)+date`,
`timestamp(4)` - `date` AS `timestamp(4)-date`,
`timestamp(4)` * `date` AS `timestamp(4)*date`,
`timestamp(4)` / `date` AS `timestamp(4)/date`,
`timestamp(4)` + `time` AS `timestamp(4)+time`,
`timestamp(4)` - `time` AS `timestamp(4)-time`,
`timestamp(4)` * `time` AS `timestamp(4)*time`,
`timestamp(4)` / `time` AS `timestamp(4)/time`,
`timestamp(4)` + `time(4)` AS `timestamp(4)+time(4)`,
`timestamp(4)` - `time(4)` AS `timestamp(4)-time(4)`,
`timestamp(4)` * `time(4)` AS `timestamp(4)*time(4)`,
`timestamp(4)` / `time(4)` AS `timestamp(4)/time(4)`,
`timestamp(4)` + `datetime` AS `timestamp(4)+datetime`,
`timestamp(4)` - `datetime` AS `timestamp(4)-datetime`,
`timestamp(4)` * `datetime` AS `timestamp(4)*datetime`,
`timestamp(4)` / `datetime` AS `timestamp(4)/datetime`,
`timestamp(4)` + `datetime(4)` AS `timestamp(4)+datetime(4)`,
`timestamp(4)` - `datetime(4)` AS `timestamp(4)-datetime(4)`,
`timestamp(4)` * `datetime(4)` AS `timestamp(4)*datetime(4)`,
`timestamp(4)` / `datetime(4)` AS `timestamp(4)/datetime(4)`,
`timestamp(4)` + `timestamp` AS `timestamp(4)+timestamp`,
`timestamp(4)` - `timestamp` AS `timestamp(4)-timestamp`,
`timestamp(4)` * `timestamp` AS `timestamp(4)*timestamp`,
`timestamp(4)` / `timestamp` AS `timestamp(4)/timestamp`,
`timestamp(4)` + `timestamp(4)` AS `timestamp(4)+timestamp(4)`,
`timestamp(4)` - `timestamp(4)` AS `timestamp(4)-timestamp(4)`,
`timestamp(4)` * `timestamp(4)` AS `timestamp(4)*timestamp(4)`,
`timestamp(4)` / `timestamp(4)` AS `timestamp(4)/timestamp(4)`,
`timestamp(4)` + `year` AS `timestamp(4)+year`,
`timestamp(4)` - `year` AS `timestamp(4)-year`,
`timestamp(4)` * `year` AS `timestamp(4)*year`,
`timestamp(4)` / `year` AS `timestamp(4)/year`,
`year` + `date` AS `year+date`,
`year` - `date` AS `year-date`,
`year` * `date` AS `year*date`,
`year` / `date` AS `year/date`,
`year` + `time` AS `year+time`,
`year` - `time` AS `year-time`,
`year` * `time` AS `year*time`,
`year` / `time` AS `year/time`,
`year` + `time(4)` AS `year+time(4)`,
`year` - `time(4)` AS `year-time(4)`,
`year` * `time(4)` AS `year*time(4)`,
`year` / `time(4)` AS `year/time(4)`,
`year` + `datetime` AS `year+datetime`,
`year` - `datetime` AS `year-datetime`,
`year` * `datetime` AS `year*datetime`,
`year` / `datetime` AS `year/datetime`,
`year` + `datetime(4)` AS `year+datetime(4)`,
`year` - `datetime(4)` AS `year-datetime(4)`,
`year` * `datetime(4)` AS `year*datetime(4)`,
`year` / `datetime(4)` AS `year/datetime(4)`,
`year` + `timestamp` AS `year+timestamp`,
`year` - `timestamp` AS `year-timestamp`,
`year` * `timestamp` AS `year*timestamp`,
`year` / `timestamp` AS `year/timestamp`,
`year` + `timestamp(4)` AS `year+timestamp(4)`,
`year` - `timestamp(4)` AS `year-timestamp(4)`,
`year` * `timestamp(4)` AS `year*timestamp(4)`,
`year` / `timestamp(4)` AS `year/timestamp(4)`,
`year` + `year` AS `year+year`,
`year` - `year` AS `year-year`,
`year` * `year` AS `year*year`,
`year` / `year` AS `year/year`
FROM test_time_table;
SHOW COLUMNS FROM test_time_type;

-- test normal value
delete from test_time_type;
delete from test_time_table;
insert into test_time_type values(null);
insert into test_time_table values
('2023-02-05',
'19:10:50', '19:10:50.3456',
'2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023-02-05 19:10:50', '2023-02-05 19:10:50.456',
'2023');
UPDATE test_time_type, test_time_table SET test_time_type.`date+date` = test_time_table.`date`+test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`date-date` = test_time_table.`date`-test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`date*date` = test_time_table.`date`*test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`date/date` = test_time_table.`date`/test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`date+time` = test_time_table.`date`+test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`date-time` = test_time_table.`date`-test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`date*time` = test_time_table.`date`*test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`date/time` = test_time_table.`date`/test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`date+time(4)` = test_time_table.`date`+test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date-time(4)` = test_time_table.`date`-test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date*time(4)` = test_time_table.`date`*test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date/time(4)` = test_time_table.`date`/test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date+datetime` = test_time_table.`date`+test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`date-datetime` = test_time_table.`date`-test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`date*datetime` = test_time_table.`date`*test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`date/datetime` = test_time_table.`date`/test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`date+datetime(4)` = test_time_table.`date`+test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date-datetime(4)` = test_time_table.`date`-test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date*datetime(4)` = test_time_table.`date`*test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date/datetime(4)` = test_time_table.`date`/test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date+timestamp` = test_time_table.`date`+test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`date-timestamp` = test_time_table.`date`-test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`date*timestamp` = test_time_table.`date`*test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`date/timestamp` = test_time_table.`date`/test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`date+timestamp(4)` = test_time_table.`date`+test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date-timestamp(4)` = test_time_table.`date`-test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date*timestamp(4)` = test_time_table.`date`*test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date/timestamp(4)` = test_time_table.`date`/test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`date+year` = test_time_table.`date`+test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`date-year` = test_time_table.`date`-test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`date*year` = test_time_table.`date`*test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`date/year` = test_time_table.`date`/test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`time+date` = test_time_table.`time`+test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`time-date` = test_time_table.`time`-test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`time*date` = test_time_table.`time`*test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`time/date` = test_time_table.`time`/test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`time+time` = test_time_table.`time`+test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`time-time` = test_time_table.`time`-test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`time*time` = test_time_table.`time`*test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`time/time` = test_time_table.`time`/test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`time+time(4)` = test_time_table.`time`+test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time-time(4)` = test_time_table.`time`-test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time*time(4)` = test_time_table.`time`*test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time/time(4)` = test_time_table.`time`/test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time+datetime` = test_time_table.`time`+test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`time-datetime` = test_time_table.`time`-test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`time*datetime` = test_time_table.`time`*test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`time/datetime` = test_time_table.`time`/test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`time+datetime(4)` = test_time_table.`time`+test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time-datetime(4)` = test_time_table.`time`-test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time*datetime(4)` = test_time_table.`time`*test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time/datetime(4)` = test_time_table.`time`/test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time+timestamp` = test_time_table.`time`+test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`time-timestamp` = test_time_table.`time`-test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`time*timestamp` = test_time_table.`time`*test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`time/timestamp` = test_time_table.`time`/test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`time+timestamp(4)` = test_time_table.`time`+test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time-timestamp(4)` = test_time_table.`time`-test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time*timestamp(4)` = test_time_table.`time`*test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time/timestamp(4)` = test_time_table.`time`/test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time+year` = test_time_table.`time`+test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`time-year` = test_time_table.`time`-test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`time*year` = test_time_table.`time`*test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`time/year` = test_time_table.`time`/test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)+date` = test_time_table.`time(4)`+test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)-date` = test_time_table.`time(4)`-test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)*date` = test_time_table.`time(4)`*test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)/date` = test_time_table.`time(4)`/test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)+time` = test_time_table.`time(4)`+test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)-time` = test_time_table.`time(4)`-test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)*time` = test_time_table.`time(4)`*test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)/time` = test_time_table.`time(4)`/test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)+time(4)` = test_time_table.`time(4)`+test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)-time(4)` = test_time_table.`time(4)`-test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)*time(4)` = test_time_table.`time(4)`*test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)/time(4)` = test_time_table.`time(4)`/test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)+datetime` = test_time_table.`time(4)`+test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)-datetime` = test_time_table.`time(4)`-test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)*datetime` = test_time_table.`time(4)`*test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)/datetime` = test_time_table.`time(4)`/test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)+datetime(4)` = test_time_table.`time(4)`+test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)-datetime(4)` = test_time_table.`time(4)`-test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)*datetime(4)` = test_time_table.`time(4)`*test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)/datetime(4)` = test_time_table.`time(4)`/test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)+timestamp` = test_time_table.`time(4)`+test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)-timestamp` = test_time_table.`time(4)`-test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)*timestamp` = test_time_table.`time(4)`*test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)/timestamp` = test_time_table.`time(4)`/test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)+timestamp(4)` = test_time_table.`time(4)`+test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)-timestamp(4)` = test_time_table.`time(4)`-test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)*timestamp(4)` = test_time_table.`time(4)`*test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)/timestamp(4)` = test_time_table.`time(4)`/test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)+year` = test_time_table.`time(4)`+test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)-year` = test_time_table.`time(4)`-test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)*year` = test_time_table.`time(4)`*test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`time(4)/year` = test_time_table.`time(4)`/test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime+date` = test_time_table.`datetime`+test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime-date` = test_time_table.`datetime`-test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime*date` = test_time_table.`datetime`*test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime/date` = test_time_table.`datetime`/test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime+time` = test_time_table.`datetime`+test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime-time` = test_time_table.`datetime`-test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime*time` = test_time_table.`datetime`*test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime/time` = test_time_table.`datetime`/test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime+time(4)` = test_time_table.`datetime`+test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime-time(4)` = test_time_table.`datetime`-test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime*time(4)` = test_time_table.`datetime`*test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime/time(4)` = test_time_table.`datetime`/test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime+datetime` = test_time_table.`datetime`+test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime-datetime` = test_time_table.`datetime`-test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime*datetime` = test_time_table.`datetime`*test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime/datetime` = test_time_table.`datetime`/test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime+datetime(4)` = test_time_table.`datetime`+test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime-datetime(4)` = test_time_table.`datetime`-test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime*datetime(4)` = test_time_table.`datetime`*test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime/datetime(4)` = test_time_table.`datetime`/test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime+timestamp` = test_time_table.`datetime`+test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime-timestamp` = test_time_table.`datetime`-test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime*timestamp` = test_time_table.`datetime`*test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime/timestamp` = test_time_table.`datetime`/test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime+timestamp(4)` = test_time_table.`datetime`+test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime-timestamp(4)` = test_time_table.`datetime`-test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime*timestamp(4)` = test_time_table.`datetime`*test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime/timestamp(4)` = test_time_table.`datetime`/test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime+year` = test_time_table.`datetime`+test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime-year` = test_time_table.`datetime`-test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime*year` = test_time_table.`datetime`*test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime/year` = test_time_table.`datetime`/test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)+date` = test_time_table.`datetime(4)`+test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)-date` = test_time_table.`datetime(4)`-test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)*date` = test_time_table.`datetime(4)`*test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)/date` = test_time_table.`datetime(4)`/test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)+time` = test_time_table.`datetime(4)`+test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)-time` = test_time_table.`datetime(4)`-test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)*time` = test_time_table.`datetime(4)`*test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)/time` = test_time_table.`datetime(4)`/test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)+time(4)` = test_time_table.`datetime(4)`+test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)-time(4)` = test_time_table.`datetime(4)`-test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)*time(4)` = test_time_table.`datetime(4)`*test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)/time(4)` = test_time_table.`datetime(4)`/test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)+datetime` = test_time_table.`datetime(4)`+test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)-datetime` = test_time_table.`datetime(4)`-test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)*datetime` = test_time_table.`datetime(4)`*test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)/datetime` = test_time_table.`datetime(4)`/test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)+datetime(4)` = test_time_table.`datetime(4)`+test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)-datetime(4)` = test_time_table.`datetime(4)`-test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)*datetime(4)` = test_time_table.`datetime(4)`*test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)/datetime(4)` = test_time_table.`datetime(4)`/test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)+timestamp` = test_time_table.`datetime(4)`+test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)-timestamp` = test_time_table.`datetime(4)`-test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)*timestamp` = test_time_table.`datetime(4)`*test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)/timestamp` = test_time_table.`datetime(4)`/test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)+timestamp(4)` = test_time_table.`datetime(4)`+test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)-timestamp(4)` = test_time_table.`datetime(4)`-test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)*timestamp(4)` = test_time_table.`datetime(4)`*test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)/timestamp(4)` = test_time_table.`datetime(4)`/test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)+year` = test_time_table.`datetime(4)`+test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)-year` = test_time_table.`datetime(4)`-test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)*year` = test_time_table.`datetime(4)`*test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`datetime(4)/year` = test_time_table.`datetime(4)`/test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp+date` = test_time_table.`timestamp`+test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp-date` = test_time_table.`timestamp`-test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp*date` = test_time_table.`timestamp`*test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp/date` = test_time_table.`timestamp`/test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp+time` = test_time_table.`timestamp`+test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp-time` = test_time_table.`timestamp`-test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp*time` = test_time_table.`timestamp`*test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp/time` = test_time_table.`timestamp`/test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp+time(4)` = test_time_table.`timestamp`+test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp-time(4)` = test_time_table.`timestamp`-test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp*time(4)` = test_time_table.`timestamp`*test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp/time(4)` = test_time_table.`timestamp`/test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp+datetime` = test_time_table.`timestamp`+test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp-datetime` = test_time_table.`timestamp`-test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp*datetime` = test_time_table.`timestamp`*test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp/datetime` = test_time_table.`timestamp`/test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp+datetime(4)` = test_time_table.`timestamp`+test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp-datetime(4)` = test_time_table.`timestamp`-test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp*datetime(4)` = test_time_table.`timestamp`*test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp/datetime(4)` = test_time_table.`timestamp`/test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp+timestamp` = test_time_table.`timestamp`+test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp-timestamp` = test_time_table.`timestamp`-test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp*timestamp` = test_time_table.`timestamp`*test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp/timestamp` = test_time_table.`timestamp`/test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp+timestamp(4)` = test_time_table.`timestamp`+test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp-timestamp(4)` = test_time_table.`timestamp`-test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp*timestamp(4)` = test_time_table.`timestamp`*test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp/timestamp(4)` = test_time_table.`timestamp`/test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp+year` = test_time_table.`timestamp`+test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp-year` = test_time_table.`timestamp`-test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp*year` = test_time_table.`timestamp`*test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp/year` = test_time_table.`timestamp`/test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)+date` = test_time_table.`timestamp(4)`+test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)-date` = test_time_table.`timestamp(4)`-test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)*date` = test_time_table.`timestamp(4)`*test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)/date` = test_time_table.`timestamp(4)`/test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)+time` = test_time_table.`timestamp(4)`+test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)-time` = test_time_table.`timestamp(4)`-test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)*time` = test_time_table.`timestamp(4)`*test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)/time` = test_time_table.`timestamp(4)`/test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)+time(4)` = test_time_table.`timestamp(4)`+test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)-time(4)` = test_time_table.`timestamp(4)`-test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)*time(4)` = test_time_table.`timestamp(4)`*test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)/time(4)` = test_time_table.`timestamp(4)`/test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)+datetime` = test_time_table.`timestamp(4)`+test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)-datetime` = test_time_table.`timestamp(4)`-test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)*datetime` = test_time_table.`timestamp(4)`*test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)/datetime` = test_time_table.`timestamp(4)`/test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)+datetime(4)` = test_time_table.`timestamp(4)`+test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)-datetime(4)` = test_time_table.`timestamp(4)`-test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)*datetime(4)` = test_time_table.`timestamp(4)`*test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)/datetime(4)` = test_time_table.`timestamp(4)`/test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)+timestamp` = test_time_table.`timestamp(4)`+test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)-timestamp` = test_time_table.`timestamp(4)`-test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)*timestamp` = test_time_table.`timestamp(4)`*test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)/timestamp` = test_time_table.`timestamp(4)`/test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)+timestamp(4)` = test_time_table.`timestamp(4)`+test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)-timestamp(4)` = test_time_table.`timestamp(4)`-test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)*timestamp(4)` = test_time_table.`timestamp(4)`*test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)/timestamp(4)` = test_time_table.`timestamp(4)`/test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)+year` = test_time_table.`timestamp(4)`+test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)-year` = test_time_table.`timestamp(4)`-test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)*year` = test_time_table.`timestamp(4)`*test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`timestamp(4)/year` = test_time_table.`timestamp(4)`/test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`year+date` = test_time_table.`year`+test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`year-date` = test_time_table.`year`-test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`year*date` = test_time_table.`year`*test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`year/date` = test_time_table.`year`/test_time_table.`date`;
UPDATE test_time_type, test_time_table SET test_time_type.`year+time` = test_time_table.`year`+test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`year-time` = test_time_table.`year`-test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`year*time` = test_time_table.`year`*test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`year/time` = test_time_table.`year`/test_time_table.`time`;
UPDATE test_time_type, test_time_table SET test_time_type.`year+time(4)` = test_time_table.`year`+test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year-time(4)` = test_time_table.`year`-test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year*time(4)` = test_time_table.`year`*test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year/time(4)` = test_time_table.`year`/test_time_table.`time(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year+datetime` = test_time_table.`year`+test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`year-datetime` = test_time_table.`year`-test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`year*datetime` = test_time_table.`year`*test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`year/datetime` = test_time_table.`year`/test_time_table.`datetime`;
UPDATE test_time_type, test_time_table SET test_time_type.`year+datetime(4)` = test_time_table.`year`+test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year-datetime(4)` = test_time_table.`year`-test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year*datetime(4)` = test_time_table.`year`*test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year/datetime(4)` = test_time_table.`year`/test_time_table.`datetime(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year+timestamp` = test_time_table.`year`+test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`year-timestamp` = test_time_table.`year`-test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`year*timestamp` = test_time_table.`year`*test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`year/timestamp` = test_time_table.`year`/test_time_table.`timestamp`;
UPDATE test_time_type, test_time_table SET test_time_type.`year+timestamp(4)` = test_time_table.`year`+test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year-timestamp(4)` = test_time_table.`year`-test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year*timestamp(4)` = test_time_table.`year`*test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year/timestamp(4)` = test_time_table.`year`/test_time_table.`timestamp(4)`;
UPDATE test_time_type, test_time_table SET test_time_type.`year+year` = test_time_table.`year`+test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`year-year` = test_time_table.`year`-test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`year*year` = test_time_table.`year`*test_time_table.`year`;
UPDATE test_time_type, test_time_table SET test_time_type.`year/year` = test_time_table.`year`/test_time_table.`year`;
select * from test_time_type;

select pg_typeof(current_date());
select date '2022-02-01' + 1 AS result;
select date '2022-02-01' - 2 AS result;
select date '2022-02-01' * 2 AS result;
select date '2022-02-01' / 2 AS result;

-- test timestamp>,>=,<,<=,=,<> with int/uint
set dolphin.sql_mode='sql_mode_strict,sql_mode_full_group,pipes_as_concat,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero';
create table t1 (id int, dt datetime, dt1 timestamp);
insert into t1 values (1,"2001-08-14 00:00:00","2001-08-14 00:00:00"),(2,"2001-08-15 00:00:00","2001-08-15 00:00:00"),(3,"2001-08-16 00:00:00","2001-08-16 00:00:00"),(4,"2003-09-15 01:20:30","2003-09-15 01:20:30");
select * from t1 where dt > 20021020 and dt1 > 20021020 order by dt;
select * from t1 where dt > 20 and dt1 > 20 order by dt;
select * from t1 where dt > 20000000000000 and dt1 > 20000000000000 order by dt;
select * from t1 where dt > 200000000000000 and dt1 > 200000000000000 order by dt;
select * from t1 where dt >= 20021020 and dt1 >= 20021020 order by dt;
select * from t1 where dt >= 20 and dt1 >= 20 order by dt;
select * from t1 where dt >= 20000000000000 and dt1 >= 20000000000000 order by dt;
select * from t1 where dt >= 200000000000000 and dt1 >= 200000000000000 order by dt;
select * from t1 where dt < 20030915 and dt1 < 20030915 order by dt;
select * from t1 where dt < 2003 and dt < 2003 order by dt;
select * from t1 where dt < 200000000000000 and dt < 200000000000000 order by dt;
select * from t1 where dt <= 20030915 and dt <= 20030915 order by dt;
select * from t1 where dt <= 2003 and dt <= 2003 order by dt;
select * from t1 where dt <= 200000000000000 and dt <= 200000000000000 order by dt;
select * from t1 where dt <> 20010814 and dt <> 20010814 order by dt;
select * from t1 where dt = 20010814;

select * from t1 where dt > 20021020::uint4 and dt1 > 20021020::uint4 order by dt;
select * from t1 where dt > 20::uint4 and dt1 > 20::uint4 order by dt;
select * from t1 where dt > 20000000000000::uint4 and dt1 > 20000000000000::uint4 order by dt;
select * from t1 where dt > 200000000000000::uint4 and dt1 > 200000000000000::uint4 order by dt;
select * from t1 where dt >= 20021020::uint4 and dt1 >= 20021020::uint4 order by dt;
select * from t1 where dt >= 20::uint4 and dt1 >= 20::uint4 order by dt;
select * from t1 where dt >= 20000000000000::uint4 and dt1 >= 20000000000000::uint4 order by dt;
select * from t1 where dt >= 200000000000000::uint4 and dt1 >= 200000000000000::uint4 order by dt;
select * from t1 where dt < 20030915::uint4 and dt1 < 20030915::uint4 order by dt;
select * from t1 where dt < 2003::uint4 and dt1 < 2003::uint4 order by dt;
select * from t1 where dt < 200000000000000::uint4 and dt1 < 200000000000000::uint4 order by dt;
select * from t1 where dt <= 20030915::uint4 and dt1 <= 20030915::uint4 order by dt;
select * from t1 where dt <= 2003::uint4 and dt1 <= 2003::uint4 order by dt;
select * from t1 where dt <= 200000000000000::uint4 and dt1 <= 200000000000000::uint4 order by dt;
select * from t1 where dt <> 20010814::uint4 and dt1 <> 20010814::uint4 order by dt;
						 
select * from t1 where dt > 20021020::int8 and dt1 > 20021020::int8 order by dt;
select * from t1 where dt > 20::int8 and dt1 > 20::int8 order by dt;
select * from t1 where dt > 20000000000000::int8 and dt1 > 20000000000000::int8 order by dt;
select * from t1 where dt > 200000000000000::int8 and dt1 > 200000000000000::int8 order by dt;
select * from t1 where dt >= 20021020::int8 and dt1 >= 20021020::int8 order by dt;
select * from t1 where dt >= 20::int8 and dt1 >= 20::int8 order by dt;
select * from t1 where dt >= 20000000000000::int8 and dt1 >= 20000000000000::int8 order by dt;
select * from t1 where dt >= 200000000000000::int8 and dt1 >= 200000000000000::int8 order by dt;
select * from t1 where dt < 20030915::int8 and dt1 < 20030915::int8 order by dt;
select * from t1 where dt < 2003::int8 and dt1 < 2003::int8 order by dt;
select * from t1 where dt < 200000000000000::int8 and dt1 < 200000000000000::int8 order by dt;
select * from t1 where dt <= 20030915::int8 and dt1 <= 20030915::int8 order by dt;
select * from t1 where dt <= 2003::int8 and dt1 <= 2003::int8 order by dt;
select * from t1 where dt <= 200000000000000::int8 and dt1 <= 200000000000000::int8 order by dt;
select * from t1 where dt <> 20010814::int8 and dt1 <> 20010814::int8 order by dt;
						 
select * from t1 where dt > 20021020::uint8 and dt1 > 20021020::uint8 order by dt;
select * from t1 where dt > 20::uint8 and dt1 > 20::uint8 order by dt;
select * from t1 where dt > 20000000000000::uint8 and dt1 > 20000000000000::uint8 order by dt;
select * from t1 where dt > 200000000000000::uint8 and dt1 > 200000000000000::uint8 order by dt;
select * from t1 where dt >= 20021020::uint8 and dt1 >= 20021020::uint8 order by dt;
select * from t1 where dt >= 20::uint8 and dt1 >= 20::uint8 order by dt;
select * from t1 where dt >= 20000000000000::uint8 and dt1 >= 20000000000000::uint8 order by dt;
select * from t1 where dt >= 200000000000000::uint8 and dt1 >= 200000000000000::uint8 order by dt;
select * from t1 where dt < 20030915::uint8 and dt1 < 20030915::uint8 order by dt;
select * from t1 where dt < 2003::uint8 and dt1 < 2003::uint8 order by dt;
select * from t1 where dt < 200000000000000::uint8 and dt1 < 200000000000000::uint8 order by dt;
select * from t1 where dt <= 20030915::uint8 and dt1 <= 20030915::uint8 order by dt;
select * from t1 where dt <= 2003::uint8 and dt1 <= 2003::uint8 order by dt;
select * from t1 where dt <= 200000000000000::uint8 and dt1 <= 200000000000000::uint8 order by dt;
select * from t1 where dt <> 20010814::uint8 and dt1 <> 20010814::uint8 order by dt;
						 
select * from t1 where dt > 20::int1 and dt1 > 20::int1 order by dt;
select * from t1 where dt >= 20::int1 and dt1 >= 20::int1 order by dt;
select * from t1 where dt < 20::int1 and dt1 < 20::int1 order by dt;
select * from t1 where dt <= 20::int1 and dt1 <= 20::int1 order by dt;
select * from t1 where dt <> 20::int1 and dt1 <> 20::int1 order by dt;
select * from t1 where dt > 2000::int2 and dt1 > 2000::int2 order by dt;
select * from t1 where dt >= 2000::int2 and dt1 >= 2000::int2 order by dt;
select * from t1 where dt < 2000::int2 and dt1 < 2000::int2 order by dt;
select * from t1 where dt <= 2000::int2 and dt1 <= 2000::int2 order by dt;
select * from t1 where dt <> 2000::int2 and dt1 <> 2000::int2 order by dt;
select * from t1 where dt > 20::uint1 and dt1 > 20::uint1 order by dt;
select * from t1 where dt >= 20::uint1 and dt1 >= 20::uint1 order by dt;
select * from t1 where dt < 20::uint1 and dt1 < 20::uint1 order by dt;
select * from t1 where dt <= 20::uint1 and dt1 <= 20::uint1 order by dt;
select * from t1 where dt <> 20::uint1 and dt1 <> 20::uint1 order by dt;
select * from t1 where dt > 2000::uint2 and dt1 > 2000::uint2 order by dt;
select * from t1 where dt >= 2000::uint2 and dt1 >= 2000::uint2 order by dt;
select * from t1 where dt < 2000::uint2 and dt1 < 2000::uint2 order by dt;
select * from t1 where dt <= 2000::uint2 and dt1 <= 2000::uint2 order by dt;
select * from t1 where dt <> 2000::uint2 and dt1 <> 2000::uint2  order by dt;


select * from t1 where dt > 20021020::float and dt1 > 20021020::float order by dt;
select * from t1 where dt > 20021020::double and dt1 > 20021020::double order by dt;
select * from t1 where dt > 20021020::numeric and dt1 > 20021020::numeric order by dt;
select * from t1 where dt <> 20021020::float and dt1 <> 20021020::float order by dt;
select * from t1 where dt <> 20021020::double and dt1 <> 20021020::double order by dt;
select * from t1 where dt <> 20021020::numeric and dt1 <> 20021020::numeric order by dt;

drop table t1;

-- test datetime >,>=,<,<=,=,<> with text(ADDDATE(datetime,INTERVAL))
create table t1 (ctime timestamp(4) without time zone);
insert into t1 values('2020-03-20 17:51:46.189');

SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) = ctime from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) > ctime from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) < ctime from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) >= ctime from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) <= ctime from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) <> ctime from t1;

SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime = ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime > ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime < ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime >= ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime <= ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime <> ADDDATE(ctime, INTERVAL 1 minute) from t1;

SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) = ctime::timestamp from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) > ctime::timestamp from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) < ctime::timestamp from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) >= ctime::timestamp from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) <= ctime::timestamp from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 hour), ADDDATE(ctime, INTERVAL 1 hour) <> ctime::timestamp from t1;

SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime::timestamp = ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime::timestamp > ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime::timestamp < ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime::timestamp >= ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime::timestamp <= ADDDATE(ctime, INTERVAL 1 minute) from t1;
SELECT ctime, ADDDATE(ctime, INTERVAL 1 minute), ctime::timestamp <> ADDDATE(ctime, INTERVAL 1 minute) from t1;

drop table t1;

---------- tail ----------
drop schema time_operator_test_schema cascade;
reset current_schema;

-- test the original type and function
set dolphin.b_compatibility_mode to off;
select curdate() + 10;
select pg_typeof(curdate() + 10);
select curdate() + 10.5;
select pg_typeof(curdate() + 10.5);
select curdate() + 10.3;
select pg_typeof(curdate() + 10.3);
select curdate() - 10;
select pg_typeof(curdate() - 10);
select curdate() - 10.5;
select pg_typeof(curdate() - 10.5);
select curdate() - 10.3;
select pg_typeof(curdate() - 10.3);

select pg_typeof(curtime() + 10);
select curtime() + 10.5;
select pg_typeof(curtime() + 10.5);
select curtime() + 10.3;
select pg_typeof(curtime() + 10.3);
select curtime() - 10;
select pg_typeof(curtime() - 10);
select curtime() - 10.5;
select pg_typeof(curtime() - 10.5);
select curtime() - 10.3;
select pg_typeof(curtime() - 10.3);
select curtime() * 2;
select pg_typeof(curtime() * 10.3);
select curtime() / 10.3;
select pg_typeof(curtime() / 10.3);

select pg_typeof(current_timestamp + 10);
select current_timestamp + 10.5;
select pg_typeof(current_timestamp + 10.5);
select current_timestamp + 10.3;
select pg_typeof(current_timestamp + 10.3);
select current_timestamp - 10;
select pg_typeof(current_timestamp - 10);
select current_timestamp - 10.5;
select pg_typeof(current_timestamp - 10.5);
select current_timestamp - 10.3;
select pg_typeof(current_timestamp - 10.3);
select current_timestamp * 2;
select pg_typeof(current_timestamp * 10.3);
select current_timestamp / 10.3;
select pg_typeof(current_timestamp / 10.3);

select current_timestamp(0);
select pg_typeof(current_timestamp(0));
select current_timestamp(3);
select pg_typeof(current_timestamp(3));
select current_timestamp(6);
select pg_typeof(current_timestamp(6));
select current_timestamp( );
select pg_typeof(current_timestamp( ));

select current_timestamp(-1);
select current_timestamp(3.5);
select current_timestamp(10);
select current_timestamp('abc');
select current_timestamp('汉字');
select current_timestamp('@');
select current_timestamp('!');

select pg_typeof(localtimestamp + 10);
select localtimestamp + 10.5;
select pg_typeof(localtimestamp + 10.5);
select localtimestamp + 10.3;
select pg_typeof(localtimestamp + 10.3);
select localtimestamp - 10;
select pg_typeof(localtimestamp - 10);
select localtimestamp - 10.5;
select pg_typeof(localtimestamp - 10.5);
select localtimestamp - 10.3;
select pg_typeof(localtimestamp - 10.3);
select localtimestamp * 2;
select pg_typeof(localtimestamp * 10.3);
select localtimestamp / 10.3;
select pg_typeof(localtimestamp / 10.3);

select localtimestamp(0);
select pg_typeof(localtimestamp(0));
select localtimestamp(3);
select pg_typeof(localtimestamp(3));
select localtimestamp(6);
select pg_typeof(localtimestamp(6));
select localtimestamp( );
select pg_typeof(localtimestamp( ));

select localtimestamp(-1);
select localtimestamp(3.5);
select localtimestamp(10);
select localtimestamp('abc');
select localtimestamp('汉字');
select localtimestamp('@');
select localtimestamp('!');

select pg_typeof(localtimestamp() + 10);
select localtimestamp() + 10.5;
select pg_typeof(localtimestamp() + 10.5);
select localtimestamp() + 10.3;
select pg_typeof(localtimestamp() + 10.3);
select localtimestamp() - 10;
select pg_typeof(localtimestamp() - 10);
select localtimestamp() - 10.5;
select pg_typeof(localtimestamp() - 10.5);
select localtimestamp() - 10.3;
select pg_typeof(localtimestamp() - 10.3);
select localtimestamp() * 2;
select pg_typeof(localtimestamp() * 2);
select localtimestamp() / 10.3;
select pg_typeof(localtimestamp() / 10.3);

select pg_typeof(localtimestamp(3) + 10);
select localtimestamp(3) + 10.5;
select pg_typeof(localtimestamp(3) + 10.5);
select localtimestamp(3) + 10.3;
select pg_typeof(localtimestamp(3) + 10.3);
select localtimestamp(3) - 10;
select pg_typeof(localtimestamp(3) - 10);
select localtimestamp(3) - 10.5;
select pg_typeof(localtimestamp(3) - 10.5);
select localtimestamp(3) - 10.3;
select pg_typeof(localtimestamp(3) - 10.3);
select localtimestamp(3) * 2;
select pg_typeof(localtimestamp(3) * 10.3);
select localtimestamp(3) / 10.3;
select pg_typeof(localtimestamp(3) / 10.3);

select now(0);
select pg_typeof(now(0));
select now(3);
select pg_typeof(now(3));
select now(6);
select pg_typeof(now(6));
select now( );
select pg_typeof(now( ));

select now(-1);
select now(3.5);
select now(10);
select now('abc');
select now('汉字');
select now('@');
select now('!');

select pg_typeof(now(3) + 10);
select now(3) + 10.5;
select pg_typeof(now(3) + 10.5);
select now(3) + 10.3;
select pg_typeof(now(3) + 10.3);
select now(3) - 10;
select pg_typeof(now(3) - 10);
select now(3) - 10.5;
select pg_typeof(now(3) - 10.5);
select now(3) - 10.3;
select pg_typeof(now(3) - 10.3);
select now(3) * 2;
select pg_typeof(now(3) * 2);
select now(3) / 2;
select pg_typeof(now(3) / 2);

select sysdate(0);
select pg_typeof(sysdate(0));
select sysdate(3);
select pg_typeof(sysdate(3));
select sysdate(6);
select pg_typeof(sysdate(6));
select sysdate( );
select pg_typeof(sysdate( ));

select sysdate(-1);
select sysdate(3.5);
select sysdate(10);
select sysdate('abc');
select sysdate('汉字');
select sysdate('@');
select sysdate('!');

select pg_typeof(sysdate(3) + 10);
select sysdate(3) + 10.5;
select pg_typeof(sysdate(3) + 10.5);
select sysdate(3) + 10.3;
select pg_typeof(sysdate(3) + 10.3);
select sysdate(3) - 10;
select pg_typeof(sysdate(3) - 10);
select sysdate(3) - 10.5;
select pg_typeof(sysdate(3) - 10.5);
select sysdate(3) - 10.3;
select pg_typeof(sysdate(3) - 10.3);
select sysdate(3) * 2;
select pg_typeof(sysdate(3) * 2);
select sysdate(3) / 2;
select pg_typeof(sysdate(3) / 2);

-- CURRENT_DATE (00:00:00) == (current date) 00:00:00
select CURRENT_DATE = time'00:00:00';
select (CURRENT_DATE::date + interval'11 hour 11 min 11 sec') = time'11:11:11';
select (CURRENT_DATE::datetime + interval'11 hour 11 min 11 sec') = time'11:11:11';
select (CURRENT_DATE::timestamp + interval'11 hour 11 min 11 sec') = time'11:11:11';
select CURRENT_DATE <> time'00:00:00';
select (CURRENT_DATE::date + interval'11 hour 11 min 11 sec') <> time'11:11:11';
select (CURRENT_DATE::datetime + interval'11 hour 11 min 11 sec') <> time'11:11:11';
select (CURRENT_DATE::timestamp + interval'11 hour 11 min 11 sec') <> time'11:11:11';
select CURRENT_DATE < time'00:00:00';
select (CURRENT_DATE::date + interval'11 hour 11 min 11 sec') < time'11:11:11';
select (CURRENT_DATE::datetime + interval'11 hour 11 min 11 sec') < time'11:11:11';
select (CURRENT_DATE::timestamp + interval'11 hour 11 min 11 sec') < time'11:11:11';
select CURRENT_DATE <= time'00:00:00';
select (CURRENT_DATE::date + interval'11 hour 11 min 11 sec') <= time'11:11:11';
select (CURRENT_DATE::datetime + interval'11 hour 11 min 11 sec') <= time'11:11:11';
select (CURRENT_DATE::timestamp + interval'11 hour 11 min 11 sec') <= time'11:11:11';
select CURRENT_DATE > time'00:00:00';
select (CURRENT_DATE::date + interval'11 hour 11 min 11 sec') > time'11:11:11';
select (CURRENT_DATE::datetime + interval'11 hour 11 min 11 sec') > time'11:11:11';
select (CURRENT_DATE::timestamp + interval'11 hour 11 min 11 sec') > time'11:11:11';
select CURRENT_DATE >= time'00:00:00';
select (CURRENT_DATE::date + interval'11 hour 11 min 11 sec') >= time'11:11:11';
select (CURRENT_DATE::datetime + interval'11 hour 11 min 11 sec') >= time'11:11:11';
select (CURRENT_DATE::timestamp + interval'11 hour 11 min 11 sec') >= time'11:11:11';
select time'11:11:11' < date'2023-01-01';
select time'11:11:11' < datetime'2023-01-01 11:11:11';
select time'11:11:11' < timestamp'2023-01-01 11:11:11';
select time'11:11:11' < timestamptz'2023-01-01 11:11:11';