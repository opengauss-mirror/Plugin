create schema test_sleep;

set current_schema to 'test_sleep';

select sleep(1);

select sleep(2);

-- 非严格模式，参数不合法，报warning，返回0
set dolphin.sql_mode = 'sql_mode_full_group';

select sleep(-1);
select sleep(null);
select sleep('a');

-- 严格模式，参数不合法，报error
set dolphin.sql_mode = 'sql_mode_strict';

select sleep(-1);
select sleep(null);
select sleep('a');

drop schema test_sleep cascade;
reset current_schema;