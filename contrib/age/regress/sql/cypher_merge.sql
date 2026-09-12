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

SELECT create_graph('cypher_merge');


/*
 * Section 1: MERGE with single vertex
 */
/*
 * test 1: Single MERGE Clause, path doesn't exist
 */
--test query
SELECT * FROM cypher('cypher_merge', $$MERGE (n {i: "Hello Merge", j: (null IS NULL), k: (null IS NOT NULL)})$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (n agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 2: Single MERGE Clause, path exists
 */
--data setup
SELECT * FROM cypher('cypher_merge', $$CREATE ({i: "Hello Merge", j: (null IS NULL)}) $$) AS (a agtype);

--test_query
SELECT * FROM cypher('cypher_merge', $$MERGE ({i: "Hello Merge"})$$) AS (a agtype);
SELECT * FROM cypher('cypher_merge', $$MERGE ({j: (null IS NULL)})$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (n agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 3: Prev clause returns no results, no data created
 */
--test query
SELECT * FROM cypher('cypher_merge', $$MATCH (n) MERGE ({i: n.i})$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (n agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 4: Prev clause has results, path exists
 */
--test query
SELECT * FROM cypher('cypher_merge', $$CREATE ({i: "Hello Merge"}) $$) AS (a agtype);
SELECT * FROM cypher('cypher_merge', $$MATCH (n) MERGE ({i: n.i})$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (n agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 5: Prev clause has results, path does not exist (differnt property name)
 */
--data setup
SELECT * FROM cypher('cypher_merge', $$CREATE ({i: "Hello Merge"}) $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MATCH (n) MERGE ({j: n.i})$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (n agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 6: MERGE with no prev clause, filters correctly, data created
 */
-- setup
SELECT * FROM cypher('cypher_merge', $$CREATE ({i: 2}) $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MERGE (n {i: 1}) RETURN n$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (n agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 7: MERGE with no prev clause, filters correctly, no data created
 */
-- setup
SELECT * FROM cypher('cypher_merge', $$CREATE ({i: 1}) $$) AS (a agtype);
SELECT * FROM cypher('cypher_merge', $$CREATE ({i: 1}) $$) AS (a agtype);
SELECT * FROM cypher('cypher_merge', $$CREATE ({i: 2}) $$) AS (a agtype);
SELECT * FROM cypher('cypher_merge', $$CREATE () $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MERGE (n {i: 1}) RETURN n$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (n agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);


/*
 * Section 2: MERGE with edges
 */

/*
 * test 8: MERGE creates edge
 */
-- setup
SELECT * FROM cypher('cypher_merge', $$CREATE () $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MATCH (n) MERGE (n)-[:e]->(:v)$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (n)-[e:e]->(m:v) RETURN n, e, m$$) AS (n agtype, e agtype, m agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);


/*
 * test 9: edge already exists
 */
-- setup
SELECT * FROM cypher('cypher_merge', $$CREATE ()-[:e]->() $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MERGE (n)-[:e]->(:v)$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (n)-[e:e]->(m:v) RETURN n, e, m$$) AS (n agtype, e agtype, m agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 10: edge doesn't exist, using MATCH
 */
-- setup
SELECT * FROM cypher('cypher_merge', $$CREATE () $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MATCH (n) MERGE (n)-[:e]->(:v)$$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH (n)-[e:e]->(m:v) RETURN n, e, m$$) AS (n agtype, e agtype, m agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 11: edge already exists, using MATCH
 */
-- setup
SELECT * FROM cypher('cypher_merge', $$CREATE ()-[:e]->() $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MATCH (n) MERGE (n)-[:e]->(:v)$$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH (n)-[e:e]->(m:v) RETURN n, e, m$$) AS (n agtype, e agtype, m agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 12: Partial Path Exists, creates whole path
 */
-- setup
SELECT * FROM cypher('cypher_merge', $$CREATE ()-[:e]->() $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MERGE ()-[:e]->()-[:e]->()$$) AS (a agtype);

--validate created correctly
--Returns 3. One for the data setup and 2 for the longer path in MERGE
SELECT count(*) FROM cypher('cypher_merge', $$MATCH p=()-[e:e]->() RETURN p$$) AS (p agtype)

-- Returns 1, the path created in MERGE
SELECT count(*) FROM cypher('cypher_merge', $$MATCH p=()-[:e]->()-[]->() RETURN p$$) AS (p agtype);

-- 5 vertices total should have been created
SELECT count(*) FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (n agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 13: edge doesn't exists (differnt label), using MATCH
 */
-- setup
SELECT * FROM cypher('cypher_merge', $$CREATE ()-[:e]->() $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MATCH (n) MERGE (n)-[:e_new]->(:v)$$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH (n)-[e]->(m:v) RETURN n, e, m$$) AS (n agtype, e agtype, m agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 14: edge doesn't exists (different label), without MATCH
 */
-- setup
SELECT * FROM cypher('cypher_merge', $$CREATE ()-[:e]->() $$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MERGE (n)-[:e_new]->(:v)$$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH (n)-[e]->(m:v) RETURN n, e, m$$) AS (n agtype, e agtype, m agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * Section 3: MERGE with writing clauses
 */

/*
 * test 15:
 */

--test query
SELECT * FROM cypher('cypher_merge', $$CREATE () MERGE (n)$$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (n agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 16:
 */

--test query
SELECT * FROM cypher('cypher_merge', $$CREATE (n) WITH n as a MERGE (a)-[:e]->() $$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH p=()-[:e]->() RETURN p$$) AS (p agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);


/*
 * test 17:
 * XXX: Incorrect Output. To FIX
 */

--test query
SELECT * FROM cypher('cypher_merge', $$CREATE (n) MERGE (n)-[:e]->() $$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH p=()-[:e]->() RETURN p$$) AS (p agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);


/*
 * test 18:
 */

--test query
SELECT * FROM cypher('cypher_merge', $$CREATE (n {i : 1}) SET n.i = 2 MERGE ({i: 2}) $$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH (a) RETURN a$$) AS (a agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 19:
 */
--test query
SELECT * FROM cypher('cypher_merge', $$CREATE (n {i : 1}) SET n.i = 2 WITH n as a MERGE ({i: 2}) $$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH (a) RETURN a$$) AS (a agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 20:
 */
--data setup
SELECT * FROM cypher('cypher_merge', $$CREATE (n {i : 1})$$) AS (a agtype);


--test query
SELECT * FROM cypher('cypher_merge', $$MATCH (n {i : 1}) SET n.i = 2 WITH n as a MERGE ({i: 2}) $$) AS (a agtype);

--validate created correctly
SELECT * FROM cypher('cypher_merge', $$MATCH (a) RETURN a$$) AS (a agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 21:
 */
--data setup
SELECT * FROM cypher('cypher_merge', $$CREATE (n {i : 1})$$) AS (a agtype);


--test query
SELECT * FROM cypher('cypher_merge', $$MATCH (n {i : 1}) DELETE n  MERGE (n)-[:e]->() $$) AS (a agtype);

--validate, transaction was rolled back because of the error message
SELECT * FROM cypher('cypher_merge', $$MATCH (a) RETURN a$$) AS (a agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 22:
 * MERGE after MERGE
 */
SELECT * FROM cypher('cypher_merge', $$
    CREATE (n:Person {name : "Rob Reiner", bornIn: "New York"})
$$) AS (a agtype);

SELECT * FROM cypher('cypher_merge', $$
    CREATE (n:Person {name : "Michael Douglas", bornIn: "New Jersey"})
$$) AS (a agtype);

SELECT * FROM cypher('cypher_merge', $$
    CREATE (n:Person {name : "Martin Sheen", bornIn: "Ohio"})
$$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$
    MATCH (person:Person)
    MERGE (city:City {name: person.bornIn})
    MERGE (person)-[r:BORN_IN]->(city)
    RETURN person.name, person.bornIn, city
$$) AS (name agtype, bornIn agtype, city agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (a) RETURN a$$) AS (a agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 23:
 */
SELECT * FROM cypher('cypher_merge', $$MERGE ()-[:e]-()$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH p=()-[]->() RETURN p$$) AS (a agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 24:
 */
SELECT * FROM cypher('cypher_merge', $$MERGE (a) RETURN a$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH (a) RETURN a$$) AS (a agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 25:
 */
SELECT * FROM cypher('cypher_merge', $$MERGE p=()-[:e]-() RETURN p$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH p=()-[]->() RETURN p$$) AS (a agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 26:
 */
SELECT * FROM cypher('cypher_merge', $$MERGE (a)-[:e]-(b) RETURN a$$) AS (a agtype);

--validate
SELECT * FROM cypher('cypher_merge', $$MATCH p=()-[]->() RETURN p$$) AS (a agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * test 27:
 */
SELECT  * FROM cypher('cypher_merge', $$CREATE p=()-[:e]->() RETURN p$$) AS (a agtype);

SELECT * FROM cypher('cypher_merge', $$MERGE p=()-[:e]-() RETURN p$$) AS (a agtype);


--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

/*
 * Section 4: Error Messages
 */
/*
 * test 28:
 * Only single paths allowed
 */
SELECT * FROM cypher('cypher_merge', $$MERGE (n), (m) RETURN n, m$$) AS (a agtype, b agtype);

/*
 * test 29:
 * Edges cannot reference existing variables
 */
SELECT * FROM cypher('cypher_merge', $$MATCH ()-[e]-() MERGE ()-[e]->()$$) AS (a agtype);

/*
 * test 30:
 * NULL vertex given to MERGE
 */
--data setup
SELECT * FROM cypher('cypher_merge', $$CREATE (n)$$) AS (a agtype);

--test query
SELECT * FROM cypher('cypher_merge', $$MATCH (n) OPTIONAL MATCH (n)-[:e]->(m) MERGE (m)$$) AS (a agtype);

-- validate only 1 vertex exits
SELECT * FROM cypher('cypher_merge', $$MATCH (n) RETURN n$$) AS (a agtype);


--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);

--
-- ON CREATE SET / ON MATCH SET tests (issue #1619)
--
SELECT create_graph('merge_actions');

-- Basic ON CREATE SET: first run creates the node
SELECT * FROM cypher('merge_actions', $$
  MERGE (n:Person {name: 'Alice'})
    ON CREATE SET n.created = true
  RETURN n.name, n.created
$$) AS (name agtype, created agtype);

-- ON MATCH SET: second run matches the existing node
SELECT * FROM cypher('merge_actions', $$
  MERGE (n:Person {name: 'Alice'})
    ON MATCH SET n.found = true
  RETURN n.name, n.created, n.found
$$) AS (name agtype, created agtype, found agtype);

-- Both ON CREATE SET and ON MATCH SET (first run = create)
SELECT * FROM cypher('merge_actions', $$
  MERGE (n:Person {name: 'Bob'})
    ON CREATE SET n.created = true
    ON MATCH SET n.matched = true
  RETURN n.name, n.created, n.matched
$$) AS (name agtype, created agtype, matched agtype);

-- Both ON CREATE SET and ON MATCH SET (second run = match)
SELECT * FROM cypher('merge_actions', $$
  MERGE (n:Person {name: 'Bob'})
    ON CREATE SET n.created = true
    ON MATCH SET n.matched = true
  RETURN n.name, n.created, n.matched
$$) AS (name agtype, created agtype, matched agtype);

-- ON CREATE SET with MERGE after MATCH (Case 1: has predecessor, first run = create)
SELECT * FROM cypher('merge_actions', $$
  MATCH (a:Person {name: 'Alice'})
  MERGE (a)-[:KNOWS]->(b:Person {name: 'Charlie'})
    ON CREATE SET b.source = 'merge_create'
  RETURN a.name, b.name, b.source
$$) AS (a agtype, b agtype, source agtype);

-- ON MATCH SET with MERGE after MATCH (Case 1: has predecessor, second run = match)
SELECT * FROM cypher('merge_actions', $$
  MATCH (a:Person {name: 'Alice'})
  MERGE (a)-[:KNOWS]->(b:Person {name: 'Charlie'})
    ON MATCH SET b.visited = true
  RETURN a.name, b.name, b.visited
$$) AS (a agtype, b agtype, visited agtype);

-- Multiple SET items in a single ON CREATE SET
SELECT * FROM cypher('merge_actions', $$
  MERGE (n:Person {name: 'Dave'})
    ON CREATE SET n.a = 1, n.b = 2
  RETURN n.name, n.a, n.b
$$) AS (name agtype, a agtype, b agtype);

-- Reverse order: ON MATCH before ON CREATE should work
SELECT * FROM cypher('merge_actions', $$
  MERGE (n:Person {name: 'Eve'})
    ON MATCH SET n.seen = true
    ON CREATE SET n.new = true
  RETURN n.name, n.new
$$) AS (name agtype, new agtype);

-- Error: ON CREATE SET specified more than once
SELECT * FROM cypher('merge_actions', $$
  MERGE (n:Person {name: 'Bad'})
    ON CREATE SET n.a = 1
    ON CREATE SET n.b = 2
  RETURN n
$$) AS (n agtype);

-- Error: ON MATCH SET specified more than once
SELECT * FROM cypher('merge_actions', $$
  MERGE (n:Person {name: 'Bad'})
    ON MATCH SET n.a = 1
    ON MATCH SET n.b = 2
  RETURN n
$$) AS (n agtype);

-- Chained (non-terminal) MERGE with ON CREATE SET (eager-buffering path)
SELECT * FROM cypher('merge_actions', $$
  MERGE (a:Person {name: 'Frank'})
    ON CREATE SET a.created = true
  MERGE (a)-[:KNOWS]->(b:Person {name: 'Grace'})
    ON CREATE SET b.created = true
  RETURN a.name, a.created, b.name, b.created
$$) AS (a_name agtype, a_created agtype, b_name agtype, b_created agtype);

-- Chained (non-terminal) MERGE with ON MATCH SET (second run = match)
SELECT * FROM cypher('merge_actions', $$
  MERGE (a:Person {name: 'Frank'})
    ON MATCH SET a.matched = true
  MERGE (a)-[:KNOWS]->(b:Person {name: 'Grace'})
    ON MATCH SET b.matched = true
  RETURN a.name, a.matched, b.name, b.matched
$$) AS (a_name agtype, a_matched agtype, b_name agtype, b_matched agtype);

-- ON keyword as label name (backward compat via safe_keywords)
SELECT * FROM cypher('merge_actions', $$
  CREATE (n:on {name: 'test'})
  RETURN n.name
$$) AS (name agtype);

-- Issue #2347: RHS of ON CREATE / ON MATCH SET referencing a bound
-- variable crashed the backend when MERGE had a previous clause, because
-- the lateral-join's ParseNamespaceItem had p_nscolumns=NULL.

-- ON CREATE SET with RHS referencing the outer MATCH's variable
SELECT * FROM cypher('merge_actions', $$ CREATE (:Person {name:'Anchor'}) $$) AS (a agtype);
SELECT * FROM cypher('merge_actions', $$
  MATCH (a:Person {name: 'Anchor'})
  MERGE (b:Person {name: 'FromOuter'})
    ON CREATE SET b.source_name = a.name
  RETURN a.name, b.name, b.source_name
$$) AS (a_name agtype, b_name agtype, b_source agtype);

-- ON CREATE SET with RHS referencing the MERGE-bound variable itself
SELECT * FROM cypher('merge_actions', $$
  MATCH (a:Person {name: 'Anchor'})
  MERGE (b:Person {name: 'SelfRef'})
    ON CREATE SET b.echo_name = b.name
  RETURN b.name, b.echo_name
$$) AS (b_name agtype, b_echo agtype);

-- ON CREATE SET driven by UNWIND with self-reference on the RHS
-- (Muhammad's second reproducer)
SELECT * FROM cypher('merge_actions', $$
  UNWIND ['U1', 'U2'] AS nm
  MERGE (n:Person {name: nm})
    ON CREATE SET n.copy_name = n.name
  RETURN n.name, n.copy_name
$$) AS (n_name agtype, n_copy agtype);

-- Multiple SET items mixing outer-ref, self-ref, and literal RHS
SELECT * FROM cypher('merge_actions', $$
  MATCH (a:Person {name: 'Anchor'})
  MERGE (b:Person {name: 'MultiItem'})
    ON CREATE SET b.from_a = a.name, b.self = b.name, b.lit = 'literal'
  RETURN b.from_a, b.self, b.lit
$$) AS (fa agtype, sf agtype, lit agtype);

-- ON MATCH SET with variable RHS (second run on existing node)
SELECT * FROM cypher('merge_actions', $$
  MATCH (a:Person {name: 'Anchor'})
  MERGE (b:Person {name: 'FromOuter'})
    ON CREATE SET b.source_name = a.name
    ON MATCH SET b.last_seen_by = a.name
  RETURN b.source_name, b.last_seen_by
$$) AS (src agtype, last agtype);

-- #2344/#2347 integration: duplicate-path reuse must retain action updates.
SELECT * FROM cypher('merge_actions', $$
  CREATE (:ActionInput {id: 1}), (:ActionInput {id: 2}),
         (:ActionInput {id: 3})
$$) AS (a agtype);
SELECT * FROM cypher('merge_actions', $$
  MATCH (i:ActionInput)
  MERGE (s:ActionShared {key: 1})
    ON CREATE SET s.created = true
    ON MATCH SET s.reused = coalesce(s.reused, 0) + 1
$$) AS (a agtype);
SELECT * FROM cypher('merge_actions', $$
  MATCH (s:ActionShared)
  RETURN s.key, s.created, s.reused
$$) AS (key agtype, created agtype, reused agtype);

-- cleanup
SELECT * FROM cypher('merge_actions', $$ MATCH (n) DETACH DELETE n $$) AS (a agtype);
SELECT drop_graph('merge_actions', true);

/*
 * Section 5: MERGE visibility in chained commands, SET specifically
 * (upstream commit 99e7c625d9). A tuple created by MERGE must be
 * visible to the chained clauses that follow, otherwise their disk
 * updates are silently lost while the RETURN projection still shows
 * the in-memory value.
 */
-- node doesn't exist, is created, then the chained SET must persist
SELECT * FROM cypher('cypher_merge', $$MERGE (n:node {name: 'Jason'}) SET n.name = 'Lisa' RETURN properties(n)$$) AS (n agtype);

-- re-check with an independent MATCH: the stored name must be 'Lisa'
SELECT * FROM cypher('cypher_merge', $$MATCH (n:node) RETURN properties(n)$$) AS (n agtype);

-- MERGE create path chained with multiple SET updates
SELECT * FROM cypher('cypher_merge', $$MERGE (n:node {name: 'Kevin'}) SET n.age = 23, n.gender = 'male' SET n.city = 'Busan' RETURN properties(n)$$) AS (n agtype);

-- re-check with an independent MATCH: all SET updates must be stored
SELECT * FROM cypher('cypher_merge', $$MATCH (n:node {name: 'Kevin'}) RETURN properties(n)$$) AS (n agtype);

-- control: MERGE match path chained with SET keeps persisting
SELECT * FROM cypher('cypher_merge', $$MERGE (n:node {name: 'Lisa'}) SET n.name = 'Mike' RETURN properties(n)$$) AS (n agtype);

-- re-check with an independent MATCH: the stored name must be 'Mike'
SELECT * FROM cypher('cypher_merge', $$MATCH (n:node {name: 'Mike'}) RETURN properties(n)$$) AS (n agtype);

-- Apache AGE #1515: standalone MERGE cannot reuse an existing variable;
-- connected vertex endpoints remain reusable and edge names remain unique.
SELECT * FROM cypher('cypher_merge', $$
    MERGE (n) MERGE (n) RETURN n
$$) AS (n agtype);
SELECT * FROM cypher('cypher_merge', $$
    MATCH (n) MERGE (n) RETURN n
$$) AS (n agtype);
SELECT * FROM cypher('cypher_merge', $$
    CREATE (n) MERGE (n) RETURN n
$$) AS (n agtype);
SELECT * FROM cypher('cypher_merge', $$
    MATCH (n) WITH n AS r MERGE (r) RETURN r
$$) AS (r agtype);
SELECT * FROM cypher('cypher_merge', $$
    WITH {id: 281474976710657, label: "", properties: {}}::vertex AS n
    MERGE (n) RETURN n
$$) AS (n agtype);
SELECT * FROM cypher('cypher_merge', $$
    MERGE (n)-[e:merge_reuse]->(n)-[e1:merge_reuse]->(n)
    MERGE (n) RETURN e
$$) AS (e agtype);
SELECT * FROM cypher('cypher_merge', $$
    MERGE (n)-[e:merge_reuse]->(m)
    MERGE (n)-[e:merge_reuse]->(m) RETURN e
$$) AS (e agtype);
SELECT * FROM cypher('cypher_merge', $$
    MERGE (n)-[e:merge_reuse]->(m)
    MERGE (e)-[:merge_reuse]->() RETURN e
$$) AS (e agtype);
SELECT * FROM cypher('cypher_merge', $$
    WITH {id: 1407374883553281, label: "merge_reuse",
          end_id: 281474976710658, start_id: 281474976710657,
          properties: {}}::edge AS e
    MERGE ()-[e:merge_reuse]->() RETURN e
$$) AS (e agtype);

-- Connected endpoint reuse remains valid.
SELECT * FROM cypher('cypher_merge', $$
    MERGE (n)-[:merge_reuse]->(m)
    MERGE (n)-[:merge_reuse]->()
$$) AS (result agtype);
SELECT * FROM cypher('cypher_merge', $$
    MERGE (n:MergeReuseContract {key: 'contract'}) WITH n AS r
    MERGE (r)-[:merge_reuse]->() RETURN properties(r)
$$) AS (reused agtype);
-- Variables removed by WITH are out of scope and can be declared again.
SELECT * FROM cypher('cypher_merge', $$
    CREATE (n), (m) WITH n AS r MERGE (m)
$$) AS (redeclared agtype);

--clean up
SELECT * FROM cypher('cypher_merge', $$MATCH (n) DETACH DELETE n $$) AS (a agtype);
SELECT drop_label('cypher_merge', 'node');

/*
 * Clean up graph
 */
SELECT drop_graph('cypher_merge', true);

/*
 * Issue #1446/#2344: duplicate-path reuse and eager chained MERGE.
 */
SELECT create_graph('cypher_merge_eager');

-- Terminal MERGE must reuse a path created by an earlier input row.
SELECT * FROM cypher('cypher_merge_eager', $$
    CREATE (:Input {id: 1}), (:Input {id: 2})
$$) AS (a agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH (x:Input)
    MERGE (:Shared {key: 1})
$$) AS (a agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH (n:Shared)
    RETURN count(*) AS shared_count
$$) AS (shared_count agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH (n) DETACH DELETE n
$$) AS (a agtype);

-- Non-terminal MERGE must buffer all rows before the sibling MERGE starts.
SELECT * FROM cypher('cypher_merge_eager', $$
    CREATE (:A), (:C)
$$) AS (a agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH (x)
    MERGE (x)-[:r]->(:t)
    MERGE (:C)-[:r]->(:t)
    RETURN count(*) AS row_count
$$) AS (row_count agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH (n:C)
    RETURN count(*) AS c_count
$$) AS (c_count agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH (n:t)
    RETURN count(*) AS t_count
$$) AS (t_count agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH ()-[e:r]->()
    RETURN count(*) AS edge_count
$$) AS (edge_count agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH (n) DETACH DELETE n
$$) AS (a agtype);

-- Empty input must remain empty and must not fall through to path creation.
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH (x:MissingInput)
    MERGE (x)-[:empty_r]->(:EmptyTarget)
    MERGE (:EmptySibling)-[:empty_r]->(:EmptyTarget)
    RETURN count(*) AS row_count
$$) AS (row_count agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH (n)
    RETURN count(*) AS node_count
$$) AS (node_count agtype);
SELECT * FROM cypher('cypher_merge_eager', $$
    MATCH ()-[e]->()
    RETURN count(*) AS edge_count
$$) AS (edge_count agtype);

SELECT drop_graph('cypher_merge_eager', true);

--
-- Issue 1954 (upstream): MERGE with ORDER BY on the same query level.
-- On openGauss this also covers the planner hook keeping the MERGE path
-- when query_pathkeys is set.
--
SELECT * FROM create_graph('issue_1954');

-- Setup: create source nodes and relationships (3 rows to trigger the bug)
SELECT * FROM cypher('issue_1954', $$
    CREATE (:A {name: 'a1'})-[:R]->(:B {name: 'b1'}),
           (:A {name: 'a2'})-[:R]->(:B {name: 'b2'}),
           (:A {name: 'a3'})-[:R]->(:B {name: 'b3'})
$$) AS (result agtype);

SELECT * FROM cypher('issue_1954', $$
    MATCH (a:A)-[:R]->(b:B)
    CREATE (c:C {name: a.name + '|' + b.name})
    WITH a, b, c
    MERGE (a)-[:LINK]->(c)
    RETURN a.name, b.name, c.name
    ORDER BY a.name
$$) AS (a agtype, b agtype, c agtype);

-- Verify edges were created
SELECT * FROM cypher('issue_1954', $$
    MATCH (a:A)-[:LINK]->(c:C)
    RETURN a.name, c.name
    ORDER BY a.name
$$) AS (a agtype, c agtype);

-- Test with two MERGEs (more complex case from the original report)
SELECT * FROM cypher('issue_1954', $$
    MATCH ()-[e:LINK]->() DELETE e
$$) AS (result agtype);
SELECT * FROM cypher('issue_1954', $$
    MATCH (c:C) DELETE c
$$) AS (result agtype);

SELECT * FROM cypher('issue_1954', $$
    MATCH (a:A)-[:R]->(b:B)
    CREATE (c:C {name: a.name + '|' + b.name})
    WITH a, b, c
    MERGE (a)-[:LINK1]->(c)
    MERGE (b)-[:LINK2]->(c)
    RETURN a.name, b.name, c.name
    ORDER BY a.name
$$) AS (a agtype, b agtype, c agtype);

-- Verify both sets of edges
SELECT * FROM cypher('issue_1954', $$
    MATCH (a:A)-[:LINK1]->(c:C)
    RETURN a.name, c.name
    ORDER BY a.name
$$) AS (a agtype, c agtype);

SELECT * FROM cypher('issue_1954', $$
    MATCH (b:B)-[:LINK2]->(c:C)
    RETURN b.name, c.name
    ORDER BY b.name
$$) AS (b agtype, c agtype);

-- MERGE with a sorted RETURN of the merged node itself
SELECT * FROM cypher('issue_1954', $$
    MERGE (n:D {name: 'sorted'}) RETURN n.name ORDER BY n.name
$$) AS (name agtype);
SELECT * FROM cypher('issue_1954', $$
    MATCH (n:D) RETURN count(*)
$$) AS (cnt agtype);

-- openGauss: agtype constants inside the serialized clause data must round
-- trip exactly (binary, not through float8out/agtype_in).
SELECT * FROM cypher('issue_1954', $$
    MERGE (n:F {x: 0.5, y: 3.141592653589793}) RETURN n.x, n.y = 3.141592653589793
$$) AS (x agtype, exact agtype);
SELECT * FROM cypher('issue_1954', $$
    MERGE (n:F {x: 0.5, y: 3.141592653589793}) RETURN count(*)
$$) AS (cnt agtype);

SELECT drop_graph('issue_1954', true);
