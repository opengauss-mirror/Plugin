drop database if exists format_test;
create database format_test dbcompatibility 'B';
\c format_test

select chara(67,66,67);
select chara('65','66','67');
select chara('A','B','C');
select chara(65.4,66.3,67.3);
select chara(null);
select chara(65);
select chara(67,66,67);
select chara(65,66.4);
select chara(65,'66');
select chara(65,'66.4');
select chara(65,nul,66);
select chara(-18446744073709551616);
select chara(18446744073709551615);
select chara(0);
select chara(18446744073709551615);
select chara(65);

\c postgres
drop database if exists format_test;
