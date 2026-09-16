/*
 * Complete openGauss AGE upgrade script: 1.0.0 -> 1.0.1
 *
 * Supports both 1.0.0 starting points:
 *   A. the historical release (314 functions, no vertex/edge types,
 *      no vector/JSONB interoperability), and
 *   B. the 1.0.0 script generated from this source tree (only missing
 *      the 1.0.1 incremental objects).
 * Every statement is idempotent: functions use CREATE OR REPLACE, types,
 * operators and aggregates are guarded by existence checks, and casts and
 * triggers are dropped with IF EXISTS before being recreated.
 *
 * Run ALTER EXTENSION age UPDATE TO '1.0.1' immediately after installing
 * the 1.0.1 binaries: Cypher queries resolve catalog helpers by name, so
 * they are unavailable inside the window between the binary swap and this
 * script completing.
 */

-- Drop functions whose signatures changed. On starting point B these old
-- signatures do not exist and IF EXISTS turns the drops into no-ops.
DROP FUNCTION IF EXISTS ag_catalog._agtype_build_edge(graphid, graphid, graphid, cstring, agtype);
DROP FUNCTION IF EXISTS ag_catalog._agtype_build_vertex(graphid, cstring, agtype);
DROP FUNCTION IF EXISTS ag_catalog.age_unnest(agtype, boolean);
DROP FUNCTION IF EXISTS ag_catalog.agtype_volatile_wrapper(agtype);
DROP FUNCTION IF EXISTS ag_catalog.create_elabel(name, name);
DROP FUNCTION IF EXISTS ag_catalog.create_vlabel(name, name);
DROP FUNCTION IF EXISTS ag_catalog.load_edges_from_file(name, name, text);
DROP FUNCTION IF EXISTS ag_catalog.load_labels_from_file(name, name, text, bool);
-- The return type changed from cstring to agtype; OR REPLACE cannot do that.
DROP FUNCTION IF EXISTS ag_catalog._label_name(oid, graphid);


DROP TRIGGER IF EXISTS trigger_ag_graph_update ON ag_graph;

create trigger trigger_ag_graph_update
BEFORE UPDATE ON ag_graph
FOR EACH ROW
EXECUTE PROCEDURE deny_update_ag_meta_tabel();

DROP TRIGGER IF EXISTS trigger_ag_graph_delete ON ag_graph;

create trigger trigger_ag_graph_delete
BEFORE DELETE ON ag_graph
FOR EACH ROW
EXECUTE PROCEDURE deny_update_ag_meta_tabel();

DROP TRIGGER IF EXISTS trigger_ag_label_update ON ag_label;

create trigger trigger_ag_label_update
BEFORE UPDATE ON ag_label
FOR EACH ROW
EXECUTE PROCEDURE deny_update_ag_meta_tabel();

DROP TRIGGER IF EXISTS trigger_ag_label_delete ON ag_label;

create trigger trigger_ag_label_delete
BEFORE DELETE ON ag_label
FOR EACH ROW
EXECUTE PROCEDURE deny_update_ag_meta_tabel();

CREATE OR REPLACE FUNCTION ag_catalog.age_rebind_graph_namespace_on_restore()
RETURNS trigger
LANGUAGE plpgsql
SET search_path = pg_catalog, ag_catalog
AS $function$
DECLARE
    target_namespace_oid oid;
BEGIN
    SELECT n.oid
    INTO target_namespace_oid
    FROM pg_catalog.pg_namespace n
    WHERE n.nspname = NEW.name;

    IF target_namespace_oid IS NULL THEN
        RAISE EXCEPTION 'cannot restore AGE graph "%": target schema does not exist',
                        NEW.name;
    END IF;

    NEW.namespaceoid := target_namespace_oid;
    RETURN NEW;
END;
$function$;

DROP TRIGGER IF EXISTS trigger_ag_graph_restore ON ag_catalog.ag_graph;

CREATE TRIGGER trigger_ag_graph_restore
BEFORE INSERT ON ag_catalog.ag_graph
FOR EACH ROW
EXECUTE PROCEDURE ag_catalog.age_rebind_graph_namespace_on_restore();

ALTER TABLE ag_catalog.ag_graph
ENABLE ALWAYS TRIGGER trigger_ag_graph_restore;

CREATE OR REPLACE FUNCTION ag_catalog.age_rebind_label_graph_on_restore()
RETURNS trigger
LANGUAGE plpgsql
SET search_path = pg_catalog, ag_catalog
AS $function$
DECLARE
    target_graph_oid oid;
BEGIN
    SELECT g.oid
    INTO target_graph_oid
    FROM pg_catalog.pg_class relation
    JOIN pg_catalog.pg_namespace namespace
      ON namespace.oid = relation.relnamespace
    JOIN ag_catalog.ag_graph g
      ON g.name = namespace.nspname
    WHERE relation.oid = NEW.relation;

    IF target_graph_oid IS NULL THEN
        RAISE EXCEPTION 'cannot restore AGE label "%": target graph does not exist',
                        NEW.name;
    END IF;

    NEW.graph := target_graph_oid;
    RETURN NEW;
END;
$function$;

DROP TRIGGER IF EXISTS trigger_ag_label_restore ON ag_catalog.ag_label;

CREATE TRIGGER trigger_ag_label_restore
BEFORE INSERT ON ag_catalog.ag_label
FOR EACH ROW
EXECUTE PROCEDURE ag_catalog.age_rebind_label_graph_on_restore();

ALTER TABLE ag_catalog.ag_label
ENABLE ALWAYS TRIGGER trigger_ag_label_restore;

