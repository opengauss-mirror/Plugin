CREATE TYPE __$spq$__.citus_task_status AS ENUM ('blocked', 'runnable', 'running', 'done', 'cancelling', 'error', 'unscheduled', 'cancelled');
ALTER TYPE __$spq$__.citus_task_status SET SCHEMA pg_catalog;
