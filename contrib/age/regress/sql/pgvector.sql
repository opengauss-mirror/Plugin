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

SELECT create_graph('pgvector');

SELECT typname
FROM pg_type
WHERE typname IN ('vector', 'halfvec', 'sparsevec')
ORDER BY typname;

SELECT * FROM cypher('pgvector', $$ RETURN [1.22, 2.22, 3.33]::vector $$) AS (n vector);
SELECT * FROM cypher('pgvector', $$ RETURN [1.22, 2.22, 3.33]::halfvec $$) AS (n halfvec);
SELECT * FROM cypher('pgvector', $$ RETURN [1.22, 2.22, 3.33]::sparsevec $$) AS (n sparsevec);
SELECT * FROM cypher('pgvector', $$ RETURN "[1.22,2.22,3.33]"::vector $$) AS (n vector);
SELECT * FROM cypher('pgvector', $$ RETURN [1.22, 2.22, 3.33]::vector $$) AS (n vector(2));

SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector <-> [1, 1, 1]::vector $$) AS (distance float8);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector <#> [1, 1, 1]::vector $$) AS (distance float8);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector <=> [1, 1, 1]::vector $$) AS (distance float8);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector <+> [1, 1, 1]::vector $$) AS (distance float8);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec <-> [1, 1, 1]::halfvec $$) AS (distance float8);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec <-> [1, 1, 1]::sparsevec $$) AS (distance float8);

SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector + [4, 5, 6]::vector $$) AS (result vector);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector - [4, 5, 6]::vector $$) AS (result vector);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector * [4, 5, 6]::vector $$) AS (result vector);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector || [4, 5]::vector $$) AS (result vector);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector = [1, 2, 3]::vector $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector <> [1, 2, 4]::vector $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector < [1, 2, 4]::vector $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector <= [1, 2, 3]::vector $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 4]::vector > [1, 2, 3]::vector $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 4]::vector >= [1, 2, 4]::vector $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec + [4, 5, 6]::halfvec $$) AS (result halfvec);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec - [4, 5, 6]::halfvec $$) AS (result halfvec);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec * [4, 5, 6]::halfvec $$) AS (result halfvec);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec || [4, 5]::halfvec $$) AS (result halfvec);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec = [1, 2, 3]::halfvec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec <> [1, 2, 4]::halfvec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec < [1, 2, 4]::halfvec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec <= [1, 2, 3]::halfvec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 4]::halfvec > [1, 2, 3]::halfvec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 4]::halfvec >= [1, 2, 4]::halfvec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec <#> [1, 1, 1]::sparsevec $$) AS (distance float8);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec <=> [1, 1, 1]::sparsevec $$) AS (distance float8);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec <+> [1, 1, 1]::sparsevec $$) AS (distance float8);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec = [1, 2, 3]::sparsevec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec <> [1, 2, 4]::sparsevec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec < [1, 2, 4]::sparsevec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec <= [1, 2, 3]::sparsevec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 4]::sparsevec > [1, 2, 3]::sparsevec $$) AS (result boolean);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 4]::sparsevec >= [1, 2, 4]::sparsevec $$) AS (result boolean);

SELECT * FROM cypher('pgvector', $$ RETURN vector_dims([1, 2, 3]::vector) $$) AS (dims int);
SELECT * FROM cypher('pgvector', $$ RETURN vector_norm([1, 2, 3]::vector) $$) AS (norm float8);
SELECT * FROM cypher('pgvector', $$ RETURN l2_normalize([3, 4]::vector) $$) AS (normalized vector);
SELECT * FROM cypher('pgvector', $$ RETURN subvector([1, 2, 3, 4]::vector, 2, 2) $$) AS (sub vector);
SELECT * FROM cypher('pgvector', $$ RETURN binary_quantize([-1, 0, 1, 2]::vector) $$) AS (bits bit(4));
SELECT * FROM cypher('pgvector', $$ RETURN vector_dims([1, 2, 3]::halfvec) $$) AS (dims int);
SELECT * FROM cypher('pgvector', $$ RETURN l2_normalize([3, 4]::halfvec) $$) AS (normalized halfvec);
SELECT * FROM cypher('pgvector', $$ RETURN subvector([1, 2, 3, 4]::halfvec, 2, 2) $$) AS (sub halfvec);
SELECT * FROM cypher('pgvector', $$ RETURN binary_quantize([-1, 0, 1, 2]::halfvec) $$) AS (bits bit(4));
SELECT * FROM cypher('pgvector', $$ RETURN l2_normalize([3, 4]::sparsevec) $$) AS (normalized sparsevec);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector::halfvec $$) AS (n halfvec);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec::vector $$) AS (n vector);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::vector::sparsevec $$) AS (n sparsevec);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::halfvec::sparsevec $$) AS (n sparsevec);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec::vector $$) AS (n vector);
SELECT * FROM cypher('pgvector', $$ RETURN [1, 2, 3]::sparsevec::halfvec $$) AS (n halfvec);
SELECT * FROM cypher('pgvector', $$ RETURN avg([1, 2, 3]::vector), sum([1, 2, 3]::vector) $$) AS (avg vector, sum vector);
SELECT * FROM cypher('pgvector', $$ RETURN avg([1, 2, 3]::halfvec), sum([1, 2, 3]::halfvec) $$) AS (avg halfvec, sum halfvec);
PREPARE pgvector_param_vector(agtype) AS
SELECT * FROM cypher('pgvector', $$ RETURN $embedding::vector, $embedding::halfvec, $embedding::sparsevec $$, $1)
AS (embedding vector, embedding_half halfvec, embedding_sparse sparsevec);
EXECUTE pgvector_param_vector('{"embedding": [1.1, 2.2, 3.3]}');
DEALLOCATE pgvector_param_vector;

