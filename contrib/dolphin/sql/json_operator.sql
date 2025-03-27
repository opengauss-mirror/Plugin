create schema test_operator;
set current_schema to 'test_operator';

drop table if exists test1;
create table test1(data json);
insert into test1 values('[10,20,[30,40]]');
select data->'$[1]' from test1;
select data->'$[2][1]' from test1;
drop table if exists test2;
create table test2(data json);
insert into test2 values('{"a":"lihua"}');
insert into test2 values('{"b":"zhanghai"}');
insert into test2 values('{"c":"susan"}');
select data->'$.a' from test2;
select data->'$.b' from test2;
select data->'$.c' from test2;
select data->'$.*' from test2;
select data->>'$.*' from test2;
select data->>'$.d' from test2;
select data->'$.d' from test2;
select data->>'$.a' from test2;
select data->>'$.b' from test2;
select data->>'$.c' from test2;
select data->'a' from test2;
select data->'b' from test2;
select data->'c' from test2;
select data->>'a' from test2;
select data->>'b' from test2;
select data->>'c' from test2;

-- test for precedence
CREATE TABLE `dept` (
  `id` int(11) NOT NULL,
  `dept` varchar(255) DEFAULT NULL,
  `json_value` json DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
insert into dept VALUES(1,'部门1','{"deptName": "部门1", "deptId": "1", "deptLeaderId": "3"}');
insert into dept VALUES(2,'部门2','{"deptName": "部门2", "deptId": "2", "deptLeaderId": "4"}');
insert into dept VALUES(3,'部门3','{"deptName": "部门3", "deptId": "3", "deptLeaderId": "5"}');
insert into dept VALUES(4,'部门4','{"deptName": "部门4", "deptId": "4", "deptLeaderId": "5"}');
insert into dept VALUES(5,'部门5','{"deptName": "部门5", "deptId": "5", "deptLeaderId": "5"}');

select * from dept where json_value->>'$.deptId' is not null;

select * from dept where json_value->'$.deptId' is not null;

select '[{"f1":[1,2,3]},[{"f2":null}]]'::jsonb -> 1;
select '{"aa":[{"f1":"q"}],"bb":[{"f2":null}]}'::jsonb -> 'aa';
select '{"cc":{"aa":[{"f1":"q"}],"bb":[{"f2":null}]}}'::jsonb -> 'aa';

create table t(jsondate json);
insert into t values( '{"goodsId": "19934345435346", "categoryId": "C456", "modelType": "MT01", "demandOutOrderId": "D789", "chargeModeCode": ""}');
select jsondate->>'$.chargeModeCode'from t;


set standard_conforming_strings = off;

create table test_json(c1 json);
insert into test_json values ('{"i18nValue":{"en_US":"BreakerClass","zh_CN":"DEV远\\\\方就地"},"defaultValue":"BreakerClass"}');
insert into test_json values ('{"i18nValue":{"en_US":"BreakerClass","zh_CN":"DEV远\\b方就地"},"defaultValue":"BreakerClass"}');
insert into test_json values ('{"i18nValue":{"en_US":"BreakerClass","zh_CN":"DEV远\\f方就地"},"defaultValue":"BreakerClass"}');
insert into test_json values ('{"i18nValue":{"en_US":"BreakerClass","zh_CN":"DEV远\\r方就地"},"defaultValue":"BreakerClass"}');

select c1->>'$.i18nValue'  from test_json order by 1;
select convert((c1->>'$.i18nValue'),json) il8_value from test_json;
select name_value, convert((name_value->>'$.i18nValue'),json)il8_value from (select CONVERT(c1 , json) name_value  from test_json) enlight_dim_model_point_detail;

drop table test_json;
reset standard_conforming_strings;

drop schema test_operator cascade;
reset current_schema;