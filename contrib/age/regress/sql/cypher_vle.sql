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

SELECT create_graph('cypher_vle');

--
-- Create table to hold the start and end vertices to test the SRF
--

CREATE TABLE start_and_end_points (start_vertex agtype, end_vertex agtype);

-- Create a graph to test
SELECT * FROM cypher('cypher_vle', $$CREATE (b:begin)-[:edge {name: 'main edge', number: 1, dangerous: {type: "all", level: "all"}}]->(u1:middle)-[:edge {name: 'main edge', number: 2, dangerous: {type: "all", level: "all"}, packages: [2,4,6]}]->(u2:middle)-[:edge {name: 'main edge', number: 3, dangerous: {type: "all", level: "all"}}]->(u3:middle)-[:edge {name: 'main edge', number: 4, dangerous: {type: "all", level: "all"}}]->(e:end), (u1)-[:self_loop {name: 'self loop', number: 1, dangerous: {type: "all", level: "all"}}]->(u1), (e)-[:self_loop {name: 'self loop', number: 2, dangerous: {type: "all", level: "all"}}]->(e), (b)-[:alternate_edge {name: 'alternate edge', number: 1, packages: [2,4,6], dangerous: {type: "poisons", level: "all"}}]->(u1), (u2)-[:alternate_edge {name: 'alternate edge', number: 2, packages: [2,4,6], dangerous: {type: "poisons", level: "all"}}]->(u3), (u3)-[:alternate_edge {name: 'alternate edge', number: 3, packages: [2,4,6], dangerous: {type: "poisons", level: "all"}}]->(e), (u2)-[:bypass_edge {name: 'bypass edge', number: 1, packages: [1,3,5,7]}]->(e), (e)-[:alternate_edge {name: 'backup edge', number: 1, packages: [1,3,5,7]}]->(u3), (u3)-[:alternate_edge {name: 'backup edge', number: 2, packages: [1,3,5,7]}]->(u2), (u2)-[:bypass_edge {name: 'bypass edge', number: 2, packages: [1,3,5,7], dangerous: {type: "poisons", level: "all"}}]->(b) RETURN b, e $$) AS (b agtype, e agtype);

-- Insert start and end points for graph
INSERT INTO start_and_end_points (SELECT * FROM cypher('cypher_vle', $$MATCH (b:begin)-[:edge]->()-[:edge]->()-[:edge]->()-[:edge]->(e:end) RETURN b, e $$) AS (b agtype, e agtype));

-- Display our points
SELECT * FROM start_and_end_points;

