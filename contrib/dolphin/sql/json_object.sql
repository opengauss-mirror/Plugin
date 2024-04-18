create schema test_json_object;
set current_schema to 'test_json_object';

-- test for b_compatibility_mode = false
set dolphin.b_compatibility_mode = off;
select json_object('{a,1,b,2,3,NULL,"d e f","a b c"}');
select json_object('{a,b,"a b c"}', '{a,1,1}');

-- test for b_compatibility_mode = true
set dolphin.b_compatibility_mode = 1;

-- test for basic functionality of json_object
select json_object('name', 'Jim', 'age', 20);
select json_object('name', 'Jim', 'age', 20, 'name', 'Tim');
select json_object('name', 'Tim', 'age', 20, 'friend', json_object('name', 'Jim', 'age', 20), 'hobby', json_build_array('games', 'sports'));
select json_object('City', 'Cairns', 'Population', 139693);
select json_object(1234,234,212,333);
select json_object(1, 'Json', 2, 'MyContex');
select json_object('{"a":1}', null, '{"b":[1,2]}', '{"c":{"c1":[11,null]}}', true, 87);

--test for json key
select json_object('{"a": "b"}'::json, true, '{"json": {"abc": "cde"}}'::json, 'TEXT');
select json_object('{"a": "b"}'::jsonb, 12.32, '{"json": {"abc": "cde"}}'::jsonb, null);
select json_object(json_object('name', 'Jim', 'age', 20), 'MyContex');

-- test for type date and time
select json_object('2022-09-26'::date,20221001::date);
select json_object('12:12:58'::time,'24:00:00'::time);

--test with ->,->>
select json_object('a','{"a":"b"}','b',234342,'c',true)->'a';
select json_object('a','{"a":"b"}'::json,'b',234342,'c',true)->'a';
select json_object('a','{"a":"b"}'::json,'b',234342,'c',true)->>'b';

--test with json functions
select json_object_field(json_object('Name','Adam','Age',23,'Address','Chengdu'),'Name');
select json_object_field(json_object('Name','Adam','Age',23,'Address','Chengdu','Test','{"a":233}'::json),'Test');
select json_object_field('{"Test":11111111111111111111111111111111111111111111111111111111111111111111111}','Test');
select json_object_field_text(json_object('Name','Adam','Age',23,'Address','Chengdu','Test','{"a":233}'::json),'Test');
select json_object_field_text(json_object('Name','Adam','Age',23,'Address','Chengdu'),'Name');
select json_object_field_text('{"Test":11111111111111111111111111111111111111111111111111111111111111111111111}','Test');
select json_extract_path(json_object('Name','Adam','Age',23,'Address','Chengdu'),'Name');
select json_extract_path_op(json_object('Name','Adam','Age',23,'Address','Chengdu'),'{Name}');
select pg_catalog.json_extract_path_op(cast('[0, 0, 0, 0]' as json), cast(pg_catalog.dolphin_types() as _text));
select json_extract_path_text(json_object('Name','Adam','Age',23,'Address','Chengdu'),'Name');
select json_extract_path_text_op(json_object('Name','Adam','Age',23,'Address','Chengdu'),'{Name}');
select * from json_each(json_object('Name','Adam','Age',23,'Address','Chengdu'));
select * from json_each_text(json_object('Name','Adam','Age',23,'Address','Chengdu'));
select * from json_object_keys(json_object('Name','Adam','Age',23,'Address','Chengdu'));
select json_typeof(json_object('Name','Adam','Age',23,'Address','Chengdu'));
select * from json_to_record(json_object('name','Adam','age',23,'address','Chengdu'), true) as x(Name text, Age int, d text);
select ref_0.idx_blks_hit as c0, ref_0.toast_blks_hit as c1, ref_0.toast_blks_read as c2, cast(coalesce(ref_0.heap_blks_hit, ref_0.toast_blks_hit) as int8) as c3, ref_0.idx_blks_hit as c4, ref_0.tidx_blks_hit as c5, ref_0.idx_blks_read as c6, ref_0.heap_blks_read as c7, pg_catalog.json_ge( cast(case when (select amopstrategy from pg_catalog.pg_amop limit 1 offset 5) >= ref_0.heap_blks_hit then (select inherited from pg_catalog.pg_gtt_stats limit 1 offset 5) else (select inherited from pg_catalog.pg_gtt_stats limit 1 offset 5) end as bool), cast(pg_catalog.json_object_field( cast( pg_catalog.json_object_agg( cast(ref_0.relname as name), cast(ref_0.heap_blks_read as int8)) over (partition by ref_0.toast_blks_read order by ref_0.relid) as json), cast(pg_catalog.create_wdr_snapshot() as "text")) as json)) as c8 from pg_catalog.pg_statio_user_tables as ref_0 where (true) or (cast(coalesce((select pg_catalog.every(is_instead) from pg_catalog.pg_rewrite) , case when cast(null as uint1) = cast(null as int2) then pg_catalog.pgxc_disaster_read_init() else pg_catalog.pgxc_disaster_read_init() end ) as bool) > pg_catalog.repeat( cast(pg_catalog.substr( cast(case when cast(null as "text") <= cast(null as mediumblob) then cast(null as longblob) else cast(null as longblob) end as longblob), cast((select xc_node_id from pg_catalog.gs_auditing_policy_privileges limit 1 offset 2) as int4)) as longblob), cast(cast(nullif((select xc_node_id from pg_catalog.pg_pltemplate limit 1 offset 6) , (select pg_catalog.bit_and(pid) from pg_catalog.pg_stat_subscription) ) as int4) as int4))) limit 1;

