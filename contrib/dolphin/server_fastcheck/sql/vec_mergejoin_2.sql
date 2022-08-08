create schema vector_mergejoin_engine_2;
set search_path to vector_mergejoin_engine_2;

--test early free
drop table if exists location cascade;
create table location
(
    location_id number(18,0) not null ,
    location_name char(40) null,
    location_open_dt date null,
    location_close_dt date null,
    location_effective_dt date null,
    location_total_area_meas number(18,4) null ,
    chain_cd varchar(50) null,
    channel_cd varchar(50) null,
    district_cd varchar(50) null,
    parent_location_id integer null ,
    location_mgr_associate_id number(18,18) null ,
    location_type_cd varchar(50) null,
    LOCATION_POINT VARCHAR(100) NULL ,
    CUSTOMER_AREAS VARCHAR(100) NULL ,
 partial cluster key(location_id,location_name,location_total_area_meas,location_mgr_associate_id)
)with (orientation=column)
partition by range (location_id )
(
partition location_1 values less than (0),
partition location_2 values less than (1),
partition location_3 values less than (2),
partition location_4 values less than (3),
partition location_5 values less than (4),
partition location_6 values less than (5),
partition location_7 values less than (6),
partition location_8 values less than (7),
partition location_9 values less than (8),
partition location_10 values less than (9),
partition location_11 values less than (10),
partition location_12 values less than (11),
partition location_13 values less than (12),
partition location_14 values less than (13),
partition location_15 values less than (14),
partition location_16 values less than (15),
partition location_17 values less than (16),
partition location_18 values less than (17),
partition location_19 values less than (18),
partition location_20 values less than (19),
partition location_21 values less than (20),
partition location_22 values less than (maxvalue)
);

drop table if exists item_inventory_plan cascade;
create table item_inventory_plan
(
    item_inventory_plan_dt date not null ,
    location_id number(35,0) not null ,
    item_id number(20,5) not null ,
    plan_on_hand_qty decimal(18,4) null,
    plan_on_hand_retail_amt number(18,4) null,
 partial cluster key(item_inventory_plan_dt,location_id,item_id,plan_on_hand_retail_amt)
)with (orientation=column)
partition by range (location_id)
(
partition item_inventory_plan_1 values less than (0),
partition item_inventory_plan_2 values less than (1),
partition item_inventory_plan_3 values less than (2),
partition item_inventory_plan_4 values less than (3),
partition item_inventory_plan_5 values less than (4),
partition item_inventory_plan_6 values less than (5),
partition item_inventory_plan_7 values less than (6),
partition item_inventory_plan_8 values less than (7),
partition item_inventory_plan_9 values less than (8),
partition item_inventory_plan_10 values less than (9),
partition item_inventory_plan_11 values less than (10),
partition item_inventory_plan_12 values less than (11),
partition item_inventory_plan_13 values less than (12),
partition item_inventory_plan_14 values less than (13),
partition item_inventory_plan_15 values less than (14),
partition item_inventory_plan_16 values less than (15),
partition item_inventory_plan_17 values less than (16),
partition item_inventory_plan_18 values less than (17),
partition item_inventory_plan_19 values less than (18),
partition item_inventory_plan_20 values less than (19),
partition item_inventory_plan_21 values less than (20),
partition item_inventory_plan_22 values less than (maxvalue)
);

