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

SELECT create_graph('cypher_match');

SELECT * FROM cypher('cypher_match', $$CREATE (:v)$$) AS (a agtype);
SELECT * FROM cypher('cypher_match', $$CREATE (:v {i: 0})$$) AS (a agtype);
SELECT * FROM cypher('cypher_match', $$CREATE (:v {i: 1})$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$MATCH (n:v) RETURN n$$) AS (n agtype);
SELECT * FROM cypher('cypher_match', $$MATCH (n:v) RETURN n.i$$) AS (i agtype);

SELECT _get_vertex_by_graphid('cypher_match', id)
FROM cypher_match.v
ORDER BY id
LIMIT 1;

SELECT * FROM cypher('cypher_match', $$
MATCH (n:v) WHERE n.i > 0
RETURN n.i
$$) AS (i agtype);

--Directed Paths
SELECT * FROM cypher('cypher_match', $$
	CREATE (:v1 {id:'initial'})-[:e1]->(:v1 {id:'middle'})-[:e1]->(:v1 {id:'end'})
$$) AS (a agtype);

--Undirected Path Tests
SELECT * FROM cypher('cypher_match', $$
	MATCH p=(:v1)-[:e1]-(:v1)-[:e1]-(:v1) RETURN p
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH p=(a:v1)-[]-()-[]-() RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[]-()-[]-(a:v1) RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[]-(a:v1)-[]-() RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[b:e1]-()-[]-() RETURN b ORDER BY id(b)
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v1)-[]->(), ()-[]->(a) RETURN a
$$) AS (a agtype);

-- Right Path Test
SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v1)-[:e1]->(b:v1)-[:e1]->(c:v1) RETURN a, b, c
$$) AS (a agtype, b agtype, c agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH p=(a:v1)-[]-()-[]->() RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH p=(a:v1)-[]->()-[]-() RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[]-()-[]->(a:v1) RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[]-(a:v1)-[]->() RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[b:e1]-()-[]->() RETURN b
$$) AS (a agtype);

--Left Path Test
SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v1)<-[:e1]-(b:v1)<-[:e1]-(c:v1) RETURN a, b, c
$$) AS (a agtype, b agtype, c agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH p=(a:v1)<-[]-()-[]-() RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH p=(a:v1)-[]-()<-[]-() RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()<-[]-()-[]-(a:v1) RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()<-[]-(a:v1)-[]-() RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()<-[b:e1]-()-[]-() RETURN b ORDER BY id(b)
$$) AS (a agtype);

--Divergent Path Tests
SELECT * FROM cypher('cypher_match', $$
	CREATE (:v2 {id:'initial'})<-[:e2]-(:v2 {id:'middle'})-[:e2]->(:v2 {id:'end'})
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()<-[]-(n:v2)-[]->()
	MATCH p=()-[]->(n)
	RETURN p
$$) AS (i agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()<-[]-(n:v2)-[]->()
	MATCH p=(n)-[]->()
	RETURN p ORDER BY p
$$) AS (i agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[]-(n:v2)
	RETURN n
$$) AS (i agtype);

--Convergent Path Tests
SELECT * FROM cypher('cypher_match', $$
	CREATE (:v3 {id:'initial'})-[:e3]->(:v3 {id:'middle'})<-[:e3]-(:v3 {id:'end'})
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[b:e1]->()
	RETURN b ORDER BY id(b)
$$) AS (i agtype);


SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[]->(n:v1)<-[]-()
	MATCH p=(n)<-[]-()
	RETURN p
$$) AS (i agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[]->(n:v1)<-[]-()
	MATCH p=()-[]->(n)
	RETURN p
$$) AS (i agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH ()-[]->(n:v1)<-[]-()
	MATCH p=(n)-[]->()
	RETURN p
$$) AS (i agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH con_path=(a)-[]->()<-[]-()
	where a.id = 'initial'
	RETURN con_path
$$) AS (con_path agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH div_path=(b)<-[]-()-[]->()
	where b.id = 'initial'
	RETURN div_path
$$) AS (div_path agtype);

