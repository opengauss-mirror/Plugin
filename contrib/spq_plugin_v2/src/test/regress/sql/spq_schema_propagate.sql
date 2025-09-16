----------------------- test 1: create schema propagate -----------------------------
CREATE SCHEMA schema11;
SELECT COUNT(*) FROM pg_namespace JOIN pg_dist_object ON pg_dist_object.objid=pg_namespace.oid WHERE nspname = 'schema11';

\c - - - :worker_1_port
SELECT COUNT(*) FROM pg_namespace JOIN pg_dist_object ON pg_dist_object.objid=pg_namespace.oid WHERE nspname = 'schema11';
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';

\c - - - :worker_2_port
SELECT COUNT(*) FROM pg_namespace JOIN pg_dist_object ON pg_dist_object.objid=pg_namespace.oid WHERE nspname = 'schema11';
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';

\c - - - :master_port
DROP SCHEMA schema11;
SELECT COUNT(*) FROM pg_namespace JOIN pg_dist_object ON pg_dist_object.objid=pg_namespace.oid WHERE nspname = 'schema11';

\c - - - :worker_1_port
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';

\c - - - :worker_2_port
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';

----------------------- test 2: failed to create schema propagate -----------------------------
\c - - - :worker_1_port
CREATE SCHEMA schema11;
SELECT COUNT(*) FROM pg_namespace JOIN pg_dist_object ON pg_dist_object.objid=pg_namespace.oid WHERE nspname = 'schema11';
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';

\c - - - :master_port
CREATE SCHEMA schema11;
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';

\c - - - :worker_1_port
DROP SCHEMA schema11;

----------------------- test 3: failed to drop schema propagate -----------------------------
\c - - - :master_port
CREATE SCHEMA schema11;
SELECT COUNT(*) FROM pg_namespace JOIN pg_dist_object ON pg_dist_object.objid=pg_namespace.oid WHERE nspname = 'schema11';
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';

\c - - - :worker_1_port
DROP SCHEMA schema11;
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';

\c - - - :master_port
DROP SCHEMA schema11;
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';
SELECT COUNT(*) FROM pg_namespace JOIN pg_dist_object ON pg_dist_object.objid=pg_namespace.oid WHERE nspname = 'schema11';

\c - - - :worker_2_port
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';
DROP SCHEMA schema11;
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';

\c - - - :master_port
SET spq.enable_ddl_propagation = OFF;
DROP SCHEMA schema11;
SET spq.enable_ddl_propagation = DEFAULT;

---------------------- test 4: disable ddl_propagate -----------------------------------------
\c - - - :master_port
SHOW spq.enable_ddl_propagation;
SET spq.enable_ddl_propagation = OFF;
CREATE SCHEMA schema11;
SELECT COUNT(*) FROM pg_namespace JOIN pg_dist_object ON pg_dist_object.objid=pg_namespace.oid WHERE nspname = 'schema11';
SELECT COUNT(*) FROM pg_namespace WHERE nspname = 'schema11';
SET spq.enable_ddl_propagation = DEFAULT;
SHOW spq.enable_ddl_propagation;
