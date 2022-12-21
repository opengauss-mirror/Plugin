create schema test_kill;
set current_schema to 'test_kill';
kill query (select sessionid from pg_stat_activity where application_name = 'JobScheduler');
kill connection (select sessionid from pg_stat_activity where application_name = 'JobScheduler');
kill (select sessionid from pg_stat_activity where application_name = 'PercentileJob');
drop schema test_kill cascade;
reset current_schema;