create schema string_index_test;
set current_schema to 'string_index_test';
set dolphin.b_compatibility_mode = on;

-- test position for boolean
select position('1' in TRUE);
select position(0 in TRUE);
select position(1 in FALSE);
select position('0' in FALSE);

-- test position for bit type
select position(b'110' in b'0110000101111010'); -- openGauss => 2, b mode => 0

create table t_bit (c1 integer, `bit64` bit(64));
insert into t_bit values (1, 0x536f6d65006f6e65); -- Some\0one
insert into t_bit values (2, 0x00536f6d656f6e65); -- \0Someone
insert into t_bit values (3, 0x536f6d656f6e6500); -- Someone\0

select position(b'01' in `bit64`) from t_bit order by c1;
select position(b'01100101' in `bit64`) from t_bit order by c1; -- e
select position('e' in `bit64`) from t_bit order by c1;
select position('yes' in `bit64`) from t_bit order by c1;
select position('one' in `bit64`) from t_bit order by c1;
drop table  t_bit;

drop schema string_index_test cascade;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;