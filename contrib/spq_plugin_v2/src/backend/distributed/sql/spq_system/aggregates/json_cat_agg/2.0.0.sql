#include "../../../udfs/spq_json_concatenate/2.0.0.sql"
#include "../../../udfs/spq_json_concatenate_final/2.0.0.sql"

CREATE AGGREGATE pg_catalog.json_cat_agg(json) (
    SFUNC = spq_json_concatenate,
    FINALFUNC = spq_json_concatenate_final,
    STYPE = json,
    INITCOND = 'null'
);
COMMENT ON AGGREGATE pg_catalog.json_cat_agg(json)
    IS 'concatenate input jsons into a single json';
