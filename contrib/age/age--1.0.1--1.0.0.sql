/*
 * Complete openGauss AGE rollback script: 1.0.1 -> 1.0.0
 *
 * Only extension-owned catalog objects introduced or changed by the 1.0.1
 * update are reverted here. AGE graph catalogs, graph schemas, label tables,
 * graph rows, and user data are intentionally preserved.
 *
 * No DROP statement uses CASCADE. If an application object depends on an AGE
 * 1.0.1 object, the DROP fails and the surrounding inplace-upgrade
 * transaction restores the pre-rollback state.
 */

-- Do not discard an active AGE pg_upgrade preparation silently. The caller
-- must finish or explicitly revert that state before changing AGE versions.
DO $rollback$
BEGIN
    IF EXISTS (
        SELECT 1
        FROM pg_catalog.pg_class relation_object
        JOIN pg_catalog.pg_namespace namespace_object
          ON namespace_object.oid = relation_object.relnamespace
        WHERE namespace_object.nspname = 'ag_catalog'
          AND relation_object.relname = '_age_pg_upgrade_backup'
    ) THEN
        RAISE EXCEPTION
            'cannot roll back AGE while pg_upgrade preparation is active'
            USING HINT = 'Run age_finish_pg_upgrade() or age_revert_pg_upgrade_changes() first.';
    END IF;
END
$rollback$;

-- Remove restore triggers before removing their trigger procedures.
DROP TRIGGER IF EXISTS trigger_ag_graph_restore ON ag_catalog.ag_graph;
DROP TRIGGER IF EXISTS trigger_ag_label_restore ON ag_catalog.ag_label;

DO $rollback$
DECLARE
    trigger_record record;
BEGIN
    FOR trigger_record IN
        SELECT namespace_object.nspname AS schema_name,
               relation_object.relname AS table_name,
               trigger_object.tgname AS trigger_name
        FROM pg_catalog.pg_trigger trigger_object
        JOIN pg_catalog.pg_class relation_object
          ON relation_object.oid = trigger_object.tgrelid
        JOIN pg_catalog.pg_namespace namespace_object
          ON namespace_object.oid = relation_object.relnamespace
        WHERE trigger_object.tgname = '_age_cache_invalidate'
          AND trigger_object.tgfoid =
              'ag_catalog.age_invalidate_graph_cache()'::pg_catalog.regprocedure
          AND EXISTS (
              SELECT 1
              FROM ag_catalog.ag_label label_object
              WHERE label_object.relation = trigger_object.tgrelid
          )
    LOOP
        EXECUTE pg_catalog.format(
            'DROP TRIGGER %I ON %I.%I',
            trigger_record.trigger_name,
            trigger_record.schema_name,
            trigger_record.table_name
        );
    END LOOP;
END
$rollback$;

-- Drop only extension-owned operators introduced by 1.0.1. The script is
-- executed with creating_extension=true, so openGauss allows deletion of
-- objects that are still members of the extension. Keeping membership intact
-- until deletion also preserves normal dependency protection for user objects.
DO $rollback$
DECLARE
    operator_record record;