SELECT pg_catalog.pg_extension_config_dump('ag_catalog.ag_graph', 'WHERE true');

SELECT pg_catalog.pg_extension_config_dump('ag_catalog.ag_label', 'WHERE true');

CREATE OR REPLACE FUNCTION ag_catalog._age_invalidate_graph_cache()
RETURNS void
LANGUAGE c
VOLATILE
AS 'MODULE_PATHNAME';

REVOKE ALL ON FUNCTION ag_catalog._age_invalidate_graph_cache() FROM PUBLIC;

CREATE OR REPLACE FUNCTION ag_catalog.age_invalidate_graph_cache()
RETURNS trigger
LANGUAGE c
VOLATILE
AS 'MODULE_PATHNAME';

-- Install cache invalidation triggers on label tables that predate 1.0.1.
-- New label tables receive the trigger from the label creation path.
DO $upgrade$
DECLARE
    label_record record;
BEGIN
    FOR label_record IN
        SELECT namespace_object.nspname AS schema_name,
               relation_object.relname AS table_name,
               label_object.relation AS label_relation_oid
        FROM ag_catalog.ag_label label_object
        JOIN pg_catalog.pg_class relation_object
          ON relation_object.oid = label_object.relation
        JOIN pg_catalog.pg_namespace namespace_object
          ON namespace_object.oid = relation_object.relnamespace
        WHERE label_object.name NOT IN ('_ag_label_vertex', '_ag_label_edge')
    LOOP
        IF NOT EXISTS (
            SELECT 1
            FROM pg_catalog.pg_trigger trigger_object
            WHERE trigger_object.tgrelid = label_record.label_relation_oid
              AND trigger_object.tgname = '_age_cache_invalidate'
        ) THEN
            EXECUTE pg_catalog.format(
                'CREATE TRIGGER _age_cache_invalidate '
                'AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE '
                'ON %I.%I '
                'FOR EACH STATEMENT '
                'EXECUTE PROCEDURE ag_catalog.age_invalidate_graph_cache()',
                label_record.schema_name,
                label_record.table_name
            );
        END IF;
    END LOOP;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.age_prepare_pg_upgrade()
RETURNS void
LANGUAGE plpgsql
SET search_path = pg_catalog, ag_catalog
AS $function$
DECLARE
    graph_count bigint;
BEGIN
    PERFORM pg_catalog.pg_advisory_xact_lock(
        pg_catalog.hashtext('age_pg_upgrade_state'));

    IF EXISTS (
        SELECT 1
        FROM information_schema.tables
        WHERE table_schema = 'ag_catalog'
          AND table_name = '_age_pg_upgrade_backup'
    ) THEN
        RAISE EXCEPTION 'AGE is already prepared for pg_upgrade'
            USING HINT = 'Run age_finish_pg_upgrade() after the server upgrade, or age_revert_pg_upgrade_changes() before it.';
    END IF;

    LOCK TABLE ag_catalog.ag_graph IN SHARE MODE;

    CREATE TABLE ag_catalog._age_pg_upgrade_backup AS
    SELECT g.oid AS old_graph_oid,
           g.name AS graph_name,
           n.nspname AS namespace_name
    FROM ag_catalog.ag_graph g
    JOIN pg_catalog.pg_namespace n ON n.oid = g.namespaceoid;

    SELECT pg_catalog.count(*)
    INTO graph_count
    FROM ag_catalog._age_pg_upgrade_backup;

    IF graph_count <> (SELECT pg_catalog.count(*) FROM ag_catalog.ag_graph) THEN
        RAISE EXCEPTION 'could not resolve every AGE graph namespace during pg_upgrade preparation';
    END IF;

    RAISE NOTICE 'AGE pg_upgrade preparation recorded % graph(s).', graph_count;
    RAISE NOTICE 'After the server upgrade, run: SELECT ag_catalog.age_finish_pg_upgrade();';
END;
$function$;

COMMENT ON FUNCTION ag_catalog.age_prepare_pg_upgrade() IS
'Captures stable graph and namespace names before an openGauss major-version upgrade.';

REVOKE ALL ON FUNCTION ag_catalog.age_prepare_pg_upgrade() FROM PUBLIC;

CREATE OR REPLACE FUNCTION ag_catalog.age_finish_pg_upgrade()
RETURNS void
LANGUAGE plpgsql
SET search_path = pg_catalog, ag_catalog
AS $function$
DECLARE
    backup_count bigint;
    mapped_count bigint;
    updated_graphs bigint;
