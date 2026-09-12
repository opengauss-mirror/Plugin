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

SELECT create_graph('list_comprehension');

SELECT * FROM cypher('list_comprehension', $$ RETURN [u IN [1, 2, 3]] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [u IN [1, 2, 3] WHERE u > 1] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [u IN [1, 2, 3] WHERE u > 1 | u * 2] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [u IN []] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [u IN null] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [{n: 1}, {n: 2}, {n: 3}] WHERE x.n > 1 | x.n] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [u IN [u IN [1, 2, 3] WHERE u > 1] WHERE u > 2 | u * 10] $$) AS (result agtype);

SELECT * FROM cypher('list_comprehension', $$ CREATE ({name: 'even', vals: [2, 4, 6]}) $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ CREATE ({name: 'mixed', vals: [1, 2, 3]}) $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ MATCH (u) RETURN u.name, [x IN u.vals WHERE x > 2 | x * 10] ORDER BY u.name $$) AS (name agtype, result agtype);
SELECT * FROM cypher('list_comprehension', $$ MATCH (u) WITH collect(u) AS ns RETURN [x IN ns WHERE x.name = 'even' | x.vals] $$) AS (result agtype);

-- Null elements remain list elements after mapping and participate in
-- openCypher's three-valued WHERE filtering.
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [null] | x + 1] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, null, 2] | x + 1] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, null, 2] | 1 + x] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, null, 2] | x] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, null, 2] | x - 1] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, null, 2] | x * 2] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, null, 2] | x / 1] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, null, 2] | x % 2] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, null, 2] | x ^ 2] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, null, 2] | -x] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [null, 1] WHERE x IS NULL] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [null, 1, null] WHERE x IS NULL] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [null, 1] WHERE x IS NOT NULL] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ RETURN [x IN [1, 2, 3] WHERE x IS NULL] $$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$ UNWIND [null, 1] AS x RETURN x, x IS NULL, x IS NOT NULL $$) AS (x agtype, a agtype, b agtype);

-- SET must propagate sublink metadata so the planner replaces the list
-- comprehension SubLink before costing the custom scan target list.
SELECT * FROM cypher('list_comprehension', $$
    CREATE (n:ListComprehensionSet {values: [1, 2, 3]})
    SET n.mapped = [x IN n.values WHERE x > 1 | x * 10]
    RETURN n.mapped
$$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$
    MATCH (n:ListComprehensionSet)
    RETURN n.mapped
$$) AS (result agtype);
SELECT * FROM cypher('list_comprehension', $$
    UNWIND [x IN [1, 2, 3] WHERE x > 1 | x * 10] AS value
    RETURN collect(value)
$$) AS (result agtype);

SELECT drop_graph('list_comprehension', true);
