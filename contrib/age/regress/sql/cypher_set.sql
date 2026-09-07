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

SELECT create_graph('cypher_set');

SELECT * FROM cypher('cypher_set', $$CREATE (:v)$$) AS (a agtype);
SELECT * FROM cypher('cypher_set', $$CREATE (:v {i: 0, j: 5, a: 0})$$) AS (a agtype);
SELECT * FROM cypher('cypher_set', $$CREATE (:v {i: 1})$$) AS (a agtype);

--Simple SET test case
SELECT * FROM cypher('cypher_set', $$MATCH (n) SET n.i = 3$$) AS (a agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (n) WHERE n.j = 5 SET n.i = NULL RETURN n$$) AS (a agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (n) RETURN n$$) AS (a agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (n) SET n.i = NULL RETURN n$$) AS (a agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (n) RETURN n$$) AS (a agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (n) SET n.i = 3 RETURN n$$) AS (a agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (n) RETURN n$$) AS (a agtype);

--Handle Inheritance
SELECT * FROM cypher('cypher_set', $$CREATE ()$$) AS (a agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (n) SET n.i = 3 RETURN n$$) AS (a agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (n) RETURN n$$) AS (a agtype);

--Validate Paths are updated
SELECT * FROM cypher('cypher_set', $$MATCH (n) CREATE (n)-[:e {j:20}]->(:other_v {k:10}) RETURN n$$) AS (a agtype);
SELECT * FROM cypher('cypher_set', $$MATCH p=(n)-[]->() SET n.i = 50 RETURN p$$) AS (a agtype);

--Edges
SELECT * FROM cypher('cypher_set', $$MATCH ()-[n]-(:other_v) SET n.i = 3 RETURN n$$) AS (a agtype);
SELECT * FROM cypher('cypher_set', $$MATCH ()-[n]->(:other_v) RETURN n$$) AS (a agtype);

SELECT * FROM cypher('cypher_set', $$
        MATCH (n {j: 5})
        SET n.y = 50
        SET n.z = 99
        RETURN n
$$) AS (a agtype);

SELECT * FROM cypher('cypher_set', $$
        MATCH (n {j: 5})
        RETURN n
$$) AS (a agtype);

--Create a loop and see that set can work after create
SELECT * FROM cypher('cypher_set', $$
	MATCH (n {j: 5})
	CREATE p=(n)-[e:e {j:34}]->(n)
	SET n.y = 99
	RETURN n, p
$$) AS (a agtype, b agtype);

--Create a loop and see that set can work after create
SELECT * FROM cypher('cypher_set', $$
	CREATE ()-[e:e {j:34}]->()
	SET e.y = 99
	RETURN e
$$) AS (a agtype);

CREATE TEMP TABLE cypher_set_return_rows (a agtype);

INSERT INTO cypher_set_return_rows
SELECT * FROM cypher('cypher_set', $$
        MATCH (n)
        MATCH (n)-[e:e {j:34}]->()
        SET n.y = 1
        RETURN n
$$) AS (a agtype);
SELECT a FROM cypher_set_return_rows ORDER BY age_id(a);
TRUNCATE cypher_set_return_rows;

INSERT INTO cypher_set_return_rows
SELECT * FROM cypher('cypher_set', $$
        MATCH (n)
        MATCH ()-[e:e {j:34}]->(n)
        SET n.y = 2
        RETURN n
$$) AS (a agtype);
SELECT a FROM cypher_set_return_rows ORDER BY age_id(a);
DROP TABLE cypher_set_return_rows;

SELECT * FROM cypher('cypher_set', $$MATCH (n)-[]->(n) SET n.y = 99 RETURN n$$) AS (a agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (n) MATCH (n)-[]->(m) SET n.t = 150 RETURN n$$) AS (a agtype);

-- prepared statements
PREPARE p_1 AS SELECT * FROM cypher('cypher_set', $$MATCH (n) SET n.i = 3 RETURN n $$) AS (a agtype);
EXECUTE p_1;

EXECUTE p_1;

PREPARE p_2 AS SELECT * FROM cypher('cypher_set', $$MATCH (n) SET n.i = $var_name RETURN n $$, $1) AS (a agtype);
EXECUTE p_2('{"var_name": 4}');

EXECUTE p_2('{"var_name": 6}');

CREATE FUNCTION set_test()
RETURNS TABLE(vertex agtype)
LANGUAGE plpgsql
VOLATILE
AS $BODY$
BEGIN
	RETURN QUERY SELECT * FROM cypher('cypher_set', $$MATCH (n) SET n.i = 7 RETURN n $$) AS (a agtype);
END
$BODY$;

SELECT set_test();

SELECT set_test();

