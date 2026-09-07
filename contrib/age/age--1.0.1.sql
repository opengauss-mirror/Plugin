/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION age" to load this file. \quit

-- Object creation below is subject to the target schema's ACL_CREATE checks.
-- openGauss executes anonymous PL/pgSQL blocks through an autonomous session,
-- so an install-time DO ownership guard cannot run reliably during CREATE EXTENSION.

--
-- catalog tables
--

CREATE TABLE ag_graph (
  name name NOT NULL,
  namespaceoid oid NOT NULL
) WITH (OIDS);

CREATE UNIQUE INDEX ag_graph_oid_index ON ag_graph USING btree (oid);

CREATE UNIQUE INDEX ag_graph_name_index ON ag_graph USING btree (name);

CREATE UNIQUE INDEX ag_graph_namespace_index
ON ag_graph
USING btree (namespaceoid);

-- 0 is an invalid label ID
CREATE DOMAIN label_id AS int NOT NULL CHECK (VALUE > 0 AND VALUE <= 65535);

CREATE DOMAIN label_kind AS "char" NOT NULL CHECK (VALUE = 'v' OR VALUE = 'e');

CREATE TABLE ag_label (
  name name NOT NULL,
  graph oid NOT NULL,
  id label_id,
  kind label_kind,
  relation regclass NOT NULL,
  CONSTRAINT fk_graph_oid
    FOREIGN KEY(graph)
    REFERENCES ag_graph(oid)
) WITH (OIDS);

CREATE OR REPLACE FUNCTION deny_update_ag_meta_tabel()
RETURNS trigger
LANGUAGE plpgsql
AS $function$
BEGIN
 RAISE EXCEPTION
    'Attention: can not update or delete table ag_graph|ag_label,Please use age extension function !';
END;
$function$;

create trigger trigger_ag_graph_update
BEFORE UPDATE ON ag_graph
FOR EACH ROW
EXECUTE PROCEDURE deny_update_ag_meta_tabel();

create trigger trigger_ag_graph_delete
BEFORE DELETE ON ag_graph
FOR EACH ROW
EXECUTE PROCEDURE deny_update_ag_meta_tabel();

create trigger trigger_ag_label_update
BEFORE UPDATE ON ag_label
FOR EACH ROW
EXECUTE PROCEDURE deny_update_ag_meta_tabel();

create trigger trigger_ag_label_delete
BEFORE DELETE ON ag_label
FOR EACH ROW
EXECUTE PROCEDURE deny_update_ag_meta_tabel();

CREATE UNIQUE INDEX ag_label_oid_index ON ag_label USING btree (oid);

CREATE UNIQUE INDEX ag_label_name_graph_index
ON ag_label
USING btree (name, graph);

CREATE UNIQUE INDEX ag_label_graph_id_index
ON ag_label
USING btree (graph, id);

CREATE UNIQUE INDEX ag_label_relation_index ON ag_label USING btree (relation);

-- Extension config data contains database-local OIDs. Rebind those OIDs from
-- stable schema and relation names when pg_dump restores rows into a new
-- database. ALWAYS triggers still fire while restore sets
-- session_replication_role = replica.
CREATE FUNCTION ag_catalog.age_rebind_graph_namespace_on_restore()
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

CREATE TRIGGER trigger_ag_graph_restore
BEFORE INSERT ON ag_catalog.ag_graph
FOR EACH ROW
EXECUTE PROCEDURE ag_catalog.age_rebind_graph_namespace_on_restore();

ALTER TABLE ag_catalog.ag_graph
ENABLE ALWAYS TRIGGER trigger_ag_graph_restore;

CREATE FUNCTION ag_catalog.age_rebind_label_graph_on_restore()
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

CREATE TRIGGER trigger_ag_label_restore
BEFORE INSERT ON ag_catalog.ag_label
FOR EACH ROW
EXECUTE PROCEDURE ag_catalog.age_rebind_label_graph_on_restore();

ALTER TABLE ag_catalog.ag_label
ENABLE ALWAYS TRIGGER trigger_ag_label_restore;

SELECT pg_catalog.pg_extension_config_dump('ag_catalog.ag_graph', 'WHERE true');
SELECT pg_catalog.pg_extension_config_dump('ag_catalog.ag_label', 'WHERE true');

--
-- openGauss pg_upgrade support
--
-- Unlike upstream PostgreSQL AGE, this catalog already stores namespaceoid as
-- oid and uses the ag_graph row OID as graph identity. Preparation therefore
-- snapshots stable graph/schema names; finish repairs only namespace OIDs and
-- verifies that graph row identities survived the upgrade unchanged.
--
CREATE FUNCTION ag_catalog._age_invalidate_graph_cache()
RETURNS void
LANGUAGE c
VOLATILE
AS 'MODULE_PATHNAME';

REVOKE ALL ON FUNCTION ag_catalog._age_invalidate_graph_cache() FROM PUBLIC;

CREATE FUNCTION ag_catalog.age_invalidate_graph_cache()
RETURNS trigger
LANGUAGE c
VOLATILE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_prepare_pg_upgrade()
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

CREATE FUNCTION ag_catalog.age_finish_pg_upgrade()
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

CREATE FUNCTION ag_catalog.age_revert_pg_upgrade_changes()
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

CREATE FUNCTION ag_catalog.age_pg_upgrade_status()
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

--
-- catalog lookup functions
--

CREATE FUNCTION ag_catalog._label_id(graph_name name, label_name name)
RETURNS label_id
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

--
-- utility functions
--

CREATE FUNCTION ag_catalog.create_graph(name)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME','create_graph';