BEGIN
    FOR operator_record IN
        SELECT operator_object.oid,
               operator_object.oprname,
               pg_catalog.format_type(operator_object.oprleft, NULL) AS left_type,
               pg_catalog.format_type(operator_object.oprright, NULL) AS right_type
        FROM pg_catalog.pg_operator operator_object
        WHERE operator_object.oprnamespace = (
                  SELECT namespace_object.oid
                  FROM pg_catalog.pg_namespace namespace_object
                  WHERE namespace_object.nspname = 'ag_catalog'
              )
          AND (
              (operator_object.oprname IN ('=', '<>')
               AND operator_object.oprleft IN (
                   'ag_catalog.vertex'::pg_catalog.regtype,
                   'ag_catalog.edge'::pg_catalog.regtype
               )
               AND operator_object.oprright = operator_object.oprleft)
              OR (operator_object.oprname IN ('->', '->>', '#>', '#>>')
                  AND operator_object.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                  AND operator_object.oprright IN (
                      'pg_catalog.text'::pg_catalog.regtype,
                      'ag_catalog.agtype'::pg_catalog.regtype,
                      'pg_catalog.int4'::pg_catalog.regtype
                  ))
              OR (operator_object.oprname IN ('=~', '@>>', '<<@')
                  AND operator_object.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                  AND operator_object.oprright = 'ag_catalog.agtype'::pg_catalog.regtype)
              OR (operator_object.oprname = '||'
                  AND operator_object.oprleft = 'ag_catalog.agtype'::pg_catalog.regtype
                  AND operator_object.oprright = 'ag_catalog.agtype'::pg_catalog.regtype)
          )
          AND EXISTS (
              SELECT 1
              FROM pg_catalog.pg_depend dependency
              JOIN pg_catalog.pg_extension extension_object
                ON extension_object.oid = dependency.refobjid
              WHERE dependency.classid = 'pg_catalog.pg_operator'::pg_catalog.regclass
                AND dependency.objid = operator_object.oid
                AND dependency.refclassid = 'pg_catalog.pg_extension'::pg_catalog.regclass
                AND dependency.deptype = 'e'
                AND extension_object.extname = 'age'
          )
    LOOP
        EXECUTE pg_catalog.format(
            'DROP OPERATOR ag_catalog.%s (%s, %s)',
            operator_record.oprname,
            operator_record.left_type,
            operator_record.right_type
        );
    END LOOP;
END
$rollback$;

-- Remove only extension-owned casts introduced by 1.0.1.
DO $rollback$
DECLARE
    cast_record record;
BEGIN
    FOR cast_record IN
        SELECT cast_object.oid,
               pg_catalog.format_type(cast_object.castsource, NULL) AS source_type,
               pg_catalog.format_type(cast_object.casttarget, NULL) AS target_type
        FROM pg_catalog.pg_cast cast_object
        WHERE (
              (cast_object.castsource = 'ag_catalog.vertex'::pg_catalog.regtype
               AND cast_object.casttarget IN (
                   'ag_catalog.agtype'::pg_catalog.regtype,
                   'pg_catalog.json'::pg_catalog.regtype,
                   'pg_catalog.jsonb'::pg_catalog.regtype
               ))
              OR (cast_object.castsource = 'ag_catalog.edge'::pg_catalog.regtype
                  AND cast_object.casttarget IN (
                      'ag_catalog.agtype'::pg_catalog.regtype,
                      'pg_catalog.json'::pg_catalog.regtype,
                      'pg_catalog.jsonb'::pg_catalog.regtype
                  ))
              OR (cast_object.castsource = 'pg_catalog.text'::pg_catalog.regtype
                  AND cast_object.casttarget = 'ag_catalog.agtype'::pg_catalog.regtype)
              OR (cast_object.castsource = 'pg_catalog.int4'::pg_catalog.regtype
                  AND cast_object.casttarget = 'ag_catalog.agtype'::pg_catalog.regtype)
              OR (cast_object.castsource = 'ag_catalog.agtype'::pg_catalog.regtype
                  AND cast_object.casttarget IN (
                      'pg_catalog.json'::pg_catalog.regtype,
                      'pg_catalog.jsonb'::pg_catalog.regtype,
                      'pg_catalog.vector'::pg_catalog.regtype,
                      'pg_catalog.halfvec'::pg_catalog.regtype,
                      'pg_catalog.sparsevec'::pg_catalog.regtype
                  ))
              OR (cast_object.castsource = 'pg_catalog.jsonb'::pg_catalog.regtype
                  AND cast_object.casttarget = 'ag_catalog.agtype'::pg_catalog.regtype)
              OR (cast_object.castsource = 'ag_catalog.agtype[]'::pg_catalog.regtype
                  AND cast_object.casttarget = 'ag_catalog.agtype'::pg_catalog.regtype)
          )
          AND EXISTS (
              SELECT 1
              FROM pg_catalog.pg_depend dependency
              JOIN pg_catalog.pg_extension extension_object
                ON extension_object.oid = dependency.refobjid
              WHERE dependency.classid = 'pg_catalog.pg_cast'::pg_catalog.regclass
                AND dependency.objid = cast_object.oid
                AND dependency.refclassid = 'pg_catalog.pg_extension'::pg_catalog.regclass
                AND dependency.deptype = 'e'
                AND extension_object.extname = 'age'
          )
    LOOP
        EXECUTE pg_catalog.format(
            'DROP CAST (%s AS %s)',
            cast_record.source_type,
            cast_record.target_type
        );
    END LOOP;