--
-- Updating multiple fieds
--
SELECT * FROM cypher('cypher_set', $$MATCH (n) SET n.i = 3, n.j = 5 RETURN n $$) AS (a agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (n)-[m]->(n) SET m.y = n.y RETURN n, m$$) AS (a agtype, b agtype);

--Errors
SELECT * FROM cypher('cypher_set', $$SET n.i = NULL$$) AS (a agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (n) SET wrong_var.i = 3$$) AS (a agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (n) SET i = 3$$) AS (a agtype);

--
-- SET refactor regression tests
--

-- INSERT INTO
CREATE TABLE tbl (result agtype);

SELECT * FROM cypher('cypher_set', $$CREATE (u:vertices) $$) AS (result agtype);
SELECT * FROM cypher('cypher_set', $$CREATE (u:begin)-[:edge]->(v:end) $$) AS (result agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) return u $$) AS (result agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (u:begin)-[:edge]->(v:end) return u, v $$) AS (u agtype, v agtype);

INSERT INTO tbl (SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) SET u.i = 7 return u $$) AS (result agtype));
INSERT INTO tbl (SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) SET u.i = 13 return u $$) AS (result agtype));

SELECT * FROM tbl;

SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) return u $$) AS (result agtype);

BEGIN;
SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) SET u.i = 1, u.j = 3, u.k = 5 return u $$) AS (result agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) return u $$) AS (result agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) SET u.i = 2, u.j = 4, u.k = 6 return u $$) AS (result agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) return u $$) AS (result agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) SET u.i = 3, u.j = 6, u.k = 9 return u $$) AS (result agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) return u $$) AS (result agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (u:begin)-[:edge]->(v:end) SET u.i = 1, v.i = 2, u.j = 3, v.j = 4 return u, v $$) AS (u agtype, v agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (u:begin)-[:edge]->(v:end) return u, v $$) AS (u agtype, v agtype);

SELECT * FROM cypher('cypher_set', $$MATCH (u:begin)-[:edge]->(v:end) SET u.i = 2, v.i = 1, u.j = 4, v.j = 3 return u, v $$) AS (u agtype, v agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (u:begin)-[:edge]->(v:end) return u, v $$) AS (u agtype, v agtype);
END;

SELECT * FROM cypher('cypher_set', $$MATCH (u:vertices) return u $$) AS (result agtype);
SELECT * FROM cypher('cypher_set', $$MATCH (u:begin)-[:edge]->(v:end) return u, v $$) AS (u agtype, v agtype);

-- Preserve null-valued keys in expression maps, but not top-level properties.
SELECT * FROM cypher('cypher_set', $$
    RETURN size(keys({keep: 1, missing: null})) = 2
$$) AS (map_keeps_null agtype);

SELECT * FROM cypher('cypher_set', $$
    WITH {outer: {inner: null}} AS m
    RETURN size(keys(m.outer)) = 1
$$) AS (nested_map_keeps_null agtype);

SELECT * FROM cypher('cypher_set', $$
    CREATE (:null_map_contract {
        marker: 2391,
        old_property: true,
        create_drop: null,
        nested: {inner: null}
    })
$$) AS (result agtype);

SELECT * FROM cypher('cypher_set', $$
    MATCH (n:null_map_contract)
    RETURN size(keys(properties(n))) = 3,
           size(keys(n.nested)) = 1
$$) AS (create_drops_top_level_null agtype,
       create_keeps_nested_null agtype);

SELECT * FROM cypher('cypher_set', $$
    MATCH (n:null_map_contract)
    SET n = {marker: 2391, set_drop: null, nested: {inner: null}}
    RETURN size(keys(properties(n))) = 2,
           size(keys(n.nested)) = 1
$$) AS (set_replaces_and_drops_top_level_null agtype,
       set_keeps_nested_null agtype);

SELECT * FROM cypher('cypher_set', $$
    MATCH (n:null_map_contract)
    SET n.payload = {inner: null}
    RETURN size(keys(properties(n))) = 3,
           size(keys(n.payload)) = 1
$$) AS (single_property_set_adds_map agtype,
       single_property_set_keeps_nested_null agtype);

-- Complete map SET support: merge maps and accept map-valued expressions.
SELECT * FROM cypher('cypher_set', $$
    CREATE (:map_set_contract {marker: 468, old: 1, gone: 2}),
           (:map_set_source {copied: 3, retained: 4})
$$) AS (result agtype);

SELECT * FROM cypher('cypher_set', $$
    MATCH (n:map_set_contract)
    SET n += {old: 7, gone: null, added: 9}
    RETURN n.old, n.gone, n.added
$$) AS (old agtype, gone agtype, added agtype);

