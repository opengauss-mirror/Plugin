create schema add_unique_index;
set current_schema = add_unique_index;
create table t_tinyint0018 (
    c1 tinyint,
    c2 tinyint(1) default null,
    c3 tinyint(10) not null default '0',
    c4 tinyint default '0',
        c5 text
);
alter table t_tinyint0018 add unique index i_tinyint0018(c1, c2, c5(10));
drop table t_tinyint0018 cascade;
reset current_schema;
drop schema add_unique_index;
