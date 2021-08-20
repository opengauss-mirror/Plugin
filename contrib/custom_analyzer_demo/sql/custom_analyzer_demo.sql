create extension custom_analyzer_demo;

-- test without custom analyzer
show custom_analyzer_demo.enable_custom_analyzer;
set custom_analyzer_demo.enable_custom_analyzer to off;

drop table if exists group_test;
create table group_test(a int, b int);
insert into group_test values(1,1),(2,2),(3,3),(3,4),(3,5),(2,66);

select * from group_test;
SELECT a, b FROM group_test GROUP BY a order by a,b;

-- test with custom analyzer
set custom_analyzer_demo.enable_custom_analyzer to on;
SELECT a, b FROM group_test GROUP BY a order by a,b;

--cleanup
set custom_analyzer_demo.enable_custom_analyzer to off;
drop table group_test;