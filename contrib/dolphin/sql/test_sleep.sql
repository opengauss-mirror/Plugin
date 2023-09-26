create schema test_sleep;

set current_schema to 'test_sleep';

select sleep(1);

select sleep(2);

create table sleep_test(a text);
-- 非严格模式，参数不合法，报warning，返回0
set dolphin.sql_mode = 'sql_mode_full_group';

select sleep(-1);
select sleep(null);
select sleep('a');
insert into sleep_test values(sleep(-1));
insert into sleep_test values(sleep(null));

-- 严格模式，参数不合法，报error
set dolphin.sql_mode = 'sql_mode_strict';

select sleep(-1);
select sleep(null);
select sleep('a');
insert into sleep_test values(sleep(-1));
insert into sleep_test values(sleep(null));
insert ignore into sleep_test values(sleep(-1));
insert ignore into sleep_test values(sleep(null));

set dolphin.sql_mode = '';
select sleep('0000-00-00'::date);

drop schema test_sleep cascade;
reset current_schema;
