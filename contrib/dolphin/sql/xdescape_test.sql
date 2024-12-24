create schema xdescape_test;
set current_schema to 'xdescape_test';

create table test1(c1 text);
create table fact_weatherstation_daily(site_id varchar(200), weather_i18n json);

insert into test1 values ("ab\"c");

insert into fact_weatherstation_daily (`site_id`, `weather_i18n`) values("Sunny", "{\"defaultValue\":\"Sunny\",\"i18nValue\":{\"en_US\":\"Sunny\",\"zh_CN\":\"晴\",\"es_ES\":\"Soleado\",\"ja_JP\":\"晴れ\"}}");


set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero,escape_quotes';

insert into test1 values ("ab\"c");

insert into fact_weatherstation_daily (`site_id`, `weather_i18n`) values("Sunny", "{\"defaultValue\":\"Sunny\",\"i18nValue\":{\"en_US\":\"Sunny\",\"zh_CN\":\"晴\",\"es_ES\":\"Soleado\",\"ja_JP\":\"晴れ\"}}");

select * from test1;
select * from fact_weatherstation_daily;

set standard_conforming_strings=off;
create table test_blob(col1 blob,col2 json);
insert into test_blob values('\xE78E8BE6ADA3E58583E5A4A7E5B885E6AF94','{"my_key": "\'my_value"}');
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero,disable_escape_bytea';
insert into test_blob values('\xE78E8BE6ADA3E58583E5A4A7E5B885E6AF94','{"my_key": "\'my_value"}');
insert into test_blob values('\x','{"my_key": "\'my_value"}');
insert into test_blob values('','{"my_key": "\'my_value"}');

select * from test_blob order by 1;

drop table test_blob;
drop table test1;
drop table fact_weatherstation_daily;
drop schema xdescape_test cascade;
reset dolphin.sql_mode;
reset current_schema;
