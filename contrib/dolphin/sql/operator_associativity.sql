create table test_op_associativity(a int);
insert into test_op_associativity values (12);
insert into test_op_associativity values (12);

set dolphin.sql_mode = '';
select * from test_op_associativity where (length(a) > 1 || length(a) >1);

set dolphin.sql_mode = 'pipes_as_concat';
select * from test_op_associativity where (length(a) > 1 || length(a) >1);

select 1 < 2 < 3;
select 2 < 1 < 3;
select 2 < (1 < 3);

select 3 > 2 > 1;
select -2 > -3 > 0;
select -2 > (-3 > 0);

select 1 <= 2 <= 2;
select 2 <= 1 <= 2;
select 2 <= (1 <= 2);

select 3 >= 2 >= 1;
select -2 >= -3 >= 0;
select -2 >= (-3 >= 0);

select 1 != 1 != 1;
select 1 != 0 != 1;
select 2 != 1 != 0;