--Patterns
SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v1), p=(a)-[]-()-[]-()
	where a.id = 'initial'
	RETURN p
$$) AS (p agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH con_path=(a)-[]->()<-[]-(), div_path=(b)<-[]-()-[]->()
	where a.id = 'initial'
	and b.id = 'initial'
	RETURN con_path, div_path
$$) AS (con_path agtype, div_path agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v), p=()-[]->()-[]->()
	RETURN a.i, p
$$) AS (i agtype, p agtype);

--Multiple Match Clauses
SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v1)
	where a.id = 'initial'
	MATCH p=(a)-[]-()-[]-()
	RETURN p
$$) AS (p agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v)
	MATCH p=()-[]->()-[]->()
	RETURN a.i, p
$$) AS (i agtype, p agtype);

-- ORDER BY keeps this independent of the join order the planner picks for the
-- cartesian product; the assertion here is the result set, not a scan order.
SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v)
	MATCH (b:v1)-[]-(c)
	RETURN a.i, b.id, c.id
	ORDER BY a.i, b.id, c.id
$$) AS (i agtype, b agtype, c agtype);

--
-- Property constraints
--
SELECT * FROM cypher('cypher_match',
 $$CREATE ({string_key: "test", int_key: 1, float_key: 3.14, map_key: {key: "value"}, list_key: [1, 2, 3]}) $$)
AS (p agtype);

SELECT * FROM cypher('cypher_match',
 $$CREATE ({lst: [1, NULL, 3.14, "string", {key: "value"}, []]}) $$)
AS (p agtype);

SELECT * FROM cypher('cypher_match',
 $$MATCH (n  {string_key: NULL}) RETURN n $$)
AS (n agtype);

SELECT * FROM cypher('cypher_match',
 $$MATCH (n  {string_key: "wrong value"}) RETURN n $$)
AS (n agtype);


SELECT * FROM cypher('cypher_match', $$
    MATCH (n {string_key: "test", int_key: 1, float_key: 3.14, map_key: {key: "value"}, list_key: [1, 2, 3]})
    RETURN n $$)
AS (p agtype);

SELECT * FROM cypher('cypher_match',
 $$MATCH (n {string_key: "test"}) RETURN n $$)
AS (p agtype);

SELECT * FROM cypher('cypher_match',
 $$MATCH (n {lst: [1, NULL, 3.14, "string", {key: "value"}, []]})  RETURN n $$)
AS (p agtype);

SELECT * FROM cypher('cypher_match',
 $$MATCH (n {lst: [1, NULL, 3.14, "string", {key: "value"}, [], "extra value"]})  RETURN n $$)
AS (p agtype);

SHOW age.enable_containment;

SET age.enable_containment = off;

SELECT count(*) FROM cypher('cypher_match',
 $$MATCH (n {string_key: "test", map_key: {key: "value"}}) RETURN n $$)
AS (n agtype);

SELECT count(*) FROM cypher('cypher_match',
 $$MATCH (n {list_key: [1, 2, 3]}) RETURN n $$)
AS (n agtype);

RESET age.enable_containment;

SHOW age.enable_containment;


--
-- Prepared Statement Property Constraint
--
PREPARE property_ps(agtype) AS SELECT * FROM cypher('cypher_match',
 $$MATCH (n $props) RETURN n $$, $1)
AS (p agtype);

EXECUTE property_ps(agtype_build_map('props',
                                     agtype_build_map('string_key', 'test')));

-- need a following RETURN clause (should fail)
SELECT * FROM cypher('cypher_match', $$MATCH (n:v)$$) AS (a agtype);

--Invalid Variables
SELECT * FROM cypher('cypher_match', $$
	MATCH (a)-[]-()-[]-(a:v1) RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (a)-[]-()-[]-(a:invalid_label) RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v1)-[]-()-[a]-() RETURN a
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (a:v1)-[]-()-[]-(a {id:'will_fail'}) RETURN a
$$) AS (a agtype);

