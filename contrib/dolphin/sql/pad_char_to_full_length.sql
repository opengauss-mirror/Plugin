create schema pad_char_to_full_length;
set current_schema to 'pad_char_to_full_length';

create table t (name char(100));
insert into t values('test');
insert into t values('test    ');
insert into t values('    test');

select * from t;
select concat(name, 't') from t;
select char_length(name), length(name) from t;
select name::text, name::varchar, name::nvarchar2 from t;
select * from t where name = 'test        ';
select * from t where name != 'test        ';
select * from t where name < 'test        ';
select * from t where name <= 'test        ';
select * from t where name > 'test        ';
select * from t where name >= 'test        ';
select * from t where name regexp 'test        %';
select * from t where name like 'test        %';
select * from t where name not like 'test        %';
select * from t where name like 'Test        %';
select * from t where name not like 'Test        %';

select cast('a' as char(10)), 'a'::char(10);

set dolphin.b_compatibility_mode = true;
select * from t where name like 'test        %';
select * from t where name not like 'test        %';
select * from t where name like 'Test        %';
select * from t where name not like 'Test        %';

select cast('a' as char(10)), 'a'::char(10);

set dolphin.b_compatibility_mode = false;

set dolphin.sql_mode = '';
select * from t;
select concat(name, 't') from t;
select char_length(name), length(name) from t;
select name::text, name::varchar, name::nvarchar2 from t;
select * from t where name = 'test        ';
select * from t where name != 'test        ';
select * from t where name < 'test        ';
select * from t where name <= 'test        ';
select * from t where name > 'test        ';
select * from t where name >= 'test        ';
select * from t where name regexp 'test        %';
select * from t where name like 'test        %';
select * from t where name not like 'test        %';
select * from t where name like 'Test        %';
select * from t where name not like 'Test        %';

select cast('a   ' as char(10)), 'a     '::char(10);

set dolphin.b_compatibility_mode = true;
select * from t where name like 'test        %';
select * from t where name not like 'test        %';
select * from t where name like 'Test        %';
select * from t where name not like 'Test        %';

select cast('a    ' as char(10)), 'a    '::char(10);

drop schema pad_char_to_full_length cascade;
reset current_schema;