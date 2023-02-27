create schema create_index;
set current_schema to 'create_index';

create table t1(a int);

--error
CREATE INDEX idx_t1 ON t1(a) USING gin;
CREATE INDEX CONCURRENTLY idx_t1 ON t1(a) USING BTREE;
CREATE INDEX public.idx_t1 ON t1(a) USING BTREE;

--success
CREATE INDEX idx_t1 ON t1(a) USING BTREE;
CREATE INDEX idx_t2 ON t1(a) USING hash;
CREATE UNIQUE INDEX idx_t3 ON t1(a) USING BTREE;


drop schema create_index cascade;
reset current_schema;