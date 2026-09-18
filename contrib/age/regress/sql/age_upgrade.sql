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

LOAD 'age';
SET search_path TO ag_catalog;

-- Exercise the supported extension upgrade path instead of inspecting only
-- the already-installed default version.
DROP EXTENSION age CASCADE;
CREATE EXTENSION age VERSION '1.0.0';
SET search_path TO ag_catalog;

CREATE ROLE age_upgrade_probe PASSWORD 'Gauss_123';

SELECT extversion AS version_before_upgrade
FROM pg_extension
WHERE extname = 'age';

SELECT pg_catalog.has_function_privilege(
           'age_upgrade_probe',
           'ag_catalog.load_labels_from_file(name,name,text,boolean)',
           'EXECUTE') AS loader_public_before_upgrade;

ALTER EXTENSION age UPDATE TO '1.0.1';

SELECT extversion AS version_after_upgrade
FROM pg_extension
WHERE extname = 'age';

SELECT count(*) = 0 AS legacy_loader_signatures_removed
FROM pg_catalog.pg_proc proc
JOIN pg_catalog.pg_namespace namespace
  ON namespace.oid = proc.pronamespace
WHERE namespace.nspname = 'ag_catalog'
  AND ((proc.proname = 'load_labels_from_file'
        AND proc.pronargs = 4)
       OR (proc.proname = 'load_edges_from_file'
           AND proc.pronargs = 3));

SELECT count(*) = 2 AS current_loader_signatures_present
FROM pg_catalog.pg_proc proc
JOIN pg_catalog.pg_namespace namespace
  ON namespace.oid = proc.pronamespace
WHERE namespace.nspname = 'ag_catalog'
  AND ((proc.proname = 'load_labels_from_file'
        AND proc.pronargs = 6)
       OR (proc.proname = 'load_edges_from_file'
           AND proc.pronargs = 5));

SELECT NOT pg_catalog.has_function_privilege(
               'age_upgrade_probe',
               'ag_catalog.load_labels_from_file(name,name,text,boolean,boolean,text)',
               'EXECUTE')
       AND NOT pg_catalog.has_function_privilege(
               'age_upgrade_probe',
               'ag_catalog.load_edges_from_file(name,name,text,boolean,text)',
               'EXECUTE') AS loaders_restricted_after_upgrade;

SELECT EXISTS (
    SELECT 1
    FROM pg_catalog.pg_proc p
    JOIN pg_catalog.pg_namespace n
      ON n.oid = p.pronamespace
    WHERE n.nspname = 'ag_catalog'
      AND p.proname = 'graph_exists'
      AND p.pronargs = 1
) AS graph_exists_created_by_upgrade;

SELECT EXISTS (
    SELECT 1
    FROM pg_catalog.pg_operator o
    JOIN pg_catalog.pg_namespace n
      ON n.oid = o.oprnamespace
    WHERE n.nspname = 'ag_catalog'
      AND o.oprname = '=~'
      AND o.oprleft = 'ag_catalog.agtype'::regtype
      AND o.oprright = 'ag_catalog.agtype'::regtype
) AS regex_operator_created_by_upgrade;

SELECT graph_exists('upgrade_graph_probe');
SELECT create_graph('upgrade_graph_probe');
SELECT graph_exists('upgrade_graph_probe');
SELECT '"upgrade"'::agtype OPERATOR(ag_catalog.=~) '"up.*"'::agtype;
SELECT drop_graph('upgrade_graph_probe', true);

SELECT EXISTS (
    SELECT 1
    FROM pg_catalog.pg_proc p
    JOIN pg_catalog.pg_depend d
      ON d.classid = 'pg_proc'::regclass
     AND d.objid = p.oid
     AND d.deptype = 'e'
    WHERE p.oid = 'ag_catalog.graph_exists(name)'::regprocedure
      AND d.refclassid = 'pg_extension'::regclass
      AND d.refobjid = (SELECT oid
                                 FROM pg_catalog.pg_extension
                                 WHERE extname = 'age')
) AS graph_exists_is_extension_member;

SELECT EXISTS (
    SELECT 1
    FROM pg_catalog.pg_operator o
    JOIN pg_catalog.pg_namespace n
      ON n.oid = o.oprnamespace
    JOIN pg_catalog.pg_depend d
      ON d.classid = 'pg_operator'::regclass
     AND d.objid = o.oid
     AND d.deptype = 'e'
    WHERE n.nspname = 'ag_catalog'
      AND o.oprname = '=~'
      AND o.oprleft = 'ag_catalog.agtype'::regtype
      AND o.oprright = 'ag_catalog.agtype'::regtype
      AND d.refclassid = 'pg_extension'::regclass
      AND d.refobjid = (SELECT oid
                                 FROM pg_catalog.pg_extension
                                 WHERE extname = 'age')
) AS regex_operator_is_extension_member;

