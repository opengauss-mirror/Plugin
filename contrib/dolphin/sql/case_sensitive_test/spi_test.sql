\c table_name_test_db;
SET lower_case_table_names TO 0;

drop table if exists T_LETTER_0028;
create table T_LETTER_0028
(col1 int
,col2 int);

create or replace function f_letter_0028()
return void
as
insert_str varchar;
begin
insert_str = 'insert into T_LETTER_0028 values'
'(1,1),(2,2),(3,3);';
execute immediate insert_str;
end;
/

call f_letter_0028();

drop table if exists T_LETTER_0029;
create table T_LETTER_0029
(col1 int
,col2 int);

create or replace procedure p_letter_0029()
as
insert_str varchar;
begin
insert_str = 'insert into T_LETTER_0029 values'
'(1,1),(2,2),(3,3);';
execute immediate insert_str;
end;
/

call p_letter_0029();