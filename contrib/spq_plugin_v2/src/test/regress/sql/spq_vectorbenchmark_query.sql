CREATE TABLE IF NOT EXISTS public.pg_vector_collection(id BIGINT PRIMARY KEY, embedding vector(4));
ALTER TABLE public.pg_vector_collection ALTER COLUMN embedding SET STORAGE PLAIN;
ALTER TABLE public.pg_vector_collection SET (parallel_workers = 32);
SELECT create_distributed_table('public.pg_vector_collection', 'id', shard_count:=4);

SELECT COUNT(*) FROM pg_dist_shard;

BEGIN;
INSERT INTO public.pg_vector_collection VALUES(1,'[1, 1, 1, 1]');
INSERT INTO public.pg_vector_collection VALUES(2,'[2, 2, 2, 2]');
INSERT INTO public.pg_vector_collection VALUES(3,'[3, 3, 3, 3]');
INSERT INTO public.pg_vector_collection VALUES(4,'[4, 4, 4, 4]');
INSERT INTO public.pg_vector_collection VALUES(5,'[5, 5, 5, 5]');
INSERT INTO public.pg_vector_collection VALUES(6,'[6, 6, 6, 6]');
INSERT INTO public.pg_vector_collection VALUES(7,'[7, 7, 7, 7]');
INSERT INTO public.pg_vector_collection VALUES(8,'[8, 8, 8, 8]');
INSERT INTO public.pg_vector_collection VALUES(9,'[9, 9, 9, 9]');
INSERT INTO public.pg_vector_collection VALUES(10,'[10, 10, 10, 10]');
COMMIT;

BEGIN;
COPY public.pg_vector_collection FROM STDIN WITH delimiter '|';
11|[11,11,11,11]
12|[12,12,12,12]
13|[13,13,13,13]
14|[14,14,14,14]
15|[15,15,15,15]
16|[16,16,16,16]
17|[17,17,17,17]
18|[18,18,18,18]
19|[19,19,19,19]
20|[20,20,20,20]
\.
COMMIT;

CREATE OR REPLACE FUNCTION generate_pg_vector_collection(count INTEGER)
    RETURNS void
    LANGUAGE plpgsql
AS $$
DECLARE
   i INTEGER;
BEGIN
    FOR i IN 21 .. count LOOP
        INSERT INTO public.pg_vector_collection(id, embedding) VALUES(i, ('[' || i || ',' || i || ',' || i || ',' || i || ']')::vector);
    END LOOP;
END;
$$;

SELECT generate_pg_vector_collection(9980);
DROP FUNCTION generate_pg_vector_collection;

-- we use ip distance for testing, its distance has better differentiation.
CREATE INDEX IF NOT EXISTS pg_vector_collection_embeding_index ON public.pg_vector_collection USING "hnsw" (embedding "vector_ip_ops") WITH ("m" = "16", "ef_construction" ="200");

SET spq.explain_all_tasks TO ON;
EXPLAIN SELECT * FROM public.pg_vector_collection ORDER BY id limit 2;
EXPLAIN ANALYZE SELECT * FROM public.pg_vector_collection ORDER BY id limit 2;
EXPLAIN SELECT id FROM public.pg_vector_collection ORDER BY embedding <#> '[1,2,3,4]' limit 2;
EXPLAIN ANALYZE SELECT id FROM public.pg_vector_collection ORDER BY embedding <#> '[1,2,3,4]' limit 2;
SET spq.explain_all_tasks TO DEFAULT;

SELECT * FROM public.pg_vector_collection ORDER BY id limit 2;
SELECT id FROM public.pg_vector_collection ORDER BY embedding <#> '[1,2,3,4]' limit 2;

SELECT COUNT(*) FROM spq_tables;
DROP TABLE public.pg_vector_collection;
SELECT COUNT(*) FROM spq_tables;
\c - - - :worker_1_port
SELECT
    table_schema AS "Schema",
    table_name AS "Name",
    table_type AS "Type"
FROM
    information_schema.tables
WHERE
    table_schema = 'public'
ORDER BY
    table_schema, table_name;
\c - - - :worker_2_port
SELECT
    table_schema AS "Schema",
    table_name AS "Name",
    table_type AS "Type"
FROM
    information_schema.tables
WHERE
    table_schema = 'public'
ORDER BY
    table_schema, table_name;
\c - - - :master_port
