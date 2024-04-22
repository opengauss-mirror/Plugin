drop table if exists t_outer1;
create table t_outer1(c1 tinyint,c2 text);
drop table if exists t_outer2;
create table t_outer2(a1 tinyint,a2 text);

insert t_outer1 values(-128,'测试'),(127,'增加');
insert t_outer2 values(-128,'姓名'),(127,'年龄');

SELECT c1,c2 FROM t_outer1 full outer JOIN t_outer2 ON t_outer1.c1 = t_outer2.a1;
SELECT c1,c2 FROM t_outer1 full JOIN t_outer2 ON t_outer1.c1 = t_outer2.a1;