--Incorrect Labels
SELECT * FROM cypher('cypher_match', $$MATCH (n)-[:v]-() RETURN n$$) AS (n agtype);

SELECT * FROM cypher('cypher_match', $$MATCH (n)-[:emissing]-() RETURN n$$) AS (n agtype);

SELECT * FROM cypher('cypher_match', $$MATCH (n:e1)-[]-() RETURN n$$) AS (n agtype);

SELECT * FROM cypher('cypher_match', $$MATCH (n:vmissing)-[]-() RETURN n$$) AS (n agtype);

SELECT * FROM cypher('cypher_match', $$MATCH (:e1)-[r]-() RETURN r$$) AS (r agtype);

SELECT * FROM cypher('cypher_match', $$MATCH (:vmissing)-[r]-() RETURN r$$) AS (r agtype);

SELECT * FROM cypher('cypher_match', $$MATCH (n),(:e1) RETURN n$$) AS (n agtype);

SELECT * FROM cypher('cypher_match', $$MATCH (n),()-[:v]-() RETURN n$$) AS (n agtype);

--
-- Path of one vertex. This should select 14
--
SELECT * FROM cypher('cypher_match', $$
       MATCH p=() RETURN p
$$) AS (p agtype);

--
-- MATCH with WHERE EXISTS(pattern)
--
SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) RETURN u, e, v $$) AS (u agtype, e agtype, v agtype);

SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((u)-[e]->(v)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);


-- Property Constraint in EXISTS
SELECT * FROM cypher('cypher_match',
 $$MATCH (u) WHERE EXISTS((u)-[]->({id: "middle"})) RETURN u $$)
AS (u agtype);

SELECT * FROM cypher('cypher_match',
 $$MATCH (u) WHERE EXISTS((u)-[]->({id: "not a valid id"})) RETURN u $$)
AS (u agtype);

SELECT * FROM cypher('cypher_match',
 $$MATCH (u) WHERE EXISTS((u)-[]->({id: NULL})) RETURN u $$)
AS (u agtype);

-- Exists checks for a loop. There shouldn't be any.
SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((u)-[e]->(u)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);

-- Create a loop
SELECT * FROM cypher('cypher_match', $$
        CREATE (u:loop {id:'initial'})-[:self]->(u)
$$) AS (a agtype);

-- dump paths
SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((u)-[e]->(v)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);

-- Exists checks for a loop. There should be one.
SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((u)-[e]->(u)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);

-- Exists checks for a loop. There should be one.
SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((v)-[e]->(v)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);

-- Exists checks for a loop. There should be none because of edge uniqueness
-- requirement.
SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((u)-[e]->(u)-[e]->(u)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);

-- Multiple exists
SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((u)) AND EXISTS((v)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);

SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((u)-[e]->(u)) AND EXISTS((v)-[e]->(v)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);

-- These should error
-- Bad pattern
SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((u)) AND EXISTS([e]) AND EXISTS((v)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);

-- variable creation error
SELECT * FROM cypher('cypher_match',
 $$MATCH (u)-[e]->(v) WHERE EXISTS((u)-[e]->(x)) RETURN u, e, v $$)
AS (u agtype, e agtype, v agtype);

--
-- Tests for EXISTS(property)
--

-- dump all vertices
SELECT * FROM cypher('cypher_match', $$MATCH (u) RETURN u $$) AS (u agtype);

-- select vertices with id as a property
SELECT * FROM cypher('cypher_match',
 $$MATCH (u) WHERE EXISTS(u.id) RETURN u $$)
AS (u agtype);

-- select vertices without id as a property
SELECT * FROM cypher('cypher_match',
 $$MATCH (u) WHERE NOT EXISTS(u.id) RETURN u $$)
AS (u agtype);

