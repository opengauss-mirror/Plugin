-- test  distinct+orderby allows expressions and implicit columns
-- 1. test default
reset dolphin.sql_mode;
create table dob_student(
    name varchar(100),
    pinyin varchar(100),
    id int,
    age int
);
insert into dob_student values('小明', 'xiaoming', 1, 10);
insert into dob_student values('小明', 'xiaoming', 11, 33);
select distinct name from dob_student order by name, id;
select distinct name from dob_student order by nlssort(name, 'NLS_SORT=SCHINESE_PINYIN_M') asc;
truncate dob_student;

-- 2. test behavior_compat_options is useless on dolphin plugin
set behavior_compat_options='allow_orderby_undistinct_column';
insert into dob_student values('小明', 'xiaoming', 1, 10);
insert into dob_student values('小明', 'xiaoming', 11, 33);
insert into dob_student values('张三', 'zhangsan', 3, 10);
insert into dob_student values('张三', 'zhangsan', 3, 10);
insert into dob_student values('小红', 'xiaohong', 2, 12);
insert into dob_student values('小红', 'xiaohong', 2, 12);
insert into dob_student values('张三', 'xiaohong', 2, 12);
select distinct name from dob_student order by name;

select distinct name from dob_student order by nlssort(name, 'NLS_SORT=SCHINESE_PINYIN_M') asc;
select distinct name from dob_student order by convert_to(name, 'GBK') desc;
select name from (select distinct name from dob_student) order by nlssort(name, 'NLS_SORT=SCHINESE_PINYIN_M') asc;

-- 3. implicit column should error on default settings
select distinct name from dob_student order by convert_to(name, 'GBK'), id asc;
select distinct name from dob_student order by id desc, age;
select distinct t.name, t.pinyin as py, 1 + 10 as c from dob_student t order by py, t.id, t.age * 2 + 1;
select distinct t.name, t.pinyin as py, 1 + 10 as c from dob_student t order by c, py, t.age * 2 + 1;

reset behavior_compat_options;
set dolphin.sql_mode='sql_mode_strict,pipes_as_concat,ansi_quotes,no_zero_date';

-- 4. test implicit columns
select distinct name from dob_student order by convert_to(name, 'GBK'), id asc;
select distinct name from dob_student order by id desc, age;

-- 5. test nesting
select name from (select distinct name from dob_student order by id desc, age) order by nlssort(name, 'NLS_SORT=SCHINESE_PINYIN_M') asc;

-- 6. test with functions, when case
create table dob_func_t(a int, b int, c int, d int);
insert into dob_func_t values(generate_series(1, 10), generate_series(1, 10), generate_series(1, 10),  generate_series(1, 10));
insert into dob_func_t values(generate_series(1, 10), generate_series(1, 10), generate_series(1, 10),  generate_series(1, 10));

select distinct a, b, c from dob_func_t where a > 1 order by random() * 0.1 + a, d;
select distinct a, b, c from dob_func_t order by case when a = 10 then 1 else 100 end,  d asc, c desc;

-- 7. test with group by
select distinct a, b, c from dob_func_t group by a, b, c order by d, c; -- passed on plugin
select distinct a, b, c from dob_func_t group by a, b, c order by c;

-- 8. test alias
create table dob_alias(a int, b int, c varchar(30), d varchar(30));
insert into dob_alias values(generate_series(1, 10), generate_series(1, 10), 'hello', 'world');
insert into dob_alias values(generate_series(1, 10), generate_series(1, 10), 'hello', 'world');
select distinct a, b + 10 as balias, c from dob_alias order by a, balias, concat(c, ' concat');
select distinct a, b, c, a + 1.1 * 10 as k from dob_alias order by k;
select distinct a, b, 1 + 10 as c from dob_alias order by c, b * 2 + 1;

select distinct t.a, t.b, 1 + 10 as c from dob_alias t order by b * 2 + 1, t.b, t.d;

-- 9. test with join clause
select distinct s.name, s.id, a.a, a.b, a.c from dob_student s 
	left join dob_alias a 
	on s.id=a.a 
	order by nlssort(name, 'NLS_SORT=SCHINESE_PINYIN_M'), s.age desc, a.d asc;
select distinct * from 
	(select distinct s.name, s.id, a.a, a.b, a.c from dob_student s 
		inner join dob_alias a 
		on s.id=a.a 
		order by s.age desc, a.d asc) t
	order by nlssort(t.name, 'NLS_SORT=SCHINESE_PINYIN_M');

-- 10. test group_concat
insert into dob_func_t values(generate_series(1, 10), generate_series(1, 10), generate_series(21, 30),  generate_series(2, 30));
set group_concat_max_len to 1024;
select a, min(b), group_concat(distinct c order by d) as order_not_in_distinct from dob_func_t group by a order by a;

reset dolphin.sql_mode;

-- 11. test group_concat should error
select a, min(b), group_concat(distinct c order by d) as order_not_in_distinct from dob_func_t group by a order by a;
reset group_concat_max_len;

drop table dob_student, dob_func_t, dob_alias;