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

SELECT create_graph('predicate_functions');

--
-- all() predicate function
--
-- all elements satisfy predicate -> true
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [1, 2, 3] WHERE x > 0)
$$) AS (result agtype);

-- not all elements satisfy predicate -> false
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [1, 2, 3] WHERE x > 1)
$$) AS (result agtype);

-- empty list -> true (vacuous truth)
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [] WHERE x > 0)
$$) AS (result agtype);

--
-- any() predicate function
--
-- at least one element satisfies -> true
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [1, 2, 3] WHERE x > 2)
$$) AS (result agtype);

-- no element satisfies -> false
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [1, 2, 3] WHERE x > 5)
$$) AS (result agtype);

-- empty list -> false
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [] WHERE x > 0)
$$) AS (result agtype);

--
-- none() predicate function
--
-- no element satisfies predicate -> true
SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [1, 2, 3] WHERE x > 5)
$$) AS (result agtype);

-- at least one satisfies -> false
SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [1, 2, 3] WHERE x > 2)
$$) AS (result agtype);

-- empty list -> true
SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [] WHERE x > 0)
$$) AS (result agtype);

--
-- single() predicate function
--
-- exactly one element satisfies -> true
SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [1, 2, 3] WHERE x > 2)
$$) AS (result agtype);

-- more than one satisfies -> false
SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [1, 2, 3] WHERE x > 1)
$$) AS (result agtype);

-- none satisfies -> false
SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [1, 2, 3] WHERE x > 5)
$$) AS (result agtype);

-- empty list -> false
SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [] WHERE x > 0)
$$) AS (result agtype);

--
-- NULL list input: all four return null
-- (NULL-list guard in the grammar produces CASE WHEN expr IS NULL
-- THEN NULL ELSE <subquery> END)
--
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN null WHERE x > 0)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN null WHERE x > 0)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN null WHERE x > 0)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN null WHERE x > 0)
$$) AS (result agtype);

--
-- NULL predicate results: three-valued logic
--
-- Null list elements are SQL NULL at the predicate boundary, so strict
-- operators produce UNKNOWN and the predicate functions retain three-valued
-- logic until the aggregate result is determined.

-- only UNKNOWN, no TRUE -> any() = null
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [null] WHERE x > 0)
$$) AS (result agtype);

-- a definite TRUE dominates UNKNOWN for any()
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [null, 1, 2] WHERE x > 0)
$$) AS (result agtype);

-- a literal null predicate also produces UNKNOWN
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [1] WHERE null)
$$) AS (result agtype);

-- no FALSE, but one UNKNOWN -> all() = null
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [1, null, 2] WHERE x > 0)
$$) AS (result agtype);

-- a definite FALSE dominates UNKNOWN for all()
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [1, null, -1] WHERE x > 0)
$$) AS (result agtype);

-- only UNKNOWN, no TRUE -> none() = null
SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [null] WHERE x > 0)
$$) AS (result agtype);

-- a definite TRUE dominates UNKNOWN for none()
SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [null, 5] WHERE x > 0)
$$) AS (result agtype);

-- one definite TRUE plus UNKNOWN cannot prove exactly one
SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [null, 5] WHERE x > 0)
$$) AS (result agtype);

-- two definite TRUE values dominate UNKNOWN
SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [null, 5, 6] WHERE x > 0)
$$) AS (result agtype);

-- only UNKNOWN values cannot determine whether exactly one is TRUE
SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [null, null] WHERE x > 0)
$$) AS (result agtype);

-- one TRUE, one FALSE and one UNKNOWN still cannot prove exactly one
SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [1, null, -1] WHERE x > 0)
$$) AS (result agtype);

-- no TRUE and at least one UNKNOWN -> any() = null
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [null, -1] WHERE x > 0)
$$) AS (result agtype);

-- no FALSE and at least one UNKNOWN -> all() = null
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [1, null] WHERE x > 0)
$$) AS (result agtype);

-- a definite FALSE still dominates UNKNOWN
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [null, -1] WHERE x > 0)
$$) AS (result agtype);

-- no TRUE and at least one UNKNOWN -> none() = null
SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [null, -1] WHERE x > 0)
$$) AS (result agtype);

-- null elements are visible to IS NULL / IS NOT NULL predicates
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [1, null] WHERE x IS NULL)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [1, null] WHERE x IS NULL)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [null, null] WHERE x IS NULL)
$$) AS (result agtype);