-- select vertices without id as a property but with a property i
SELECT * FROM cypher('cypher_match',
 $$MATCH (u) WHERE NOT EXISTS(u.id) AND EXISTS(u.i) RETURN u $$)
AS (u agtype);

-- select vertices with id as a property and have a self loop
SELECT * FROM cypher('cypher_match',
 $$MATCH (u) WHERE EXISTS(u.id) AND EXISTS((u)-[]->(u)) RETURN u$$)
AS (u agtype);

-- should give an error
SELECT * FROM cypher('cypher_match',
 $$MATCH (u) WHERE EXISTS(u) RETURN u$$)
AS (u agtype);

--
--Distinct
--
SELECT * FROM cypher('cypher_match', $$
	MATCH (u)
	RETURN DISTINCT u.id
$$) AS (i agtype);

SELECT * FROM cypher('cypher_match', $$
	CREATE (u:duplicate)-[:dup_edge {id:1 }]->(:other_v)
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (u:duplicate)
	CREATE (u)-[:dup_edge {id:2 }]->(:other_v)
$$) AS (a agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (u:duplicate)-[]-(:other_v)
	RETURN DISTINCT u
$$) AS (i agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH p=(:duplicate)-[]-(:other_v)
	RETURN DISTINCT p
$$) AS (i agtype);

--
-- Limit
--
SELECT * FROM cypher('cypher_match', $$
	MATCH (u)
	RETURN u
$$) AS (i agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (u)
	RETURN u LIMIT 3
$$) AS (i agtype);

--
-- Skip
--
SELECT * FROM cypher('cypher_match', $$
	MATCH (u)
	RETURN u SKIP 7
$$) AS (i agtype);

SELECT * FROM cypher('cypher_match', $$
	MATCH (u)
	RETURN u SKIP 7 LIMIT 3
$$) AS (i agtype);


--
-- Optional Match
--
SELECT * FROM cypher('cypher_match', $$
    CREATE (:opt_match_v {name: 'someone'})-[:opt_match_e]->(:opt_match_v {name: 'somebody'}),
           (:opt_match_v {name: 'anybody'})-[:opt_match_e]->(:opt_match_v {name: 'nobody'})
$$) AS (u agtype);

SELECT * FROM cypher('cypher_match', $$
    MATCH (u:opt_match_v)
    OPTIONAL MATCH (u)-[m]-(l)
    RETURN u.name as u, type(m), l.name as l
    ORDER BY u, m, l
$$) AS (u agtype, m agtype, l agtype);

-- Entity DISTINCT uses graph identifiers internally but must preserve the
-- complete entity value for property access by following clauses.
SELECT * FROM cypher('cypher_match', $$
    MATCH (u:opt_match_v), (duplicate:opt_match_v)
    WITH DISTINCT u
    RETURN u.name AS name
    ORDER BY name
$$) AS (name agtype);

SELECT * FROM cypher('cypher_match', $$
    OPTIONAL MATCH (n:opt_match_v)-[r]->(p), (m:opt_match_v)-[s]->(q)
    WHERE id(n) <> id(m)
    RETURN n.name as n, type(r) AS r, p.name as p,
           m.name AS m, type(s) AS s, q.name AS q
    ORDER BY n, p, m, q
$$) AS (n agtype, r agtype, p agtype, m agtype, s agtype, q agtype);

SELECT * FROM cypher('cypher_match', $$
    MATCH (n:opt_match_v), (m:opt_match_v)
    WHERE id(n) <> id(m)
    OPTIONAL MATCH (n)-[r]->(p), (m)-[s]->(q)
    RETURN n.name AS n, type(r) AS r, p.name AS p,
           m.name AS m, type(s) AS s, q.name AS q
    ORDER BY n, p, m, q
 $$) AS (n agtype, r agtype, p agtype, m agtype, s agtype, q agtype);

