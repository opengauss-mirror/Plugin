--
-- CREATE_VIEW3
--
-- Enforce use of COMMIT instead of 2PC for temporary objects

\c table_name_test_db;
set dolphin.lower_case_table_names TO 0;
/* unkown type */

create table test_a (a integer not null); 
insert into test_a values(1); 
create view test_b as select '*' as "XX",a from test_a; 
select * from test_b; 
select * from test_b where "XX" = '*';
drop table test_a cascade;