-- Test the VLE match integration
-- Each should find 400
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*]->(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*..]->(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*0..]->(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*1..]->(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*1..200]->(v:end) RETURN count(*) $$) AS (e agtype);
-- Each should find 2
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)<-[*]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)<-[*..]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)<-[*0..]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)<-[*1..]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)<-[*1..200]-(v:end) RETURN count(*) $$) AS (e agtype);
-- Each should find 7092
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*..]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*0..]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*1..]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[*1..200]-(v:end) RETURN count(*) $$) AS (e agtype);
-- Each should find 1
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[:edge*]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[:edge* {name: "main edge"}]-(v:end) RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[* {name: "main edge"}]-(v:end) RETURN count(*) $$) AS (e agtype);
-- Each should find 1
SELECT * FROM cypher('cypher_vle', $$MATCH ()<-[*4..4 {name: "main edge"}]-() RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u)<-[*4..4 {name: "main edge"}]-() RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH ()<-[*4..4 {name: "main edge"}]-(v) RETURN count(*) $$) AS (e agtype);
-- Each should find 2922
SELECT * FROM cypher('cypher_vle', $$MATCH ()-[*]->() RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH (u)-[*]->() RETURN count(*) $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH ()-[*]->(v) RETURN count(*) $$) AS (e agtype);
-- Should find 2
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)<-[e*]-(v:end) RETURN e ORDER BY e ASC $$) AS (e agtype);
-- Should find 5
SELECT * FROM cypher('cypher_vle', $$MATCH p=(:begin)<-[*1..1]-()-[]-() RETURN p ORDER BY p ASC $$) AS (e agtype);
-- Should find 2922
SELECT * FROM cypher('cypher_vle', $$MATCH p=()-[*]->(v) RETURN count(*) $$) AS (e agtype);
-- Should find 2
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)-[*3..3]->(v:end) RETURN p ORDER BY p ASC $$) AS (e agtype);
-- Should find 12
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)-[*3..3]-(v:end) RETURN p ORDER BY p ASC $$) AS (e agtype);
-- Each should find 2
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)<-[*]-(v:end) RETURN p ORDER BY p ASC $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)<-[e*]-(v:end) RETURN p ORDER BY p ASC $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)<-[e*]-(v:end) RETURN e ORDER BY e ASC $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(:begin)<-[*]-()<-[]-(:end) RETURN p ORDER BY p ASC $$) AS (e agtype);
-- Each should return 31
SELECT count(*) FROM cypher('cypher_vle', $$ MATCH ()-[e1]->(v)-[e2]->() RETURN e1,e2 $$) AS (e1 agtype, e2 agtype);
SELECT count(*) FROM cypher('cypher_vle', $$
	MATCH ()-[e1*1..1]->(v)-[e2*1..1]->()
	RETURN e1, e2
$$) AS (e1 agtype, e2 agtype);
SELECT count(*) FROM cypher('cypher_vle', $$
	MATCH (v)-[e1*1..1]->()-[e2*1..1]->()
	RETURN e1, e2
$$) AS (e1 agtype, e2 agtype);
SELECT count(*) FROM cypher('cypher_vle', $$
	MATCH ()-[e1]->(v)-[e2*1..1]->()
	RETURN e1, e2
$$) AS (e1 agtype, e2 agtype);
SELECT count(*) FROM cypher('cypher_vle', $$
    MATCH ()-[e1]->()-[e2*1..1]->()
    RETURN e1, e2
$$) AS (e1 agtype, e2 agtype);
SELECT count(*) FROM cypher('cypher_vle', $$
	MATCH ()-[e1*1..1]->(v)-[e2]->()
	RETURN e1, e2
$$) AS (e1 agtype, e2 agtype);
SELECT count(*) FROM cypher('cypher_vle', $$
    MATCH ()-[e1*1..1]->()-[e2]->()
    RETURN e1, e2
$$) AS (e1 agtype, e2 agtype);
SELECT count(*) FROM cypher('cypher_vle', $$
    MATCH (a)-[e1]->(a)-[e2*1..1]->()
    RETURN e1, e2
$$) AS (e1 agtype, e2 agtype);
SELECT count(*) FROM cypher('cypher_vle', $$
        MATCH (a) MATCH (a)-[e1*1..1]->(v)
        RETURN e1
$$) AS (e1 agtype);
SELECT count(*) FROM cypher('cypher_vle', $$
        MATCH (a) MATCH ()-[e1*1..1]->(a)
        RETURN e1
$$) AS (e1 agtype);
-- Should return 1 path
SELECT * FROM cypher('cypher_vle', $$ MATCH p=()<-[e1*]-(:end)-[e2*]->(:begin) RETURN p ORDER BY p ASC $$) AS (result agtype);
-- Each should return 3
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[e*0..1]->(v) RETURN id(u), e, id(v) ORDER BY id(u) ASC, e ASC, id(v) ASC $$) AS (u agtype, e agtype, v agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)-[e*0..1]->(v) RETURN p ORDER BY p ASC $$) AS (p agtype);
-- Each should return 5
SELECT * FROM cypher('cypher_vle', $$MATCH (u)-[e*0..0]->(v) RETURN id(u), e, id(v) ORDER BY id(u) ASC, e ASC, id(v) ASC $$) AS (u agtype, e agtype, v agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u)-[e*0..0]->(v) RETURN id(u), p, id(v) ORDER BY id(u) ASC, p ASC, id(v) ASC $$) AS (u agtype, p agtype, v agtype);
-- Each should return 13 and will be the same
SELECT * FROM cypher('cypher_vle', $$MATCH p=()-[*0..0]->()-[]->() RETURN p ORDER BY p ASC $$) AS (p agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=()-[]->()-[*0..0]->() RETURN p ORDER BY p ASC $$) AS (p agtype);