--
-- Integration with graph data
--
SELECT * FROM cypher('predicate_functions', $$
    CREATE ({name: 'even', vals: [2, 4, 6, 8]})
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    CREATE ({name: 'mixed', vals: [1, 2, 3, 4]})
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    CREATE ({name: 'odd', vals: [1, 3, 5, 7]})
$$) AS (result agtype);

-- all() with graph properties
SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WHERE all(x IN u.vals WHERE x % 2 = 0)
    RETURN u.name
    ORDER BY u.name
$$) AS (result agtype);

-- any() with graph properties
SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WHERE any(x IN u.vals WHERE x > 6)
    RETURN u.name
    ORDER BY u.name
$$) AS (result agtype);

-- none() with graph properties
SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WHERE none(x IN u.vals WHERE x < 0)
    RETURN u.name
    ORDER BY u.name
$$) AS (result agtype);

-- single() with graph properties
SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WHERE single(x IN u.vals WHERE x = 8)
    RETURN u.name
    ORDER BY u.name
$$) AS (result agtype);

--
-- Property access on the loop variable
--
-- any: true (2 > 1) / false (none > 5)
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [{n: 1}, {n: 2}] WHERE x.n > 1)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [{n: 1}, {n: 2}] WHERE x.n > 5)
$$) AS (result agtype);

-- all: true (both > 0) / false (not all > 1)
SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [{n: 1}, {n: 2}] WHERE x.n > 0)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN all(x IN [{n: 1}, {n: 2}] WHERE x.n > 1)
$$) AS (result agtype);

-- none: true (neither > 2) / false (one matches)
SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [{n: 1}, {n: 2}] WHERE x.n > 2)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [{n: 1}, {n: 2}] WHERE x.n = 1)
$$) AS (result agtype);

-- single: true (exactly one) / false (both match)
SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [{n: 1}, {n: 2}] WHERE x.n = 1)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN single(x IN [{n: 1}, {n: 2}] WHERE x.n > 0)
$$) AS (result agtype);

-- Property access on vertex loop variables over a collected node list
-- any: true ('even' exists) / false (no 'missing')
SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WITH collect(u) AS ns
    RETURN any(x IN ns WHERE x.name = 'even')
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WITH collect(u) AS ns
    RETURN any(x IN ns WHERE x.name = 'missing')
$$) AS (result agtype);

-- all: true (all have non-empty vals) / false (not all named 'even')
SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WITH collect(u) AS ns
    RETURN all(x IN ns WHERE size(x.vals) > 0)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WITH collect(u) AS ns
    RETURN all(x IN ns WHERE x.name = 'even')
$$) AS (result agtype);

-- none: true (none 'missing') / false ('even' matches)
SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WITH collect(u) AS ns
    RETURN none(x IN ns WHERE x.name = 'missing')
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WITH collect(u) AS ns
    RETURN none(x IN ns WHERE x.name = 'even')
$$) AS (result agtype);

-- single: true (only one 'odd') / false (all have non-empty vals)
SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WITH collect(u) AS ns
    RETURN single(x IN ns WHERE x.name = 'odd')
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    MATCH (u) WITH collect(u) AS ns
    RETURN single(x IN ns WHERE size(x.vals) > 0)
$$) AS (result agtype);

--
-- Predicate functions in boolean expressions
--
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [1, 2, 3] WHERE x > 2)
           AND all(y IN [4, 5, 6] WHERE y > 0)
$$) AS (result agtype);

SELECT * FROM cypher('predicate_functions', $$
    RETURN none(x IN [1, 2, 3] WHERE x > 5)
           OR single(y IN [1, 2, 3] WHERE y = 2)
$$) AS (result agtype);

--
-- Nested predicate functions
--
SELECT * FROM cypher('predicate_functions', $$
    RETURN any(x IN [1, 2, 3] WHERE all(y IN [1, 2] WHERE y < x))
$$) AS (result agtype);

--
-- Keywords as property key names (safe_keywords backward compatibility)
--
SELECT * FROM cypher('predicate_functions', $$
    RETURN {any: 1, none: 2, single: 3}
$$) AS (result agtype);

--
-- Cleanup
--
SELECT * FROM drop_graph('predicate_functions', true);