-- A correlated three-edge OPTIONAL MATCH must preserve both the selective
-- endpoint traversal and the NULL-extended row for an unmatched outer vertex.
SELECT * FROM cypher('cypher_match', $$
    CREATE (:opt_long_v {name: 'matched'})
               -[:opt_long_e]->(:opt_long_v {name: 'first'})
               -[:opt_long_e]->(:opt_long_v {name: 'second'})
               -[:opt_long_e]->(:opt_long_v {name: 'target'}),
           (:opt_long_v {name: 'unmatched'})
$$) AS (result agtype);

SELECT * FROM cypher('cypher_match', $$
    MATCH (source:opt_long_v)
    WHERE source.name IN ['matched', 'unmatched']
    OPTIONAL MATCH (source)-[:opt_long_e]->()-[:opt_long_e]->()-[:opt_long_e]->(target)
    RETURN source.name AS source, target.name AS target
    ORDER BY source
$$) AS (source agtype, target agtype);

-- Tests to catch match following optional match logic
-- this syntax is invalid in cypher
SELECT * FROM cypher('cypher_match', $$
    OPTIONAL MATCH (n)
    MATCH (m)
    RETURN n,m
 $$) AS (n agtype, m agtype);

SELECT * FROM cypher('cypher_match', $$
    MATCH (n)
    OPTIONAL MATCH (m)
    MATCH (o)
    RETURN n,m,o
 $$) AS (n agtype, m agtype, o agtype);

--
-- JIRA: AGE2-544
--

-- Clean up
SELECT DISTINCT * FROM cypher('cypher_match', $$
    MATCH (u) DETACH DELETE (u)
$$) AS (i agtype);

-- Prepare
SELECT * FROM cypher('cypher_match', $$
    CREATE (u {name: "orphan"})
    CREATE (u1 {name: "F"})-[u2:e1]->(u3 {name: "T"})
    RETURN u1, u2, u3
$$) as (u1 agtype, u2 agtype, u3 agtype);

-- Querying NOT EXISTS syntax
SELECT * FROM cypher('cypher_match', $$
     MATCH (f),(t)
     WHERE NOT EXISTS((f)-[]->(t))
     RETURN f.name, t.name
 $$) as (f agtype, t agtype);

-- Querying EXISTS syntax
SELECT * FROM cypher('cypher_match', $$
    MATCH (f),(t)
    WHERE EXISTS((f)-[]->(t))
    RETURN f.name, t.name
 $$) as (f agtype, t agtype);

-- Querying ALL
SELECT * FROM cypher('cypher_match', $$
    MATCH (f),(t)
    WHERE NOT EXISTS((f)-[]->(t)) or true
    RETURN f.name, t.name
$$) as (f agtype, t agtype);

-- Querying ALL
SELECT * FROM cypher('cypher_match', $$
    MATCH (f),(t)
    RETURN f.name, t.name
$$) as (f agtype, t agtype);

--
-- #2303 parser composite entities and direct accessor fields
--
SELECT create_graph('c2303_parser');

SELECT * FROM cypher('c2303_parser', $$
    CREATE (:Person {name: 'Alice', age: 30})
           -[:KNOWS {since: 2020}]->
           (:Person {name: 'Bob', age: 25})
$$) AS (result agtype);

SELECT * FROM cypher('c2303_parser', $$
    MATCH (a:Person {name: 'Alice'})-[r:KNOWS]->(b:Person)
    RETURN id(a) = start_id(r), id(b) = end_id(r),
           label(a), type(r), properties(a), properties(r)
$$) AS (start_ok agtype, end_ok agtype, vertex_label agtype,
       edge_label agtype, vertex_properties agtype, edge_properties agtype);

SELECT * FROM cypher('c2303_parser', $$
    MATCH (a:Person {name: 'Alice'})-[r:KNOWS]->(b:Person)
    RETURN startNode(r) = a, endNode(r) = b,
           start_id(r) = id(startNode(r)),
           end_id(r) = id(endNode(r)), type(r) = label(r)
$$) AS (start_vertex_ok agtype, end_vertex_ok agtype,
       start_alias_ok agtype, end_alias_ok agtype, label_alias_ok agtype);

