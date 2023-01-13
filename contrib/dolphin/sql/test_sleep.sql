create schema test_sleep;

set current_schema to 'test_sleep';

select sleep(1);

select sleep(2);
drop schema test_sleep cascade;
reset current_schema;