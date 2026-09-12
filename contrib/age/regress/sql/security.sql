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

-- Security smoke for openGauss AGE 1.0-era permissions.
SELECT create_graph('security_smoke');

SELECT * FROM cypher('security_smoke', $$
    CREATE (:Person {name: 'Alice'}), (:Person {name: 'Bob'})
$$) AS (a agtype);

SELECT * FROM cypher('security_smoke', $$
    MATCH (a:Person {name: 'Alice'}), (b:Person {name: 'Bob'})
    CREATE (a)-[:KNOWS]->(b)
$$) AS (a agtype);

CREATE ROLE security_smoke_readonly PASSWORD 'Gauss_123';
GRANT USAGE ON SCHEMA security_smoke TO security_smoke_readonly;
GRANT SELECT ON ALL TABLES IN SCHEMA security_smoke TO security_smoke_readonly;
GRANT USAGE ON SCHEMA ag_catalog TO security_smoke_readonly;
GRANT SELECT ON ALL TABLES IN SCHEMA ag_catalog TO security_smoke_readonly;

CREATE ROLE security_smoke_detach PASSWORD 'Gauss_123';
GRANT USAGE ON SCHEMA security_smoke TO security_smoke_detach;
GRANT SELECT ON ALL TABLES IN SCHEMA security_smoke TO security_smoke_detach;
GRANT DELETE ON security_smoke."Person" TO security_smoke_detach;
GRANT USAGE ON SCHEMA ag_catalog TO security_smoke_detach;
GRANT SELECT ON ALL TABLES IN SCHEMA ag_catalog TO security_smoke_detach;

SET ROLE security_smoke_readonly PASSWORD 'Gauss_123';

-- SELECT should work with read-only permissions.
SELECT * FROM cypher('security_smoke', $$
    MATCH (p:Person) RETURN p.name ORDER BY p.name
$$) AS (name agtype);

-- CREATE should fail without INSERT permission on the label table.
SELECT * FROM cypher('security_smoke', $$
    CREATE (:Person {name: 'Carol'})
$$) AS (a agtype);

-- SET and REMOVE must fail without UPDATE permission on the label table.
SELECT * FROM cypher('security_smoke', $$
    MATCH (p:Person {name: 'Bob'}) SET p.name = 'Robert'
$$) AS (a agtype);

SELECT * FROM cypher('security_smoke', $$
    MATCH (p:Person {name: 'Bob'}) REMOVE p.name
$$) AS (a agtype);

-- Plain DELETE must fail without DELETE permission on the label table.
SELECT * FROM cypher('security_smoke', $$
    MATCH (p:Person {name: 'Bob'}) DELETE p
$$) AS (a agtype);

RESET ROLE;

SET ROLE security_smoke_detach PASSWORD 'Gauss_123';

-- DETACH DELETE must fail without DELETE on the connected edge label.
SELECT * FROM cypher('security_smoke', $$
    MATCH (p:Person {name: 'Alice'}) DETACH DELETE p
$$) AS (a agtype);

RESET ROLE;

GRANT DELETE ON security_smoke."KNOWS" TO security_smoke_detach;

CREATE FUNCTION ag_catalog.security_smoke_deny_edge_delete()
RETURNS boolean
LANGUAGE sql
STABLE
AS $$ SELECT false $$;

ALTER TABLE security_smoke."KNOWS" ENABLE ROW LEVEL SECURITY;
CREATE ROW LEVEL SECURITY POLICY security_smoke_knows_select
    ON security_smoke."KNOWS" AS PERMISSIVE FOR SELECT
    TO security_smoke_detach USING (true);
CREATE ROW LEVEL SECURITY POLICY security_smoke_knows_delete
    ON security_smoke."KNOWS" AS RESTRICTIVE FOR DELETE
    TO security_smoke_detach
    USING (ag_catalog.security_smoke_deny_edge_delete());

SET ROLE security_smoke_detach PASSWORD 'Gauss_123';

-- RLS must abort the whole DETACH DELETE rather than leave a dangling edge.
SELECT * FROM cypher('security_smoke', $$
    MATCH (p:Person {name: 'Alice'}) DETACH DELETE p
$$) AS (a agtype);

RESET ROLE;

SELECT count(*) AS person_count_after_rls_failure
FROM security_smoke."Person";
SELECT count(*) AS edge_count_after_rls_failure
FROM security_smoke."KNOWS";
SELECT count(*) AS dangling_edge_count_after_rls_failure
FROM security_smoke."KNOWS" edge
LEFT JOIN security_smoke."Person" source_vertex
    ON source_vertex.id = edge.start_id
LEFT JOIN security_smoke."Person" target_vertex
    ON target_vertex.id = edge.end_id
WHERE source_vertex.id IS NULL OR target_vertex.id IS NULL;

DROP ROW LEVEL SECURITY POLICY security_smoke_knows_delete
    ON security_smoke."KNOWS";
DROP ROW LEVEL SECURITY POLICY security_smoke_knows_select
    ON security_smoke."KNOWS";
ALTER TABLE security_smoke."KNOWS" DISABLE ROW LEVEL SECURITY;
DROP FUNCTION ag_catalog.security_smoke_deny_edge_delete();

SET ROLE security_smoke_detach PASSWORD 'Gauss_123';

-- After edge DELETE is granted and RLS allows it, DETACH DELETE succeeds.
SELECT * FROM cypher('security_smoke', $$
    MATCH (p:Person {name: 'Alice'}) DETACH DELETE p
$$) AS (a agtype);

RESET ROLE;

-- The heap-level CREATE path follows the kernel RLS bypass rule: the label
-- owner is not subject to the policies (no FORCE ROW LEVEL SECURITY) and may
-- create, a role the policies apply to is rejected.
ALTER TABLE security_smoke."Person" ENABLE ROW LEVEL SECURITY;
CREATE ROW LEVEL SECURITY POLICY security_smoke_person_insert
    ON security_smoke."Person" AS RESTRICTIVE
    TO security_smoke_detach USING (false);
SELECT * FROM cypher('security_smoke', $$
    CREATE (:Person {name: 'Dave'})
$$) AS (a agtype);
GRANT INSERT ON security_smoke."Person" TO security_smoke_detach;
GRANT USAGE ON SEQUENCE security_smoke."Person_id_seq" TO security_smoke_detach;
SET ROLE security_smoke_detach PASSWORD 'Gauss_123';
SELECT * FROM cypher('security_smoke', $$
    CREATE (:Person {name: 'Eve'})
$$) AS (a agtype);
RESET ROLE;
SELECT count(*) AS person_count_after_rls_create
FROM security_smoke."Person";
DROP ROW LEVEL SECURITY POLICY security_smoke_person_insert
    ON security_smoke."Person";
ALTER TABLE security_smoke."Person" DISABLE ROW LEVEL SECURITY;

SELECT drop_graph('security_smoke', true);
REVOKE SELECT ON ALL TABLES IN SCHEMA ag_catalog FROM security_smoke_readonly;
REVOKE USAGE ON SCHEMA ag_catalog FROM security_smoke_readonly;
REVOKE SELECT ON ALL TABLES IN SCHEMA ag_catalog FROM security_smoke_detach;
REVOKE USAGE ON SCHEMA ag_catalog FROM security_smoke_detach;
DROP ROLE security_smoke_readonly;
DROP ROLE security_smoke_detach;