CREATE FUNCTION ag_catalog.drop_graph(graph_name name, cascade boolean = false)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.create_vlabel(graph_name cstring, label_name cstring)
    RETURNS void
    LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.create_elabel(graph_name cstring, label_name cstring)
    RETURNS void
    LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.alter_graph(graph_name name, operation cstring, new_value name)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.drop_label(graph_name name, label_name name,
                           force boolean = false)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.load_labels_from_file(graph_name name,
                                                 label_name name,
                                                 file_path text,
                                                 id_field_exists bool default true,
                                                 load_as_agtype bool default false,
                                                 delimiter text default ',')
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.load_edges_from_file(graph_name name,
                                                label_name name,
                                                file_path text,
                                                load_as_agtype bool default false,
                                                delimiter text default ',')
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';
CREATE FUNCTION ag_catalog.load_labels_from_file_with_analysefile(graph_name name,
                                            label_name name,
                                            file_path text,
                                            id_field_exists bool default true)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.load_edges_from_file_with_analysefile(graph_name name,
                                                label_name name,
                                                file_path text)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

REVOKE ALL ON FUNCTION ag_catalog.load_labels_from_file(name, name, text, bool, bool, text) FROM PUBLIC;
REVOKE ALL ON FUNCTION ag_catalog.load_edges_from_file(name, name, text, bool, text) FROM PUBLIC;
REVOKE ALL ON FUNCTION ag_catalog.load_labels_from_file_with_analysefile(name, name, text, bool) FROM PUBLIC;
REVOKE ALL ON FUNCTION ag_catalog.load_edges_from_file_with_analysefile(name, name, text) FROM PUBLIC;

--
-- graphid type
--
-- define graphid as a shell type first
CREATE TYPE graphid;

CREATE FUNCTION ag_catalog.graphid_in(cstring)
RETURNS graphid
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.graphid_out(graphid)
RETURNS cstring
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

-- binary I/O functions
CREATE FUNCTION ag_catalog.graphid_send(graphid)
RETURNS bytea
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.graphid_recv(internal)
RETURNS graphid
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE TYPE graphid (
  INPUT = ag_catalog.graphid_in,
  OUTPUT = ag_catalog.graphid_out,
  SEND = ag_catalog.graphid_send,
  RECEIVE = ag_catalog.graphid_recv,
  INTERNALLENGTH = 8,
  PASSEDBYVALUE,
  ALIGNMENT = float8,
  STORAGE = plain
);

--
-- graphid - comparison operators (=, <>, <, >, <=, >=)
--

CREATE FUNCTION ag_catalog.graphid_eq(graphid, graphid)
RETURNS boolean
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  PROCEDURE  = ag_catalog.graphid_eq,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  HASHES,
  MERGES
);

CREATE FUNCTION ag_catalog.graphid_ne(graphid, graphid)
RETURNS boolean
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  PROCEDURE  = ag_catalog.graphid_ne,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.graphid_lt(graphid, graphid)
RETURNS boolean
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  PROCEDURE  = ag_catalog.graphid_lt,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.graphid_gt(graphid, graphid)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  PROCEDURE  = ag_catalog.graphid_gt,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.graphid_le(graphid, graphid)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  PROCEDURE  = ag_catalog.graphid_le,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.graphid_ge(graphid, graphid)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  PROCEDURE  = ag_catalog.graphid_ge,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

--
-- graphid - B-tree support functions
--

-- comparison support
CREATE FUNCTION ag_catalog.graphid_btree_cmp(graphid, graphid)
RETURNS int
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

-- sort support
CREATE FUNCTION ag_catalog.graphid_btree_sort(internal)
RETURNS void
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

--
-- define operator classes for graphid
--

-- B-tree strategies
--   1: less than
--   2: less than or equal
--   3: equal
--   4: greater than or equal
--   5: greater than
--
-- B-tree support functions
--   1: compare two keys and return an integer less than zero, zero, or greater
--      than zero, indicating whether the first key is less than, equal to, or
--      greater than the second
--   2: return the addresses of C-callable sort support function(s) (optional)
--   3: compare a test value to a base value plus/minus an offset, and return
--      true or false according to the comparison result (optional)
CREATE OPERATOR CLASS graphid_ops DEFAULT FOR TYPE graphid USING btree AS
  OPERATOR 1 <,
  OPERATOR 2 <=,
  OPERATOR 3 =,
  OPERATOR 4 >=,
  OPERATOR 5 >,
  FUNCTION 1 ag_catalog.graphid_btree_cmp (graphid, graphid),
  FUNCTION 2 ag_catalog.graphid_btree_sort (internal);

--
-- graphid functions
--

CREATE FUNCTION ag_catalog._graphid(label_id int, entry_id bigint)
RETURNS graphid
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._extract_label_id(graphid)
RETURNS label_id
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';
--
-- agtype type and its support functions
--

-- define agtype as a shell type first
CREATE TYPE agtype;

CREATE FUNCTION ag_catalog.agtype_in(cstring)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_out(agtype)
RETURNS cstring
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

-- binary I/O functions
CREATE FUNCTION ag_catalog.agtype_send(agtype)
RETURNS bytea
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_recv(internal)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE TYPE agtype (
  INPUT = ag_catalog.agtype_in,
  OUTPUT = ag_catalog.agtype_out,
  SEND = ag_catalog.agtype_send,
  RECEIVE = ag_catalog.agtype_recv,
  LIKE = jsonb
);

CREATE FUNCTION ag_catalog.graph_exists(graph_name name)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME', 'age_graph_exists';

CREATE FUNCTION ag_catalog._label_name(graph_oid oid, graphid)
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

--
-- Composite graph entity types
--
CREATE TYPE ag_catalog.vertex AS (
  id graphid,
  label agtype,
  properties agtype
);

CREATE TYPE ag_catalog.edge AS (
  id graphid,
  label agtype,
  end_id graphid,
  start_id graphid,
  properties agtype
);

CREATE FUNCTION ag_catalog.vertex_to_agtype(vertex)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.edge_to_agtype(edge)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (vertex AS agtype)
WITH FUNCTION ag_catalog.vertex_to_agtype(vertex)
AS IMPLICIT;

CREATE CAST (edge AS agtype)
WITH FUNCTION ag_catalog.edge_to_agtype(edge)
AS IMPLICIT;

CREATE FUNCTION ag_catalog.vertex_to_json(vertex)
RETURNS json
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.edge_to_json(edge)
RETURNS json
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (vertex AS json)
WITH FUNCTION ag_catalog.vertex_to_json(vertex);