BEGIN
    PERFORM pg_catalog.pg_advisory_xact_lock(
        pg_catalog.hashtext('age_pg_upgrade_state'));

    IF NOT EXISTS (
        SELECT 1
        FROM information_schema.tables
        WHERE table_schema = 'ag_catalog'
          AND table_name = '_age_pg_upgrade_backup'
    ) THEN
        RAISE EXCEPTION 'AGE pg_upgrade backup is missing'
            USING HINT = 'Run age_prepare_pg_upgrade() before the server upgrade.';
    END IF;

    LOCK TABLE ag_catalog.ag_graph IN SHARE ROW EXCLUSIVE MODE;

    SELECT pg_catalog.count(*)
    INTO backup_count
    FROM ag_catalog._age_pg_upgrade_backup;

    SELECT pg_catalog.count(*)
    INTO mapped_count
    FROM ag_catalog._age_pg_upgrade_backup b
    JOIN ag_catalog.ag_graph g
      ON g.oid = b.old_graph_oid
     AND g.name = b.graph_name
    JOIN pg_catalog.pg_namespace n
      ON n.nspname = b.namespace_name;

    IF mapped_count <> backup_count
       OR mapped_count <> (SELECT pg_catalog.count(*) FROM ag_catalog.ag_graph) THEN
        RAISE EXCEPTION 'AGE graph catalog changed after pg_upgrade preparation: mapped % of % backup row(s)',
                        mapped_count, backup_count
            USING HINT = 'Restore the prepared database or repair graph/schema names before retrying.';
    END IF;

    ALTER TABLE ag_catalog.ag_graph
        DISABLE TRIGGER trigger_ag_graph_update;

    UPDATE ag_catalog.ag_graph g
    SET namespaceoid = n.oid
    FROM ag_catalog._age_pg_upgrade_backup b
    JOIN pg_catalog.pg_namespace n ON n.nspname = b.namespace_name
    WHERE g.oid = b.old_graph_oid
      AND g.name = b.graph_name
      AND g.namespaceoid <> n.oid;

    GET DIAGNOSTICS updated_graphs = ROW_COUNT;

    ALTER TABLE ag_catalog.ag_graph
        ENABLE TRIGGER trigger_ag_graph_update;

    -- Refresh the current backend immediately and broadcast an ag_graph
    -- relcache invalidation so other connected backends discard stale AGE
    -- graph entries without changing graph schemas or their ownership.
    PERFORM ag_catalog._age_invalidate_graph_cache();

    DROP TABLE ag_catalog._age_pg_upgrade_backup;

    RAISE NOTICE 'AGE pg_upgrade finish repaired % namespace reference(s).',
                 updated_graphs;
END;
$function$;

COMMENT ON FUNCTION ag_catalog.age_finish_pg_upgrade() IS
'Remaps AGE namespace OIDs after an openGauss major-version upgrade and validates graph identity.';

REVOKE ALL ON FUNCTION ag_catalog.age_finish_pg_upgrade() FROM PUBLIC;

CREATE OR REPLACE FUNCTION ag_catalog.age_revert_pg_upgrade_changes()
RETURNS void
LANGUAGE plpgsql
SET search_path = pg_catalog, ag_catalog
AS $function$
BEGIN
    PERFORM pg_catalog.pg_advisory_xact_lock(
        pg_catalog.hashtext('age_pg_upgrade_state'));

    IF NOT EXISTS (
        SELECT 1
        FROM information_schema.tables
        WHERE table_schema = 'ag_catalog'
          AND table_name = '_age_pg_upgrade_backup'
    ) THEN
        RAISE EXCEPTION 'AGE pg_upgrade preparation is not active'
            USING HINT = 'Run age_prepare_pg_upgrade() before requesting a revert.';
    END IF;

    DROP TABLE ag_catalog._age_pg_upgrade_backup;
    RAISE NOTICE 'AGE pg_upgrade preparation has been reverted.';
END;
$function$;

COMMENT ON FUNCTION ag_catalog.age_revert_pg_upgrade_changes() IS
'Cancels AGE pg_upgrade preparation before the server upgrade starts.';

REVOKE ALL ON FUNCTION ag_catalog.age_revert_pg_upgrade_changes() FROM PUBLIC;

CREATE OR REPLACE FUNCTION ag_catalog.age_pg_upgrade_status()
RETURNS TABLE (
    status text,
    namespace_type text,
    graph_count bigint,
    backup_exists boolean,
    message text
)
LANGUAGE plpgsql
SET search_path = pg_catalog, ag_catalog
AS $function$
DECLARE
    ns_type text;
    current_graph_count bigint;
    backup_graph_count bigint;
    mapped_graph_count bigint;
    has_backup boolean;
BEGIN
    SELECT data_type
    INTO ns_type
    FROM information_schema.columns
    WHERE table_schema = 'ag_catalog'
      AND table_name = 'ag_graph'
      AND column_name = 'namespaceoid';

    SELECT pg_catalog.count(*)
    INTO current_graph_count
    FROM ag_catalog.ag_graph;

    SELECT EXISTS (
        SELECT 1
        FROM information_schema.tables
        WHERE table_schema = 'ag_catalog'
          AND table_name = '_age_pg_upgrade_backup'
    )
    INTO has_backup;

    IF NOT has_backup THEN
        RETURN QUERY SELECT
            'NORMAL'::text,
            ns_type,
            current_graph_count,
            false,
            'Run SELECT ag_catalog.age_prepare_pg_upgrade(); before the server upgrade.'::text;
        RETURN;
    END IF;

    SELECT pg_catalog.count(*)
    INTO backup_graph_count
    FROM ag_catalog._age_pg_upgrade_backup;

    SELECT pg_catalog.count(*)
    INTO mapped_graph_count
    FROM ag_catalog._age_pg_upgrade_backup b
    JOIN ag_catalog.ag_graph g
      ON g.oid = b.old_graph_oid
     AND g.name = b.graph_name
    JOIN pg_catalog.pg_namespace n
      ON n.nspname = b.namespace_name;

    IF mapped_graph_count = backup_graph_count
       AND mapped_graph_count = current_graph_count THEN
        RETURN QUERY SELECT
            'PREPARED - AWAITING FINISH'::text,
            ns_type,
            current_graph_count,
            true,
            'After the server upgrade, run SELECT ag_catalog.age_finish_pg_upgrade();'::text;
    ELSE
        RETURN QUERY SELECT
            'WARNING'::text,
            ns_type,
            current_graph_count,
            true,
            'The prepared graph/name mapping no longer matches ag_graph.'::text;
    END IF;