END
$rollback$;

-- Remove the 1.0.1-only reduce aggregate before its transition function.
DROP AGGREGATE IF EXISTS ag_catalog.age_reduce(
    ag_catalog.agtype,
    pg_catalog.text,
    ag_catalog.agtype,
    ag_catalog.agtype[]
);

-- Remove extension-owned functions that are absent from 1.0.0, plus the
-- current signatures replaced by 1.0.1. The membership predicate prevents
-- accidental removal of an unrelated same-name function.
DO $rollback$
DECLARE
    function_record record;
BEGIN
    FOR function_record IN
        SELECT procedure_object.oid,
               namespace_object.nspname,
               procedure_object.proname,
               pg_catalog.pg_get_function_identity_arguments(procedure_object.oid)
                   AS identity_arguments
        FROM pg_catalog.pg_proc procedure_object
        JOIN pg_catalog.pg_namespace namespace_object
          ON namespace_object.oid = procedure_object.pronamespace
        WHERE namespace_object.nspname = 'ag_catalog'
          AND procedure_object.proname = ANY (ARRAY[
              '_ag_enforce_edge_uniqueness2',
              '_ag_enforce_edge_uniqueness3',
              '_ag_enforce_edge_uniqueness4',
              '_age_invalidate_graph_cache',
              '_agehash_self_test',
              '_agtype_build_edge',
              '_agtype_build_vertex',
              '_get_vertex_by_graphid',
              'age_all_shortest_paths',
              'age_create_barbell_graph',
              'age_finish_pg_upgrade',
              'age_graph_stats',
              'age_invalidate_graph_cache',
              'age_is_valid_label_name',
              'age_isempty',
              'age_pg_upgrade_status',
              'age_pi',
              'age_prepare_cypher',
              'age_prepare_pg_upgrade',
              'age_rand',
              'age_rebind_graph_namespace_on_restore',
              'age_rebind_label_graph_on_restore',
              'age_reduce_item',
              'age_reduce_size',
              'age_reduce_transfn',
              'age_revert_pg_upgrade_changes',
              'age_shortest_path',
              'age_tail',
              'age_tofloatlist',
              'age_tointegerlist',
              'age_tostringlist',
              'age_unnest',
              'agtype_array_element',
              'agtype_array_element_text',
              'agtype_array_to_agtype',
              'agtype_build_map_nonull',
              'agtype_concat',
              'agtype_contained_by_top_level',
              'agtype_contains_top_level',
              'agtype_extract_path',
              'agtype_extract_path_text',
              'agtype_object_field',
              'agtype_object_field_agtype',
              'agtype_object_field_text',
              'agtype_object_field_text_agtype',
              'agtype_to_halfvec',
              'agtype_to_json',
              'agtype_to_jsonb',
              'agtype_to_sparsevec',
              'agtype_to_vector',
              'agtype_typecast_bool',
              'create_complete_graph',
              'create_elabel',
              'create_subgraph',
              'create_vlabel',
              'edge_eq',
              'edge_ne',
              'edge_to_agtype',
              'edge_to_json',
              'edge_to_jsonb',
              'graph_exists',
              'int4_to_agtype',
              'jsonb_to_agtype',
              'text_to_agtype',
              'vertex_eq',
              'vertex_ne',
              'vertex_to_agtype',
              'vertex_to_json',
              'vertex_to_jsonb',
              'cypher',
              '_label_name',
              'agtype_volatile_wrapper',
              'load_edges_from_file',
              'load_labels_from_file'
          ])
          AND (
              procedure_object.proname <> 'cypher'
              OR procedure_object.pronargs < 3
          )
          AND EXISTS (
              SELECT 1
              FROM pg_catalog.pg_depend dependency
              JOIN pg_catalog.pg_extension extension_object
                ON extension_object.oid = dependency.refobjid
              WHERE dependency.classid = 'pg_catalog.pg_proc'::pg_catalog.regclass
                AND dependency.objid = procedure_object.oid
                AND dependency.refclassid = 'pg_catalog.pg_extension'::pg_catalog.regclass
                AND dependency.deptype = 'e'
                AND extension_object.extname = 'age'
          )
        ORDER BY CASE
                     WHEN procedure_object.proname IN (
                         'create_subgraph', 'age_finish_pg_upgrade', 'agtype_to_jsonb'
                     ) THEN 0
                     WHEN procedure_object.proname IN (
                         '_age_invalidate_graph_cache', 'agtype_to_json'
                     ) THEN 1
                     ELSE 2
                 END,
                 procedure_object.oid
    LOOP
        EXECUTE pg_catalog.format(
            'DROP FUNCTION %I.%I(%s)',
            function_record.nspname,
            function_record.proname,
            function_record.identity_arguments
        );
    END LOOP;
