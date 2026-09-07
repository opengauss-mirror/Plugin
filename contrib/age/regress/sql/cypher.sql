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

SELECT create_graph('cypher');

-- cypher() function takes only a dollar-quoted string constant as an argument.
-- All other cases throw an error.

SELECT * FROM cypher('none', $$RETURN 0$$) as q(c agtype);

SELECT * FROM cypher('cypher', $$RETURN 0$$) AS r(c agtype);
WITH r(c) AS (
  SELECT * FROM cypher('cypher', $$RETURN 0$$) AS r(c agtype)
)
SELECT * FROM r;
SELECT * FROM cypher('cypher', 'RETURN 0') AS r(c text);
SELECT * FROM cypher('cypher', NULL) AS r(c text);
WITH q(s) AS (
  VALUES (textout($$RETURN 0$$))
)
SELECT * FROM q, cypher('cypher', q.s) AS r(c text);

-- The numbers of the attributes must match.

SELECT * FROM cypher('cypher', $$RETURN 0$$) AS r(c text, x text);

-- cypher() function can be called in ROWS FROM only if it is there solely.

SELECT * FROM ROWS FROM (cypher('cypher', $$RETURN 0$$) AS (c agtype));
SELECT * FROM ROWS FROM (cypher('cypher', $$RETURN 0$$) AS (c text),
                         generate_series(1, 2));

-- WITH ORDINALITY is not supported.

SELECT *
FROM ROWS FROM (cypher('cypher', $$RETURN 0$$) AS (c text)) WITH ORDINALITY;

-- cypher() function cannot be called in expressions.
-- However, it can be called in subqueries.

SELECT cypher('cypher', $$RETURN 0$$);
SELECT (SELECT * FROM cypher('cypher', $$RETURN 0$$) AS r(c agtype));

-- Attributes returned from cypher() function are agtype. If other than agtype
-- is given in the column definition list and there is a type coercion from
-- agtype to the given type, agtype will be coerced to that type. If there is
-- not such a coercion, an error is thrown.

SELECT * FROM cypher('cypher', $$RETURN true$$) AS (c bool);
SELECT * FROM cypher('cypher', $$RETURN 0$$) AS (c oid);

-- Apache AGE #1799: CREATE TABLE AS must preserve transformed Cypher
-- columns and named graph entity types through openGauss CTAS rewriting.
SELECT * FROM cypher('cypher', $$
    CREATE ()-[:knows]->()
$$) AS (result agtype);

CREATE TABLE cypher_ctas_vertices AS
    SELECT * FROM cypher('cypher', $$
        MATCH (u) RETURN u ORDER BY u.id
    $$) AS (u agtype);

CREATE TABLE cypher_ctas_edges AS
    SELECT * FROM cypher('cypher', $$
        MATCH ()-[e]->() RETURN e
    $$) AS (e agtype);

CREATE TABLE cypher_ctas_paths AS
    SELECT * FROM cypher('cypher', $$
        MATCH p=(u)-[e]->(v) RETURN u, e, v, p
    $$) AS (u agtype, e agtype, v agtype, p agtype);

SELECT count(*) AS vertex_count FROM cypher_ctas_vertices;
SELECT count(*) AS edge_count FROM cypher_ctas_edges;
SELECT count(*) AS path_count FROM cypher_ctas_paths;
SELECT age_id(u) IS NOT NULL AS vertex_ok,
       age_id(e) IS NOT NULL AS edge_ok,
       age_id(v) IS NOT NULL AS end_vertex_ok,
       age_size(age_nodes(p)) = '2'::agtype AS path_ok
FROM cypher_ctas_paths;

DROP TABLE cypher_ctas_vertices;
DROP TABLE cypher_ctas_edges;
DROP TABLE cypher_ctas_paths;

SELECT drop_graph('cypher', true);