END;
$function$;

COMMENT ON FUNCTION ag_catalog.age_pg_upgrade_status() IS
'Returns the current openGauss AGE pg_upgrade preparation state.';

REVOKE ALL ON FUNCTION ag_catalog.age_pg_upgrade_status() FROM PUBLIC;

CREATE OR REPLACE FUNCTION ag_catalog.create_vlabel(graph_name cstring, label_name cstring)
    RETURNS void
    LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.create_elabel(graph_name cstring, label_name cstring)
    RETURNS void
    LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.load_labels_from_file(graph_name name,
                                                 label_name name,
                                                 file_path text,
                                                 id_field_exists bool default true,
                                                 load_as_agtype bool default false,
                                                 delimiter text default ',')
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.load_edges_from_file(graph_name name,
                                                label_name name,
                                                file_path text,
                                                load_as_agtype bool default false,
                                                delimiter text default ',')
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

REVOKE ALL ON FUNCTION ag_catalog.load_labels_from_file(name, name, text, bool, bool, text) FROM PUBLIC;

REVOKE ALL ON FUNCTION ag_catalog.load_edges_from_file(name, name, text, bool, text) FROM PUBLIC;

REVOKE ALL ON FUNCTION ag_catalog.load_labels_from_file_with_analysefile(name, name, text, bool) FROM PUBLIC;

REVOKE ALL ON FUNCTION ag_catalog.load_edges_from_file_with_analysefile(name, name, text) FROM PUBLIC;

CREATE OR REPLACE FUNCTION ag_catalog.graphid_eq(graphid, graphid)
RETURNS boolean
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.graphid_ne(graphid, graphid)
RETURNS boolean
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.graphid_lt(graphid, graphid)
RETURNS boolean
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.graph_exists(graph_name name)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME', 'age_graph_exists';

CREATE OR REPLACE FUNCTION ag_catalog._label_name(graph_oid oid, graphid)
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_type t
                   JOIN pg_catalog.pg_namespace n ON n.oid = t.typnamespace
                   WHERE t.typname = 'vertex' AND n.nspname = 'ag_catalog') THEN
        EXECUTE $ct$
CREATE TYPE ag_catalog.vertex AS (
  id graphid,
  label agtype,
  properties agtype
)
        $ct$;
    END IF;
END
$upgrade$;

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_type t
                   JOIN pg_catalog.pg_namespace n ON n.oid = t.typnamespace
                   WHERE t.typname = 'edge' AND n.nspname = 'ag_catalog') THEN
        EXECUTE $ct$
