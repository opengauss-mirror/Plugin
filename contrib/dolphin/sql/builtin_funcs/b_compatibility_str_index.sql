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
select position('' in `bit64`) from t_bit order by c1;

-- test locate for boolean
create table t_bool (c1 integer, c2 boolean);
insert into t_bool values (1, 0);
insert into t_bool values (2, 1);
select locate('1', `c2`), locate(0, `c2`) from t_bool order by c1;
select locate('0', `c2`, 1), locate(1, `c2`, 2) from t_bool order by c1;
drop table t_bool;

-- test locate for bit
select locate(b'01', `bit64`), locate(b'01', `bit64`, 2) from t_bit order by c1;
select locate(b'01100101', `bit64`), locate(b'01100101', `bit64`, 6) from t_bit order by c1; -- e
select locate('e', `bit64`), locate('e', `bit64`, 6) from t_bit order by c1;
select locate('yes', `bit64`), locate('yes', `bit64`, 5) from t_bit order by c1;
select locate('one', `bit64`), locate('one', `bit64`, 4) from t_bit order by c1;
select locate('', `bit64`), locate('', `bit64`, 4) from t_bit order by c1;

drop table t_bit;

-- test multi_byte charatar binary string
create table t_bin(c1 binary(50), c2 varbinary(50), c3 varchar(50));
insert into t_bin values ('脸映桃红桃映脸', '脸映桃红桃映脸', '脸映桃红桃映脸');
select locate('映', c1), locate('映', c2, 5), locate('映', c3, 5) from t_bin;
select position('映' in c1), position('映' in c2), position('映' in c3) from t_bin;
select instr(c1, '映'), instr(c2, '映'), instr(c3, '映') from t_bin;

drop table t_bin;

drop schema string_index_test cascade;
reset dolphin.sql_mode;
reset dolphin.b_compatibility_mode;
reset current_schema;