-- test for empty strings
select json_object('City', '', 'Population', 139693);
select json_object('', 'Cairns', 'Population', 139693);

-- test for empty lists
select json_object();
select json_object();

-- test for null values
select json_object('City', 'Cairns', 'Population', NULL);
select json_object('City', 'Cairns', 139693, NULL);

-- test for null keys
select json_object(NULL, 'nothing');
select json_object(NULL, NULL);

-- test for float number
select json_object('City', '', 'Population', 139693.123);
select json_object('City', '', 'Population', 0.123);
select json_object('City', '', 'Population', 5/7);

-- test for enormous number
select json_object('a', 132454676878465464652222238888856744654563565446554798798451344787945.3153);

-- test for bool values
select json_object('Red', true, 'Blue', false);
select json_object(true, 1, false, 0);
select json_object(true, false, false, true);

-- test for lots of pairs
select json_object('Region', 'Asia', 'color', 'red', 'City', '', 'Population', 139693.123, 'name', 'Tim', 'age', 20, 'friend', json_object('name', 'Jim', 'age', 20), 'hobby', json_build_array('games', 'sports'), 'name', 'Tim', 'age', 20, 'friend', json_object('name', 'Jim', 'age', 20), 'hobby', json_build_array('games', 'sports'));

-- test for negative input
select json_object('Number', -123);
select json_object('Number', -123.12134);
select json_object('Number', -1234564564868789790867567.456);

-- test for numeric input starting with '+'
select json_object('Number', +123);
select json_object('Number', ++123);

-- test for type check
select json_object('Type', abc);
select json_object('Type', t);

-- wrong number of param
select json_object('key');
select json_object('key', 'abc', 'value');

-- test for trim
select json_object( 'City',            'Cairns',          'Population'          , NULL );

-- test for inserting object into tables
create table tab_json1(data json);
insert into tab_json1 values (json_object('names','David','address','Beijing','Tel',1324053333,'names','Mike'));
select * from tab_json1;

-- use the data in tables
create table info1(name varchar(30),address varchar(20),tel int);
insert into info1 values ('Jack','Zhongguo',1323394);
insert into info1 values ('Tobbo','Meiguo',132333394);
insert into info1 values ('Hnana','Riben',3403234);
insert into tab_json1 select json_object('name',name,'address',address,'tel',tel) from info1;
select * from tab_json1;
create table tab_json2(data json, my_text text);
insert into tab_json2 values('{"abc": 1234}', 'MyText');
select json_object(data, my_text) from tab_json2;

-- test for setting b_compatibility_mode back to false
set dolphin.b_compatibility_mode = 0;
select json_object('{a,1,b,2,3,NULL,"d e f","a b c"}');
select json_object('{a,b,"a b c"}', '{a,1,1}');

drop schema test_json_object cascade;
reset current_schema;