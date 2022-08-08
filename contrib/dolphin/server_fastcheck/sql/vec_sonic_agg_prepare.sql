

----
--- Create Table and Insert Data
----
drop schema vsonic_agg_engine cascade;
create schema vsonic_agg_engine;
set current_schema=vsonic_agg_engine;
set enable_sonic_hashjoin=on;
set enable_sort = off;
set time zone prc;
set time zone prc;
set datestyle to iso;

create table vsonic_agg_engine.ROW_AGG_TABLE_01
( 
  col_a1 int1
  ,col_a2 int2
  ,col_a3 int4
  ,col_a4 int8
  ,col_smallint smallint null
  ,col_integer  integer  
  ,col_bigint  bigint  
  ,col_oid   Oid
  ,col_real real 
  ,col_numeric numeric(18,12) 
  ,col_numeric2 numeric(888,666) 
  ,col_numeric3 numeric(10,2) 
  ,col_numeric4 numeric(2,1) 
  ,col_float float4
  ,col_float2 float8
  ,col_t1 timestamptz
  ,col_t2 interval
  ,col_t3 timestamp(3)
  ,col_inet inet
  ,col_double_precision double precision
  ,col_decimal decimal(19) default 923423423
  ,col_char  char(57) null
  ,col_char2  char default '0'
  ,col_char3  char(10000) null
  ,col_varchar 	varchar(19)
  ,col_varchar2  varchar2(20)
  ,col_varchar3 varchar(10000)
  ,col_text text  null
  ,col_time_without_time_zone time without time zone null
  ,col_time_with_time_zone  time with time zone
  ,col_timestamp_without_timezone timestamp
  ,col_timestamp_with_timezone timestamptz
  ,col_smalldatetime smalldatetime
  ,col_money money
  ,col_date date
) distribute by hash(col_integer);
create table vsonic_agg_engine.VECTOR_AGG_TABLE_01
( 
  col_a1 int1
  ,col_a2 int2
  ,col_a3 int4
  ,col_a4 int8
  ,col_smallint smallint null
  ,col_integer  integer  
  ,col_bigint  bigint  
  ,col_oid   Oid
  ,col_real real 
  ,col_numeric numeric(18,12) 
  ,col_numeric2 numeric(888,666) 
  ,col_numeric3 numeric(10,2) 
  ,col_numeric4 numeric(2,1) 
  ,col_float float4
  ,col_float2 float8
  ,col_t1 timestamptz
  ,col_t2 interval
  ,col_t3 timestamp(3)
  ,col_inet inet
  ,col_double_precision double precision
  ,col_decimal decimal(19) default 923423423
  ,col_char  char(57) null
  ,col_char2  char default '0'
  ,col_char3  char(10000) null
  ,col_varchar 	varchar(19)
  ,col_varchar2  varchar2(20)
  ,col_varchar3 varchar(10000)
  ,col_text text  null
  ,col_time_without_time_zone time without time zone null
  ,col_time_with_time_zone  time with time zone
  ,col_timestamp_without_timezone timestamp
  ,col_timestamp_with_timezone timestamptz
  ,col_smalldatetime smalldatetime
  ,col_money money
  ,col_date date
) with (orientation=column) distribute by hash(col_bigint);

create table vsonic_agg_engine.VECTOR_AGG_TABLE_02
(  col_int  int
  ,col_num  numeric
  ,col_bint bigint
)with(orientation = column) distribute by hash(col_int);