SELECT * FROM cypher('c2303_parser', $$
    MATCH p=(a:Person {name: 'Alice'})-[r:KNOWS]->(b:Person)
    RETURN properties(a).name, [a, r, b],
           {vertex: a, edge: r}, a {.name, .age}, p
$$) AS (name agtype, entities agtype, entity_map agtype,
       projected agtype, path agtype);

SELECT * FROM cypher('c2303_parser', $$
    MATCH (a:Person {name: 'Alice'})-[r:KNOWS]->(b:Person)
    RETURN CASE WHEN a.age > b.age THEN a ELSE b END
$$) AS (older agtype);

SELECT * FROM cypher('c2303_parser', $$
    MATCH (n:Person)
    RETURN n ORDER BY n.name
$$) AS (n vertex);

SELECT * FROM cypher('c2303_parser', $$
    MATCH ()-[r:KNOWS]->()
    RETURN r
$$) AS (r edge);

SELECT * FROM cypher('c2303_parser', $$
    MATCH (n:Person)
    WITH n AS m
    RETURN m ORDER BY m.name
$$) AS (n vertex);

SELECT * FROM cypher('c2303_parser', $$
    MATCH (a:Person {name: 'Alice'})-[r:KNOWS]->(b:Person)
    RETURN a = a, a <> b, r = r, r <> r
$$) AS (vertex_eq agtype, vertex_ne agtype,
       edge_eq agtype, edge_ne agtype);

SELECT * FROM cypher('c2303_parser', $$
    MATCH (a:Person {name: 'Alice'})-[r:KNOWS]->()
    RETURN a, r
$$) AS (vertex_json json, edge_jsonb jsonb);

SELECT * FROM cypher('c2303_parser', $$
    MATCH (n:Person)
    RETURN n ORDER BY n.name
$$) AS (n agtype);

SELECT drop_graph('c2303_parser', true);

--
-- issue 2308 / #2340: MATCH after CREATE or SET must evaluate after DML.
--
SELECT * FROM cypher('cypher_match', $$
    CREATE (a:v {issue_2308: true})-[e:e1]->(b:v)
    WITH a, e, b
    MATCH (a)-[e]->(b)
    RETURN a.issue_2308
$$) AS (value agtype);

SELECT * FROM cypher('cypher_match', $$
    MATCH (a:v {issue_2308: true})-[e:e1]->(b:v)
    SET a.issue_2308 = false
    WITH a, e, b
    MATCH (a)-[e]->(b)
    RETURN a.issue_2308
$$) AS (value agtype);

--
-- issue 2193 / #2341: defer label validation until preceding writes are
-- transformed, so labels created in the same query are immediately visible.
--
SELECT create_graph('match_new_label_visibility');

SELECT * FROM cypher('match_new_label_visibility', $$
    CREATE (u:FreshUser {name: 'Neo'})-[e:FreshOwns]->
           (f:FreshFolder {name: 'Inbox'})
    WITH u, e, f
    MATCH p=(u:FreshUser)-[e:FreshOwns]->(f:FreshFolder)
    RETURN u.name = 'Neo', type(e) = 'FreshOwns', f.name = 'Inbox'
$$) AS (user_ok agtype, edge_ok agtype, folder_ok agtype);

-- An invalid deferred label must still register MATCH variables and execute
-- the preceding write instead of allowing constant folding to remove it.
SELECT * FROM cypher('match_new_label_visibility', $$
    CREATE (created:FreshUser {name: 'Alice'})
    WITH created
    MATCH (missing:NeverCreated)
    RETURN missing
$$) AS (result agtype);

SELECT * FROM cypher('match_new_label_visibility', $$
    MATCH (created:FreshUser {name: 'Alice'})
    RETURN created.name
$$) AS (result agtype);

SELECT drop_graph('match_new_label_visibility', true);

