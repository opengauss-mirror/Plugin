create schema test_op_xor_unsignedint;
set current_schema to 'test_op_xor_unsignedint';

select (-1)::uint1 ^ 2::int1;
select (-1)::uint1 ^ 2::int2;
select (-1)::uint1 ^ 2::int4;
select (-1)::uint1 ^ 2::int8;
select (-1)::uint1 ^ 2::uint1;
select (-1)::uint1 ^ 2::uint2;
select (-1)::uint1 ^ 2::uint4;
select (-1)::uint1 ^ 2::uint8;
select (-1)::uint1 ^ 2::bool;
select (-1)::uint1 ^ 2::char;
select (-1)::uint1 ^ 2::varchar;


select (-10)::uint1 ^ (-2)::int2;
select (-10)::uint1 ^ (-2)::int4;
select (-10)::uint1 ^ (-2)::int8;
select (-10)::uint1 ^ (-2)::uint1;
select (-10)::uint1 ^ (-2)::uint2;
select (-10)::uint1 ^ (-2)::uint4;
select (-10)::uint1 ^ (-2)::uint8;
select (-10)::uint1 ^ (-2)::bool;
select (-10)::uint1 ^ (-2)::varchar;


select 2 ::uint1 ^ 2::int1;
select 2 ::uint1 ^ 2::int2;
select 2 ::uint1 ^ 2::int4;
select 2 ::uint1 ^ 2::int8;
select 2 ::uint1 ^ 2::uint1;
select 2 ::uint1 ^ 2::uint2;
select 2 ::uint1 ^ 2::uint4;
select 2 ::uint1 ^ 2::uint8;
select 2 ::uint1 ^ 2::bool;
select 2 ::uint1 ^ 2::varchar;



select (-1)::uint2 ^ 2::int1;
select (-1)::uint2 ^ 2::int2;
select (-1)::uint2 ^ 2::int4;
select (-1)::uint2 ^ 2::int8;
select (-1)::uint2 ^ 2::uint1;
select (-1)::uint2 ^ 2::uint2;
select (-1)::uint2 ^ 2::uint4;
select (-1)::uint2 ^ 2::uint8;
select (-1)::uint2 ^ 2::bool;
select (-1)::uint2 ^ 2::char;
select (-1)::uint2 ^ 2::varchar;


select (-10)::uint2 ^ (-2)::int2;
select (-10)::uint2 ^ (-2)::int4;
select (-10)::uint2 ^ (-2)::int8;
select (-10)::uint2 ^ (-2)::uint1;
select (-10)::uint2 ^ (-2)::uint2;
select (-10)::uint2 ^ (-2)::uint4;
select (-10)::uint2 ^ (-2)::uint8;
select (-10)::uint2 ^ (-2)::bool;
select (-10)::uint2 ^ (-2)::varchar;


select 2 ::uint2 ^ 2::int1;
select 2 ::uint2 ^ 2::int2;
select 2 ::uint2 ^ 2::int4;
select 2 ::uint2 ^ 2::int8;
select 2 ::uint2 ^ 2::uint1;
select 2 ::uint2 ^ 2::uint2;
select 2 ::uint2 ^ 2::uint4;
select 2 ::uint2 ^ 2::uint8;
select 2 ::uint2 ^ 2::bool;
select 2 ::uint2 ^ 2::char;
select 2 ::uint2 ^ 2::varchar;



select (-1)::uint4 ^ 2::int1;
select (-1)::uint4 ^ 2::int2;
select (-1)::uint4 ^ 2::int4;
select (-1)::uint4 ^ 2::int8;
select (-1)::uint4 ^ 2::uint1;
select (-1)::uint4 ^ 2::uint2;
select (-1)::uint4 ^ 2::uint4;
select (-1)::uint4 ^ 2::uint8;
select (-1)::uint4 ^ 2::bool;
select (-1)::uint4 ^ 2::char;
select (-1)::uint4 ^ 2::varchar;