DROP ROLE age_upgrade_probe;

SELECT installed_version, default_version
FROM pg_available_extensions
WHERE name = 'age';

SELECT count(*) AS available_versions
FROM pg_available_extension_versions
WHERE name = 'age';

SELECT count(*) > 0 AS has_functions
FROM pg_proc
WHERE pronamespace = (SELECT oid FROM pg_namespace WHERE nspname = 'ag_catalog');

SELECT count(*) > 0 AS has_types
FROM pg_type
WHERE typnamespace = (SELECT oid FROM pg_namespace WHERE nspname = 'ag_catalog')
  AND typname NOT LIKE 'pg_toast%';

SELECT count(*) > 0 AS has_operators
FROM pg_operator
WHERE oprnamespace = (SELECT oid FROM pg_namespace WHERE nspname = 'ag_catalog');

SELECT count(*) > 0 AS has_extension_members
FROM pg_depend d
WHERE d.deptype = 'e'
  AND d.refclassid = 'pg_extension'::regclass
  AND d.refobjid = (SELECT oid FROM pg_extension WHERE extname = 'age');

-- A fresh install must attach every SQL-visible function implemented by
-- age.so to the extension.  Otherwise DROP EXTENSION can leave executable
-- objects behind, or an install assembled from stale SQL can silently omit
-- ownership metadata.
\pset tuples_only true
SELECT count(*) = 0 AS all_c_functions_are_members
FROM pg_proc p
WHERE p.probin = '$libdir/age'
  AND NOT EXISTS (
      SELECT 1
      FROM pg_depend d
      WHERE d.classid = 'pg_proc'::regclass
        AND d.objid = p.oid
        AND d.refclassid = 'pg_extension'::regclass
        AND d.refobjid = (SELECT oid
                          FROM pg_extension
                          WHERE extname = 'age')
        AND d.deptype = 'e'
  );

SELECT count(*) = 0 AS no_c_functions_outside_ag_catalog
FROM pg_proc p
JOIN pg_namespace n ON n.oid = p.pronamespace
WHERE p.probin = '$libdir/age'
  AND n.nspname <> 'ag_catalog';

SELECT e.extowner = n.nspowner AS schema_owner_matches_extension
FROM pg_extension e
JOIN pg_namespace n ON n.nspname = 'ag_catalog'
WHERE e.extname = 'age';

WITH expected_immutable_function(function_name) AS (
    VALUES
        ('graphid_eq'),
        ('graphid_ne'),
        ('graphid_lt'),
        ('_agtype_build_vertex'),
        ('_agtype_build_edge'),
        ('age_id'),
        ('age_start_id'),
        ('age_end_id'),
        ('age_head'),
        ('age_last'),
        ('age_properties'),
        ('age_length'),
        ('age_toboolean'),
        ('age_tofloat'),
        ('age_reverse'),
        ('age_toupper'),
        ('age_tolower'),
        ('age_ltrim'),
        ('age_rtrim'),
        ('age_trim'),
        ('age_right'),
        ('age_left'),
        ('age_substring'),
        ('age_split'),
        ('age_replace'),
        ('age_sin'),
        ('age_cos'),
        ('age_tan'),
        ('age_cot'),
        ('age_asin'),
        ('age_acos'),
        ('age_atan2'),
        ('age_degrees'),
        ('age_radians'),
        ('age_floor'),
        ('age_log10'),
        ('age_e'),
        ('age_exp'),
        ('age_sqrt'),
        ('agtype_typecast_int'),
        ('agtype_typecast_numeric'),
        ('agtype_typecast_float'),
        ('agtype_typecast_vertex'),
        ('agtype_typecast_edge'),
        ('agtype_typecast_path'),
        ('age_atan'),
        ('age_round'),
        ('age_ceil'),
        ('age_range'),
        ('age_unnest'),
        ('age_tointeger'),
        ('age_tostring'),
        ('age_size'),
        ('age_tobooleanlist'),
        ('age_tail')
)
SELECT count(*) = 55 AND bool_and(p.provolatile = 'i')
       AS migrated_functions_are_immutable
FROM expected_immutable_function expected
JOIN pg_proc p ON p.proname = expected.function_name
JOIN pg_namespace n ON n.oid = p.pronamespace
WHERE n.nspname = 'ag_catalog';

SELECT count(*) = 2
       AND count(DISTINCT c.relname) = 2
       AND bool_and(c.relname IN ('ag_graph', 'ag_label'))
       AS catalog_tables_are_extension_config
FROM pg_extension e,
     unnest(e.extconfig) AS configured_table(configured_table_oid)
JOIN pg_class c ON c.oid = configured_table.configured_table_oid
WHERE e.extname = 'age';
\pset tuples_only false

