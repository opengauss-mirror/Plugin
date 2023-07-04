create schema db_func_call_2;
set current_schema to 'db_func_call_2';


create table t1 (a int);
create table t2 (a int);
insert into t1 values(1),(2),(3);
insert into t2 values(4),(5),(6);

create procedure proc1() select a from t1;
create procedure proc2() select a from t2;
create procedure proc3() select a from t2 where a > 3;
create procedure proc4() select a from t2 where a > 3 and a < 5;
create procedure proc5(x int) select a,x from t2 where a > 3 and a < 5;
create procedure proc6() select t2.a from t2 join t1 on t1.a < t2.a ;
create procedure proc7() NOT FENCED select a from t2 where a > 3 and a < 5;
create procedure proc8() NO SQL select a from t2 where a > 3 and a < 5;
create procedure proc9() LANGUAGE SQL select a from t2 where a > 3 and a < 5;


CREATE TABLE pbu_trade_collect_mbr_m (
    collect_date date,
    biz_date date,
    member_code varchar(36),
    breed_scope varchar(6),
    period_type varchar(60),
    mbr_org_num integer,
    mbr_pbu_num integer,
    allmbr_pbu_num integer,
    mbr_trade_amt numeric(18,2),
    allmbr_trade_amt numeric(18,2),
    mbr_amt_rate numeric(9,6),
    mbr_amt_rank integer,
    collect_des varchar(90)
);

INSERT INTO pbu_trade_collect_mbr_m (collect_date, biz_date, member_code, breed_scope, period_type, mbr_org_num, mbr_pbu_num, allmbr_pbu_num, mbr_trade_amt, allmbr_trade_amt, mbr_amt_rate, mbr_amt_rank, collect_des) VALUES('2022-08-24', '2022-06-30', '00030', '01', 'MONTH', 130, 492, 16851, 33694248540.86, 520204149074.12, 6.477120, 2, '2022-06');
INSERT INTO pbu_trade_collect_mbr_m (collect_date, biz_date, member_code, breed_scope, period_type, mbr_org_num, mbr_pbu_num, allmbr_pbu_num, mbr_trade_amt, allmbr_trade_amt, mbr_amt_rate, mbr_amt_rank, collect_des) VALUES('2022-08-24', '2022-06-30', '00395', '01', 'MONTH', 103, 459, 16851, 19332245263.44, 520204149074.12, 3.716280, 8, '2022-06');
INSERT INTO pbu_trade_collect_mbr_m (collect_date, biz_date, member_code, breed_scope, period_type, mbr_org_num, mbr_pbu_num, allmbr_pbu_num, mbr_trade_amt, allmbr_trade_amt, mbr_amt_rate, mbr_amt_rank, collect_des) VALUES('2022-08-24', '2022-06-30', '00701', '01', 'MONTH', 132, 799, 16851, 26630727808.08, 520204149074.12, 5.119284, 3, '2022-06');
INSERT INTO pbu_trade_collect_mbr_m (collect_date, biz_date, member_code, breed_scope, period_type, mbr_org_num, mbr_pbu_num, allmbr_pbu_num, mbr_trade_amt, allmbr_trade_amt, mbr_amt_rate, mbr_amt_rank, collect_des) VALUES('2022-08-24', '2022-06-30', '00558', '01', 'MONTH', 138, 846, 16851, 24208138747.60, 520204149074.12, 4.653584, 5, '2022-06');
INSERT INTO pbu_trade_collect_mbr_m (collect_date, biz_date, member_code, breed_scope, period_type, mbr_org_num, mbr_pbu_num, allmbr_pbu_num, mbr_trade_amt, allmbr_trade_amt, mbr_amt_rate, mbr_amt_rank, collect_des) VALUES('2022-08-24', '2022-06-30', '00129', '01', 'MONTH', 134, 548, 16851, 23636305947.68, 520204149074.12, 4.543660, 7, '2022-06');
INSERT INTO pbu_trade_collect_mbr_m (collect_date, biz_date, member_code, breed_scope, period_type, mbr_org_num, mbr_pbu_num, allmbr_pbu_num, mbr_trade_amt, allmbr_trade_amt, mbr_amt_rate, mbr_amt_rank, collect_des) VALUES('2022-08-24', '2022-06-30', '00015', '01', 'MONTH', 137, 748, 16851, 23862755529.64, 520204149074.12, 4.587191, 6, '2022-06');

