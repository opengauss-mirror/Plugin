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

SELECT create_graph('cypher_unwind');

SELECT * FROM cypher('cypher_unwind', $$
    CREATE (node1 {name: 'node1', a: [1, 2, 3]}),
           (node2 {name: 'node2', a: [4, 5, 6]}),
           (node3 {name: 'node3', a: [7, 8, 9]}),
           (node1)-[:KNOWS]->(node2),
           (node2)-[:KNOWS]->(node3)
$$) as (i agtype);

SELECT * FROM cypher('cypher_unwind', $$
    UNWIND [1, 2, 3] AS i RETURN i
$$) as (i agtype);

SELECT * FROM cypher('cypher_unwind', $$
    MATCH (n)
    WITH n.a AS a
    UNWIND a AS i
    RETURN *
    ORDER BY a, i
$$) as (i agtype, j agtype);

SELECT * FROM cypher('cypher_unwind', $$
    WITH [[1, 2], [3, 4], 5] AS nested
    UNWIND nested AS x
    UNWIND x AS y
    RETURN y
$$) as (i agtype);

-- UNWIND vertices
SELECT * FROM cypher('cypher_unwind', $$
    MATCH p=()-[:KNOWS]->()
    UNWIND nodes(p) AS node
    RETURN node.name
    ORDER BY node.name
$$) as (name agtype);

-- UNWIND edges
SELECT * FROM cypher('cypher_unwind', $$
    MATCH p=()-[:KNOWS]->()
    UNWIND relationships(p) AS relation
    RETURN type(relation)
    ORDER BY type(relation)
$$) as (relation_type agtype);

-- UNWIND paths
SELECT * FROM cypher('cypher_unwind', $$
    MATCH p=({name: 'node1'})-[:KNOWS*1..2]->({name: 'node3'})
    UNWIND [p] AS path
    RETURN size(relationships(path))
$$) as (edge_count agtype);

-- #1304: SQL NULL input propagates through UNWIND without detoast errors.
SELECT * FROM cypher('cypher_unwind', $$
    UNWIND NULL AS i
    RETURN i
$$) AS (i agtype);

-- any SubLink-producing expression may be unwound, not only a list
-- comprehension
SELECT * FROM cypher('cypher_unwind', $$
    UNWIND reduce(acc = [], x IN [1, 2] | acc + [x]) AS y
    RETURN y
$$) AS (y agtype);
SELECT * FROM cypher('cypher_unwind', $$
    UNWIND [x IN [1, 2, 3] WHERE x > 1] AS y
    RETURN y
$$) AS (y agtype);

SELECT drop_graph('cypher_unwind', true);