SELECT tgname, tgenabled = 'A' AS fires_during_restore
FROM pg_catalog.pg_trigger
WHERE tgrelid IN ('ag_catalog.ag_graph'::regclass,
                  'ag_catalog.ag_label'::regclass)
  AND tgname IN ('trigger_ag_graph_restore', 'trigger_ag_label_restore')
ORDER BY tgname;

BEGIN;
CREATE SCHEMA age_restore_probe;
CREATE TABLE age_restore_probe.probe_label (id integer);
INSERT INTO ag_catalog.ag_graph (name, namespaceoid)
VALUES ('age_restore_probe',
        (SELECT oid FROM pg_catalog.pg_namespace WHERE nspname = 'pg_catalog'));
SELECT g.namespaceoid = n.oid AS graph_namespace_rebound
FROM ag_catalog.ag_graph g
JOIN pg_catalog.pg_namespace n ON n.nspname = g.name
WHERE g.name = 'age_restore_probe';
INSERT INTO ag_catalog.ag_label (name, graph, id, kind, relation)
VALUES ('probe_label', 1, 65000, 'v', 'age_restore_probe.probe_label'::regclass);
SELECT l.graph = g.oid AS label_graph_rebound
FROM ag_catalog.ag_label l
JOIN ag_catalog.ag_graph g ON g.name = 'age_restore_probe'
WHERE l.name = 'probe_label';
ROLLBACK;

SELECT EXISTS (
    SELECT 1
    FROM pg_available_extension_versions
    WHERE name = 'age'
      AND version = '1.0.0'
) AND EXISTS (
    SELECT 1
    FROM pg_available_extension_versions
    WHERE name = 'age'
      AND version = '1.0.1'
) AS supported_upgrade_versions_present;

-- Rollback must only remove cache triggers managed by AGE label relations.
-- A user trigger that depends on a 1.0.1-only function must block rollback
-- transactionally instead of being deleted behind the user's back.
SELECT create_graph('age_rollback_label_probe');
CREATE SCHEMA age_rollback_user_probe;
CREATE TABLE age_rollback_user_probe.probe_table (id integer);
CREATE TRIGGER _age_cache_invalidate
AFTER INSERT OR UPDATE OR DELETE OR TRUNCATE
ON age_rollback_user_probe.probe_table
FOR EACH STATEMENT
EXECUTE PROCEDURE ag_catalog.age_invalidate_graph_cache();

DO $rollback_dependency_probe$
BEGIN
    BEGIN
        ALTER EXTENSION age UPDATE TO '1.0.0';
        RAISE EXCEPTION 'AGE rollback unexpectedly ignored a user dependency';
    EXCEPTION
        WHEN dependent_objects_still_exist THEN
            NULL;
    END;
END
$rollback_dependency_probe$;

SELECT extversion = '1.0.1' AS rollback_dependency_kept_version
FROM pg_extension
WHERE extname = 'age';

SELECT EXISTS (
    SELECT 1
    FROM pg_catalog.pg_trigger trigger_object
    WHERE trigger_object.tgrelid =
              'age_rollback_user_probe.probe_table'::regclass
      AND trigger_object.tgname = '_age_cache_invalidate'
      AND trigger_object.tgfoid =
              'ag_catalog.age_invalidate_graph_cache()'::regprocedure
) AS user_trigger_preserved_after_failed_rollback;

SELECT count(*) > 0 AS age_label_triggers_restored_after_failed_rollback
FROM pg_catalog.pg_trigger trigger_object
WHERE trigger_object.tgname = '_age_cache_invalidate'
  AND trigger_object.tgfoid =
          'ag_catalog.age_invalidate_graph_cache()'::regprocedure
  AND EXISTS (
      SELECT 1
      FROM ag_catalog.ag_label label_object
      WHERE label_object.relation = trigger_object.tgrelid
  );

DROP TRIGGER _age_cache_invalidate
ON age_rollback_user_probe.probe_table;
DROP SCHEMA age_rollback_user_probe CASCADE;

ALTER EXTENSION age UPDATE TO '1.0.0';

SELECT extversion = '1.0.0' AS rollback_succeeds_after_dependency_cleanup
FROM pg_extension
WHERE extname = 'age';

SELECT count(*) = 0 AS age_label_triggers_removed_by_rollback
FROM pg_catalog.pg_trigger trigger_object
WHERE trigger_object.tgname = '_age_cache_invalidate'
  AND trigger_object.tgrelid IN (
      SELECT label_object.relation
      FROM ag_catalog.ag_label label_object
  );

ALTER EXTENSION age UPDATE TO '1.0.1';

SELECT extversion = '1.0.1' AS reupgrade_succeeds_after_rollback
FROM pg_extension
WHERE extname = 'age';

SELECT drop_graph('age_rollback_label_probe', true);
