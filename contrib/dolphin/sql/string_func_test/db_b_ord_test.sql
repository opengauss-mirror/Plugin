create schema db_b_ord_test;
set current_schema to 'db_b_ord_test';

-- test 1 byte
select ord('1111');
select ord('sss111');

-- test 2 byte
select ord('Ŷ1111');
select ord('߷1111');

-- test 3 byte
select ord('অ1111');
select ord('ꬤ1111');

-- test 4 byte
select ord('𒁖1111');
select ord('𓃔1111');

-- test number
select ord(11111);
select ord(99999);

-- test empty
select ord('');

-- test from table

create table test_ord (name text);
insert into test_ord values('1234'), ('嬴政'), ('𓃔𓃘𓃲𓃰');

select ord(name) from test_ord;

drop schema db_b_ord_test cascade;
reset current_schema;