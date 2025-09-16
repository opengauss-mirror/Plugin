CREATE FUNCTION pg_catalog.master_run_on_worker(worker_name text[],
                 port integer[],
                 command text[],
                 parallel boolean,
                 OUT node_name text,
                 OUT node_port integer,
                 OUT success boolean,
                 OUT result text )
 RETURNS SETOF record
 LANGUAGE C STABLE STRICT
 AS 'MODULE_PATHNAME', $$master_run_on_worker$$;
