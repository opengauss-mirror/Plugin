create schema test_json_unquote;
set current_schema to 'test_json_unquote';

select json_unquote('""');
select json_unquote('"abc"');
select json_unquote('abc');
select json_unquote('"abc');
select json_unquote('"abc\t');
select json_unquote('"abc\\t');
select json_unquote('"abc\\t"');
select json_unquote('"ab\\tc"');
select json_unquote('"abc"\t');
select json_unquote('"abc"\\t');
select json_unquote('"ab"djoada"c"');
select json_unquote('a"bdw\tc');
select json_unquote('"abdmad\\tc"');
select json_unquote('abc\\t"dwaj');
select json_unquote('"abcmdw\tmdkw"');
select json_unquote('"abs""c"\t');
select json_unquote('abc\\t');
select json_unquote('abcdw\t');
select json_unquote('"ab"c');
select json_unquote('a"bc');
select json_unquote('ab\tc"');
select json_unquote('ab\\tc"');
select json_unquote('\t"abc"');
select json_unquote('\\t"abc"');
select json_unquote('a\\t"bc"');
SELECT JSON_UNQUOTE('"\\t\\u0032"');
select json_unquote('"\\u0032a"');
select json_unquote('" \\u554a kbhjfuk..."');
select json_unquote('"abcd\\t kjh \\u554 kbhjfuk"');


create table data(name VARCHAR(20) PRIMARY KEY);
insert into data (name) value(json_unquote('"sjy"'));
select name from data;
drop table data;


set standard_conforming_strings = off;
set dolphin.sql_mode = 'sql_mode_strict,sql_mode_full_group,pipes_as_concat,no_zero_date,pad_char_to_full_length,auto_recompile_function,error_for_division_by_zero';

CREATE TABLE enlight_dim_model_point_detail (
    model_id character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci NOT NULL,
    model_name_i18n text CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    model_name_en character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    ouid character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    root_model_id character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    root_model_name_en character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    hierachy_path character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    parent_model_id character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    parent_model_name character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    child_json_en text CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    attr_json text CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    identifier character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci NOT NULL,
    description text CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    name_value text CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    tags text CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    unit text CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    `dataType` character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    `hasQuality` boolean,
    `signalType` character varying(255) CHARACTER SET `UTF8` COLLATE utf8mb4_general_ci,
    update_time timestamp(0) with time zone DEFAULT b_db_statement_start_timestamp(0) ON UPDATE CURRENT_TIMESTAMP(0) NOT NULL
)
CHARACTER SET = "UTF8" COLLATE = "utf8mb4_general_ci"
WITH (orientation=row, compression=no);
ALTER TABLE enlight_dim_model_point_detail ADD CONSTRAINT enlight_dim_model_point_detail_pkey PRIMARY KEY USING btree  (model_id, identifier);

INSERT INTO enlight_dim_model_point_detail (model_id, model_name_i18n, model_name_en, ouid, root_model_id, root_model_name_en, hierachy_path, parent_model_id, parent_model_name, child_json_en, attr_json, identifier, description, name_value, tags, unit, dataType, hasQuality, signalType, update_time) VALUES('dd', '{"i18nValue": {}, "defaultValue": "aaaa"}', NULL, 'o16361044812021863', 'bbbb', NULL, '/bbbb/cc/dd', 'cc', NULL, NULL, NULL, 'HNHJGF.PRI.AI31', NULL, '{"i18nValue":{"en_US":"Vpv2 /PV2输\\n入电压","es_ES":"","zh_CN":"Vpv2 /PV2输\\n入电压","ja_JP":""},"defaultValue":"Vpv2 /PV2输\\n入电压"}', '{}', NULL, 'DOUBLE', false, 'AI', '2025-02-18 23:03:01.000');
 
select model_id, point_id, case when isi18n_zh_flag=1 then il8_value->>'$.zh_CN' else name_value->>'$.defaultValue' end point_name_zh, 
case when isi18n_en_flag=1 then il8_value->>'$.en_US' else name_value->>'$.defaultValue' end point_name_en, 
signalType, unit, group_name, Enlight_tag, ensight_tag, kong_tag 
from (select name_value, convert((name_value->>'$.i18nValue'),json)il8_value, 
coalesce(json_contains_path((name_value->>'$.i18nValue'),'one', '$.zh_CN'),0)isi18n_zh_flag,
coalesce(json_contains_path((name_value->>'$.i18nValue'),'one', '$.en_US'),0)isi18n_en_flag, 
json_contains_path(name_value,'one', '$.i18nValue'),model_id,identifier point_id, 
replace (unit ->"$.unitId",'"','') unit,signalType, replace (tags ->"$.group",'"','') group_name,
(case when replace (tags ->"$.Enlight",'"','') ='true' then 1 else 0 end ) Enlight_tag, 
(case when replace (tags ->"$.Ensight",'"','') ='true' then 1 else 0 end )  ensight_tag,
(case when replace (tags ->"$.kong",'"','') ='true' then 1 else 0 end )  kong_tag
from (select CONVERT (name_value , json) name_value,CONVERT (unit , json) unit, CONVERT (tags , json) tags,
model_id,identifier,signalType from enlight_dim_model_point_detail)enlight_dim_model_point_detail 
where  identifier  NOT REGEXP '_1m$|_5m$|_10m$'and json_contains(tags,'{"calculate":"true"}')=0)aa;

create table tt1(il8_value text);

insert into tt1 values ('{"en_US": "Vpv2 /PV2输\\n入电压", "es_ES": "", "ja_JP": "", "zh_CN": {"aaa":"b\\nbb","kkk\":"jj\\nj"}}');
insert into tt1 values ('{"en_US": "Vpv2 /PV2输\\n入电压", "es_ES": "", "ja_JP": "", "zh_CN": "b\\nbb"}');
insert into tt1 values ('{"en_US": "Vpv2 /PV2输\\n入电压", "es_ES": "", "ja_JP": "", "zh_CN":    {"aaa":"b\\nbb","kkk":"jj\\nj"}}');
insert into tt1 values ('{"en_US": "Vpv2 /PV2输\\n入电压", "es_ES": "", "ja_JP": "", "zh_CN": {"aaa":"b\\nbb"}}');
select il8_value::json->>'$.zh_CN' from tt1;

drop table tt1;
drop table enlight_dim_model_point_detail;

reset standard_conforming_strings;
reset dolphin.sql_mode;

drop schema test_json_unquote cascade;
reset current_schema;
