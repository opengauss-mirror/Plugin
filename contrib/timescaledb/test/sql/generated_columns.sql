-- This file and its contents are licensed under the Apache License 2.0.
-- Please see the included NOTICE for copyright information and
-- LICENSE-APACHE for a copy of the license.

--
-- Generated columns support tests
--
CREATE TABLE foo (
    a INT NOT NULL,
    b INT,
    c INT GENERATED ALWAYS AS (a + b) STORED
);
SELECT table_name FROM create_hypertable('foo', 'a', chunk_time_interval=>10);

INSERT INTO foo(a, b) VALUES(1, 2);
INSERT INTO foo(a, b) VALUES(2, 3);

-- Ensure generated column cannot be updated
\set ON_ERROR_STOP 0
INSERT INTO foo VALUES(3, 5, 8);
\set ON_ERROR_STOP 1

SELECT * FROM foo ORDER BY a;

DROP TABLE foo;

-- Ensure that generated column cannot be used for partitioning

-- Generated as expression
CREATE TABLE bar (
    a INT NOT NULL,
    b INT GENERATED ALWAYS AS (a + 123) STORED
);
\set ON_ERROR_STOP 0
SELECT table_name FROM create_hypertable('bar', 'a', 'b', 2, chunk_time_interval=>10);
\set ON_ERROR_STOP 1
DROP TABLE bar;
