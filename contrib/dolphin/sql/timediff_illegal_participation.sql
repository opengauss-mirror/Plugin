create schema timediff_illegal_participation;
set current_schema = timediff_illegal_participation;
set dolphin.sql_mode = default;
create table test(c time);
insert into test values(timediff(B'101', B'1010'));
insert into test values(timediff(B'1010', B'101'));
insert into test values(timediff(B'101', B'100000'));
insert into test values(timediff(B'100000', B'101'));
insert into test values(timediff(B'101', B'1001'));
insert into test values(timediff(B'1001', B'101'));
insert into test values(timediff(B'101', B'1101'));
insert into test values(timediff(B'1101', B'101'));
insert into test values(timediff(B'101', B'1011'));
insert into test values(timediff(B'1011', B'101'));
insert into test values(timediff(B'101', B'1100'));
insert into test values(timediff(B'1100', B'101'));
insert ignore into test values(timediff(B'101', B'1010'));
insert ignore into test values(timediff(B'1010', B'101'));
insert ignore into test values(timediff(B'101', B'100000'));
insert ignore into test values(timediff(B'100000', B'101'));
insert ignore into test values(timediff(B'101', B'1001'));
insert ignore into test values(timediff(B'1001', B'101'));
insert ignore into test values(timediff(B'101', B'1101'));
insert ignore into test values(timediff(B'1101', B'101'));
insert ignore into test values(timediff(B'101', B'1011'));
insert ignore into test values(timediff(B'1011', B'101'));
insert ignore into test values(timediff(B'101', B'1100'));
insert ignore into test values(timediff(B'1100', B'101'));
select c from test order by c;
set dolphin.sql_mode = '';
select timediff('asdf', '00:00:00');
-- test \n
select timediff(B'101', B'1010');
select timediff(B'1010', B'101');
-- test ' '
select timediff(B'101', B'100000');
select timediff(B'100000', B'101');
-- test \t
select timediff(B'101', B'1001');
select timediff(B'1001', B'101');
-- test \r
select timediff(B'101', B'1101');
select timediff(B'1101', B'101');
-- test \v
select timediff(B'101', B'1011');
select timediff(B'1011', B'101');
--test \f
select timediff(B'101', B'1100');
select timediff(B'1100', B'101'); 
-- test normal
select timediff(B'101', B'10101');
select timediff(time'00:00:00', 'asdfg');
insert into test values(timediff(B'101', B'1010'));
insert into test values(timediff(B'1010', B'101'));
insert into test values(timediff(B'101', B'100000'));
insert into test values(timediff(B'100000', B'101'));
insert into test values(timediff(B'101', B'1001'));
insert into test values(timediff(B'1001', B'101'));
insert into test values(timediff(B'101', B'1101'));
insert into test values(timediff(B'1101', B'101'));
insert into test values(timediff(B'101', B'1011'));
insert into test values(timediff(B'1011', B'101'));
insert into test values(timediff(B'101', B'1100'));
insert into test values(timediff(B'1100', B'101'));
select c from test order by c;
drop table test;
reset current_schema;
drop schema timediff_illegal_participation cascade;