create table vsonic_agg_engine.VECTOR_AGG_TABLE_03
( 
  col_a1 int1
  ,col_a2 int2
  ,col_a3 int4
  ,col_a4 int8
  ,col_smallint smallint null
  ,col_integer  integer  
  ,col_bigint  bigint  
  ,col_oid   Oid
  ,col_real real 
  ,col_numeric numeric(18,12) 
  ,col_numeric2 numeric(888,666) 
  ,col_numeric3 numeric(10,2) 
  ,col_numeric4 numeric(2,1) 
  ,col_float float4
  ,col_float2 float8
  ,col_t1 timestamptz
  ,col_t2 interval
  ,col_t3 timestamp(3)
  ,col_inet inet
  ,col_double_precision double precision
  ,col_decimal decimal(19) default 923423423
  ,col_char  char(57) null
  ,col_char2  char default '0'
  ,col_char3  char(10000) null
  ,col_varchar 	varchar(19)
  ,col_varchar2  varchar2(20)
  ,col_varchar3 varchar(10000)
  ,col_text text  null
  ,col_time_without_time_zone time without time zone null
  ,col_time_with_time_zone  time with time zone
  ,col_timestamp_without_timezone timestamp
  ,col_timestamp_with_timezone timestamptz
  ,col_smalldatetime smalldatetime
  ,col_money money
  ,col_date date
) with (orientation=column) distribute by hash(col_a4)
partition by range(col_numeric, col_date, col_integer)(
partition partition_p1 values less than(3,'2002-02-04 00:01:00',20),
partition partition_p2 values less than(21,'2005-03-26 00:00:00',1061) ,
partition partition_p3 values less than(121,'2005-06-01 20:00:00',1600),
partition partition_p4 values less than(121,'2006-08-01 20:00:00',1987) ,
partition partition_p5 values less than(1456,'2007-12-03 10:00:00',2567),
partition partition_p6 values less than(2678,'2008-02-03 11:01:34',2800),
partition partition_p7 values less than(3601,'2008-02-13 01:01:34',3801),
partition partition_p8 values less than(3601,'2012-04-18 23:01:44',4560),
partition partition_p9 values less than(4500,'2012-06-18 23:01:44',4900),
partition partition_p10 values less than(9845,'2016-06-28 23:21:44',6200)) ;

--replication
create table vsonic_agg_engine.VECTOR_AGG_TABLE_04
( 
  col_a1 int1
  ,col_a2 int2
  ,col_a3 int4
  ,col_a4 int8
  ,col_smallint smallint null
  ,col_integer  integer  
  ,col_bigint  bigint  
  ,col_oid   Oid
  ,col_real real 
  ,col_numeric numeric(18,12) 
  ,col_numeric2 numeric(888,666) 
  ,col_numeric3 numeric(10,2) 
  ,col_numeric4 numeric(2,1) 
  ,col_float float4
  ,col_float2 float8
  ,col_t1 timestamptz
  ,col_t2 interval
  ,col_t3 timestamp(3)
  ,col_inet inet
  ,col_double_precision double precision
  ,col_decimal decimal(19) default 923423423
  ,col_char  char(57) null
  ,col_char2  char default '0'
  ,col_char3  char(10000) null
  ,col_varchar 	varchar(19)
  ,col_varchar2  varchar2(20)
  ,col_varchar3 varchar(10000)
  ,col_text text  null
  ,col_time_without_time_zone time without time zone null
  ,col_time_with_time_zone  time with time zone
  ,col_timestamp_without_timezone timestamp
  ,col_timestamp_with_timezone timestamptz
  ,col_smalldatetime smalldatetime
  ,col_money money
  ,col_date date
) with (orientation=column) DISTRIBUTE BY REPLICATION
partition by range(col_numeric, col_date, col_integer)(
partition partition_p1 values less than(3,'2002-02-04 00:01:00',20),
partition partition_p2 values less than(21,'2005-03-26 00:00:00',1061) ,
partition partition_p3 values less than(121,'2005-06-01 20:00:00',1600),
partition partition_p4 values less than(121,'2006-08-01 20:00:00',1987) ,
partition partition_p5 values less than(1456,'2007-12-03 10:00:00',2567),
partition partition_p6 values less than(2678,'2008-02-03 11:01:34',2800),
partition partition_p7 values less than(3601,'2008-02-13 01:01:34',3801),
partition partition_p8 values less than(3601,'2012-04-18 23:01:44',4560),
partition partition_p9 values less than(4500,'2012-06-18 23:01:44',4900),
partition partition_p10 values less than(9845,'2016-06-28 23:21:44',6200)) ;

create table src(a int);
insert into src values(1);

