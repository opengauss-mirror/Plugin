/*
 * Regression tests for migrated scalar functions: pi(), rand(), isEmpty(),
 * toFloatList()
 */
LOAD 'age';
SET search_path TO ag_catalog;

SELECT create_graph('scalar');

SELECT * FROM cypher('scalar', $$ RETURN pi() > 3.14159 AND pi() < 3.1416 $$) AS (result agtype);
SELECT * FROM cypher('scalar', $$ RETURN rand() >= 0.0 AND rand() < 1.0 $$) AS (result agtype);
SELECT * FROM cypher('scalar', $$ RETURN isEmpty([]) $$) AS (result agtype);
SELECT * FROM cypher('scalar', $$ RETURN isEmpty([1, 2, 3]) $$) AS (result agtype);
SELECT * FROM cypher('scalar', $$ RETURN isEmpty('') $$) AS (result agtype);
SELECT * FROM cypher('scalar', $$ RETURN isEmpty('abc') $$) AS (result agtype);
SELECT * FROM cypher('scalar', $$ RETURN isEmpty({}) $$) AS (result agtype);

-- #2451: existing agtype floats must not pass through a fixed-size string buffer.
SELECT * FROM cypher('scalar', $$
    RETURN toFloatList([1.0e308, -1.0e308]) = [1.0e308, -1.0e308]
$$) AS (result agtype);

-- #2451: direct float8 propagation must preserve more than six fractional digits.
SELECT * FROM cypher('scalar', $$
    RETURN toFloatList([0.123456789]) = [0.123456789]
$$) AS (result agtype);

-- #2313: an agtype null element is represented by a null iterator value.
-- toStringList() must preserve it instead of dereferencing the missing value.
SELECT * FROM cypher('scalar', $$
    RETURN toStringList(['age', null, 42]) = ['age', null, '42']
$$) AS (result agtype);

-- Strict ledger sequence 116: preserve numeric inputs and parse bounded strings.
SELECT * FROM cypher('scalar', $$
    RETURN toFloatList([1, 1.0, 2::numeric, '0', '2.5', 'bad', null])
$$) AS (result agtype);

-- Strict ledger sequence 117: numeric elements follow toInteger() semantics.
SELECT * FROM cypher('scalar', $$
    RETURN toIntegerList([2.9::numeric, -2.9::numeric, null])
$$) AS (result agtype);

SELECT drop_graph('scalar', true);
