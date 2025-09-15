CREATE TABLE pg_catalog.pg_dist_background_job (
    job_id bigserial NOT NULL,
    state pg_catalog.citus_job_status DEFAULT 'scheduled' NOT NULL,
    job_type name NOT NULL,
    description text NOT NULL,
    started_at timestamptz,
    finished_at timestamptz,

    CONSTRAINT pg_dist_background_job_pkey PRIMARY KEY (job_id)
);
GRANT SELECT ON pg_catalog.pg_dist_background_job TO PUBLIC;
GRANT SELECT ON pg_catalog.pg_dist_background_job_job_id_seq TO PUBLIC;