END
$rollback$;

-- Drop the 1.0.1-only entity types last. A user object depending on either
-- type aborts the rollback instead of being removed silently.
DO $rollback$
DECLARE
    type_record record;
BEGIN
    FOR type_record IN
        SELECT type_object.oid,
               namespace_object.nspname,
               type_object.typname
        FROM pg_catalog.pg_type type_object
        JOIN pg_catalog.pg_namespace namespace_object
          ON namespace_object.oid = type_object.typnamespace
        WHERE namespace_object.nspname = 'ag_catalog'
          AND type_object.typname IN ('vertex', 'edge')
          AND EXISTS (
              SELECT 1
              FROM pg_catalog.pg_depend dependency
              JOIN pg_catalog.pg_extension extension_object
                ON extension_object.oid = dependency.refobjid
              WHERE dependency.classid = 'pg_catalog.pg_type'::pg_catalog.regclass
                AND dependency.objid = type_object.oid
                AND dependency.refclassid = 'pg_catalog.pg_extension'::pg_catalog.regclass
                AND dependency.deptype = 'e'
                AND extension_object.extname = 'age'
          )
    LOOP
        EXECUTE pg_catalog.format(
            'DROP TYPE %I.%I',
            type_record.nspname,
            type_record.typname
        );
    END LOOP;
END
$rollback$;

-- Restore the 1.0.0 definitions for changed signatures.
CREATE FUNCTION ag_catalog._label_name(graph_oid oid, graphid)
RETURNS cstring
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.create_vlabel(graph_name name, label_name name)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.create_elabel(graph_name name, label_name name)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.load_labels_from_file(
    graph_name name,
    label_name name,
    file_path text,
    id_field_exists bool default true
)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.load_edges_from_file(
    graph_name name,
    label_name name,
    file_path text
)
RETURNS void
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._agtype_build_vertex(graphid, cstring, agtype)
RETURNS agtype
LANGUAGE c
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._agtype_build_edge(
    graphid,
    graphid,
    graphid,
    cstring,
    agtype
)
RETURNS agtype
LANGUAGE c
STABLE
CALLED ON NULL INPUT
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_volatile_wrapper(agt agtype)
RETURNS agtype AS $return_value$
BEGIN
	RETURN agt;
END;
$return_value$ LANGUAGE plpgsql
VOLATILE
CALLED ON NULL INPUT;

CREATE FUNCTION ag_catalog.age_unnest(agtype, block_types boolean = false)
RETURNS SETOF agtype
LANGUAGE c
STABLE
AS 'MODULE_PATHNAME';

-- Restore the 1.0.0 default PUBLIC EXECUTE privilege for these unchanged
-- loader entry points, which 1.0.1 explicitly restricted.
GRANT EXECUTE ON FUNCTION ag_catalog.load_labels_from_file_with_analysefile(
    name,
    name,
    text,
    bool
) TO PUBLIC;
GRANT EXECUTE ON FUNCTION ag_catalog.load_edges_from_file_with_analysefile(
    name,
    name,
    text
) TO PUBLIC;