-- A missing edge label still permits the zero-hop self-binding, but it must
-- never traverse an edge of another label for a positive hop.
SELECT create_graph('vle_missing_edge_label');

SELECT * FROM cypher('vle_missing_edge_label', $$
    CREATE (:Person {name: 'Alice'})-[:KNOWS]->(:Person {name: 'Bob'})
$$) AS (result agtype);

SELECT * FROM cypher('vle_missing_edge_label', $$
    MATCH (a:Person {name: 'Alice'})-[:NOEXIST*0..1]->(b)
    RETURN a.name, b.name
    ORDER BY b.name
$$) AS (start_name agtype, end_name agtype);

SELECT * FROM cypher('vle_missing_edge_label', $$
    MATCH (:Person {name: 'Alice'})-[:NOEXIST*1..1]->()
    RETURN count(*)
$$) AS (positive_hops agtype);

SELECT drop_graph('vle_missing_edge_label', true);

-- An upper LIMIT can make openGauss choose an IndexOnlyScan-backed physical
-- slot for an anonymous VLE seed.  The executor must deform that slot before
-- replacing its empty seed arrays, otherwise projection restores a zero-hop
-- result even when the minimum depth is two.
SELECT create_graph('vle_limit_slot');

SELECT * FROM cypher('vle_limit_slot', $$
    CREATE (:d), (:d), (:d),
           (:v1)-[:e]->(:v1)-[:e]->(:v1)
$$) AS (result agtype);

SELECT * FROM cypher('vle_limit_slot', $$
    MATCH ()-[edges *2..2]->()
    RETURN size(edges), edges[0] IS NOT NULL, edges[1] IS NOT NULL
    LIMIT 1
$$) AS (path_size agtype, first_edge_present boolean,
        second_edge_present boolean);

SELECT drop_graph('vle_limit_slot', true);

-- A prepared VLE plan must discard its local DFS cache after the graph cache
-- is rebuilt, even if the allocator reuses the same address.
SELECT create_graph('vle_generation');

SELECT * FROM cypher('vle_generation', $$
    CREATE (:Person {name: 'Alice'})-[:KNOWS {weight: 1}]->(:Person {name: 'Bob'})
$$) AS (result agtype);

PREPARE vle_generation_probe AS
SELECT count(*) AS reachable
FROM cypher('vle_generation', $$
    MATCH (:Person {name: 'Alice'})-[:KNOWS*1..2]->(f:Person)
    RETURN f.name
$$) AS (friend agtype);

EXECUTE vle_generation_probe;

SELECT * FROM cypher('vle_generation', $$
    MATCH (b:Person {name: 'Bob'})
    CREATE (b)-[:KNOWS {weight: 2}]->(:Person {name: 'Carol'})
$$) AS (result agtype);

EXECUTE vle_generation_probe;
EXECUTE vle_generation_probe;
DEALLOCATE vle_generation_probe;

PREPARE vle_property_probe(agtype) AS
SELECT count(*) AS matched
FROM cypher('vle_generation', $$
    MATCH (:Person {name: 'Alice'})-[r:KNOWS*1..1 {weight: $weight}]->()
    RETURN r
$$, $1) AS (edges agtype);

EXECUTE vle_property_probe('{"weight": 1}');
EXECUTE vle_property_probe('{"weight": 2}');
DEALLOCATE vle_property_probe;

