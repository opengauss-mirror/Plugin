create schema db_field;
set current_schema to 'db_field';

select field(4,1,2,3,4);
select field(2,1.1,2.1,3.1);
select field(2,'1','2','3','4');
select field(3.1,1,2,3,4);
select field('2a','1','2','3','4');
select field('2a',1,'2','3','4');
select field(1,'1a', 2);
select field('ceshi.000','wo','ceshi','disange');
select field(true,'1a', 2);
select field(true,1, 2);
select field(true);
select field('sdhfgs','dhgf', 2);
select field('sdhfgs','dhgf', '2');
select field('sdhfgs',null,1,'dhgf', '2');
select field('测试',null,1,'dhgf', '2');
select field(' ','@',null,' ','',' ');

create table tb_1116592(c1 int, c2 varchar, c3 text);
insert into tb_1116592 values(1, 'test1', 'city1');
insert into tb_1116592 values(2, 'test2', 'city2');
insert into tb_1116592 values(3, 'test3', 'city3');
insert into tb_1116592 values(4, 'test4', 'city4');
insert into tb_1116592 values(5, 'test5', 'city5');
insert into tb_1116592 values(6, 'test1', 'city3');
insert into tb_1116592 values(7, 'test3', 'city2');
insert into tb_1116592 values(8, 'test5', 'city1');
insert into tb_1116592 values(9, 'test2', 'city2');
insert into tb_1116592 values(10, 'test4', 'city4');
insert into tb_1116592 values(11, NULL, 'city5');

select *,field(c1, 1) from tb_1116592 order by field(c1, 1),c1;
select *,field(c2, 'test2') from tb_1116592 order by field(c2, 'test2'),c1;
select *,field(c3, 'city1') from tb_1116592 order by field(c3, 'city1'),c1;

drop table tb_1116592;

drop schema db_field cascade;
reset current_schema;