create procedure proc10() 
SELECT c.biz_date   biz_date, c.member_code   member_code, c.breed_scope   breed_scope, c.collect_des   collect_des, c.trade_amt   trade_amt
, c.last_trade_amt   last_trade_amt
, (CASE WHEN(c.last_trade_amt = 0) THEN 0 ELSE c.trade_amt / c.last_trade_amt * 100 END)    yoy
FROM (
SELECT a.biz_date   biz_date, a.member_code   member_code, a.breed_scope   breed_scope, a.collect_des   collect_des, a.mbr_trade_amt   trade_amt
, lag(a.mbr_trade_amt, 1) OVER (PARTITION BY a.member_code, a.breed_scope ORDER BY a.biz_date ASC)   last_trade_amt
FROM pbu_trade_collect_mbr_m a
WHERE 
a.period_type = 'MONTH'
UNION ALL
SELECT b.biz_date   biz_date, b.member_code   member_code, 'AETF'   breed_scope, b.collect_des   collect_des, b.trade_amt   trade_amt
, lag(b.trade_amt, 1) OVER (PARTITION BY b.member_code ORDER BY b.biz_date ASC)   last_trade_amt
FROM (
SELECT a.biz_date   biz_date, a.member_code   member_code, a.collect_des   collect_des, SUM(a.mbr_trade_amt)   trade_amt
FROM pbu_trade_collect_mbr_m a
GROUP BY a.biz_date, a.member_code, a.collect_des
) b
UNION ALL
SELECT b.biz_date   biz_date, b.member_code   member_code, 'ABETF'   breed_scope, b.collect_des   collect_des, b.trade_amt   trade_amt
, lag(b.trade_amt, 1) OVER (PARTITION BY b.member_code ORDER BY b.biz_date ASC)   last_trade_amt
FROM (
SELECT a.biz_date   biz_date, a.member_code   member_code, a.collect_des   collect_des, SUM(a.mbr_trade_amt)   trade_amt
FROM pbu_trade_collect_mbr_m a

GROUP BY a.biz_date, a.member_code, a.collect_des
) b
) c order by trade_amt;


CREATE TABLE base_info_pbu_trade (
    id varchar(96) NOT NULL,
    data_start_date date,
    data_end_date date,
    pbu_code varchar(15),
    org_code varchar(36),
    org_name varchar(360),
    member_code varchar(36),
    sec_dim varchar(6),
    trade_amount numeric(18,2)
);

CREATE TABLE base_info_pbu_org (
    id varchar(96) NOT NULL,
    biz_date timestamp(0) without time zone,
    org_code varchar(36),
    org_name varchar(360),
    data_date timestamp(0) without time zone
);

INSERT INTO base_info_pbu_trade (id, data_start_date, data_end_date, pbu_code, org_code, org_name, member_code, sec_dim, trade_amount) VALUES('4e78edc2432140248142cefdd44e11e3', '2022-06-13', '2022-06-17', '46002', 'CH1BJST 004', '长盛', '00683', '04', 0.00);
INSERT INTO base_info_pbu_trade (id, data_start_date, data_end_date, pbu_code, org_code, org_name, member_code, sec_dim, trade_amount) VALUES('cdb0da0a27dc40939f8ad9a09d499b46', '2022-06-13', '2022-06-17', '46002', 'CH1BJST 004', '长盛', '00683', '07', 0.00);
INSERT INTO base_info_pbu_trade (id, data_start_date, data_end_date, pbu_code, org_code, org_name, member_code, sec_dim, trade_amount) VALUES('1dea9a8696d14e97b4b520b7dd25c7b6', '2022-06-13', '2022-06-17', '46002', 'CH1BJST 004', '长盛', '00683', '05', 0.00);

