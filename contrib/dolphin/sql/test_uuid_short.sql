create schema test_uuid_short;
set current_schema to 'test_uuid_short';

select uuid_short();

select uuid_short();

drop schema test_uuid_short cascade;
reset current_schema;