select (-10)::uint4 ^ (-2)::int2;
select (-10)::uint4 ^ (-2)::int4;
select (-10)::uint4 ^ (-2)::int8;
select (-10)::uint4 ^ (-2)::uint1;
select (-10)::uint4 ^ (-2)::uint2;
select (-10)::uint4 ^ (-2)::uint4;
select (-10)::uint4 ^ (-2)::uint8;
select (-10)::uint4 ^ (-2)::bool;
select (-10)::uint4 ^ (-2)::varchar;


select 2 ::uint4 ^ 2::int1;
select 2 ::uint4 ^ 2::int2;
select 2 ::uint4 ^ 2::int4;
select 2 ::uint4 ^ 2::int8;
select 2 ::uint4 ^ 2::uint1;
select 2 ::uint4 ^ 2::uint2;
select 2 ::uint4 ^ 2::uint4;
select 2 ::uint4 ^ 2::uint8;
select 2 ::uint4 ^ 2::bool;
select 2 ::uint4 ^ 2::char;
select 2 ::uint4 ^ 2::varchar;


select (-1)::uint8 ^ 2::int1;
select (-1)::uint8 ^ 2::int2;
select (-1)::uint8 ^ 2::int4;
select (-1)::uint8 ^ 2::int8;
select (-1)::uint8 ^ 2::uint1;
select (-1)::uint8 ^ 2::uint2;
select (-1)::uint8 ^ 2::uint4;
select (-1)::uint8 ^ 2::uint8;
select (-1)::uint8 ^ 2::bool;
select (-1)::uint8 ^ 2::char;
select (-1)::uint8 ^ 2::varchar;


select (-10)::uint8 ^ (-2)::int2;
select (-10)::uint8 ^ (-2)::int4;
select (-10)::uint8 ^ (-2)::int8;
select (-10)::uint8 ^ (-2)::uint1;
select (-10)::uint8 ^ (-2)::uint2;
select (-10)::uint8 ^ (-2)::uint4;
select (-10)::uint8 ^ (-2)::uint8;
select (-10)::uint8 ^ (-2)::bool;
select (-10)::uint8 ^ (-2)::varchar;


select 2 ::uint8 ^ 2::int1;
select 2 ::uint8 ^ 2::int2;
select 2 ::uint8 ^ 2::int4;
select 2 ::uint8 ^ 2::int8;
select 2 ::uint8 ^ 2::uint1;
select 2 ::uint8 ^ 2::uint2;
select 2 ::uint8 ^ 2::uint4;
select 2 ::uint8 ^ 2::uint8;
select 2 ::uint8 ^ 2::bool;
select 2 ::uint8 ^ 2::char;
select 2 ::uint8 ^ 2::varchar;


create table t_operator_0035
(col1 int
,col2 int
,col3 float
,col4 float
,col5 bit(4)
,col6 bit(4)
,col7 date
,col8 date
,col9 time
,col10 time
,col11 text
,col12 text
,col13 boolean
,col14 boolean)
with (orientation=row);

insert into t_operator_0035 values
(15,-39,2.34,96.1,b'1011',b'1001','2022-08-20','2022-08-20','11:11:11','10:10:10','123a','456b',false,true),
(1,25,0.001,0.71,b'1000',null,'2022-08-20','2022-08-20','10:19:15','09:19:19','333','456',true,false),
(1,0,0.001,0.71,b'1000',b'1111','2022-08-20','2022-08-20','23:59:59','00:00:01','333','123a',true,false);