INSERT INTO LOCATION VALUES (20,  'SHANG HAI' , DATE '1970-01-01', DATE '1970-01-01', DATE '1970-01-01', 0.20, 'A' , 'A' , 'CHINA', 0.6555   , 0.1238790, 'A','POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (1,  NULL, NULL             , NULL             , NULL             , 1.8, 'B' , NULL, 'JAP' , 1   , NULL, 'B','POINT(12 20)','POLYGON((0 1, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (2,  'HU NAN' , DATE '1976-01-01', DATE '1976-01-01', DATE '1976-01-01', 2.0, NULL, 'C' , 'CHINA' , 17  , 0.2328787, 'C','POINT(10 20)','POLYGON((0 2, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (3,  'BEI JING' , DATE '1979-01-01', DATE '1979-01-01', DATE '1979-01-01', 3.3, 'D' , 'D' , 'CHINA' , 3   , 0.9808787 , NULL,'POINT(10 20)','POLYGON((0 3, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (4,  'TOKYO' , NULL             , DATE '1982-01-01', DATE '1982-01-01', 4.0, 'E' , NULL ,'JAP' , 4   , 0.878787 , 'E','POINT(10 20)','POLYGON((0 0, 0 21, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (5,  NULL, DATE '1985-01-01', NULL             , NULL             , 5.7, NULL, 'F' , 'NICAR' , NULL, -0.7887879 , 'F','POINT(10 21)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (6,  'LOSANGELES' , DATE '1988-01-01', DATE '1988-01-01', DATE '1988-01-01', 6.0, 'G' , 'F' , 'AMER' , 6   , 0.97832979, 'G','POINT(10 26)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (7,  'DENVER' , DATE '1991-01-01', DATE '1991-01-01', DATE '1991-01-01', 7.5, 'H' , 'G' , 'AMER', 7   , 0.78789789 , NULL,'POINT(10 10)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (8,  ' ' , DATE '1994-01-01', DATE '1994-01-01', DATE '1994-01-01', 8.0, 'I' , NULL, 'COLOMA' , 8   , NULL, 'F','POINT(10 23)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (9,  'PLEASANTON' , DATE '1997-01-01', DATE '1997-01-01', DATE '1997-01-01', 9.6, NULL, 'G' , 'BRA' , 7 , 0.0000000000001 , 'H','POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (10, 'NORTHHOLLYWOOD' , DATE '1970-01-01', NULL             , DATE '1970-01-01', 3.0, 'A' , 'A' , 'AMER' , NULL, 0.999999999  , 'A','POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (11, NULL, NULL             , DATE '2999-12-31', DATE '1973-01-01', 1.5, 'B' , 'B' , 'KOR' , 14  , -0.898839797 , 'JI','POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (12, ' ' , DATE '1976-01-01', DATE '1976-01-01', DATE '1976-01-01', 2.8, 'C' , 'M' , NULL, 2   , 0.782973089 , 'M','POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (13, 'PUEBLO' , DATE '1979-01-01', DATE '1979-01-01', DATE '1979-01-01', 3.4, 'D' , 'A' , 'IND' , 3   , 0.78972897 , 'B','POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (14, 'HONG KONG' , DATE '2999-12-31', DATE '1982-01-01', NULL             , 4.0, NULL, 'E' , 'CHINA' , 4   , 0.76767896 , 'I','POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (15, 'IRVINE' , DATE '1985-01-01', DATE '1985-01-01', DATE '1985-01-01', 5.9, 'F' , 'F' , NULL , NULL, 0.45647654  , NULL,'POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (16, 'LON' , DATE '1988-01-01', DATE '1988-01-01', DATE '1988-01-01', 6.0, 'G' , 'K' , 'L' , 12  , 0.68769786  , 'G','POINT(10 20)',NULL);
INSERT INTO LOCATION VALUES (17, NULL, DATE '1991-01-01', NULL             , DATE '1991-01-01', 7.0, NULL, NULL, ''  , 7   , NULL, 'G','POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (18, ''  , NULL             , DATE '1994-01-01', NULL             , 3.2, 'I' , ''  , 'BOL' , 8   , 0.468765   , NULL,'POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');
INSERT INTO LOCATION VALUES (19, NULL, DATE '1997-01-01', NULL             , DATE '1997-01-01', 9.0, ''  , 'G' , 'N', NULL, 0.78907978 , '','POINT(10 20)','POLYGON((0 0, 0 20, 20 20, 20 0, 0 0),(5 5, 5 10, 10 10, 10 5, 5 5))');

INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1970-01-01', 1, 0.12, 0.30 , NULL);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1973-01-01', 1, 0.12, NULL, 1.0);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1976-01-01', 2, 1.3, 2.0 , 2.0);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1979-01-01', 3, 5.01, 3.0 , 3.0);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1982-01-01', 4, 5.01, 4.0 , NULL);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1985-01-01', 5, 5.01, NULL, 5.0);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1988-01-01', 5, 9.12, 6.0 , 6.0);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1991-01-01', 6, 9.12, 7.0 , 7.0);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1994-01-01', 6, 0.7, 8.0 , 8.0);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1997-01-01', 6, 0.08, 9.0 , 9.0);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1979-01-01',13, 0.08, NULL, NULL);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1983-01-01',14, 13.99, 4.0 , 4.0);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1986-01-01',15, 15, 5.0 , NULL);
INSERT INTO ITEM_INVENTORY_PLAN VALUES (DATE '1987-01-01',15, 6, NULL, 6.0);

set enable_early_free=on;
set enable_partitionwise = on;
set enable_nestloop=off;
set enable_hashjoin=on;
set enable_mergejoin=off;

SELECT iip.location_id + iip.plan_on_hand_qty
FROM location loc, item_inventory_plan iip
WHERE iip.LOCATION_ID = loc.LOCATION_ID
ORDER BY 1 DESC;

set enable_hashjoin=off;
set enable_mergejoin=on;
SELECT iip.location_id + iip.plan_on_hand_qty
FROM location loc, item_inventory_plan iip
WHERE iip.LOCATION_ID = loc.LOCATION_ID
ORDER BY 1 DESC;

drop schema  vector_mergejoin_engine_2 cascade;
