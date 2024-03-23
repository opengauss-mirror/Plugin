create schema funcname_with_odd_single_quote;
set current_schema = funcname_with_odd_single_quote;
create or replace procedure "p_'''_1"() is
begin
null;
end;
/
drop procedure "p_'''_1";
create or replace function "p_'''_1"() return void
as
begin
null;
end;
drop function "p_'''_1";
reset current_schema;
drop schema funcname_with_odd_single_quote
