CREATE TABLE pg_catalog.pg_dist_background_task_depend(
   job_id bigint NOT NULL REFERENCES pg_catalog.pg_dist_background_job(job_id) ON DELETE CASCADE,
   task_id bigint NOT NULL,
   depends_on bigint NOT NULL,

   PRIMARY KEY (job_id, task_id, depends_on),
   FOREIGN KEY (job_id, task_id) REFERENCES pg_catalog.pg_dist_background_task (job_id, task_id) ON DELETE CASCADE,
   FOREIGN KEY (job_id, depends_on) REFERENCES pg_catalog.pg_dist_background_task (job_id, task_id) ON DELETE CASCADE
);

CREATE INDEX pg_dist_background_task_depend_task_id ON pg_catalog.pg_dist_background_task_depend USING btree(job_id, task_id);
CREATE INDEX pg_dist_background_task_depend_depends_on ON pg_catalog.pg_dist_background_task_depend USING btree(job_id, depends_on);
GRANT SELECT ON pg_catalog.pg_dist_background_task_depend TO PUBLIC;