-- Restore the 1.0.0 volatility for functions whose signatures are unchanged.
ALTER FUNCTION ag_catalog.graphid_eq(graphid, graphid) STABLE;
ALTER FUNCTION ag_catalog.graphid_ne(graphid, graphid) STABLE;
ALTER FUNCTION ag_catalog.graphid_lt(graphid, graphid) STABLE;
ALTER FUNCTION ag_catalog.age_acos("any") STABLE;
ALTER FUNCTION ag_catalog.age_asin("any") STABLE;
ALTER FUNCTION ag_catalog.age_atan("any") STABLE;
ALTER FUNCTION ag_catalog.age_atan2("any") STABLE;
ALTER FUNCTION ag_catalog.age_ceil("any") STABLE;
ALTER FUNCTION ag_catalog.age_cos("any") STABLE;
ALTER FUNCTION ag_catalog.age_cot("any") STABLE;
ALTER FUNCTION ag_catalog.age_degrees("any") STABLE;
ALTER FUNCTION ag_catalog.age_e() STABLE;
ALTER FUNCTION ag_catalog.age_end_id(agtype) STABLE;
ALTER FUNCTION ag_catalog.age_exp("any") STABLE;
ALTER FUNCTION ag_catalog.age_floor("any") STABLE;
ALTER FUNCTION ag_catalog.age_head(agtype) STABLE;
ALTER FUNCTION ag_catalog.age_id(agtype) STABLE;
ALTER FUNCTION ag_catalog.age_last(agtype) STABLE;
ALTER FUNCTION ag_catalog.age_left("any") STABLE;
ALTER FUNCTION ag_catalog.age_length(agtype) STABLE;
ALTER FUNCTION ag_catalog.age_log10("any") STABLE;
ALTER FUNCTION ag_catalog.age_ltrim("any") STABLE;
ALTER FUNCTION ag_catalog.age_properties(agtype) STABLE;
ALTER FUNCTION ag_catalog.age_radians("any") STABLE;
ALTER FUNCTION ag_catalog.age_range("any") STABLE;
ALTER FUNCTION ag_catalog.age_replace("any") STABLE;
ALTER FUNCTION ag_catalog.age_reverse("any") STABLE;
ALTER FUNCTION ag_catalog.age_right("any") STABLE;
ALTER FUNCTION ag_catalog.age_round("any") STABLE;
ALTER FUNCTION ag_catalog.age_rtrim("any") STABLE;
ALTER FUNCTION ag_catalog.age_sin("any") STABLE;
ALTER FUNCTION ag_catalog.age_size("any") STABLE;
ALTER FUNCTION ag_catalog.age_split("any") STABLE;
ALTER FUNCTION ag_catalog.age_sqrt("any") STABLE;
ALTER FUNCTION ag_catalog.age_start_id(agtype) STABLE;
ALTER FUNCTION ag_catalog.age_substring("any") STABLE;
ALTER FUNCTION ag_catalog.age_tan("any") STABLE;
ALTER FUNCTION ag_catalog.age_toboolean("any") STABLE;
ALTER FUNCTION ag_catalog.age_tobooleanlist("any") STABLE;
ALTER FUNCTION ag_catalog.age_tofloat("any") STABLE;
ALTER FUNCTION ag_catalog.age_tointeger("any") STABLE;
ALTER FUNCTION ag_catalog.age_tolower("any") STABLE;
ALTER FUNCTION ag_catalog.age_tostring("any") STABLE;
ALTER FUNCTION ag_catalog.age_toupper("any") STABLE;
ALTER FUNCTION ag_catalog.age_trim("any") STABLE;
ALTER FUNCTION ag_catalog.agtype_typecast_edge("any") STABLE;
ALTER FUNCTION ag_catalog.agtype_typecast_float("any") STABLE;
ALTER FUNCTION ag_catalog.agtype_typecast_int("any") STABLE;
ALTER FUNCTION ag_catalog.agtype_typecast_numeric("any") STABLE;
ALTER FUNCTION ag_catalog.agtype_typecast_path("any") STABLE;
ALTER FUNCTION ag_catalog.agtype_typecast_vertex("any") STABLE;
