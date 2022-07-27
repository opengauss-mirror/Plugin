drop database if exists test_kill;
create database test_kill dbcompatibility 'b';
\c test_kill
kill query (select sessionid from pg_stat_activity where application_name = 'JobScheduler');
kill connection (select sessionid from pg_stat_activity where application_name = 'JobScheduler');
kill (select sessionid from pg_stat_activity where application_name = 'PercentileJob');
\c postgres
drop database if exists test_kill;