create schema tinyint_index;
set current_schema to 'tinyint_index';

create table t1(a int1);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);
insert into t1 select generate_series(-128, 127);


create index idx1 on t1(a);
analyze t1;

explain(costs off, verbose)select * from t1 where a = 1::int1;
explain(costs off, verbose)select * from t1 where a = 1::int2;
explain(costs off, verbose)select * from t1 where a = 1::int4;
explain(costs off, verbose)select * from t1 where a = 1::int8;

select * from t1 where a = 1::int1;
select * from t1 where a = 1::int2;
select * from t1 where a = 1::int4;
select * from t1 where a = 1::int8;

explain(costs off, verbose)select * from t1 where a > 1::int1 and a < 3::int1;
explain(costs off, verbose)select * from t1 where a > 1::int2 and a < 3::int2;
explain(costs off, verbose)select * from t1 where a > 1::int4 and a < 3::int4;
explain(costs off, verbose)select * from t1 where a > 1::int8 and a < 3::int8;

select * from t1 where a > 1::int1 and a < 3::int1;
select * from t1 where a > 1::int2 and a < 3::int2;
select * from t1 where a > 1::int4 and a < 3::int4;
select * from t1 where a > 1::int8 and a < 3::int8;

explain(costs off, verbose)select * from t1 where a >= -2::int1 and a <= -1::int1;
explain(costs off, verbose)select * from t1 where a >= -2::int2 and a <= -1::int2;
explain(costs off, verbose)select * from t1 where a >= -2::int4 and a <= -1::int4;
explain(costs off, verbose)select * from t1 where a >= -2::int8 and a <= -1::int8;

select * from t1 where a >= -2::int1 and a <= -1::int1;
select * from t1 where a >= -2::int2 and a <= -1::int2;
select * from t1 where a >= -2::int4 and a <= -1::int4;
select * from t1 where a >= -2::int8 and a <= -1::int8;

drop index idx1;
create index idx1 on t1 using hash (a);
analyze t1;

explain(costs off, verbose)select * from t1 where a = 1::int1;
explain(costs off, verbose)select * from t1 where a = 1::int2;
explain(costs off, verbose)select * from t1 where a = 1::int4;
explain(costs off, verbose)select * from t1 where a = 1::int8;

select * from t1 where a = 1::int1;
select * from t1 where a = 1::int2;
select * from t1 where a = 1::int4;
select * from t1 where a = 1::int8;


drop table t1;

create table t1(a int1) with (orientation = column);
create table t2(a int4) with (orientation = column);
insert into t2 select generate_series(0, 1000000);
insert into t1 select a % 128 from t2;

create index idx1 on t1 using btree (a);
analyze t1;

explain(costs off, verbose)select * from t1 where a >= -1::int1 and a <= 0::int1;
explain(costs off, verbose)select * from t1 where a >= -1::int2 and a <= 0::int2;
explain(costs off, verbose)select * from t1 where a >= -1::int4 and a <= 0::int4;
explain(costs off, verbose)select * from t1 where a >= -1::int8 and a <= 0::int8;

-- test tinyint column index in int array
CREATE TABLE t_ota_firmware (                                                                                                                                                            
     id uint4(11) AUTO_INCREMENT NOT NULL,                                                                                                                                                
     hardware_model character varying(128) DEFAULT ''::character varying NOT NULL,                                                           
     hardware_version character varying(128) DEFAULT ''::character varying NOT NULL,                                                         
     app_model character varying(32) DEFAULT ''::character varying,                                                                          
     app_version character varying(16) DEFAULT ''::character varying,                                                                        
     last_hardware_version character varying(16) DEFAULT ''::character varying,                                                              
     last_app_version character varying(16) DEFAULT ''::character varying,                                                                   
     doc_path character varying(128) DEFAULT ''::character varying NOT NULL,                                                                 
     fw_path character varying(128) DEFAULT ''::character varying NOT NULL,                                                                  
     fw_type tinyint(1) DEFAULT 0,                                                                                                                                                        
     fw_status tinyint(1) DEFAULT 0,                                                                                                                                                      
     attributes character varying(256) DEFAULT '{}'::character varying NOT NULL,                                                             
     create_time timestamp(0) with time zone DEFAULT (pg_systimestamp())::timestamp(0) with time zone NOT NULL,                                                                           
     update_time timestamp(0) with time zone DEFAULT (pg_systimestamp())::timestamp(0) with time zone ON UPDATE CURRENT_TIMESTAMP(0) NOT NULL,                                            
     creator character varying(64),                                                                                                          
     CONSTRAINT pk_t_ota_firmware_1743388445_42 PRIMARY KEY (id)                                                                                                                          
 ) AUTO_INCREMENT = 1                                                                                                                                                                     
 WITH (orientation=row, compression=no);
CREATE UNIQUE INDEX idx_unique_ver_t_ota_firm_1743388445_4258_43 ON t_ota_firmware USING btree (hardware_model, hardware_version, app_model, app_version, fw_type) TABLESPACE pg_default;

explain (costs off) select count(1) from t_ota_firmware where fw_status = 0 and fw_type in (0, 1);
select count(1) from t_ota_firmware where fw_status = 0 and fw_type in (0, 1);

drop schema tinyint_index cascade;
reset current_schema;