create table t1(a int);
insert into t1 select generate_series(-1001,1) from src;
--                                                                           col_a4                     	   col_oid    col_real      col_numeric                                  col_numeric2                                   col_numeric4         col_float              col_float2               timestamptz           interval         timestamp(3)         inet      col_double_precision                         char(57)     char     char(10000)                                                     varchar(19)       varchar2(20)           varchar(10000)                  text                                
insert into vsonic_agg_engine.row_agg_table_01 select 1, i.a%10, -i.a-111111, i.a*2+23, 2, i.a%11, i.a*5+121111, i.a*3+45, i.a + 10.001, 561.322815379585 + i.a, 561345364347334533345346349630963096859457457.322815379585 + i.a,i.a, 1.1,           8885.169 - i.a * 0.125, 61032419811910588 + i.a, '2018-11-15 00:00:00+9', '3 days', '2028-12-19 00:00:00.3426','192.168.1.226/24',i.a*12.2342, i.a*12235263453.2342,'test_agg_'||i.a, 'F', 'vector_agg_'||i.a|| '597b5b23f4aadf9513306bcd59afb6e4c9_'||i.a, 'beijing_agg'||i.a, 'beijing_agg'||i.a*2,'beijing_agg2434235'||i.a,   'beijing_agg'||i.a*0.5,  '08:20:12', '06:26:42+08', 'Mon Feb 10 17:32:01.4 1997 PST', '1971-03-23 11:14:05', '1997-03-03 11:04', 56 + i.a, '2005-02-14' 
 from t1 i;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
create table t2(a int);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
insert into t2 select generate_series(2000,2487) from src;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
insert into vsonic_agg_engine.row_agg_table_01 select  2, i.a%10,-i.a-121111, i.a*3+45, 3, i.a%12, i.a*2+111131, i.a*4+345, i.a + 9.08, 27.25684426652 + i.a * 0.001, 5616585673453634534637456445645745777444.322815379585 + i.a, i.a, 2.2,         8885.169 - i.a * 0.125, 61032419811910588 + i.a, '2018-11-19 08:00:00+9', '3 days', '2018-11-14 10:00:00.3456', '11.1.2.3/8',      i.a*33.2342,  i.a*33242345523.2342,'test_agg_'||i.a, 'B', 'vector_agg_'||i.a|| '597b5b23f4aadf9513fsdgsdgsdgsdgs6e4c9_'||i.a, 'beijing_agg'||i.a, 'beijing_agg'||i.a, 'beijing_agg768678678'||i.a,'beijing_agg'||i.a*1.1, '14:21:56', '15:12:22+08', 'Mon Feb 10 17:32:01.4 1997 PST', '1971-03-23 11:14:05', '1996-06-12 03:06', 56 + i.a, '2008-02-14' 

 from t2 i;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
create table t3(a int);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
insert into t3 select generate_series(2488, 6000) from src;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
insert into vsonic_agg_engine.row_agg_table_01 select 3, i.a%10, -i.a, i.a*4+345, 1, i.a%17, i.a, i.a*2+23,  i.a + 2.047, 39.2456977995 + i.a * 0.3,  56164574574585684353463745746575687654345.322815379585 + i.a, i.a,3.3,          8885.169 - i.a * 0.125, 61032419811910588 + i.a, '2017-11-19 00:00:20+9', '3 days', '2018-11-16 00:20:00.3456', '192.168.1.255/25', i.a*-23.4322,i.a*-234234234525.4322, 'test_agg_'||i.a, 'A', 'vector_agg_'||i.a||'597b5b23f4aadsdgsdgwer32ewtr24_'||i.a, 'beijing_agg'||i.a,'beijing_agg'||i.a,'beijing_aggfgsdfgdfgdfhd'||i.a,  'beijing_agg'||i.a*2.3,    '19:07:24', '22:32:36+08', 'Mon Feb 10 17:32:01.4 1997 PST', '1971-03-23 11:14:05', '1992-02-06 03:08', 56 + i.a, '2015-02-14' 
 from t3 i;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
drop table t1;
drop table t2;
drop table t3;

insert into  vector_agg_table_01 select * from row_agg_table_01;
insert into  VECTOR_AGG_TABLE_03  select * from row_agg_table_01;
insert into  VECTOR_AGG_TABLE_04  select * from row_agg_table_01;

COPY VECTOR_AGG_TABLE_02(col_int, col_num, col_bint) FROM stdin;
1	1.2	111111
4	2.1	111111
8	3.6	121111
2	2.4	111131
9	4.2	111111
3	1.6	111131
5	21.7	121111
3	5.6	111111
\.

 
analyze vector_agg_table_01;
analyze vector_agg_table_02;
analyze vector_agg_table_03;
analyze vector_agg_table_04;