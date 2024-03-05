create schema recursive_sql;
set current_schema to 'recursive_sql';
set dolphin.b_compatibility_mode = on;

WITH RECURSIVE cte AS
(SELECT 1 AS n UNION SELECT n+1 FROM cte WHERE n<3)
SELECT * FROM cte;

create view view1 as WITH RECURSIVE cte AS
(SELECT 1 AS n UNION SELECT n+1 FROM cte WHERE n<3)
SELECT * FROM cte;
desc view1;

-- error
create view view2 as WITH RECURSIVE cte AS
(SELECT 1 AS n UNION SELECT 'abc' FROM cte)
SELECT * FROM cte;

create view view2 as WITH RECURSIVE cte AS
(SELECT 'abc' AS n UNION SELECT 1 FROM cte)
SELECT * FROM cte;
desc view2;

create view view3 as WITH cte AS
(SELECT 1 AS n UNION SELECT 'abc')
SELECT * FROM cte;
desc view3;

create view view4 as select 1 union select 1;
desc view4;

reset current_schema;
drop schema recursive_sql cascade;