SELECT drop_graph('vle_generation', true);

--
-- #2420: VLE endpoints are ordinary graphid columns in the ExtensiblePlan.
-- Terminal and adjacent VLE joins must not depend on legacy path-inspection
-- helper functions, and prepared plans must preserve the same endpoint quals.
--
SELECT create_graph('vle_endpoint_quals');

SELECT * FROM cypher('vle_endpoint_quals', $$
    CREATE (:Node {name: 'A'})-[:E]->(:Node {name: 'B'})-
           [:E]->(:Node {name: 'C'})-[:E]->(:Node {name: 'D'})
$$) AS (result agtype);

SELECT count(*) AS terminal_paths
FROM cypher('vle_endpoint_quals', $$
    MATCH (:Node {name: 'A'})-[:E*1..3]->(:Node {name: 'D'})
    RETURN 1
$$) AS (result agtype);

SELECT count(*) AS chained_paths
FROM cypher('vle_endpoint_quals', $$
    MATCH (:Node {name: 'A'})-[:E*1..2]->()-[:E*1..2]->(:Node {name: 'D'})
    RETURN 1
$$) AS (result agtype);

SELECT count(*) AS vle_then_edge_paths
FROM cypher('vle_endpoint_quals', $$
    MATCH (:Node {name: 'A'})-[:E*1..2]->()-[:E]->(:Node {name: 'D'})
    RETURN 1
$$) AS (result agtype);

SELECT count(*) AS edge_then_vle_paths
FROM cypher('vle_endpoint_quals', $$
    MATCH (:Node {name: 'A'})-[:E]->()-[:E*1..2]->(:Node {name: 'D'})
    RETURN 1
$$) AS (result agtype);

PREPARE vle_endpoint_qual_probe AS
SELECT count(*) AS chained_paths
FROM cypher('vle_endpoint_quals', $$
    MATCH (:Node {name: 'A'})-[:E*1..2]->()-[:E*1..2]->(:Node {name: 'D'})
    RETURN 1
$$) AS (result agtype);

EXECUTE vle_endpoint_qual_probe;
EXECUTE vle_endpoint_qual_probe;
DEALLOCATE vle_endpoint_qual_probe;

PREPARE vle_regular_edge_qual_probe AS
SELECT count(*) AS mixed_paths
FROM cypher('vle_endpoint_quals', $$
    MATCH (:Node {name: 'A'})-[:E*1..2]->()-[:E]->(:Node {name: 'D'})
    RETURN 1
$$) AS (result agtype);

EXECUTE vle_regular_edge_qual_probe;
EXECUTE vle_regular_edge_qual_probe;
DEALLOCATE vle_regular_edge_qual_probe;

SELECT drop_graph('vle_endpoint_quals', true);

-- A single-segment VLE path variable should source anonymous endpoints from
-- the VLE itself instead of adding vertex scans or terminal joins.
\pset format unaligned
SELECT create_graph('vle_path_endpoint_plan');

SELECT * FROM cypher('vle_path_endpoint_plan', $$
    CREATE (:Node {name: 'A'})-[:E]->(:Node {name: 'B'})
$$) AS (result agtype);

SELECT * FROM cypher('vle_path_endpoint_plan', $$
    EXPLAIN (costs off) MATCH p=()-[*]-() RETURN p
$$) AS (plan agtype);

SELECT * FROM cypher('vle_path_endpoint_plan', $$
    EXPLAIN (costs off) MATCH p=(u)-[*]-() RETURN p
$$) AS (plan agtype);

SELECT drop_graph('vle_path_endpoint_plan', true);
\pset format aligned

-- Exercise VLE local-context cache eviction, rebuild, and LRU promotion.
PREPARE vle_cache_probe_1 AS
SELECT count(*) AS cached_paths
FROM cypher('cypher_vle', $$
    MATCH (:begin)-[:edge*1..1]->()
    RETURN 1
$$) AS (result agtype);

