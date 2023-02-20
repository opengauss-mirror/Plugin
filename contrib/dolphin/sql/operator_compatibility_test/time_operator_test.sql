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

---------- tail ----------
drop schema time_operator_test_schema cascade;
reset current_schema;