select col1 ^ col1, col1 ^ col2, col1 ^ col3, col1 ^ col4, col1 ^ col5, col1 ^ col6, col1 ^ col7, col1 ^ col8, col1 ^ col9, col1 ^ col10, col1 ^ col11, col1 ^ col12, col1 ^ col3, col1 ^ col14  from t_operator_0035;
select col2 ^ col1, col2 ^ col2, col2 ^ col3, col2 ^ col4, col2 ^ col5, col2 ^ col6, col2 ^ col7, col2 ^ col8, col2 ^ col9, col2 ^ col10, col2 ^ col11, col2 ^ col12, col2 ^ col3, col2 ^ col14  from t_operator_0035;
select col3 ^ col1, col3 ^ col2, col3 ^ col3, col3 ^ col4, col3 ^ col5, col3 ^ col6, col3 ^ col7, col3 ^ col8, col3 ^ col9, col3 ^ col10, col3 ^ col11, col3 ^ col12, col3 ^ col3, col3 ^ col14  from t_operator_0035;
select col4 ^ col1, col4 ^ col2, col4 ^ col3, col4 ^ col4, col4 ^ col5, col4 ^ col6, col4 ^ col7, col4 ^ col8, col4 ^ col9, col4 ^ col10, col4 ^ col11, col4 ^ col12, col4 ^ col3, col4 ^ col14  from t_operator_0035;
select col5 ^ col1, col5 ^ col2, col5 ^ col3, col5 ^ col4, col5 ^ col5, col5 ^ col6, col5 ^ col7, col5 ^ col8, col5 ^ col9, col5 ^ col10, col5 ^ col11, col5 ^ col12, col5 ^ col3, col5 ^ col14  from t_operator_0035;
select col6 ^ col1, col6 ^ col2, col6 ^ col3, col6 ^ col4, col6 ^ col5, col6 ^ col6, col6 ^ col7, col6 ^ col8, col6 ^ col9, col6 ^ col10, col6 ^ col11, col6 ^ col12, col6 ^ col3, col6 ^ col14  from t_operator_0035;
select col7 ^ col1, col7 ^ col2, col7 ^ col3, col7 ^ col4, col7 ^ col5, col7 ^ col6, col7 ^ col7, col7 ^ col8, col7 ^ col9, col7 ^ col10, col7 ^ col11, col7 ^ col12, col7 ^ col3, col7 ^ col14  from t_operator_0035;
select col8 ^ col1, col8 ^ col2, col8 ^ col3, col8 ^ col4, col8 ^ col5, col8 ^ col6, col8 ^ col7, col8 ^ col8, col8 ^ col9, col8 ^ col10, col8 ^ col11, col8 ^ col12, col8 ^ col3, col8 ^ col14  from t_operator_0035;
select col9 ^ col1, col9 ^ col2, col9 ^ col3, col9 ^ col4, col9 ^ col5, col9 ^ col6, col9 ^ col7, col9 ^ col8, col9 ^ col9, col9 ^ col10, col9 ^ col11, col9 ^ col12, col9 ^ col3, col9 ^ col14  from t_operator_0035;
select col10 ^ col1, col10 ^ col2, col10 ^ col3, col10 ^ col4, col10 ^ col5, col10 ^ col6, col10 ^ col7, col10 ^ col8, col10 ^ col9, col10 ^ col10, col10 ^ col11, col10 ^ col12, col10 ^ col13, col10 ^ col14  from t_operator_0035;
select col11 ^ col1, col11 ^ col2, col11 ^ col3, col11 ^ col4, col11 ^ col5, col11 ^ col6, col11 ^ col7, col11 ^ col8, col11 ^ col9, col11 ^ col10, col11 ^ col11, col11 ^ col12, col11 ^ col13, col11 ^ col14  from t_operator_0035;
select col12 ^ col1, col12 ^ col2, col12 ^ col3, col12 ^ col4, col12 ^ col5, col12 ^ col6, col12 ^ col7, col12 ^ col8, col12 ^ col9, col12 ^ col10, col12 ^ col11, col12 ^ col12, col12 ^ col13, col12 ^ col14  from t_operator_0035;
select col13 ^ col1, col13 ^ col2, col13 ^ col3, col13 ^ col4, col13 ^ col5, col13 ^ col6, col13 ^ col7, col13 ^ col8, col13 ^ col9, col13 ^ col10, col13 ^ col11, col13 ^ col12, col13 ^ col13, col13 ^ col14  from t_operator_0035;
select col14 ^ col1, col14 ^ col2, col14 ^ col3, col14 ^ col4, col14 ^ col5, col14 ^ col6, col14 ^ col7, col14 ^ col8, col14 ^ col9, col14 ^ col10, col14 ^ col11, col14 ^ col12, col14 ^ col13, col14 ^ col14  from t_operator_0035;

drop table t_operator_0035;

drop schema test_op_xor_unsignedint cascade;
reset current_schema;
