create or replace procedure text2bool()
as
begin
if('TRUE') then
    raise info 'true';
else
    raise info 'false';
end if;
end;
/
call text2bool();

create or replace procedure text2bool()
as
declare
    a text;
begin
a := 'TRUE';
if(a) then
    raise info 'true';
else
    raise info 'false';
end if;
end;
/
call text2bool();