CREATE TYPE __$spq$__.citus_job_status AS ENUM ('scheduled', 'running', 'finished', 'cancelling', 'cancelled', 'failing', 'failed');
ALTER TYPE __$spq$__.citus_job_status SET SCHEMA pg_catalog;
