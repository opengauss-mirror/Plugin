create schema ignore_keyword_list;
set current_schema to 'ignore_keyword_list';

select 1 interval; --error
set disable_keyword_options = 'interval';
select 1 interval; --ok
set disable_keyword_options = 'intervalxx';
set disable_keyword_options = 'interval,interval';
select 1 interval; --ok
reset disable_keyword_options;
select 1 interval; --error
create table t1 (binary int);
create table t2 (prior int);
set disable_keyword_options ='prior,binary';
create table t1 (binary int);
create table t2 (prior int);

declare
keyword_name varchar;
catcode_name char;
create_sql varchar;
drop_sql varchar;
begin
    for keyword_name,catcode_name in (select word,catcode from pg_get_keywords()) loop
        if catcode_name = 'U' or catcode_name = 'C' then
            create_sql = 'create table ignore_keyword_list.' || keyword_name || '(' || keyword_name|| ' int)';
            drop_sql = 'drop table ignore_keyword_list.' || keyword_name;
        elsif catcode_name = 'T' then
            create_sql = 'create function ignore_keyword_list.' || keyword_name || '() returns int language sql as ''select 1''';
            drop_sql = 'drop function ignore_keyword_list.' || keyword_name;
        else
            continue;
        end if;
        begin
            EXECUTE IMMEDIATE create_sql;
            EXECUTE IMMEDIATE drop_sql;
            EXCEPTION when OTHERS then
            raise notice 'test failed. test sql: %; %;', create_sql, drop_sql;
        end;
    end loop;
end;
/

reset current_schema;
drop schema ignore_keyword_list cascade;

