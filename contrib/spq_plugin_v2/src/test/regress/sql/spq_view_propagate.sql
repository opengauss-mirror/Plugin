----------------------- test 1: create/replace view propagate -----------------------------
CREATE VIEW dv AS
SELECT 
  1 AS id,
  'Hello' AS message;

select * from dv;
\c - - - :worker_1_port
select * from dv;
\c - - - :worker_2_port
select * from dv;

\c - - - :master_port
CREATE OR REPLACE VIEW dv AS
SELECT 
  1 AS id,
  'Hello' AS message,
  'World' AS additional_info;

select * from dv;
\c - - - :worker_1_port
select * from dv;
\c - - - :worker_2_port
select * from dv;

----------------------- test 2: alter view propagate -----------------------------
\c - - - :master_port
ALTER VIEW dv SET (security_barrier=false);
\d+ dv;
\c - - - :worker_1_port
\d+ dv;
\c - - - :worker_2_port
\d+ dv;

----------------------- test 3: rename view propagate -----------------------------
\c - - - :master_port
ALTER VIEW dv RENAME TO dv1;
\d+ dv1;
\c - - - :worker_1_port
\d+ dv1;
\c - - - :worker_2_port
\d+ dv1;

----------------------- test 4: alter schema view propagate -----------------------------
\c - - - :master_port
create schema s1;
ALTER VIEW dv1 SET schema s1;
\d+ dv1;
\d+ s1.dv1;
\c - - - :worker_1_port
\d+ dv1;
\d+ s1.dv1;
\c - - - :worker_2_port
\d+ dv1;
\d+ s1.dv1;

----------------------- test 5: drop view propagate -----------------------------
\c - - - :master_port
drop view s1.dv1;
\d+ s1.dv1;
\c - - - :worker_1_port
\d+ s1.dv1;
\c - - - :worker_2_port
\d+ s1.dv1;

\c - - - :master_port
drop schema s1;