CREATE CAST (edge AS json)
WITH FUNCTION ag_catalog.edge_to_json(edge);

CREATE FUNCTION ag_catalog.vertex_to_jsonb(vertex)
RETURNS jsonb
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.edge_to_jsonb(edge)
RETURNS jsonb
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (vertex AS jsonb)
WITH FUNCTION ag_catalog.vertex_to_jsonb(vertex);

CREATE CAST (edge AS jsonb)
WITH FUNCTION ag_catalog.edge_to_jsonb(edge);

CREATE FUNCTION ag_catalog.vertex_eq(vertex, vertex)
RETURNS boolean
LANGUAGE sql
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS $$ SELECT $1.id = $2.id $$;

CREATE OPERATOR = (
  PROCEDURE = ag_catalog.vertex_eq,
  LEFTARG = vertex,
  RIGHTARG = vertex,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.vertex_ne(vertex, vertex)
RETURNS boolean
LANGUAGE sql
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS $$ SELECT $1.id <> $2.id $$;

CREATE OPERATOR <> (
  PROCEDURE = ag_catalog.vertex_ne,
  LEFTARG = vertex,
  RIGHTARG = vertex,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.edge_eq(edge, edge)
RETURNS boolean
LANGUAGE sql
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS $$ SELECT $1.id = $2.id $$;

CREATE OPERATOR = (
  PROCEDURE = ag_catalog.edge_eq,
  LEFTARG = edge,
  RIGHTARG = edge,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.edge_ne(edge, edge)
RETURNS boolean
LANGUAGE sql
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS $$ SELECT $1.id <> $2.id $$;

CREATE OPERATOR <> (
  PROCEDURE = ag_catalog.edge_ne,
  LEFTARG = edge,
  RIGHTARG = edge,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

--
-- agtype - mathematical operators (+, -, *, /, %, ^)
--

CREATE FUNCTION ag_catalog.agtype_add(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_add,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, smallint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  smallint,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(smallint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = smallint,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, integer)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  integer,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(integer, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = integer,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, bigint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  bigint,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(bigint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = bigint,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, real)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  real,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(real, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = real,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, double precision)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  double precision,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(double precision, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = double precision,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, numeric)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  numeric,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(numeric, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR +(
  PROCEDURE= ag_catalog.agtype_any_add,
  LEFTARG = numeric,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_concat(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR || (
  PROCEDURE = ag_catalog.agtype_concat,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_sub(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_sub,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, smallint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  smallint
);

CREATE FUNCTION ag_catalog.agtype_any_sub(smallint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = smallint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, integer)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  integer
);

CREATE FUNCTION ag_catalog.agtype_any_sub(integer, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = integer,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, bigint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  bigint
);

CREATE FUNCTION ag_catalog.agtype_any_sub(bigint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = bigint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, real)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  real
);

CREATE FUNCTION ag_catalog.agtype_any_sub(real, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = real,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, double precision)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  double precision
);

CREATE FUNCTION ag_catalog.agtype_any_sub(double precision, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = double precision,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, numeric)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  numeric
);

CREATE FUNCTION ag_catalog.agtype_any_sub(numeric, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_any_sub,
  LEFTARG = numeric,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_neg(agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -(
  PROCEDURE= ag_catalog.agtype_neg,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_mul(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_mul,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, smallint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  smallint,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(smallint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = smallint,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, integer)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  integer,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(integer, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = integer,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, bigint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  bigint,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(bigint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = bigint,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, real)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  real,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(real, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = real,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, double precision)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  double precision,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(double precision, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = double precision,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, numeric)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  numeric,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(numeric, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR *(
  PROCEDURE= ag_catalog.agtype_any_mul,
  LEFTARG = numeric,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_div(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_div,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, smallint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  smallint
);

CREATE FUNCTION ag_catalog.agtype_any_div(smallint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = smallint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, integer)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  integer
);

CREATE FUNCTION ag_catalog.agtype_any_div(integer, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = integer,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, bigint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  bigint
);

CREATE FUNCTION ag_catalog.agtype_any_div(bigint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = bigint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, real)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  real
);

CREATE FUNCTION ag_catalog.agtype_any_div(real, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = real,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, double precision)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  double precision
);

CREATE FUNCTION ag_catalog.agtype_any_div(double precision, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = double precision,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, numeric)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  numeric
);

CREATE FUNCTION ag_catalog.agtype_any_div(numeric, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR /(
  PROCEDURE= ag_catalog.agtype_any_div,
  LEFTARG = numeric,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_mod(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_mod,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, smallint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  smallint
);

CREATE FUNCTION ag_catalog.agtype_any_mod(smallint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = smallint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, integer)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  integer
);

CREATE FUNCTION ag_catalog.agtype_any_mod(integer, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = integer,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, bigint)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  bigint
);

CREATE FUNCTION ag_catalog.agtype_any_mod(bigint, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = bigint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, real)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  real
);

CREATE FUNCTION ag_catalog.agtype_any_mod(real, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = real,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, double precision)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  double precision
);

CREATE FUNCTION ag_catalog.agtype_any_mod(double precision, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = double precision,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, numeric)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  numeric
);

CREATE FUNCTION ag_catalog.agtype_any_mod(numeric, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR %(
  PROCEDURE= ag_catalog.agtype_any_mod,
  LEFTARG = numeric,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_pow(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ^(
  PROCEDURE= ag_catalog.agtype_pow,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.graphid_hash_cmp(graphid)
RETURNS INTEGER
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE OPERATOR CLASS graphid_ops_hash
  DEFAULT
  FOR TYPE graphid
  USING hash AS
  OPERATOR 1 =,
  FUNCTION 1 ag_catalog.graphid_hash_cmp(graphid);

--
-- agtype - comparison operators (=, <>, <, >, <=, >=)
--

CREATE FUNCTION ag_catalog.agtype_eq(agtype, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_eq,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  HASHES
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, smallint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(smallint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, integer)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(integer, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, bigint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(bigint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, real)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(real, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, double precision)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(double precision, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, numeric)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(numeric, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR =(
  PROCEDURE= ag_catalog.agtype_any_eq,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_ne(agtype, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_ne,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, smallint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(smallint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, integer)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(integer, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, bigint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(bigint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, real)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(real, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, double precision)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(double precision, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, numeric)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(numeric, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <>(
  PROCEDURE= ag_catalog.agtype_any_ne,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_lt(agtype, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_lt,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, smallint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(smallint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, integer)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(integer, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, bigint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(bigint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, real)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(real, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, double precision)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(double precision, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, numeric)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(numeric, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <(
  PROCEDURE= ag_catalog.agtype_any_lt,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_gt(agtype, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_gt,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, smallint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(smallint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, integer)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(integer, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, bigint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(bigint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, real)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(real, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, double precision)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(double precision, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, numeric)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(numeric, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >(
  PROCEDURE= ag_catalog.agtype_any_gt,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_le(agtype, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_le,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, smallint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(smallint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, integer)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(integer, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, bigint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(bigint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, real)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(real, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, double precision)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(double precision, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, numeric)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(numeric, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <=(
  PROCEDURE= ag_catalog.agtype_any_le,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_ge(agtype, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_ge,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, smallint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(smallint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, integer)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(integer, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, bigint)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(bigint, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, real)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(real, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, double precision)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(double precision, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, numeric)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(numeric, agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR >=(
  PROCEDURE= ag_catalog.agtype_any_ge,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_btree_cmp(agtype, agtype)
RETURNS INTEGER
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE OPERATOR CLASS agtype_ops_btree
  DEFAULT
  FOR TYPE agtype
  USING btree AS
  OPERATOR 1 <,
  OPERATOR 2 <=,
  OPERATOR 3 =,
  OPERATOR 4 >,
  OPERATOR 5 >=,
  FUNCTION 1 ag_catalog.agtype_btree_cmp(agtype, agtype);

CREATE FUNCTION ag_catalog.agtype_hash_cmp(agtype)
RETURNS INTEGER
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE OPERATOR CLASS agtype_ops_hash
  DEFAULT
  FOR TYPE agtype
  USING hash AS
  OPERATOR 1 =,
  FUNCTION 1 ag_catalog.agtype_hash_cmp(agtype);

--
-- graph id conversion function
--
CREATE FUNCTION ag_catalog.graphid_to_agtype(graphid)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (graphid AS agtype)
WITH FUNCTION ag_catalog.graphid_to_agtype(graphid);

CREATE FUNCTION ag_catalog.agtype_to_graphid(agtype)
RETURNS graphid
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS graphid)
WITH FUNCTION ag_catalog.agtype_to_graphid(agtype)
AS IMPLICIT;

--
-- agtype - path
--
CREATE FUNCTION ag_catalog._agtype_build_path(VARIADIC "any")
RETURNS agtype
LANGUAGE c
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

--
-- agtype - vertex
--
CREATE FUNCTION ag_catalog._agtype_build_vertex(graphid, agtype, agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

--
-- agtype - edge
--
CREATE FUNCTION ag_catalog._agtype_build_edge(graphid, graphid, graphid, agtype, agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._get_vertex_by_graphid(text, graphid)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

-- Fast specialized edge-uniqueness checks for the 2/3/4-edge cases; take plain
-- graphid arguments and do direct pairwise comparison (chosen by edge count in
-- prevent_duplicate_edges()).
CREATE FUNCTION ag_catalog._ag_enforce_edge_uniqueness2(graphid, graphid)
    RETURNS bool
    LANGUAGE c
    STABLE
as 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._ag_enforce_edge_uniqueness3(graphid, graphid, graphid)
    RETURNS bool
    LANGUAGE c
    STABLE
as 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._ag_enforce_edge_uniqueness4(graphid, graphid, graphid, graphid)
    RETURNS bool
    LANGUAGE c
    STABLE
as 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._ag_enforce_edge_uniqueness(VARIADIC "any")
RETURNS bool
LANGUAGE c
STABLE
as 'MODULE_PATHNAME';

--
-- agtype - map literal (`{key: expr, ...}`)
--

CREATE FUNCTION ag_catalog.agtype_build_map(VARIADIC "any")
RETURNS agtype
LANGUAGE c
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_build_map()
RETURNS agtype
LANGUAGE c
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME', 'agtype_build_map_noargs';

CREATE FUNCTION ag_catalog.agtype_build_map_nonull(VARIADIC "any")
RETURNS agtype
LANGUAGE c
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

--
-- There are times when the optimizer might eliminate
-- functions we need. Wrap the function with this to
-- prevent that from happening
--
CREATE FUNCTION ag_catalog.agtype_volatile_wrapper("any")
RETURNS agtype
LANGUAGE c
VOLATILE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

--
-- agtype - list literal (`[expr, ...]`)
--

CREATE FUNCTION ag_catalog.agtype_build_list(VARIADIC "any")
RETURNS agtype
LANGUAGE c
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_build_list()
RETURNS agtype
LANGUAGE c
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME', 'agtype_build_list_noargs';

--
-- agtype - type coercions
--
-- agtype -> text (explicit)
CREATE FUNCTION ag_catalog.agtype_to_text(agtype)
RETURNS text
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS text)
WITH FUNCTION ag_catalog.agtype_to_text(agtype);

-- text -> agtype (explicit)
CREATE FUNCTION ag_catalog.text_to_agtype(text)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (text AS agtype)
WITH FUNCTION ag_catalog.text_to_agtype(text);

-- agtype -> boolean (implicit)
CREATE FUNCTION ag_catalog.agtype_to_bool(agtype)
RETURNS boolean
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS boolean)
WITH FUNCTION ag_catalog.agtype_to_bool(agtype)
AS IMPLICIT;

-- boolean -> agtype (explicit)
CREATE FUNCTION ag_catalog.bool_to_agtype(boolean)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (boolean AS agtype)
WITH FUNCTION ag_catalog.bool_to_agtype(boolean);

-- float8 -> agtype (explicit)
CREATE FUNCTION ag_catalog.float8_to_agtype(float8)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (float8 AS agtype)
WITH FUNCTION ag_catalog.float8_to_agtype(float8);

-- agtype -> float8 (implicit)
CREATE FUNCTION ag_catalog.agtype_to_float8(agtype)
RETURNS float8
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS float8)
WITH FUNCTION ag_catalog.agtype_to_float8(agtype);

-- int8 -> agtype (explicit)
CREATE FUNCTION ag_catalog.int8_to_agtype(int8)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (int8 AS agtype)
WITH FUNCTION ag_catalog.int8_to_agtype(int8);

-- agtype -> int8
CREATE FUNCTION ag_catalog.agtype_to_int8(variadic "any")
RETURNS bigint
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS bigint)
WITH FUNCTION ag_catalog.agtype_to_int8(variadic "any")
AS ASSIGNMENT;

-- int4 -> agtype (explicit)
CREATE FUNCTION ag_catalog.int4_to_agtype(int4)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (int4 AS agtype)
WITH FUNCTION ag_catalog.int4_to_agtype(int4);

-- agtype -> int4
CREATE FUNCTION ag_catalog.agtype_to_int4(variadic "any")
RETURNS int
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS int)
WITH FUNCTION ag_catalog.agtype_to_int4(variadic "any");

-- agtype -> int2
CREATE FUNCTION ag_catalog.agtype_to_int2(variadic "any")
RETURNS smallint
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS smallint)
WITH FUNCTION ag_catalog.agtype_to_int2(variadic "any");

-- agtype -> int4[]
CREATE FUNCTION ag_catalog.agtype_to_int4_array(variadic "any")
    RETURNS int[]
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS int[])
    WITH FUNCTION ag_catalog.agtype_to_int4_array(variadic "any");

-- agtype -> json (explicit)
CREATE FUNCTION ag_catalog.agtype_to_json(agtype)
RETURNS json
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS json)
WITH FUNCTION ag_catalog.agtype_to_json(agtype);

-- agtype -> jsonb (explicit)
CREATE FUNCTION ag_catalog.agtype_to_jsonb(agtype)
RETURNS jsonb
LANGUAGE sql
STABLE
RETURNS NULL ON NULL INPUT
AS 'SELECT ag_catalog.agtype_to_json($1)::jsonb';

CREATE CAST (agtype AS jsonb)
WITH FUNCTION ag_catalog.agtype_to_jsonb(agtype);

-- agtype -> vector family (explicit)
CREATE FUNCTION ag_catalog.agtype_to_vector(agtype)
RETURNS pg_catalog.vector
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS pg_catalog.vector)
WITH FUNCTION ag_catalog.agtype_to_vector(agtype);

CREATE FUNCTION ag_catalog.agtype_to_halfvec(agtype)
RETURNS pg_catalog.halfvec
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS pg_catalog.halfvec)
WITH FUNCTION ag_catalog.agtype_to_halfvec(agtype);

CREATE FUNCTION ag_catalog.agtype_to_sparsevec(agtype)
RETURNS pg_catalog.sparsevec
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS pg_catalog.sparsevec)
WITH FUNCTION ag_catalog.agtype_to_sparsevec(agtype);

-- jsonb -> agtype (explicit)
CREATE FUNCTION ag_catalog.jsonb_to_agtype(jsonb)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (jsonb AS agtype)
WITH FUNCTION ag_catalog.jsonb_to_agtype(jsonb);

-- agtype[] -> agtype (explicit)
CREATE FUNCTION ag_catalog.agtype_array_to_agtype(agtype[])
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE CAST (agtype[] AS agtype)
WITH FUNCTION ag_catalog.agtype_array_to_agtype(agtype[]);
--
-- agtype - access operators
--

CREATE FUNCTION ag_catalog.agtype_object_field(agtype, text)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -> (
  LEFTARG = agtype,
  RIGHTARG = text,
  PROCEDURE = ag_catalog.agtype_object_field
);

CREATE FUNCTION ag_catalog.agtype_object_field_text(agtype, text)
RETURNS text
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ->> (
  LEFTARG = agtype,
  RIGHTARG = text,
  PROCEDURE = ag_catalog.agtype_object_field_text
);

CREATE FUNCTION ag_catalog.agtype_object_field_agtype(agtype, agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_object_field_agtype
);

CREATE FUNCTION ag_catalog.agtype_object_field_text_agtype(agtype, agtype)
RETURNS text
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ->> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_object_field_text_agtype
);

CREATE FUNCTION ag_catalog.agtype_array_element(agtype, int4)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR -> (
  LEFTARG = agtype,
  RIGHTARG = int4,
  PROCEDURE = ag_catalog.agtype_array_element
);

CREATE FUNCTION ag_catalog.agtype_array_element_text(agtype, int4)
RETURNS text
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ->> (
  LEFTARG = agtype,
  RIGHTARG = int4,
  PROCEDURE = ag_catalog.agtype_array_element_text
);

CREATE FUNCTION ag_catalog.agtype_extract_path(agtype, agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR #> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_extract_path
);

CREATE FUNCTION ag_catalog.agtype_extract_path_text(agtype, agtype)
RETURNS text
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR #>> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_extract_path_text
);

-- for series of `map.key` and `container[expr]`
CREATE FUNCTION ag_catalog.agtype_access_operator(VARIADIC agtype[])
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_access_slice(agtype, agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_in_operator(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

--
-- agtype - string matching (`STARTS WITH`, `ENDS WITH`, `CONTAINS`, & =~)
--

CREATE FUNCTION ag_catalog.agtype_string_match_starts_with(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_string_match_ends_with(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_string_match_contains(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_eq_tilde(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE OPERATOR =~ (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.age_eq_tilde
);

CREATE FUNCTION ag_catalog.age_is_valid_label_name(agtype)
RETURNS boolean
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

--
-- functions for updating clauses
--

-- This function is defined as a VOLATILE function to prevent the optimizer
-- from pulling up Query's for CREATE clauses.
CREATE FUNCTION ag_catalog._cypher_create_clause(internal)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._cypher_set_clause(internal)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._cypher_delete_clause(internal)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._cypher_merge_clause(internal)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';
CREATE FUNCTION ag_catalog._cypher_vle_clause(internal)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

--
-- query functions
--
CREATE FUNCTION ag_catalog.cypher(graph_name name, query_string cstring,
                       params agtype = NULL)
RETURNS SETOF record
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.cypher()
RETURNS SETOF record
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.cypher(graph_name name)
RETURNS SETOF record
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_prepare_cypher(text, text)
RETURNS boolean
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.get_cypher_keywords(OUT word text, OUT catcode "char",
                                    OUT catdesc text)
RETURNS SETOF record
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
COST 10
ROWS 60
AS 'MODULE_PATHNAME';

--
-- Scalar Functions
--
CREATE FUNCTION ag_catalog.age_id(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_start_id(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_end_id(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_head(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_last(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tail(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_reduce_size(agtype)
RETURNS int8
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_reduce_item(agtype, int8)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';


CREATE FUNCTION ag_catalog.age_properties(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_startnode(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_endnode(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_length(agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_toboolean(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tobooleanlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

-- toFloatList() / toIntegerList() / toStringList(): convert each element of a
-- list, non-convertible elements become null in the returned list.
CREATE FUNCTION ag_catalog.age_tofloatlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tointegerlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tostringlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tofloat(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tointeger(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_date_part(format_str agtype,timelong agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tostring(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_size(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_type(agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_exists(agtype)
RETURNS boolean
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_label(agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._property_constraint_check(agtype, agtype)
RETURNS boolean
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

--
-- String functions
--
CREATE FUNCTION ag_catalog.age_reverse(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_toupper(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tolower(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_ltrim(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_rtrim(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_trim(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_right(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_left(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_substring(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_split(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_replace(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

--
-- Trig functions - radian input
--
CREATE FUNCTION ag_catalog.age_sin(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_cos(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tan(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_cot(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_asin(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_acos(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_atan(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_atan2(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_degrees(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_radians(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_round(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_ceil(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_floor(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_abs(variadic "any")
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_sign(variadic "any")
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_log(variadic "any")
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_log10(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_e()
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_exp(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_sqrt(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_timestamp()
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

--
-- aggregate function components for stdev(internal, agtype)
-- and stdevp(internal, agtype)
--
-- wrapper for the stdev final function to pass 0 instead of null
CREATE FUNCTION ag_catalog.age_float8_stddev_samp_aggfinalfn(_float8)
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- wrapper for the float8_accum to use agtype input
CREATE FUNCTION ag_catalog.age_agtype_float8_accum(_float8, agtype)
RETURNS _float8
LANGUAGE c
IMMUTABLE
STRICT
AS 'MODULE_PATHNAME';

-- aggregate definition for age_stdev(agtype)
CREATE AGGREGATE ag_catalog.age_stdev(agtype)
(
   stype = _float8,
   sfunc = ag_catalog.age_agtype_float8_accum,
   finalfunc = ag_catalog.age_float8_stddev_samp_aggfinalfn,
   initcond = '{0,0,0}');

-- wrapper for the stdevp final function to pass 0 instead of null
CREATE FUNCTION ag_catalog.age_float8_stddev_pop_aggfinalfn(_float8)
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- aggregate definition for age_stdevp(agtype)
CREATE AGGREGATE ag_catalog.age_stdevp(agtype)
(
   stype = _float8,
   sfunc = age_agtype_float8_accum,
   finalfunc = ag_catalog.age_float8_stddev_pop_aggfinalfn,
   initcond = '{0,0,0}'
);

--
-- aggregate function components for avg(agtype) and sum(agtype)
--
-- aggregate definition for avg(agytpe)
CREATE AGGREGATE ag_catalog.age_avg(agtype)
(
   stype = _float8,
   sfunc = ag_catalog.age_agtype_float8_accum,
   finalfunc = float8_avg,
   initcond = '{0,0,0}');

-- sum aggtransfn
CREATE FUNCTION ag_catalog.age_agtype_sum(agtype, agtype)
RETURNS agtype
LANGUAGE c
IMMUTABLE
STRICT
AS 'MODULE_PATHNAME';

-- aggregate definition for sum(agytpe)
CREATE AGGREGATE ag_catalog.age_sum(agtype)
(
   stype = agtype,
   sfunc = ag_catalog.age_agtype_sum
);

--
-- aggregate functions for min(variadic "any") and max(variadic "any")
--
-- max transfer function
CREATE FUNCTION ag_catalog.age_agtype_larger_aggtransfn(agtype, variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- aggregate definition for max(variadic "any")
CREATE AGGREGATE ag_catalog.age_max("any")
(
   stype = agtype,
   sfunc = ag_catalog.age_agtype_larger_aggtransfn
);

-- min transfer function
CREATE FUNCTION ag_catalog.age_agtype_smaller_aggtransfn(agtype, variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- aggregate definition for min(variadic "any")
CREATE AGGREGATE ag_catalog.age_min("any")
(
   stype = agtype,
   sfunc = ag_catalog.age_agtype_smaller_aggtransfn
);

--
-- aggregate functions percentileCont(internal, agtype) and
-- percentileDisc(internal, agtype)
--
-- percentile transfer function
--CREATE FUNCTION ag_catalog.age_percentile_aggtransfn(internal, agtype, agtype)
CREATE FUNCTION ag_catalog.age_percentile_aggtransfn(internal, agtype, agtype)
RETURNS internal
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- percentile_cont final function
CREATE FUNCTION ag_catalog.age_percentile_cont_aggfinalfn(internal)
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- percentile_disc final function
CREATE FUNCTION ag_catalog.age_percentile_disc_aggfinalfn(internal)
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- aggregate definition for _percentilecont(agtype, agytpe)
CREATE AGGREGATE ag_catalog.age_percentilecont(agtype, agtype)
(
   stype = internal,
   sfunc = ag_catalog.age_percentile_aggtransfn,
   finalfunc = ag_catalog.age_percentile_cont_aggfinalfn
);

-- -- aggregate definition for percentiledisc(agtype, agytpe)
CREATE AGGREGATE ag_catalog.age_percentiledisc(agtype, agtype)
(
   stype = internal,
   sfunc = ag_catalog.age_percentile_aggtransfn,
   finalfunc = ag_catalog.age_percentile_disc_aggfinalfn
);

--
-- aggregate functions for collect(variadic "any")
--
-- collect transfer function
CREATE FUNCTION ag_catalog.age_collect_aggtransfn(internal, agtype)
RETURNS internal
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- collect final function
CREATE FUNCTION ag_catalog.age_collect_aggfinalfn(internal)
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- aggregate definition for age_collect(variadic "any")
CREATE AGGREGATE ag_catalog.age_collect(agtype)
(
    --stype = agtype,
    stype = internal,
    sfunc = ag_catalog.age_collect_aggtransfn,
    finalfunc = ag_catalog.age_collect_aggfinalfn
);

--
-- reduce(acc = init, var IN list | body) fold support
--
-- Transition function for the age_reduce aggregate. The fold body is compiled
-- by transform_cypher_reduce() with the accumulator and element rewritten to
-- PARAM_EXEC params 0 and 1 and serialized into the text argument; the
-- transition evaluates it for each element in list order. The trailing
-- agtype[] argument carries the loop-invariant outer values (outer-query
-- variables and cypher() parameters) referenced by the body, bound to
-- PARAM_EXEC params 2, 3, ... It must be callable with a NULL transition state
-- (no initcond), so it is intentionally not STRICT.
CREATE FUNCTION ag_catalog.age_reduce_transfn(agtype, agtype, text, agtype, agtype[])
    RETURNS agtype
    LANGUAGE c
AS 'MODULE_PATHNAME';

-- aggregate definition for reduce(); direct arguments are
-- (init, serialized-body, element, captured-outer-values), with the element
-- fed ORDER BY ordinality.
CREATE AGGREGATE ag_catalog.age_reduce(agtype, text, agtype, agtype[])
(
    stype = agtype,
    sfunc = ag_catalog.age_reduce_transfn
);


--
-- function for typecasting an agtype value to another agtype value
--
CREATE FUNCTION ag_catalog.agtype_typecast_int(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_numeric(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_float(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

-- typecast an agtype (integer or boolean) to an agtype boolean
CREATE FUNCTION ag_catalog.agtype_typecast_bool(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_vertex(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_edge(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_path(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_vle(IN agtype, IN agtype, IN agtype, IN agtype,
                                   IN agtype, IN agtype, IN agtype,
                                   OUT edges agtype)
RETURNS SETOF agtype
LANGUAGE C
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

-- This is an overloaded function definition to allow for the VLE local context
-- caching mechanism to coexist with the previous VLE version.
CREATE FUNCTION ag_catalog.age_vle(IN agtype, IN agtype, IN agtype, IN agtype,
                                   IN agtype, IN agtype, IN agtype, IN agtype,
                                   OUT edges agtype)
RETURNS SETOF agtype
LANGUAGE C
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

-- Unweighted (hop-count) shortest path between two vertices, computed over the
-- cached global graph adjacency via BFS. Returns a single path (0 or 1 rows).
-- Argument order mirrors the Cypher shortestPath() pattern
--   (a)-[:type*min_hops..max_hops]->(b):
--   graph, start, end, edge_types, direction, min_hops, max_hops
CREATE FUNCTION ag_catalog.age_shortest_path(IN agtype, IN agtype, IN agtype,
                                             IN agtype DEFAULT NULL,
                                             IN agtype DEFAULT NULL,
                                             IN agtype DEFAULT NULL,
                                             IN agtype DEFAULT NULL)
    RETURNS SETOF agtype
LANGUAGE C
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

-- All unweighted shortest paths between two vertices (one path per row).
-- Same argument order as age_shortest_path.
CREATE FUNCTION ag_catalog.age_all_shortest_paths(IN agtype, IN agtype, IN agtype,
                                                  IN agtype DEFAULT NULL,
                                                  IN agtype DEFAULT NULL,
                                                  IN agtype DEFAULT NULL,
                                                  IN agtype DEFAULT NULL)
    RETURNS SETOF agtype
LANGUAGE C
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

-- function to build an edge for a VLE match
CREATE FUNCTION ag_catalog.age_build_vle_match_edge(agtype, agtype)
RETURNS agtype
LANGUAGE C
STABLE
AS 'MODULE_PATHNAME';

-- function to match a terminal vle edge
CREATE FUNCTION ag_catalog.age_match_vle_terminal_edge(variadic "any")
RETURNS boolean
LANGUAGE C
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_match_vle_terminal_edge_arr(variadic "any")
RETURNS boolean
LANGUAGE C
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

-- function to create an AGTV_PATH from a VLE_path_container
CREATE FUNCTION ag_catalog.age_materialize_vle_path(agtype)
RETURNS agtype
LANGUAGE C
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

-- function to create an AGTV_ARRAY of edges from a VLE_path_container
CREATE FUNCTION ag_catalog.age_materialize_vle_edges(agtype)
RETURNS agtype
LANGUAGE C
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

-- function to create an AGTV_ARRAY of edges from a VLE_path_container
CREATE FUNCTION ag_catalog.age_materialize_vle_edges_arr(agtype)
RETURNS agtype
LANGUAGE C
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_match_vle_edge_to_id_qual(variadic "any")
RETURNS boolean
LANGUAGE C
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';


CREATE FUNCTION ag_catalog.age_match_two_vle_edges(agtype, agtype)
RETURNS boolean
LANGUAGE C
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_vertex_stats(agtype, agtype)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_graph_stats(agtype)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_delete_global_graphs(agtype)
RETURNS boolean
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.create_complete_graph(graph_name name, nodes int,
                                                 edge_label name,
                                                 node_label name = NULL)
RETURNS void
LANGUAGE c
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_create_barbell_graph(graph_name name,
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

-- list functions
CREATE FUNCTION ag_catalog.age_keys(agtype)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_labels(agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_nodes(agtype)
RETURNS agtype
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_relationships(agtype)
RETURNS agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_range(variadic "any")
RETURNS agtype
LANGUAGE c
IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_unnest(agtype)
    RETURNS SETOF agtype
    LANGUAGE c
    IMMUTABLE
AS 'MODULE_PATHNAME';

--
-- End
--

--
-- Key Existence Operators ? ?| ?&
--
CREATE FUNCTION ag_catalog.agtype_exists(agtype, text)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ? (
  LEFTARG = agtype,
  RIGHTARG = text,
  PROCEDURE = ag_catalog.agtype_exists,
  COMMUTATOR = '?',
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_agtype(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ? (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_exists_agtype,
  COMMUTATOR = '?',
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_any(agtype, text[])
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ?| (
  LEFTARG = agtype,
  RIGHTARG = text[],
  PROCEDURE = ag_catalog.agtype_exists_any,
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_any_agtype(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ?| (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_exists_any_agtype,
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_all(agtype, text[])
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ?& (
  LEFTARG = agtype,
  RIGHTARG = text[],
  PROCEDURE = ag_catalog.agtype_exists_all,
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_all_agtype(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR ?& (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_exists_all_agtype,
  RESTRICT = contsel,
  JOIN = contjoinsel
);

--
-- Contains operators @> <@
--
CREATE FUNCTION ag_catalog.agtype_contains(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR @> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_contains,
  COMMUTATOR = '<@',
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_contained_by(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <@ (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_contained_by,
  COMMUTATOR = '@>',
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_contains_top_level(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR @>> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_contains_top_level,
  COMMUTATOR = '<<@',
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_contained_by_top_level(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE OPERATOR <<@ (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  PROCEDURE = ag_catalog.agtype_contained_by_top_level,
  COMMUTATOR = '@>>',
  RESTRICT = contsel,
  JOIN = contjoinsel
);
--
-- agtype GIN support
--
CREATE FUNCTION ag_catalog.gin_compare_agtype(text, text)
    RETURNS int
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT;

CREATE FUNCTION gin_extract_agtype(agtype, internal)
    RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT;

CREATE FUNCTION ag_catalog.gin_extract_agtype_query(agtype, internal, int2,
                                                    internal, internal)
    RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT;

CREATE FUNCTION ag_catalog.gin_consistent_agtype(internal, int2, agtype, int4,
                                                 internal, internal)
    RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT;

CREATE FUNCTION ag_catalog.gin_triconsistent_agtype(internal, int2, agtype, int4,
                                                    internal, internal, internal)
    RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT;

CREATE OPERATOR CLASS ag_catalog.gin_agtype_ops
DEFAULT FOR TYPE agtype USING gin AS
  OPERATOR 7 @>(agtype, agtype),
  OPERATOR 8 <@(agtype, agtype),
  OPERATOR 9 ?(agtype, agtype),
  OPERATOR 10 ?|(agtype, agtype),
  OPERATOR 11 ?&(agtype, agtype),
  FUNCTION 1 ag_catalog.gin_compare_agtype(text,text),
  FUNCTION 2 ag_catalog.gin_extract_agtype(agtype, internal),
  FUNCTION 3 ag_catalog.gin_extract_agtype_query(agtype, internal, int2,
                                                 internal, internal),
  FUNCTION 4 ag_catalog.gin_consistent_agtype(internal, int2, agtype, int4,
                                              internal, internal),
  FUNCTION 6 ag_catalog.gin_triconsistent_agtype(internal, int2, agtype, int4,
                                                 internal, internal, internal),
STORAGE text;



-- migrated Apache AGE 1.7 scalar functions: isEmpty(), pi(), rand()
CREATE FUNCTION ag_catalog.age_isempty(agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_pi()
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_rand()
    RETURNS agtype
    LANGUAGE c
AS 'MODULE_PATHNAME';

--
-- create_subgraph(): materialized subgraph extraction.
--
-- Builds a new, persistent AGE graph that is the subgraph of an existing graph
-- selected by a node predicate and a relationship predicate. The semantics
-- follow the graph-theory "induced subgraph" definition as operationalized by
-- Neo4j GDS gds.graph.filter():
--
--   * a vertex is kept iff node_filter evaluates true ('*' keeps all);
--   * an edge is kept iff relationship_filter evaluates true AND BOTH of its
--     endpoints were kept (the induced rule -- no dangling edges).
--
-- Unlike the Neo4j in-memory projection, the result is a real, ACID,
-- fully-Cypher-queryable AGE graph; properties of any agtype are preserved, and
-- self-loops / parallel edges (multigraph structure) are kept.
--
-- node_filter / relationship_filter are Cypher predicates bound to a single
-- entity -- the node variable is `n`, the relationship variable is `r` -- or
-- the literal '*' to keep all. They are evaluated by AGE's own Cypher engine
-- against the source graph, so the full Cypher predicate language is available.
--
-- Internal entity ids (graphids) are reassigned in the new graph (a graphid
-- encodes the source graph's label id, which differs in the destination), and
-- edge endpoints are remapped accordingly. Properties are copied verbatim.
--
-- openGauss note: the plugin's ag_label catalog does not carry a seq_name
-- column, so the per-label id sequence is resolved from the label table's own
-- "id" column via pg_get_serial_sequence() (the sequence is OWNED BY that
-- column at label-creation time).
--
CREATE FUNCTION ag_catalog.create_subgraph(new_graph name,
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

-- Internal boundary self-test for the agehash Robin Hood open-addressing
-- hashtable (the hot-path cache primitive). Returns 'OK' or 'FAIL: <reason>'.
CREATE FUNCTION ag_catalog._agehash_self_test()
    RETURNS text
    LANGUAGE c
    VOLATILE
AS 'MODULE_PATHNAME';