PREPARE vle_cache_probe_2 AS
SELECT count(*) AS cached_paths
FROM cypher('cypher_vle', $$
    MATCH (:begin)-[:edge*1..1]->()
    RETURN 1
$$) AS (result agtype);

PREPARE vle_cache_probe_3 AS
SELECT count(*) AS cached_paths
FROM cypher('cypher_vle', $$
    MATCH (:begin)-[:edge*1..1]->()
    RETURN 1
$$) AS (result agtype);

PREPARE vle_cache_probe_4 AS
SELECT count(*) AS cached_paths
FROM cypher('cypher_vle', $$
    MATCH (:begin)-[:edge*1..1]->()
    RETURN 1
$$) AS (result agtype);

PREPARE vle_cache_probe_5 AS
SELECT count(*) AS cached_paths
FROM cypher('cypher_vle', $$
    MATCH (:begin)-[:edge*1..1]->()
    RETURN 1
$$) AS (result agtype);

PREPARE vle_cache_probe_6 AS
SELECT count(*) AS cached_paths
FROM cypher('cypher_vle', $$
    MATCH (:begin)-[:edge*1..1]->()
    RETURN 1
$$) AS (result agtype);

EXECUTE vle_cache_probe_1;
EXECUTE vle_cache_probe_2;
EXECUTE vle_cache_probe_3;
EXECUTE vle_cache_probe_4;
EXECUTE vle_cache_probe_5;
EXECUTE vle_cache_probe_6;
EXECUTE vle_cache_probe_1;
EXECUTE vle_cache_probe_3;

DEALLOCATE vle_cache_probe_1;
DEALLOCATE vle_cache_probe_2;
DEALLOCATE vle_cache_probe_3;
DEALLOCATE vle_cache_probe_4;
DEALLOCATE vle_cache_probe_5;
DEALLOCATE vle_cache_probe_6;

-- Apache AGE #1037: chained access must materialize a VLE edge container and
-- continue through nested edge properties.
SELECT create_graph('vle_access_operator');

SELECT * FROM cypher('vle_access_operator', $$
    CREATE (:AccessNode {name: 'a'})-
           [:AccessEdge {id: 0, nested: {value: 10}}]->
           (:AccessNode {name: 'b'})-
           [:AccessEdge {id: 1, nested: {value: 20}}]->
           (:AccessNode {name: 'c'})
$$) AS (result agtype);

SELECT * FROM cypher('vle_access_operator', $$
    MATCH ()-[edges:AccessEdge*2..2]->()
    RETURN edges[0].id,
           edges[1].nested.value,
           properties(edges[0]).nested.value
$$) AS (first_id agtype, second_nested agtype,
       first_properties_nested agtype);

SELECT drop_graph('vle_access_operator', true);

-- Apache AGE #1924 plus the openGauss native-VLE rescan boundary: EXISTS
-- short-circuits after its first path, so every correlated outer tuple must
-- restart the DFS stack from its own seed vertex.
SELECT create_graph('vle_exists_rescan');

SELECT count(*) AS empty_fixed_vle
FROM cypher('vle_exists_rescan', $$
    MATCH (n)
    WHERE EXISTS((n)-[*1]-({name: 'Willem Defoe'}))
    RETURN n
$$) AS (n agtype);

SELECT * FROM cypher('vle_exists_rescan', $$
    CREATE ({name: 'Jane Doe'})-[:KNOWS]->({name: 'John Doe'}),
           ({name: 'Donald Defoe'})-[:KNOWS]->({name: 'Willem Defoe'})
$$) AS (result agtype);
SELECT * FROM cypher('vle_exists_rescan', $$
    MATCH (u {name: 'John Doe'})
    MERGE (u)-[:KNOWS]->({name: 'Willem Defoe'})
$$) AS (result agtype);