SELECT * FROM cypher('cypher_set', $$
    MATCH (n:map_set_contract), (source:map_set_source)
    SET n = source
    RETURN n.copied, n.retained, n.old
$$) AS (copied agtype, retained agtype, old agtype);

-- Apache AGE #1160/#1637: preserve nested values during += and accept a
-- map-valued WITH alias as the complete property replacement.
SELECT * FROM cypher('cypher_set', $$
    CREATE (:nested_map_set_contract {
        marker: 1160,
        map: {a: 1, b: {c: 2, d: []}, c: [{d: -100, e: []}]},
        list: [1, 'string', [{a: []}, [[1, 2]]]]
    })
$$) AS (result agtype);

SELECT * FROM cypher('cypher_set', $$
    MATCH (n:nested_map_set_contract {marker: 1160})
    SET n += {added: {x: [1, {y: 2}]}}
    RETURN n.map.b.c, n.map.c[0].d, n.list[2][1][0], n.added.x[1].y
$$) AS (map_value agtype, nested_list_value agtype,
       original_list_value agtype, added_value agtype);

SELECT * FROM cypher('cypher_set', $$
    WITH {first: 'jon', last: 'snow'} AS map
    MERGE (n:dynamic_map_set_contract {id: '1'})
    SET n = map
    RETURN n.first, n.last, n.id, map.first
$$) AS (first agtype, last agtype, removed_id agtype, source_first agtype);

-- Apache AGE #2306: SET RHS variables use an internal target name, and nested
-- graph values remain accessible after serialization.
SELECT * FROM cypher('cypher_set', $$
    CREATE (n:self_reference_contract {name: 'self'})
    SET n.copy = n
    RETURN label(n.copy), n.copy.name
$$) AS (copy_label agtype, copy_name agtype);

SELECT * FROM cypher('cypher_set', $$
    CREATE (a:edge_reference_contract {name: 'a'})-
           [e:stored_edge_contract {weight: 1}]->
           (b:edge_reference_contract {name: 'b'})
    SET a.edge_copy = e, b.source_copy = a
    RETURN type(a.edge_copy), a.edge_copy.weight,
           label(b.source_copy), b.source_copy.name
$$) AS (edge_type agtype, edge_weight agtype,
       source_label agtype, source_name agtype);

--
-- openGauss: SET must be applied when the same query level sorts, limits or
-- de-duplicates its output (see cypher_create.sql for the planner background).
--
SELECT create_graph('cypher_set_sorted');
SELECT * FROM cypher('cypher_set_sorted', $$
    CREATE (:P {name: 'a'}), (:P {name: 'b'}), (:P {name: 'c'})
$$) AS (r agtype);
SELECT * FROM cypher('cypher_set_sorted', $$
    MATCH (n:P) SET n.seen = true RETURN n.name ORDER BY n.name LIMIT 2
$$) AS (name agtype);
SELECT * FROM cypher('cypher_set_sorted', $$
    MATCH (n:P) SET n.twice = 1 RETURN DISTINCT n.twice
$$) AS (twice agtype);
SELECT * FROM cypher('cypher_set_sorted', $$
    MATCH (n:P) SET n.tag = 'x' RETURN n.name
$$) AS (name agtype) ORDER BY name;
SELECT * FROM cypher('cypher_set_sorted', $$
    MATCH (n:P) RETURN n.name, n.seen, n.twice, n.tag ORDER BY n.name
$$) AS (name agtype, seen agtype, twice agtype, tag agtype);
-- every SubLink-producing expression is allowed on the right-hand side, not
-- only list comprehensions
SELECT * FROM cypher('cypher_set_sorted', $$
    MATCH (n:P) SET n.all_pos = all(x IN [1, 2] WHERE x > 0) RETURN n.all_pos
$$) AS (all_pos agtype);
SELECT * FROM cypher('cypher_set_sorted', $$
    CREATE (:P {name: 'y'})-[:E]->(:P {name: 'z'})
$$) AS (r agtype);
SELECT * FROM cypher('cypher_set_sorted', $$
    MATCH (n:P) SET n.has_out = EXISTS((n)-->()) RETURN n.name, n.has_out ORDER BY n.name
$$) AS (name agtype, has_out agtype);
SELECT * FROM cypher('cypher_set_sorted', $$
    MATCH (n:P {name: 'z'}) SET n.doubled = [x IN [1, 2, 3] WHERE x > 1 | x * 2] RETURN n.doubled
$$) AS (doubled agtype);
SELECT drop_graph('cypher_set_sorted', true);

--
-- Clean up
--
DROP TABLE tbl;
DROP FUNCTION set_test;
SELECT drop_graph('cypher_set', true);

--
-- End
--