--
-- issue 2378 / #2380: OPTIONAL MATCH WHERE belongs to the LEFT JOIN ON clause.
--
SELECT create_graph('optional_match_predicate');

SELECT * FROM cypher('optional_match_predicate', $$
    CREATE (a:Person {name: 'Alice'}),
           (b:Person {name: 'Bob'}),
           (c:Person {name: 'Charlie'}),
           (a)-[:KNOWS]->(b),
           (a)-[:KNOWS]->(c)
$$) AS (result agtype);

-- Correlated EXISTS referencing the optional variable.
SELECT * FROM cypher('optional_match_predicate', $$
    MATCH (p:Person)
    OPTIONAL MATCH (p)-[:KNOWS]->(friend:Person)
    WHERE EXISTS {(friend)-[:KNOWS]->(:Person)}
    RETURN p.name AS name, friend.name AS friend
    ORDER BY name
$$) AS (name agtype, friend agtype);

-- Correlated EXISTS referencing the outer variable.
SELECT * FROM cypher('optional_match_predicate', $$
    MATCH (p:Person)
    OPTIONAL MATCH (p)-[:KNOWS]->(friend:Person)
    WHERE EXISTS {(p)-[:KNOWS]->(:Person)}
    RETURN p.name AS name, friend.name AS friend
    ORDER BY name, friend
$$) AS (name agtype, friend agtype);

-- Non-correlated EXISTS guard.
SELECT * FROM cypher('optional_match_predicate', $$
    MATCH (p:Person)
    OPTIONAL MATCH (p)-[:KNOWS]->(friend:Person)
    WHERE EXISTS {MATCH (x:Person) RETURN x}
    RETURN p.name AS name, friend.name AS friend
    ORDER BY name, friend
$$) AS (name agtype, friend agtype);

-- Scalar predicate guard.
SELECT * FROM cypher('optional_match_predicate', $$
    MATCH (p:Person)
    OPTIONAL MATCH (p)-[:KNOWS]->(friend:Person)
    WHERE friend.name = 'Bob'
    RETURN p.name AS name, friend.name AS friend
    ORDER BY name
$$) AS (name agtype, friend agtype);

-- Constant-false guard.
SELECT * FROM cypher('optional_match_predicate', $$
    MATCH (p:Person)
    OPTIONAL MATCH (p)-[:KNOWS]->(friend:Person)
    WHERE false
    RETURN p.name AS name, friend.name AS friend
    ORDER BY name
$$) AS (name agtype, friend agtype);

SELECT drop_graph('optional_match_predicate', true);

-- #2339: parameterized =properties uses top-level containment.
SELECT create_graph('match_equals_parameter');

SELECT * FROM cypher('match_equals_parameter', $$
    CREATE (:Person {name: 'Alice', address: {city: 'Toronto', zip: 1}}),
           (:Person {name: 'Bob', address: {city: 'Toronto'}}),
           (:Person {name: 'Alice'})-[:KNOWS {since: 2020}]->(:Person {name: 'Bob'})
$$) AS (result agtype);

PREPARE match_equals_vertex(agtype) AS
SELECT count(*) FROM cypher('match_equals_parameter',
    $$ MATCH (n = $props) RETURN n $$, $1) AS (result agtype);
EXECUTE match_equals_vertex(
    '{"props": {"address": {"city": "Toronto"}}}');
DEALLOCATE match_equals_vertex;

PREPARE match_contains_vertex(agtype) AS
SELECT count(*) FROM cypher('match_equals_parameter',
    $$ MATCH (n $props) RETURN n $$, $1) AS (result agtype);
EXECUTE match_contains_vertex(
    '{"props": {"address": {"city": "Toronto"}}}');
DEALLOCATE match_contains_vertex;

PREPARE match_equals_edge(agtype) AS
SELECT count(*) FROM cypher('match_equals_parameter',
    $$ MATCH ()-[r = $props]->() RETURN r $$, $1) AS (result agtype);
