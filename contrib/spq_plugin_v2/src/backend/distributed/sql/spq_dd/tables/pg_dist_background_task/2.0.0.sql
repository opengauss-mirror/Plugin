CREATE TABLE pg_catalog.pg_dist_background_task(
    job_id bigint NOT NULL REFERENCES pg_catalog.pg_dist_background_job(job_id),
    task_id bigserial NOT NULL,
    owner oid NOT NULL,
    pid bigint,
    status pg_catalog.citus_task_status default 'runnable' NOT NULL,
    command text NOT NULL,
    retry_count integer,
    not_before timestamptz, -- can be null to indicate no delay for start of the task, will be set on failure to delay retries
    message text NOT NULL DEFAULT '',
    nodes_involved int[] DEFAULT NULL,
    CONSTRAINT pg_dist_background_task_pkey PRIMARY KEY (task_id),
    CONSTRAINT pg_dist_background_task_job_id_task_id UNIQUE (job_id, task_id) -- required for FK's to enforce tasks only reference other tasks within the same job
);
CREATE INDEX pg_dist_background_task_status_task_id_index ON pg_catalog.pg_dist_background_task USING btree(status, task_id);
GRANT SELECT ON pg_catalog.pg_dist_background_task TO PUBLIC;
GRANT SELECT ON pg_catalog.pg_dist_background_task_task_id_seq TO PUBLIC;
