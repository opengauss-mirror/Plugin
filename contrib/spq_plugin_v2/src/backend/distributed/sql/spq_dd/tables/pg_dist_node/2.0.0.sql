-- add pg_dist_node
CREATE SEQUENCE pg_catalog.pg_dist_groupid_seq
 MINVALUE 1
 MAXVALUE 4294967296;

-- reserve UINT32_MAX (4294967295) for a special node
CREATE SEQUENCE pg_catalog.pg_dist_node_nodeid_seq
 MINVALUE 1
 MAXVALUE 4294967294;
CREATE TABLE __$spq$__.pg_dist_node(
 nodeid int NOT NULL DEFAULT nextval('pg_dist_node_nodeid_seq') PRIMARY KEY,
 groupid int NOT NULL DEFAULT nextval('pg_dist_groupid_seq'),
 nodename text NOT NULL,
 nodeport int NOT NULL DEFAULT 5432,
 noderack text NOT NULL DEFAULT 'default',
 hasmetadata bool NOT NULL DEFAULT false,
 isactive bool NOT NULL DEFAULT true,
 noderole noderole NOT NULL DEFAULT 'primary',
 nodecluster name NOT NULL DEFAULT 'default',
 metadatasynced BOOLEAN DEFAULT FALSE,
 shouldhaveshards bool NOT NULL DEFAULT true,
 UNIQUE (nodename, nodeport)
);

ALTER TABLE __$spq$__.pg_dist_node ADD CONSTRAINT pg_dist_node_groupid_unique UNIQUE (groupid);
ALTER TABLE __$spq$__.pg_dist_node SET SCHEMA pg_catalog;

#include "../../../udfs/spq_dist_node_cache_invalidate/2.0.0.sql"
CREATE TRIGGER dist_node_cache_invalidate
    AFTER INSERT OR UPDATE OR DELETE
    ON pg_catalog.pg_dist_node
    FOR EACH ROW EXECUTE PROCEDURE spq_dist_node_cache_invalidate();
GRANT SELECT ON pg_catalog.pg_dist_node TO public;
GRANT SELECT ON pg_catalog.pg_dist_groupid_seq TO public;
GRANT SELECT ON pg_catalog.pg_dist_node_nodeid_seq TO public;

ALTER TABLE pg_dist_node
  ADD CONSTRAINT primaries_are_only_allowed_in_the_default_cluster
  CHECK (NOT (nodecluster <> 'default' AND noderole = 'primary'));

COMMENT ON COLUMN pg_dist_node.metadatasynced IS
    'indicates whether the node has the most recent metadata';
COMMENT ON COLUMN pg_catalog.pg_dist_node.shouldhaveshards IS
    'indicates whether the node is eligible to contain data from distributed tables';

CREATE OR REPLACE FUNCTION __$spq_internal$__.pg_dist_node_trigger_func()
RETURNS TRIGGER AS $$
  BEGIN
    -- AddNodeMetadata also takes out a ShareRowExclusiveLock
    LOCK TABLE pg_dist_node IN SHARE ROW EXCLUSIVE MODE;
    IF (TG_OP = 'INSERT') THEN
      IF NEW.noderole = 'primary'
          AND EXISTS (SELECT 1 FROM pg_dist_node WHERE groupid = NEW.groupid AND
                                                       noderole = 'primary' AND
                                                       nodeid <> NEW.nodeid) THEN
        RAISE EXCEPTION 'there cannot be two primary nodes in a group';
      END IF;
      RETURN NEW;
    ELSIF (TG_OP = 'UPDATE') THEN
      IF NEW.noderole = 'primary'
           AND EXISTS (SELECT 1 FROM pg_dist_node WHERE groupid = NEW.groupid AND
                                                        noderole = 'primary' AND
                                                        nodeid <> NEW.nodeid) THEN
         RAISE EXCEPTION 'there cannot be two primary nodes in a group';
      END IF;
      RETURN NEW;
    END IF;
  END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER pg_dist_node_trigger
  BEFORE INSERT OR UPDATE ON pg_dist_node
  FOR EACH ROW EXECUTE PROCEDURE __$spq_internal$__.pg_dist_node_trigger_func();
