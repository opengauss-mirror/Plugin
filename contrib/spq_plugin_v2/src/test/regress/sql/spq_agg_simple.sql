--------------------- unsupported agg vs citus ----------------------------
SELECT jsonb_cat_agg(i) FROM (VALUES ('[1,{"a":2}]'::jsonb), ('[null]'::jsonb), (NULL), ('["3",5,4]'::jsonb)) AS t(i);
CREATE TABLE nulltable(id int);
INSERT INTO nulltable values (0);
SELECT pg_catalog.worker_partial_agg('string_agg(text,text)'::regprocedure, id) FROM nulltable;
SELECT pg_catalog.coord_combine_agg('sum(float8)'::regprocedure, id::text::cstring, null::text) FROM nulltable;
DROP TABLE nulltable;

------------------------ array_cat_agg -----------------------------------
SELECT array_cat_agg(i) FROM (VALUES (ARRAY[1,2]), (NULL), (ARRAY[3,4])) AS t(i);

---------------------- json_cat_agg -------------------------------------
SELECT json_cat_agg(i) FROM (VALUES ('[1,{"a":2}]'::json), ('[null]'::json), (NULL), ('["3",5,4]'::json)) AS t(i);