create procedure proc11()
SELECT 
a.member_code,       
a.org_code,          
b.org_name,          
a.breed_scope,         
'WEEK',                
a.mbr_org_pbunum,      
a.allmbr_org_pbunum,   
a.mbr_org_amt,         
a.allmbr_org_amt      
FROM (SELECT member_code                                                        member_code,         
 org_code                                                           org_code,            
 sec_dim                                                            breed_scope,         
 COUNT(DISTINCT pbu_code)                                           mbr_org_pbunum,      
 SUM(COUNT(DISTINCT pbu_code)) over (PARTITION BY org_code,sec_dim) allmbr_org_pbunum, 
 SUM(trade_amount)                                                  mbr_org_amt,       
 SUM(SUM(trade_amount)) over (PARTITION BY org_code,sec_dim)        allmbr_org_amt     
FROM base_info_pbu_trade
GROUP BY member_code, org_code, sec_dim
UNION ALL
SELECT member_code                                                member_code,       
 org_code                                                   org_code,          
 'AL'                                                       breed_scope,       
 COUNT(DISTINCT pbu_code)                                   mbr_org_pbunum,    
 SUM(COUNT(DISTINCT pbu_code)) over (PARTITION BY org_code) allmbr_org_pbunum, 
 SUM(trade_amount)                                          mbr_org_amt,     
 SUM(SUM(trade_amount)) over (PARTITION BY org_code)        allmbr_org_amt     
FROM base_info_pbu_trade
WHERE 
 sec_dim IN ('01', '02', '05', '06','07','09')
GROUP BY member_code, org_code) a
LEFT JOIN base_info_pbu_org b ON a.org_code = b.org_code  order by org_code;



select proc1();
select proc2();
select proc3();
select proc4();
select proc5();
select proc5(1);
select proc6();
select proc7();
select proc8();
select proc9();
select proc10();
select proc11();

set dolphin.sql_mode = 'block_return_multi_results'; 
call proc1();
call proc2();
call proc3();
call proc4();
call proc5();
call proc5(1);
call proc6();
call proc7();
call proc8();
call proc9();
call proc10();
call proc11();
set dolphin.sql_mode = default;

--error now check function 
create procedure proc33 () select z from tz;

-- test delimiter use in create procedure situation
-- report gram error in server ,not subprogram end error, success in plugin 
create table test_table (dot_no int);
insert into test_table values(1);
insert into test_table values(NULL);

delimiter //

create procedure test91()
begin
  declare rec_curs_value int;
  declare curs_dot cursor for select dot_no from test_table;
    open curs_dot;
    fetch curs_dot into rec_curs_value;
    while rec_curs_value is not null do
      fetch curs_dot into rec_curs_value;
    end while;
    close curs_dot;
end;
//
delimiter ;

set dolphin.sql_mode = 'block_return_multi_results'; 
call test91();
set dolphin.sql_mode = default;

delimiter //

create procedure test92()
begin
  declare rec_curs_value int;
  declare curs_dot cursor for select dot_no from test_table;
    open curs_dot;
    fetch curs_dot into rec_curs_value;
    while rec_curs_value is null do
      fetch curs_dot into rec_curs_value;
    end while;
    close curs_dot;
end;
//

delimiter ;

set dolphin.sql_mode = 'block_return_multi_results'; 
call test92();
set dolphin.sql_mode = default;

-- test deterministic error

create function fun2(age1 int)return int DETERMINISTIC 
NOT SHIPPABLE NOT FENCED EXTERNAL SECURITY INVOKER  
AS 
declare
a1 int;
begin
return a1;
end;
/
set dolphin.sql_mode = 'block_return_multi_results'; 
select fun2(1);

call fun2(1);
-- test create procedure select error 
create table t1_t (a int);

insert into t1_t values (1),(2);

-- server should has gram error,plugin pass
create procedure pro_test_t() select a from t1_t;

-- server should has gram error, plugin pass;
create procedure pro_test_t2() select a as b from t1_t;

select  pro_test_t();

call pro_test_t;

select  pro_test_t2();

call pro_test_t2;
set dolphin.sql_mode = default;

drop schema db_func_call_2 cascade;
reset current_schema;