EXECUTE match_equals_edge('{"props": {"since": 2020}}');
DEALLOCATE match_equals_edge;

SELECT drop_graph('match_equals_parameter', true);

-- #1288: standalone anonymous MATCH clauses preserve row cardinality.
SELECT create_graph('anonymous_match_cardinality');

SELECT * FROM cypher('anonymous_match_cardinality', $$
    CREATE (:Part {n: 1}), (:Part {n: 2}),
           (:Part {n: 3}), (:Part {n: 4})
$$) AS (result agtype);

SELECT * FROM cypher('anonymous_match_cardinality', $$
    MATCH (:Part) RETURN count(*)
$$) AS (result agtype);

SELECT count(*) FROM cypher('anonymous_match_cardinality', $$
    MATCH (:Part) RETURN 0
$$) AS (result agtype);

SELECT * FROM cypher('anonymous_match_cardinality', $$
    MATCH (:Part) MATCH (:Part) RETURN count(*)
$$) AS (result agtype);

SELECT drop_graph('anonymous_match_cardinality', true);

-- #1295: path variables declared by MATCH are visible in its WHERE clause.
SELECT create_graph('path_where_contract');

SELECT * FROM cypher('path_where_contract', $$
    CREATE (:N {n: 1})-[:R]->(:N {n: 2})-[:R]->(:N {n: 3})
$$) AS (result agtype);

SELECT * FROM cypher('path_where_contract', $$
    MATCH p=()-[*]->()
    RETURN length(p)
$$) AS (length agtype);

SELECT * FROM cypher('path_where_contract', $$
    MATCH p=()-[*]->()
    WHERE length(p) > 1
    RETURN length(p)
$$) AS (length agtype);

SELECT * FROM cypher('path_where_contract', $$
    MATCH p=()-[*]->()
    WHERE size(nodes(p)) = 3
    RETURN nodes(p)[0]
$$) AS (first_node agtype);

SELECT * FROM cypher('path_where_contract', $$
    MATCH (n:N {n: 1})
    MATCH p=()-[*]->()
    WHERE nodes(p)[0] = n
    RETURN length(p)
$$) AS (length agtype);

SELECT * FROM cypher('path_where_contract', $$
    MATCH p1=(n:N {n: 1})-[]->()
    MATCH p2=()-[*]->()
    WHERE p2 = p1
    RETURN p2 = p1
$$) AS (same_path agtype);

-- #1400: a missing relationship label inside EXISTS is false, not an error.
SELECT * FROM cypher('path_where_contract', $$
    MATCH (n:N)
    WHERE exists((n)-[:MISSING]->())
    RETURN count(n)
$$) AS (matched agtype);

SELECT * FROM cypher('path_where_contract', $$
    MATCH (n:N)
    WHERE NOT exists((n)-[:MISSING]->())
    RETURN count(n)
$$) AS (matched agtype);

SELECT drop_graph('path_where_contract', true);

--
-- ORDER BY with several items: the resjunk target entries added for earlier
-- sort items have no resname and must be skipped by the alias lookup.
--
SELECT create_graph('cypher_match_sort');
SELECT * FROM cypher('cypher_match_sort', $$
    CREATE (:P {name: 'a'})-[:E]->(:P {name: 'b'}),
           (:P {name: 'c'})-[:E]->(:P {name: 'd'})
$$) AS (r agtype);
SELECT * FROM cypher('cypher_match_sort', $$
    MATCH (n:P)-[]->(m:P) RETURN n.name ORDER BY m.name, m
$$) AS (name agtype);
SELECT * FROM cypher('cypher_match_sort', $$
    MATCH (n:P)-[]->(m:P) RETURN n.name AS src, m.name ORDER BY m.name DESC, src, n
$$) AS (src agtype, dst agtype);
SELECT drop_graph('cypher_match_sort', true);

--
-- Clean up
--
SELECT drop_graph('cypher_match', true);

--
-- End
--