CREATE TYPE ag_catalog.edge AS (
  id graphid,
  label agtype,
  end_id graphid,
  start_id graphid,
  properties agtype
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.vertex_to_agtype(vertex)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.edge_to_agtype(edge)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (vertex AS agtype);

CREATE CAST (vertex AS agtype)
WITH FUNCTION ag_catalog.vertex_to_agtype(vertex)
AS IMPLICIT;

DROP CAST IF EXISTS (edge AS agtype);

CREATE CAST (edge AS agtype)
WITH FUNCTION ag_catalog.edge_to_agtype(edge)
AS IMPLICIT;

CREATE OR REPLACE FUNCTION ag_catalog.vertex_to_json(vertex)
RETURNS json
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.edge_to_json(edge)
RETURNS json
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (vertex AS json);

CREATE CAST (vertex AS json)
WITH FUNCTION ag_catalog.vertex_to_json(vertex);

DROP CAST IF EXISTS (edge AS json);

CREATE CAST (edge AS json)
WITH FUNCTION ag_catalog.edge_to_json(edge);

CREATE OR REPLACE FUNCTION ag_catalog.vertex_to_jsonb(vertex)
RETURNS jsonb
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.edge_to_jsonb(edge)
RETURNS jsonb
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (vertex AS jsonb);

CREATE CAST (vertex AS jsonb)
WITH FUNCTION ag_catalog.vertex_to_jsonb(vertex);

DROP CAST IF EXISTS (edge AS jsonb);

CREATE CAST (edge AS jsonb)
WITH FUNCTION ag_catalog.edge_to_jsonb(edge);

CREATE OR REPLACE FUNCTION ag_catalog.vertex_eq(vertex, vertex)
RETURNS boolean
LANGUAGE sql
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS $$ SELECT $1.id = $2.id $$;

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '='
                     AND o.oprleft = 'ag_catalog.vertex'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.vertex'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR = (
  PROCEDURE = ag_catalog.vertex_eq,
  LEFTARG = vertex,
  RIGHTARG = vertex,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.vertex_ne(vertex, vertex)
RETURNS boolean
LANGUAGE sql
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS $$ SELECT $1.id <> $2.id $$;

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '<>'
                     AND o.oprleft = 'ag_catalog.vertex'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.vertex'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR <> (
  PROCEDURE = ag_catalog.vertex_ne,
  LEFTARG = vertex,
  RIGHTARG = vertex,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.edge_eq(edge, edge)
RETURNS boolean
LANGUAGE sql
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS $$ SELECT $1.id = $2.id $$;

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '='
                     AND o.oprleft = 'ag_catalog.edge'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.edge'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR = (
  PROCEDURE = ag_catalog.edge_eq,
  LEFTARG = edge,
  RIGHTARG = edge,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.edge_ne(edge, edge)
RETURNS boolean
LANGUAGE sql
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS $$ SELECT $1.id <> $2.id $$;

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '<>'
                     AND o.oprleft = 'ag_catalog.edge'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.edge'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR <> (
  PROCEDURE = ag_catalog.edge_ne,
  LEFTARG = edge,
  RIGHTARG = edge,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_concat(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '||'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR || (
  PROCEDURE = ag_catalog.agtype_concat,
  LEFTARG = agtype,
  RIGHTARG = agtype
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog._agtype_build_vertex(graphid, agtype, agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog._agtype_build_edge(graphid, graphid, graphid, agtype, agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog._get_vertex_by_graphid(text, graphid)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog._ag_enforce_edge_uniqueness2(graphid, graphid)
    RETURNS bool
    LANGUAGE c
    STABLE
as 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog._ag_enforce_edge_uniqueness3(graphid, graphid, graphid)
    RETURNS bool
    LANGUAGE c
    STABLE
as 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog._ag_enforce_edge_uniqueness4(graphid, graphid, graphid, graphid)
    RETURNS bool
    LANGUAGE c
    STABLE
as 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.agtype_build_map_nonull(VARIADIC "any")
RETURNS agtype
LANGUAGE c
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.agtype_volatile_wrapper("any")
RETURNS agtype
LANGUAGE c
VOLATILE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.text_to_agtype(text)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (text AS agtype);

CREATE CAST (text AS agtype)
WITH FUNCTION ag_catalog.text_to_agtype(text);

CREATE OR REPLACE FUNCTION ag_catalog.int4_to_agtype(int4)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (int4 AS agtype);

CREATE CAST (int4 AS agtype)
WITH FUNCTION ag_catalog.int4_to_agtype(int4);

CREATE OR REPLACE FUNCTION ag_catalog.agtype_to_json(agtype)
RETURNS json
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (agtype AS json);

CREATE CAST (agtype AS json)
WITH FUNCTION ag_catalog.agtype_to_json(agtype);

CREATE OR REPLACE FUNCTION ag_catalog.agtype_to_jsonb(agtype)
RETURNS jsonb
LANGUAGE sql
STABLE
RETURNS NULL ON NULL INPUT
AS 'SELECT ag_catalog.agtype_to_json($1)::jsonb';

DROP CAST IF EXISTS (agtype AS jsonb);

CREATE CAST (agtype AS jsonb)
WITH FUNCTION ag_catalog.agtype_to_jsonb(agtype);

CREATE OR REPLACE FUNCTION ag_catalog.agtype_to_vector(agtype)
RETURNS pg_catalog.vector
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (agtype AS pg_catalog.vector);

CREATE CAST (agtype AS pg_catalog.vector)
WITH FUNCTION ag_catalog.agtype_to_vector(agtype);

CREATE OR REPLACE FUNCTION ag_catalog.agtype_to_halfvec(agtype)
RETURNS pg_catalog.halfvec
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (agtype AS pg_catalog.halfvec);

CREATE CAST (agtype AS pg_catalog.halfvec)
WITH FUNCTION ag_catalog.agtype_to_halfvec(agtype);

CREATE OR REPLACE FUNCTION ag_catalog.agtype_to_sparsevec(agtype)
RETURNS pg_catalog.sparsevec
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (agtype AS pg_catalog.sparsevec);

CREATE CAST (agtype AS pg_catalog.sparsevec)
WITH FUNCTION ag_catalog.agtype_to_sparsevec(agtype);

CREATE OR REPLACE FUNCTION ag_catalog.jsonb_to_agtype(jsonb)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (jsonb AS agtype);

CREATE CAST (jsonb AS agtype)
WITH FUNCTION ag_catalog.jsonb_to_agtype(jsonb);

CREATE OR REPLACE FUNCTION ag_catalog.agtype_array_to_agtype(agtype[])
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DROP CAST IF EXISTS (agtype[] AS agtype);

CREATE CAST (agtype[] AS agtype)
WITH FUNCTION ag_catalog.agtype_array_to_agtype(agtype[]);

CREATE OR REPLACE FUNCTION ag_catalog.agtype_object_field(agtype, text)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '->'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'pg_catalog.text'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR -> (
  LEFTARG = agtype,
  RIGHTARG = text,
  PROCEDURE = ag_catalog.agtype_object_field
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_object_field_text(agtype, text)
RETURNS text
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '->>'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'pg_catalog.text'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR ->> (
  LEFTARG = agtype,
  RIGHTARG = text,
  PROCEDURE = ag_catalog.agtype_object_field_text
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_object_field_agtype(agtype, agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '->'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR -> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_object_field_agtype
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_object_field_text_agtype(agtype, agtype)
RETURNS text
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '->>'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR ->> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_object_field_text_agtype
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_array_element(agtype, int4)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '->'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'pg_catalog.int4'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR -> (
  LEFTARG = agtype,
  RIGHTARG = int4,
  PROCEDURE = ag_catalog.agtype_array_element
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_array_element_text(agtype, int4)
RETURNS text
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '->>'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'pg_catalog.int4'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR ->> (
  LEFTARG = agtype,
  RIGHTARG = int4,
  PROCEDURE = ag_catalog.agtype_array_element_text
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_extract_path(agtype, agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '#>'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR #> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_extract_path
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_extract_path_text(agtype, agtype)
RETURNS text
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '#>>'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR #>> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_extract_path_text
)
        $ct$;
    END IF;
END
$upgrade$;

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '=~'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR =~ (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.age_eq_tilde
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.age_is_valid_label_name(agtype)
RETURNS boolean
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.cypher()
RETURNS SETOF record
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.cypher(graph_name name)
RETURNS SETOF record
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_prepare_cypher(text, text)
RETURNS boolean
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_id(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_start_id(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_end_id(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_head(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_last(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tail(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_reduce_size(agtype)
RETURNS int8
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_reduce_item(agtype, int8)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_properties(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_length(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_toboolean(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tobooleanlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tofloatlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tointegerlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tostringlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tofloat(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tointeger(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tostring(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_size(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_reverse(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_toupper(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tolower(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_ltrim(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_rtrim(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_trim(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_right(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_left(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_substring(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_split(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_replace(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_sin(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_cos(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_tan(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_cot(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_asin(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_acos(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_atan(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_atan2(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_degrees(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_radians(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_round(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_ceil(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_floor(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_log10(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_e()
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_exp(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_sqrt(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_reduce_transfn(agtype, agtype, text, agtype, agtype[])
    RETURNS agtype
    LANGUAGE c
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_proc p
                   JOIN pg_catalog.pg_namespace n ON n.oid = p.pronamespace
                   WHERE n.nspname = 'ag_catalog' AND p.proname = 'age_reduce') THEN
        EXECUTE $ct$
CREATE AGGREGATE ag_catalog.age_reduce(agtype, text, agtype, agtype[])
(
    stype = agtype,
    sfunc = ag_catalog.age_reduce_transfn
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_typecast_int(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.agtype_typecast_numeric(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.agtype_typecast_float(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.agtype_typecast_bool(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.agtype_typecast_vertex(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.agtype_typecast_edge(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.agtype_typecast_path(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_shortest_path(IN agtype, IN agtype, IN agtype,
                                             IN agtype DEFAULT NULL,
                                             IN agtype DEFAULT NULL,
                                             IN agtype DEFAULT NULL,
                                             IN agtype DEFAULT NULL)
    RETURNS SETOF agtype
LANGUAGE C
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_all_shortest_paths(IN agtype, IN agtype, IN agtype,
                                                  IN agtype DEFAULT NULL,
                                                  IN agtype DEFAULT NULL,
                                                  IN agtype DEFAULT NULL,
                                                  IN agtype DEFAULT NULL)
    RETURNS SETOF agtype
LANGUAGE C
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_graph_stats(agtype)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.create_complete_graph(graph_name name, nodes int,
                                                 edge_label name,
                                                 node_label name = NULL)
RETURNS void
LANGUAGE c
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_create_barbell_graph(graph_name name,
                                                    graph_size int,
                                                    bridge_size int,
                                                    node_label name = NULL,
                                                    node_properties agtype = NULL,
                                                    edge_label name = NULL,
                                                    edge_properties agtype = NULL)
RETURNS void
LANGUAGE c
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_range(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_unnest(agtype)
    RETURNS SETOF agtype
    LANGUAGE c
    IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.agtype_contains_top_level(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '@>>'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR @>> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_contains_top_level,
  COMMUTATOR = '<<@',
  RESTRICT = contsel,
  JOIN = contjoinsel
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.agtype_contained_by_top_level(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

DO $upgrade$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_operator o
                   JOIN pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
                   WHERE n.nspname = 'ag_catalog' AND o.oprname = '<<@'
                     AND o.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprright = 'ag_catalog.agtype'::pg_catalog.regtype
                     AND o.oprcode <> 0) THEN
        EXECUTE $ct$
CREATE OPERATOR <<@ (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_contained_by_top_level,
  COMMUTATOR = '@>>',
  RESTRICT = contsel,
  JOIN = contjoinsel
)
        $ct$;
    END IF;
END
$upgrade$;

CREATE OR REPLACE FUNCTION ag_catalog.age_isempty(agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_pi()
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.age_rand()
    RETURNS agtype
    LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE OR REPLACE FUNCTION ag_catalog.create_subgraph(new_graph name,
                                           from_graph name,
                                           node_filter text DEFAULT '*',
                                           relationship_filter text DEFAULT '*')
    RETURNS TABLE(node_count bigint, relationship_count bigint)
    LANGUAGE plpgsql
    VOLATILE
    SET search_path = ag_catalog, pg_catalog
    AS $function$
DECLARE
    from_oid oid;
    new_oid  oid;
    v_node_count bigint := 0;
    v_rel_count  bigint := 0;
    rec RECORD;
    cypher_q text;
    where_clause text;
    dst_label_id int;
    dst_seq_fqn text;
    dst_relation text;
    inserted bigint;
    has_rows boolean;
BEGIN
    -- Argument validation.
    IF new_graph IS NULL THEN
        RAISE EXCEPTION 'new graph name must not be NULL';
    END IF;
    IF from_graph IS NULL THEN
        RAISE EXCEPTION 'source graph name must not be NULL';
    END IF;
    IF new_graph = from_graph THEN
        RAISE EXCEPTION 'cannot extract a subgraph of "%" into itself', from_graph;
    END IF;

    -- NULL predicate is treated as the '*' wildcard (keep all).
    IF node_filter IS NULL THEN
        node_filter := '*';
    END IF;
    IF relationship_filter IS NULL THEN
        relationship_filter := '*';
    END IF;

    -- The predicates are embedded into a dollar-quoted cypher() query using the
    -- $age_subgraph$ tag; reject predicates that contain the tag to keep the
    -- quoting unambiguous.
    IF position('$age_subgraph$' IN node_filter) > 0
       OR position('$age_subgraph$' IN relationship_filter) > 0 THEN
        RAISE EXCEPTION 'filter predicate must not contain the reserved token $age_subgraph$';
    END IF;

    -- Validate source graph exists.
    -- openGauss note: the plugin's ag_graph carries no "graphid" column; the
    -- graph is identified by the catalog row's own oid (ag_label.graph is an
    -- FK to ag_graph.oid). A bare SELECT ... INTO raises "query returned no
    -- rows" on a missing graph here, so existence is checked first and the oid
    -- is only fetched once the row is known to exist.
    IF NOT EXISTS (SELECT 1 FROM ag_catalog.ag_graph WHERE name = from_graph) THEN
        RAISE EXCEPTION 'graph "%" does not exist', from_graph;
    END IF;
    SELECT oid INTO from_oid
    FROM ag_catalog.ag_graph WHERE name = from_graph;

    -- Validate destination graph does not exist (create_graph also enforces
    -- naming rules and uniqueness, but we give a clear early error).
    IF EXISTS (SELECT 1 FROM ag_catalog.ag_graph WHERE name = new_graph) THEN
        RAISE EXCEPTION 'graph "%" already exists', new_graph;
    END IF;

    -- Create the destination graph (default labels are created automatically).
    PERFORM ag_catalog.create_graph(new_graph);

    SELECT oid INTO new_oid
    FROM ag_catalog.ag_graph WHERE name = new_graph;

    -- Working sets / mapping (uniquely named to avoid colliding with user temps).
    DROP TABLE IF EXISTS _ag_sg_kept_v;
    DROP TABLE IF EXISTS _ag_sg_kept_e;
    DROP TABLE IF EXISTS _ag_sg_vmap;
    DROP TABLE IF EXISTS _ag_sg_vstage;
    DROP TABLE IF EXISTS _ag_sg_estage;

    --
    -- Kept vertices: evaluate node_filter with AGE's Cypher engine. The node
    -- variable `n` is bound exactly as in the spec; '*' selects all vertices.
    --
    IF node_filter IS NULL OR btrim(node_filter) = '*' THEN
        where_clause := '';
    ELSE
        where_clause := ' WHERE ' || node_filter;
    END IF;
    cypher_q := 'MATCH (n)' || where_clause || ' RETURN id(n)';

    -- openGauss note: the cypher() query argument must be a literal
    -- dollar-quoted constant present in the source text; a query assembled at
    -- run time cannot be passed positionally. The plugin's session-info path
    -- (age_prepare_cypher(graph, stmt) followed by cypher(NULL, NULL)) exists
    -- precisely to run a dynamically built cypher statement, so it is used here.
    PERFORM ag_catalog.age_prepare_cypher(from_graph, cypher_q);
    EXECUTE
        'CREATE TEMP TABLE _ag_sg_kept_v AS '
        'SELECT DISTINCT ag_catalog.agtype_to_graphid(vid) AS gid '
        'FROM ag_catalog.cypher(NULL, NULL) AS (vid agtype)';
    CREATE INDEX ON _ag_sg_kept_v (gid);

    --
    -- Kept edges: evaluate relationship_filter with AGE's Cypher engine. The
    -- relationship variable `r` is bound exactly as in the spec.
    --
    IF relationship_filter IS NULL OR btrim(relationship_filter) = '*' THEN
        where_clause := '';
    ELSE
        where_clause := ' WHERE ' || relationship_filter;
    END IF;
    cypher_q := 'MATCH ()-[r]->()' || where_clause || ' RETURN id(r)';

    PERFORM ag_catalog.age_prepare_cypher(from_graph, cypher_q);
    EXECUTE
        'CREATE TEMP TABLE _ag_sg_kept_e AS '
        'SELECT DISTINCT ag_catalog.agtype_to_graphid(eid) AS gid '
        'FROM ag_catalog.cypher(NULL, NULL) AS (eid agtype)';
    CREATE INDEX ON _ag_sg_kept_e (gid);

    -- old -> new vertex id mapping (graphid is unique within a graph).
    CREATE TEMP TABLE _ag_sg_vmap (old_id graphid PRIMARY KEY,
                                   new_id graphid NOT NULL);

    --
    -- PASS 1: copy kept vertices, label by label, assigning new graphids and
    -- recording the old->new mapping for edge remapping.
    --
    FOR rec IN
        SELECT name, id, relation
        FROM ag_catalog.ag_label
        WHERE graph = from_oid AND kind = 'v'
        ORDER BY id
    LOOP
        -- Skip labels with no surviving vertices. Read ONLY this label's own
        -- rows: AGE label tables use table inheritance (custom labels inherit
        -- from _ag_label_vertex), so a plain scan of a parent would also return
        -- its children and copy them twice.
        EXECUTE format(
            'SELECT EXISTS (SELECT 1 FROM ONLY %s t '
            'WHERE EXISTS (SELECT 1 FROM _ag_sg_kept_v k WHERE k.gid = t.id))',
            rec.relation::regclass::text)
        INTO has_rows;
        IF NOT has_rows THEN
            CONTINUE;
        END IF;

        -- Ensure the label exists in the destination graph.
        IF rec.name <> '_ag_label_vertex' THEN
            PERFORM 1 FROM ag_catalog.ag_label
            WHERE graph = new_oid AND name = rec.name;
            IF NOT FOUND THEN
                EXECUTE format('SELECT ag_catalog.create_vlabel(%L, %L)',
                               new_graph, rec.name);
            END IF;
        END IF;

        SELECT id, relation::regclass::text
        INTO dst_label_id, dst_relation
        FROM ag_catalog.ag_label
        WHERE graph = new_oid AND name = rec.name;
        -- Resolve the destination label's id sequence from its "id" column.
        dst_seq_fqn := pg_get_serial_sequence(dst_relation, 'id');

        -- Stage surviving vertices with freshly generated ids in a real temp
        -- table (single evaluation), then copy to the label table and record
        -- the old->new mapping. A materialized stage avoids any ambiguity from
        -- referencing a nextval-bearing CTE more than once.
        DROP TABLE IF EXISTS _ag_sg_vstage;
        EXECUTE format(
            'CREATE TEMP TABLE _ag_sg_vstage AS '
            'SELECT t.id AS old_id, '
            '       ag_catalog._graphid(%s, nextval(%L::regclass)) AS new_id, '
            '       t.properties AS props '
            'FROM ONLY %s t '
            'WHERE EXISTS (SELECT 1 FROM _ag_sg_kept_v k WHERE k.gid = t.id)',
            dst_label_id, dst_seq_fqn, rec.relation::regclass::text);

        EXECUTE format('INSERT INTO %s (id, properties) '
                       'SELECT new_id, props FROM _ag_sg_vstage', dst_relation);

        INSERT INTO _ag_sg_vmap (old_id, new_id)
            SELECT old_id, new_id FROM _ag_sg_vstage;

        DROP TABLE _ag_sg_vstage;
    END LOOP;

    SELECT count(*) INTO v_node_count FROM _ag_sg_vmap;

    --
    -- PASS 2: copy kept edges, remapping endpoints. The joins to _ag_sg_vmap
    -- enforce the induced rule (an edge survives only if BOTH endpoints were
    -- kept); membership in _ag_sg_kept_e applies relationship_filter.
    --
    FOR rec IN
        SELECT name, id, relation
        FROM ag_catalog.ag_label
        WHERE graph = from_oid AND kind = 'e'
        ORDER BY id
    LOOP
        -- Skip labels with no surviving edges. Read ONLY this label's own rows
        -- (see the vertex pass for why inheritance requires ONLY).
        EXECUTE format(
            'SELECT EXISTS ('
            '  SELECT 1 FROM ONLY %s x '
            '  JOIN _ag_sg_vmap vs ON vs.old_id = x.start_id '
            '  JOIN _ag_sg_vmap ve ON ve.old_id = x.end_id '
            '  WHERE EXISTS (SELECT 1 FROM _ag_sg_kept_e k WHERE k.gid = x.id))',
            rec.relation::regclass::text)
        INTO has_rows;
        IF NOT has_rows THEN
            CONTINUE;
        END IF;

        IF rec.name <> '_ag_label_edge' THEN
            PERFORM 1 FROM ag_catalog.ag_label
            WHERE graph = new_oid AND name = rec.name;
            IF NOT FOUND THEN
                EXECUTE format('SELECT ag_catalog.create_elabel(%L, %L)',
                               new_graph, rec.name);
            END IF;
        END IF;

        SELECT id, relation::regclass::text
        INTO dst_label_id, dst_relation
        FROM ag_catalog.ag_label
        WHERE graph = new_oid AND name = rec.name;
        -- Resolve the destination label's id sequence from its "id" column.
        dst_seq_fqn := pg_get_serial_sequence(dst_relation, 'id');

        -- Stage surviving edges, remapping endpoints through _ag_sg_vmap. The
        -- joins enforce the induced rule (both endpoints kept); membership in
        -- _ag_sg_kept_e applies relationship_filter.
        DROP TABLE IF EXISTS _ag_sg_estage;
        EXECUTE format(
            'CREATE TEMP TABLE _ag_sg_estage AS '
            'SELECT ag_catalog._graphid(%s, nextval(%L::regclass)) AS new_id, '
            '       vs.new_id AS new_start, ve.new_id AS new_end, '
            '       x.properties AS props '
            'FROM ONLY %s x '
            'JOIN _ag_sg_vmap vs ON vs.old_id = x.start_id '
            'JOIN _ag_sg_vmap ve ON ve.old_id = x.end_id '
            'WHERE EXISTS (SELECT 1 FROM _ag_sg_kept_e k WHERE k.gid = x.id)',
            dst_label_id, dst_seq_fqn, rec.relation::regclass::text);

        EXECUTE format('INSERT INTO %s (id, start_id, end_id, properties) '
                       'SELECT new_id, new_start, new_end, props '
                       'FROM _ag_sg_estage', dst_relation);
        GET DIAGNOSTICS inserted = ROW_COUNT;
        v_rel_count := v_rel_count + inserted;

        DROP TABLE _ag_sg_estage;
    END LOOP;

    -- openGauss does not support CREATE TEMP TABLE ... ON COMMIT DROP, so the
    -- working sets are plain session-temp tables; drop them explicitly here
    -- (the leading DROP IF EXISTS guards against any residue on re-entry).
    DROP TABLE IF EXISTS _ag_sg_kept_v;
    DROP TABLE IF EXISTS _ag_sg_kept_e;
    DROP TABLE IF EXISTS _ag_sg_vmap;

    RETURN QUERY SELECT v_node_count, v_rel_count;
END;
$function$;

COMMENT ON FUNCTION ag_catalog.create_subgraph(name, name, text, text) IS
'Materializes a new persistent graph as the induced subgraph of from_graph selected by a Cypher node predicate (on n) and relationship predicate (on r); ''*'' keeps all. An edge is kept only if its predicate holds and both endpoints are kept. Returns (node_count, relationship_count).';

CREATE OR REPLACE FUNCTION ag_catalog._agehash_self_test()
    RETURNS text
    LANGUAGE c
    VOLATILE
AS 'MODULE_PATHNAME';
