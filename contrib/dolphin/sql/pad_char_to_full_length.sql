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

set try_vector_engine_strategy='force';
select char_length(name), length(name) from t;
set try_vector_engine_strategy='off';

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

set try_vector_engine_strategy='force';
select char_length(name), length(name) from t;
set try_vector_engine_strategy='off';

reset dolphin.sql_mode;

-- test cstore
create table t_col (name char(100)) with (orientation = column);
insert into t_col values('test');
insert into t_col values('test    ');
insert into t_col values('    test');

select * from t_col;
select concat(name, 't') from t_col;
select char_length(name), length(name) from t_col;
select name::text, name::varchar, name::nvarchar2 from t_col;
select * from t_col where name = 'test        ';
select * from t_col where name != 'test        ';
select * from t_col where name < 'test        ';
select * from t_col where name <= 'test        ';
select * from t_col where name > 'test        ';
select * from t_col where name >= 'test        ';
select * from t_col where name regexp 'test        %';
select * from t_col where name like 'test        %';
select * from t_col where name not like 'test        %';
select * from t_col where name like 'Test        %';
select * from t_col where name not like 'Test        %';

set dolphin.b_compatibility_mode = true;
select * from t_col where name like 'test        %';
select * from t_col where name not like 'test        %';
select * from t_col where name like 'Test        %';
select * from t_col where name not like 'Test        %';

set dolphin.b_compatibility_mode = false;

set try_vector_engine_strategy='force';
select char_length(name), length(name) from t_col;
set try_vector_engine_strategy='off';

set enable_codegen to true;
set codegen_cost_threshold to 0;
select char_length(name), length(name) from t_col;
reset enable_codegen;
reset codegen_cost_threshold;

set dolphin.sql_mode = '';
select * from t_col;
select concat(name, 't') from t_col;
select char_length(name), length(name) from t_col;
select name::text, name::varchar, name::nvarchar2 from t_col;
select * from t_col where name = 'test        ';
select * from t_col where name != 'test        ';
select * from t_col where name < 'test        ';
select * from t_col where name <= 'test        ';
select * from t_col where name > 'test        ';
select * from t_col where name >= 'test        ';
select * from t_col where name regexp 'test        %';
select * from t_col where name like 'test        %';
select * from t_col where name not like 'test        %';
select * from t_col where name like 'Test        %';
select * from t_col where name not like 'Test        %';

set dolphin.b_compatibility_mode = true;
select * from t_col where name like 'test        %';
select * from t_col where name not like 'test        %';
select * from t_col where name like 'Test        %';
select * from t_col where name not like 'Test        %';

set try_vector_engine_strategy='force';
select char_length(name), length(name) from t_col;
set try_vector_engine_strategy='off';

set enable_codegen to true;
set codegen_cost_threshold to 0;
select char_length(name), length(name) from t_col;

drop schema pad_char_to_full_length cascade;
reset current_schema;