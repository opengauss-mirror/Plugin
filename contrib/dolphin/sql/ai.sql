create schema ai_test;
set current_schema to 'ai_test';

CREATE TABLE tab_ysl_1 (col1 int, col2 int, col3 text);
INSERT INTO tab_ysl_1 select i,i,concat('t',i) from generate_series(1, 30000) i;
ANALYZE tab_ysl_1;
SELECT indexname FROM hypopg_create_index('CREATE INDEX ON tab_ysl_1(col1)');
set enable_hypo_index = on;
explain (costs off) SELECT * FROM tab_ysl_1 WHERE col1 = 100;

drop schema ai_test cascade;