SELECT count(*) AS unbounded_vle
FROM cypher('vle_exists_rescan', $$
    MATCH (n)
    WHERE EXISTS((n)-[*]-({name: 'Willem Defoe'}))
    RETURN n.name
$$) AS (name agtype);
SELECT count(*) AS fixed_one_vle
FROM cypher('vle_exists_rescan', $$
    MATCH (n)
    WHERE EXISTS((n)-[*1]-({name: 'Willem Defoe'}))
    RETURN n.name
$$) AS (name agtype);
SELECT count(*) AS fixed_two_vle
FROM cypher('vle_exists_rescan', $$
    MATCH (n)
    WHERE EXISTS((n)-[*2..2]-({name: 'Willem Defoe'}))
    RETURN n.name
$$) AS (name agtype);

SELECT drop_graph('vle_exists_rescan', true);

-- Apache AGE #2337: chained OPTIONAL MATCH VLE predicates must return false
-- for NULL-extended rows instead of raising errors or exhausting cached state.
SELECT create_graph('issue_2092');

SELECT * FROM cypher('issue_2092', $$
    CREATE (a:Person {name: 'Alice'}),
           (b:Person {name: 'Bob'}),
           (c:City {name: 'NYC'}),
           (d:City {name: 'LA'}),
           (e:Place {name: 'Central Park'}),
           (a)-[:LIVES_IN]->(c),
           (c)-[:HAS_PLACE]->(e),
           (b)-[:LIVES_IN]->(d)
$$) AS (result agtype);

SELECT * FROM cypher('issue_2092', $$
    MATCH (p:Person)-[:LIVES_IN*]->(c:City)
    OPTIONAL MATCH (c)-[:HAS_PLACE*]->(place)
    OPTIONAL MATCH (place)-[:NEARBY*]->(other)
    WHERE place IS NOT NULL
    RETURN p.name, place.name, other
    ORDER BY p.name
$$) AS (person agtype, place agtype, other agtype);

SELECT * FROM cypher('issue_2092', $$
    MATCH (p:Person)-[:LIVES_IN*]->(c:City)
    OPTIONAL MATCH (c)-[:HAS_PLACE*]->(place)
    OPTIONAL MATCH (place)-[:NEARBY*]->(other)
    RETURN p.name, place.name, other
    ORDER BY p.name
$$) AS (person agtype, place agtype, other agtype);

SELECT * FROM cypher('issue_2092', $$
    MATCH (p:Person)-[:LIVES_IN*]->(c:City)
    OPTIONAL MATCH (c)-[:HAS_PLACE*]->(place)
    WHERE place IS NOT NULL
    RETURN p.name, c.name, place.name
    ORDER BY p.name
$$) AS (person agtype, city agtype, place agtype);

SELECT drop_graph('issue_2092', true);

--
-- openGauss: a float edge property constraint is an agtype constant inside
-- the serialized VLE clause data; it must round trip without going through
-- float8out()/agtype_in().
--
SELECT create_graph('cypher_vle_float');
SELECT * FROM cypher('cypher_vle_float', $$
    CREATE (:N {name: 'a'})-[:E {w: 0.5}]->(:N {name: 'b'})-[:E {w: 0.5}]->(:N {name: 'c'}),
           (:N {name: 'x'})-[:E {w: 1.5}]->(:N {name: 'y'})
$$) AS (r agtype);
SELECT * FROM cypher('cypher_vle_float', $$
    MATCH (a:N {name: 'a'})-[e*1..2 {w: 0.5}]->(b) RETURN b.name ORDER BY b.name
$$) AS (name agtype);
SELECT * FROM cypher('cypher_vle_float', $$
    MATCH (a:N)-[e*1..2 {w: 1.5}]->(b) RETURN a.name, b.name
$$) AS (a agtype, b agtype);
SELECT drop_graph('cypher_vle_float', true);

--
-- Clean up
--

DROP TABLE start_and_end_points;

SELECT drop_graph('cypher_vle', true);

--
-- End
--
