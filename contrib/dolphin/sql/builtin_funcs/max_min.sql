-- test json
drop table if exists json_maxmin_t cascade;
create table json_maxmin_t (a json);
insert into json_maxmin_t values('{"a":1}');
insert into json_maxmin_t values('{"a":2}');
select max(a), min(a) from json_maxmin_t;
create view json_maxmin_v as select
    max(a) as max_a,
    min(a) as min_a
    from json_maxmin_t;
desc json_maxmin_v;
insert into json_maxmin_t values(null);
insert into json_maxmin_t values('{"a":2, "b":1}');
select * from json_maxmin_v;
insert into json_maxmin_t values('{"a":1, "b":2}');
select * from json_maxmin_v;
insert into json_maxmin_t values(null);
insert into json_maxmin_t values('{"a":2, "b":3}');
select * from json_maxmin_v;
insert into json_maxmin_t values('{"b":3,"a":2}');
select * from json_maxmin_v;
insert into json_maxmin_t values('{"b":2,"a":3}');
select * from json_maxmin_v;

-- test blobs
drop table if exists blob_maxmin_t cascade;
create table blob_maxmin_t(
    a tinyblob,
    b blob,
    c mediumblob,
    d longblob
);
insert into blob_maxmin_t values ('1.23.abc', '1.23.abc', '1.23.abc', '1.23.abc'),
                                 (null, null, null, null),
                                 ('3.23.abc', '3.23.abc', '3.23.abc', '3.23.abc'),
                                 (null, null, null, null),
                                 ('2.23.abc', '2.23.abc', '2.23.abc', '2.23.abc');
create view blob_maxmin_v as select
    max(a) as max_a,
    min(a) as min_a,
    max(b) as max_b,
    min(b) as min_b,
    max(c) as max_c,
    min(c) as min_c,
    max(d) as max_d,
    min(d) as min_d
    from blob_maxmin_t;
desc blob_maxmin_v;
select * from blob_maxmin_v;

-- test bit type with max/min
drop table if exists bit_maxmin_t cascade;
create table bit_maxmin_t(
    a bit,
    b bit(64)
);
insert into bit_maxmin_t values (b'1', b'111');
insert into bit_maxmin_t values (b'0', b'101');
select max(a), min(a) from bit_maxmin_t;
select max(b), min(b) from bit_maxmin_t;
create view bit_maxmin_v as select
    max(a) as max_a,
    min(a) as min_a,
    max(b) as max_b,
    min(b) as min_b
    from bit_maxmin_t;
desc bit_maxmin_v;
delete from bit_maxmin_t;
insert into bit_maxmin_t values (null, null);
insert into bit_maxmin_t values (null, null);
select * from bit_maxmin_v;
insert into bit_maxmin_t values (b'1', b'111');
insert into bit_maxmin_t values (null, null);
insert into bit_maxmin_t values (b'0', b'101');
select * from bit_maxmin_v;
