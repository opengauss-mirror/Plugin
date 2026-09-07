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

--
-- Load data
--
SELECT create_graph('cypher_with');

SELECT * FROM cypher('cypher_with', $$
    CREATE (andres {name : 'Andres', age : 36})
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    CREATE (caesar {name : 'Caesar', age : 25})
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    CREATE (bossman {name : 'Bossman', age : 55})
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    CREATE (david {name : 'David', age : 35})
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    CREATE (george {name : 'George', age : 37})
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    MATCH (andres {name : 'Andres'}), (caesar {name : 'Caesar'})
    CREATE (andres)-[:BLOCKS]->(caesar)
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    MATCH (andres {name : 'Andres'}), (bossman {name : 'Bossman'})
    CREATE (andres)-[:KNOWS]->(bossman)
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    MATCH (caesar {name : 'Caesar'}), (george {name : 'George'})
    CREATE (caesar)-[:KNOWS]->(george)
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    MATCH (bossman {name : 'Bossman'}), (david {name : 'David'})
    CREATE (bossman)-[:BLOCKS]->(david)
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    MATCH (bossman {name : 'Bossman'}), (george {name : 'George'})
    CREATE (bossman)-[:KNOWS]->(george)
$$) AS (a agtype);
SELECT * FROM cypher('cypher_with', $$
    MATCH (david {name : 'David'}), (andres {name : 'Andres'})
    CREATE (david)-[:KNOWS]->(andres)
$$) AS (a agtype);

--
-- Test WITH clause
--
SELECT * FROM cypher('cypher_with', $$
    MATCH (n)-[e]->(m)
    WITH n, e, m
    RETURN n.name, type(e), m.name
    ORDER BY id(n) ASC, id(e) ASC, id(m) ASC
$$) AS (start_node agtype, edge agtype, end_node agtype);

-- WITH/AS
SELECT * FROM cypher('cypher_with', $$
    MATCH (n)-[e]->(m)
    WITH n.name AS n1, e AS edge, m.name AS n2
    RETURN n1, type(edge), n2
    ORDER BY id(edge) ASC
$$) AS (start_node agtype, edge agtype, end_node agtype);

SELECT * FROM cypher('cypher_with', $$
    MATCH (person)-[r]->(otherPerson)
    WITH *, type(r) AS connectionType
    RETURN person.name, connectionType, otherPerson.name
    ORDER BY id(person) ASC, id(r) ASC, id(otherPerson) ASC
$$) AS (start_node agtype, connection agtype, end_node agtype);

SELECT * FROM cypher('cypher_with', $$
WITH true AS b
RETURN b
$$) AS (b bool);

-- WITH/WHERE
SELECT * FROM cypher('cypher_with', $$
    MATCH (george {name: 'George'})<-[]-(otherPerson)
    WITH otherPerson, toUpper(otherPerson.name) AS upperCaseName
    WHERE upperCaseName STARTS WITH 'C'
    RETURN otherPerson.name
    ORDER BY id(otherPerson) ASC
$$) AS (name agtype);

SELECT * FROM cypher('cypher_with', $$
    MATCH (david {name: 'David'})-[]-(otherPerson)-[]->()
    WITH otherPerson, count(*) AS foaf
    WHERE foaf > 1
    RETURN otherPerson.name
    ORDER BY id(otherPerson) ASC
$$) AS (name agtype);

-- MATCH/WHERE with WITH/WHERE
SELECT * FROM cypher('cypher_with', $$
    MATCH (m)-[e]->(b)
    WHERE type(e) = 'KNOWS'
    WITH *
    WHERE m.name = 'Andres'
    RETURN m.name, type(e), b.name
    ORDER BY id(m) ASC, id(e) ASC, id(b) ASC
$$) AS (start_node agtype, edge agtype, end_node agtype);

-- WITH/ORDER BY
SELECT * FROM cypher('cypher_with', $$
    MATCH (n)
    WITH n
    ORDER BY id(n) ASC
    RETURN n.name
$$) AS (name agtype);

-- WITH/ORDER BY/DESC
SELECT * FROM cypher('cypher_with', $$
    MATCH (n)
    WITH n
    ORDER BY n.name DESC
    LIMIT 3
    RETURN collect(n.name)
$$) AS (names agtype);

SELECT * FROM cypher('cypher_with', $$
    MATCH (n {name: 'Andres'})-[]-(m)
    WITH m
    ORDER BY m.name DESC
    LIMIT 1
    MATCH (m)-[]-(o)
    RETURN o.name
    ORDER BY o.name ASC
$$) AS (name agtype);

-- multiple WITH clauses
SELECT * FROM cypher('cypher_with', $$
    MATCH (n)-[e]->(m)
    WITH n, e, m
    WHERE type(e) = 'KNOWS'
    WITH id(e) AS eid, n.name AS n1, type(e) AS edge, m.name AS n2
    WHERE n1 = 'Andres'
    RETURN n1, edge, n2
    ORDER BY eid ASC
$$) AS (start_node agtype, edge agtype, end_node agtype);

SELECT * FROM cypher('cypher_with', $$
    UNWIND [1, 2, 3, 4, 5, 6] AS x
    WITH x
    WHERE x > 2
    WITH x
    LIMIT 5
    RETURN x
    ORDER BY x ASC
$$) AS (value agtype);

SELECT * FROM cypher('cypher_with', $$
    MATCH (m)-[]->(b)
    WITH m, b
    ORDER BY id(m) DESC
    LIMIT 5
    WITH m AS start_node, b AS end_node
    WHERE end_node.name = 'George'
    RETURN id(start_node), start_node.name, id(end_node), end_node.name
    ORDER BY id(start_node) ASC, id(end_node) ASC
$$) AS (id1 agtype, name1 agtype, id2 agtype, name2 agtype);

-- Expression item must be aliased.
SELECT * FROM cypher('cypher_with', $$
WITH 1 + 1
RETURN i
$$) AS (i int);

SELECT * FROM cypher('cypher_with', $$
    MATCH (m)-[]->(b)
    WITH id(m)
    RETURN m
$$) AS (id agtype);

SELECT * FROM cypher('cypher_with', $$
    MATCH (m)-[]->(b)
    WITH m AS start_node, b AS end_node
    WHERE start_node.name = 'Andres'
    WITH start_node
    WHERE start_node.name = 'George'
    RETURN id(start_node), end_node.name
$$) AS (id agtype, node agtype);

SELECT drop_graph('cypher_with', true);
