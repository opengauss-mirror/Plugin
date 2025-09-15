-- add view pg_dist_shard_placement
CREATE OR REPLACE VIEW __$spq$__.pg_dist_shard_placement AS
  SELECT shardid, shardstate, shardlength, nodename, nodeport, placementid
  FROM pg_dist_placement placement INNER JOIN pg_dist_node node ON (
    placement.groupid = node.groupid AND node.noderole = 'primary'
  );

ALTER VIEW __$spq$__.pg_dist_shard_placement SET SCHEMA pg_catalog;
GRANT SELECT ON pg_catalog.pg_dist_shard_placement TO public;
GRANT SELECT ON pg_catalog.pg_dist_placement_placementid_seq TO public;
-- add some triggers which make it look like pg_dist_shard_placement is still a table
ALTER VIEW pg_catalog.pg_dist_shard_placement
  ALTER placementid SET DEFAULT nextval('pg_dist_placement_placementid_seq');

CREATE OR REPLACE FUNCTION __$spq_internal$__.find_groupid_for_node(text, int)
RETURNS int AS $$
DECLARE
  groupid int := (SELECT groupid FROM pg_dist_node WHERE nodename = $1 AND nodeport = $2);
BEGIN
  IF groupid IS NULL THEN
    RAISE EXCEPTION 'There is no node at "%:%"', $1, $2;
  ELSE
    RETURN groupid;
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION __$spq_internal$__.pg_dist_shard_placement_trigger_func()
RETURNS TRIGGER AS $$
  BEGIN
    IF (TG_OP = 'DELETE') THEN
      DELETE FROM pg_dist_placement WHERE placementid = OLD.placementid;
      RETURN OLD;
    ELSIF (TG_OP = 'UPDATE') THEN
      UPDATE pg_dist_placement
        SET shardid = NEW.shardid, shardstate = NEW.shardstate,
            shardlength = NEW.shardlength, placementid = NEW.placementid,
            groupid = __$spq_internal$__.find_groupid_for_node(NEW.nodename, NEW.nodeport)
        WHERE placementid = OLD.placementid;
      RETURN NEW;
    ELSIF (TG_OP = 'INSERT') THEN
      INSERT INTO pg_dist_placement
        (placementid, shardid, shardstate, shardlength, groupid)
      VALUES (NEW.placementid, NEW.shardid, NEW.shardstate, NEW.shardlength,
        __$spq_internal$__.find_groupid_for_node(NEW.nodename, NEW.nodeport));
      RETURN NEW;
    END IF;
  END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER pg_dist_shard_placement_trigger
  INSTEAD OF INSERT OR UPDATE OR DELETE ON pg_dist_shard_placement
  FOR EACH ROW EXECUTE PROCEDURE __$spq_internal$__.pg_dist_shard_placement_trigger_func();
