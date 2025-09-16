CREATE TABLE __$spq$__.pg_dist_local_group(
    groupid int NOT NULL PRIMARY KEY);

-- insert the default value for being the coordinator node
INSERT INTO __$spq$__.pg_dist_local_group VALUES (0);
ALTER TABLE __$spq$__.pg_dist_local_group SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.pg_dist_local_group TO public;
#include "../../../udfs/spq_dist_local_group_cache_invalidate/2.0.0.sql"
CREATE TRIGGER dist_local_group_cache_invalidate
    AFTER UPDATE
    ON pg_catalog.pg_dist_local_group
    FOR EACH ROW EXECUTE PROCEDURE spq_dist_local_group_cache_invalidate();
