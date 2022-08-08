-- add for LLT
create table range_table_LLT (a int, b int, c int, d int) 
partition by range (a, b, c, d) 
(
partition p1 values less than (2, 10, 2, 2),
partition p2 values less than (3, 5, 3, 3),
partition p3 values less than (4, 4, 4, 4),
partition p4 values less than (5, 5, 5, 5)
);

select * from range_table_LLT where a=2 and b>10 and b<20 and c=2 and d=2;

drop table range_table_LLT;



create table rt_TESTTABLE (a int, b int)
partition by range(a, b)
(
partition rt_TESTTABLE_p1 values less than(9, 3),
partition rt_TESTTABLE_p2 values less than(10, 1)
);

insert into rt_TESTTABLE values (9, 4);

select * from rt_TESTTABLE where b=4;

drop table rt_TESTTABLE;


create table rt_TESTTABLE (a int, b int)
partition by range(a, b)
(
partition rt_TESTTABLE_p1 values less than(9, 1),
partition rt_TESTTABLE_p2 values less than(10, 3)
);

insert into rt_TESTTABLE values (9, 4);

select * from rt_TESTTABLE where b=4;

drop table rt_TESTTABLE;


create table rt_TESTTABLE (a int, b int, c int)
partition by range(a, b, c)
(
partition rt_TESTTABLE_p1 values less than(2, 9, 3),
partition rt_TESTTABLE_p2 values less than(2, 10, 1)
);

insert into rt_TESTTABLE values (2, 9, 4);

select * from rt_TESTTABLE where c=4;

drop table rt_TESTTABLE;


create table rt_TESTTABLE (a int, b int, c int)
partition by range(a, b, c)
(
partition rt_TESTTABLE_p1 values less than(2, 9, 1),
partition rt_TESTTABLE_p2 values less than(2, 10, 3)
);

insert into rt_TESTTABLE values (2, 9, 4);

select * from rt_TESTTABLE where c=4;

drop table rt_TESTTABLE;