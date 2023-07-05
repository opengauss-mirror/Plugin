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

-- Count the total paths from left (start) to right (end) -[]-> should be 400
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {}}::edge'::agtype, '1'::agtype, 'null'::agtype, '1'::agtype) group by ctid;

-- Count the total paths from right (end) to left (start) <-[]- should be 2
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {}}::edge'::agtype, '1'::agtype, 'null'::agtype, '-1'::agtype) group by ctid;

-- Count the total paths undirectional -[]- should be 7092
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {}}::edge'::agtype, '1'::agtype, 'null'::agtype, '0'::agtype) group by ctid;

-- All paths of length 3 -[]-> should be 2
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {}}::edge'::agtype, '3'::agtype, '3'::agtype, '1'::agtype);

-- All paths of length 3 <-[]- should be 1
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {}}::edge'::agtype, '3'::agtype, '3'::agtype, '-1'::agtype);

-- All paths of length 3 -[]- should be 12
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {}}::edge'::agtype, '3'::agtype, '3'::agtype, '0'::agtype);

-- Test edge label matching - should match 1
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "edge", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {}}::edge'::agtype, '1'::agtype, 'null'::agtype, '1'::agtype);

-- Test scalar property matching - should match 1
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {"name": "main edge"}}::edge'::agtype, '1'::agtype, 'null'::agtype, '1'::agtype);

-- Test object property matching - should match 4
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {"dangerous": {"type": "all", "level": "all"}}}::edge'::agtype, '1'::agtype, 'null'::agtype, '1'::agtype);

-- Test array property matching - should match 2
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {"packages": [1,3,5,7]}}::edge'::agtype, '1'::agtype, 'null'::agtype, '0'::agtype);

-- Test array property matching - should match 1
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {"packages": [2,4,6]}}::edge'::agtype, '1'::agtype, 'null'::agtype, '0'::agtype);

-- Test object property matching - should match 1
SELECT count(edges) FROM start_and_end_points, age_vle( '"cypher_vle"'::agtype, start_vertex, end_vertex, '{"id": 1111111111111111, "label": "", "end_id": 2222222222222222, "start_id": 333333333333333, "properties": {"dangerous": {"type": "poisons", "level": "all"}}}::edge'::agtype, '1'::agtype, 'null'::agtype, '0'::agtype);

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
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)<-[e*]-(v:end) RETURN e $$) AS (e agtype);
-- Should find 5
SELECT * FROM cypher('cypher_vle', $$MATCH p=(:begin)<-[*1..1]-()-[]-() RETURN p ORDER BY p $$) AS (e agtype);
-- Should find 2922
SELECT * FROM cypher('cypher_vle', $$MATCH p=()-[*]->(v) RETURN count(*) $$) AS (e agtype);
-- Should find 2
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)-[*3..3]->(v:end) RETURN p $$) AS (e agtype);
-- Should find 12
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)-[*3..3]-(v:end) RETURN p $$) AS (e agtype);
-- Each should find 2
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)<-[*]-(v:end) RETURN p $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)<-[e*]-(v:end) RETURN p $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)<-[e*]-(v:end) RETURN e $$) AS (e agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(:begin)<-[*]-()<-[]-(:end) RETURN p $$) AS (e agtype);
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
SELECT * FROM cypher('cypher_vle', $$ MATCH p=()<-[e1*]-(:end)-[e2*]->(:begin) RETURN p $$) AS (result agtype);
-- Each should return 3
SELECT * FROM cypher('cypher_vle', $$MATCH (u:begin)-[e*0..1]->(v) RETURN id(u), e, id(v) $$) AS (u agtype, e agtype, v agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u:begin)-[e*0..1]->(v) RETURN p $$) AS (p agtype);
-- Each should return 5
SELECT * FROM cypher('cypher_vle', $$MATCH (u)-[e*0..0]->(v) RETURN id(u), e, id(v) $$) AS (u agtype, e agtype, v agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=(u)-[e*0..0]->(v) RETURN id(u), p, id(v) $$) AS (u agtype, p agtype, v agtype);
-- Each should return 13 and will be the same
SELECT * FROM cypher('cypher_vle', $$MATCH p=()-[*0..0]->()-[]->() RETURN p $$) AS (p agtype);
SELECT * FROM cypher('cypher_vle', $$MATCH p=()-[]->()-[*0..0]->() RETURN p $$) AS (p agtype);
--
-- Clean up
--

DROP TABLE start_and_end_points;

SELECT drop_graph('cypher_vle', true);

--
-- End
--