SELECT * FROM cypher('pgvector', $$
    CREATE (:Movie {title: "The Matrix", embedding: [-0.07594558, 0.04081754, 0.29592122, -0.11921061]}),
           (:Movie {title: "The Terminator", embedding: [0.33666933, 0.18040994, -0.01075103, -0.11117851]})
$$) AS (result agtype);

SELECT * FROM cypher('pgvector', $$
    MATCH (m:Movie)
    RETURN m.title, m.embedding::vector, m.embedding::halfvec, m.embedding::sparsevec
    ORDER BY m.title
$$) AS (title agtype, embedding vector, embedding_half halfvec, embedding_sparse sparsevec);

SELECT * FROM cypher('pgvector', $$
    MATCH (m:Movie)
    RETURN m.title, m.embedding::vector <-> [-0.1, 0.05, 0.3, -0.12]::vector AS distance
    ORDER BY distance
$$) AS (title agtype, distance float8);

SELECT * FROM cypher('pgvector', $$
    MATCH (m:Movie)
    RETURN avg(m.embedding::vector), sum(m.embedding::vector)
$$) AS (avg vector, sum vector);

SELECT * FROM cypher('pgvector', $$
    MATCH (m:Movie)
    RETURN avg(m.embedding::halfvec), sum(m.embedding::halfvec)
$$) AS (avg halfvec, sum halfvec);

SELECT * FROM cypher('pgvector', $$
    MATCH (m:Movie)
    WHERE false
    RETURN avg(m.embedding::vector), sum(m.embedding::vector)
$$) AS (avg vector, sum vector);

SELECT * FROM cypher('pgvector', $$
    MATCH (m:Movie)
    WHERE false
    RETURN avg(m.embedding::halfvec), sum(m.embedding::halfvec)
$$) AS (avg halfvec, sum halfvec);

CREATE INDEX movie_embedding_hnsw_l2_idx ON pgvector."Movie"
USING hnsw (((ag_catalog.agtype_to_vector(ag_catalog.agtype_access_operator(properties, '"embedding"'::agtype)))::vector(4)) vector_l2_ops)
WITH (m = 4, ef_construction = 10);

CREATE INDEX movie_embedding_ivfflat_l2_idx ON pgvector."Movie"
USING ivfflat (((ag_catalog.agtype_to_vector(ag_catalog.agtype_access_operator(properties, '"embedding"'::agtype)))::vector(4)) vector_l2_ops)
WITH (lists = 1);

CREATE INDEX movie_embedding_diskann_l2_idx ON pgvector."Movie"
USING diskann (((ag_catalog.agtype_to_vector(ag_catalog.agtype_access_operator(properties, '"embedding"'::agtype)))::vector(4)) vector_l2_ops);

SELECT ag_catalog.agtype_access_operator(properties, '"title"'::agtype) AS title
FROM pgvector."Movie"
ORDER BY ((ag_catalog.agtype_to_vector(ag_catalog.agtype_access_operator(properties, '"embedding"'::agtype)))::vector(4)
          <-> '[-0.1,0.05,0.3,-0.12]'::vector)
LIMIT 2;

DROP INDEX pgvector.movie_embedding_diskann_l2_idx;
DROP INDEX pgvector.movie_embedding_ivfflat_l2_idx;
DROP INDEX pgvector.movie_embedding_hnsw_l2_idx;

SELECT drop_graph('pgvector', true);
