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

-- pg_upgrade helpers resolve built-ins from pg_catalog before ag_catalog.
SELECT p.proname,
       array_to_string(p.proconfig, ', ') AS proconfig
FROM pg_proc p
JOIN pg_namespace n ON n.oid = p.pronamespace
WHERE n.nspname = 'ag_catalog'
  AND p.proname IN ('age_prepare_pg_upgrade', 'age_finish_pg_upgrade',
                    'age_revert_pg_upgrade_changes', 'age_pg_upgrade_status')
ORDER BY p.proname;

-- Security-sensitive built-ins are explicitly schema-qualified in helper bodies.
SELECT p.proname,
       (pg_catalog.strpos(
            pg_catalog.replace(p.prosrc, 'pg_catalog.hashtext(', ''),
            'hashtext('
        ) > 0) AS has_unqualified_hashtext
FROM pg_proc p
JOIN pg_namespace n ON n.oid = p.pronamespace
WHERE n.nspname = 'ag_catalog'
  AND p.proname IN ('age_prepare_pg_upgrade', 'age_finish_pg_upgrade',
                    'age_revert_pg_upgrade_changes')
ORDER BY p.proname;

-- Probe the install guard's ownership predicate without disturbing ag_catalog.
CREATE ROLE age_probe_role PASSWORD 'Gauss_123';
CREATE SCHEMA age_probe AUTHORIZATION age_probe_role;

-- Maintenance entry points and the internal cache primitive are owner-only.
SELECT p.proname,
       pg_catalog.has_function_privilege('age_probe_role', p.oid, 'EXECUTE')
           AS probe_can_execute
FROM pg_proc p
JOIN pg_namespace n ON n.oid = p.pronamespace
WHERE n.nspname = 'ag_catalog'
  AND p.proname IN ('_age_invalidate_graph_cache',
                    'age_prepare_pg_upgrade', 'age_finish_pg_upgrade',
                    'age_revert_pg_upgrade_changes', 'age_pg_upgrade_status')
ORDER BY p.proname;

SELECT EXISTS (
    SELECT 1
    FROM pg_catalog.pg_namespace n
    WHERE n.nspname = 'age_probe'
      AND n.nspowner <> (SELECT r.oid FROM pg_catalog.pg_roles r
                         WHERE r.rolname = current_user)
) AS foreign_owner_detected;

SELECT EXISTS (
    SELECT 1
    FROM pg_catalog.pg_namespace n
    WHERE n.nspname = 'ag_catalog'
      AND n.nspowner <> (SELECT r.oid FROM pg_catalog.pg_roles r
                         WHERE r.rolname = current_user)
) AS installer_owned_flagged;

DROP SCHEMA age_probe;
DROP ROLE age_probe_role;

-- Exercise the openGauss-specific pg_upgrade state machine.
SELECT create_graph('age_upgrade_a');
SELECT create_graph('age_upgrade_b');

SELECT status, namespace_type, backup_exists
FROM age_pg_upgrade_status();

SET client_min_messages TO WARNING;
SELECT age_prepare_pg_upgrade();
RESET client_min_messages;

SELECT status, namespace_type, backup_exists
FROM age_pg_upgrade_status();

-- Repeating prepare cannot overwrite the stable-name snapshot.
SELECT age_prepare_pg_upgrade();

SELECT count(*) = (SELECT count(*) FROM ag_catalog.ag_graph)
       AS backup_covers_all_graphs
FROM ag_catalog._age_pg_upgrade_backup b
JOIN ag_catalog.ag_graph g
  ON g.oid = b.old_graph_oid
 AND g.name = b.graph_name
JOIN pg_catalog.pg_namespace n
  ON n.nspname = b.namespace_name;

-- A graph created after prepare is detected as a state mismatch.
SELECT create_graph('age_upgrade_late');
SELECT status, namespace_type, backup_exists
FROM age_pg_upgrade_status();
SELECT drop_graph('age_upgrade_late', true);
SELECT status, namespace_type, backup_exists
FROM age_pg_upgrade_status();

-- Simulate the stale namespace OID that a major-version upgrade can leave.
ALTER TABLE ag_catalog.ag_graph DISABLE TRIGGER trigger_ag_graph_update;
UPDATE ag_catalog.ag_graph
SET namespaceoid = (SELECT oid FROM pg_catalog.pg_namespace
                    WHERE nspname = 'pg_catalog')
WHERE name = 'age_upgrade_a';
ALTER TABLE ag_catalog.ag_graph ENABLE TRIGGER trigger_ag_graph_update;

SELECT g.namespaceoid <> n.oid AS namespace_repair_needed
FROM ag_catalog.ag_graph g
JOIN ag_catalog._age_pg_upgrade_backup b ON b.old_graph_oid = g.oid
JOIN pg_catalog.pg_namespace n ON n.nspname = b.namespace_name
WHERE g.name = 'age_upgrade_a';

-- Prime the graph cache with the stale namespace before finish repairs it.
SELECT ag_catalog._age_invalidate_graph_cache();
SELECT * FROM cypher('age_upgrade_a', $$ RETURN 1 $$) AS (result agtype);

SET client_min_messages TO WARNING;
SELECT age_finish_pg_upgrade();
RESET client_min_messages;

SELECT g.namespaceoid = n.oid AS namespace_repaired
FROM ag_catalog.ag_graph g
JOIN pg_catalog.pg_namespace n ON n.nspname = g.name
WHERE g.name = 'age_upgrade_a';

SELECT status, namespace_type, backup_exists
FROM age_pg_upgrade_status();

-- Cache invalidation leaves the graph immediately usable after repair.
SELECT * FROM cypher('age_upgrade_a', $$ RETURN 2 $$) AS (result agtype);

-- Revert only removes preparation state because namespaceoid is already oid.
SET client_min_messages TO WARNING;
SELECT age_prepare_pg_upgrade();
RESET client_min_messages;
SELECT status, namespace_type, backup_exists
FROM age_pg_upgrade_status();
SET client_min_messages TO WARNING;
SELECT age_revert_pg_upgrade_changes();
RESET client_min_messages;
SELECT status, namespace_type, backup_exists
FROM age_pg_upgrade_status();

-- Revert without a prepared snapshot is rejected.
SELECT age_revert_pg_upgrade_changes();

SELECT drop_graph('age_upgrade_a', true);
SELECT drop_graph('age_upgrade_b', true);

-- finish without a prepared snapshot fails cleanly.
SELECT age_finish_pg_upgrade();
