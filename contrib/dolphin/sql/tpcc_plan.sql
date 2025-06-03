create schema tpcc_plan;
set current_schema to 'tpcc_plan';

drop table if exists warehouse;

create table warehouse (
w_id smallint not null,
w_name varchar(10), 
w_street_1 varchar(20), 
w_street_2 varchar(20), 
w_city varchar(20), 
w_state char(2), 
w_zip char(9), 
w_tax decimal(4,2), 
w_ytd decimal(12,2),
primary key (w_id) ) Engine=InnoDB;

drop table if exists district;

create table district (
d_id tinyint not null, 
d_w_id smallint not null, 
d_name varchar(10), 
d_street_1 varchar(20), 
d_street_2 varchar(20), 
d_city varchar(20), 
d_state char(2), 
d_zip char(9), 
d_tax decimal(4,2), 
d_ytd decimal(12,2), 
d_next_o_id int,
primary key (d_w_id, d_id) ) Engine=InnoDB;

drop table if exists customer;

create table customer (
c_id int not null, 
c_d_id tinyint not null,
c_w_id smallint not null, 
c_first varchar(16), 
c_middle char(2), 
c_last varchar(16), 
c_street_1 varchar(20), 
c_street_2 varchar(20), 
c_city varchar(20), 
c_state char(2), 
c_zip char(9), 
c_phone char(16), 
c_since datetime, 
c_credit char(2), 
c_credit_lim bigint, 
c_discount decimal(4,2), 
c_balance decimal(12,2), 
c_ytd_payment decimal(12,2), 
c_payment_cnt smallint, 
c_delivery_cnt smallint, 
c_data text,
PRIMARY KEY(c_w_id, c_d_id, c_id) ) Engine=InnoDB;

drop table if exists history;

create table history (
h_c_id int, 
h_c_d_id tinyint, 
h_c_w_id smallint,
h_d_id tinyint,
h_w_id smallint,
h_date datetime,
h_amount decimal(6,2), 
h_data varchar(24) ) Engine=InnoDB;

drop table if exists new_orders;

create table new_orders (
no_o_id int not null,
no_d_id tinyint not null,
no_w_id smallint not null,
PRIMARY KEY(no_w_id, no_d_id, no_o_id)) Engine=InnoDB;

drop table if exists orders;

create table orders (
o_id int not null, 
o_d_id tinyint not null, 
o_w_id smallint not null,
o_c_id int,
o_entry_d datetime,
o_carrier_id tinyint,
o_ol_cnt tinyint, 
o_all_local tinyint,
PRIMARY KEY(o_w_id, o_d_id, o_id) ) Engine=InnoDB ;

drop table if exists order_line;

create table order_line ( 
ol_o_id int not null, 
ol_d_id tinyint not null,
ol_w_id smallint not null,
ol_number tinyint not null,
ol_i_id int, 
ol_supply_w_id smallint,
ol_delivery_d datetime, 
ol_quantity tinyint, 
ol_amount decimal(6,2), 
ol_dist_info char(24),
PRIMARY KEY(ol_w_id, ol_d_id, ol_o_id, ol_number) ) Engine=InnoDB ;

drop table if exists item;

create table item (
i_id int not null, 
i_im_id int, 
i_name varchar(24), 
i_price decimal(5,2), 
i_data varchar(50),
PRIMARY KEY(i_id) ) Engine=InnoDB;

drop table if exists stock;

create table stock (
s_i_id int not null, 
s_w_id smallint not null, 
s_quantity smallint, 
s_dist_01 char(24), 
s_dist_02 char(24),
s_dist_03 char(24),
s_dist_04 char(24), 
s_dist_05 char(24), 
s_dist_06 char(24), 
s_dist_07 char(24), 
s_dist_08 char(24), 
s_dist_09 char(24), 
s_dist_10 char(24), 
s_ytd decimal(8,0), 
s_order_cnt smallint, 
s_remote_cnt smallint,
s_data varchar(50),
PRIMARY KEY(s_w_id, s_i_id) ) Engine=InnoDB ;

analyze customer;  
analyze district; 
analyze history;
analyze item;
analyze new_orders;
analyze order_line;
analyze orders;
analyze stock;
analyze warehouse;

set enable_seqscan = off;
set dolphin.transform_unknown_param_type_as_column_type_first = true;

prepare s1 as SELECT c_discount, c_last, c_credit, w_tax FROM customer, warehouse WHERE w_id = ? AND c_w_id = w_id AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3;


prepare s1 as SELECT d_next_o_id, d_tax FROM district WHERE d_id = ? AND d_w_id = ? FOR UPDATE;
explain (costs off) execute s1 using 1, 2;


prepare s1 as UPDATE district SET d_next_o_id = ? + 1 WHERE d_id = ? AND d_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d, o_ol_cnt, o_all_local) VALUES(?, ?, ?, ?, ?, ?, ?);
explain (costs off) execute s1 using 1, 2, 3, 4, '2024-10-10 10:10:10', 6, 7;


prepare s1 as INSERT INTO new_orders (no_o_id, no_d_id, no_w_id) VALUES (?,?,?);
explain (costs off) execute s1 using 1, 2, 3;


prepare s1 as SELECT i_price, i_name, i_data FROM item WHERE i_id = ?;
explain (costs off) execute s1 using 1;

prepare s1 as SELECT s_quantity, s_data, s_dist_01, s_dist_02, s_dist_03, s_dist_04, s_dist_05, s_dist_06, s_dist_07, s_dist_08, s_dist_09, s_dist_10 FROM stock WHERE s_i_id = ? AND s_w_id = ? FOR UPDATE;
explain (costs off) execute s1 using 1, 2;

prepare s1 as UPDATE stock SET s_quantity = ? WHERE s_i_id = ? AND s_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_dist_info) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);
explain (costs off) execute s1 using 1, 2, 3, 4, 5, 6, 7, 8, 9;

prepare s1 as UPDATE warehouse SET w_ytd = w_ytd + ? WHERE w_id = ?;
explain (costs off) execute s1 using 1, 2;

prepare s1 as SELECT w_street_1, w_street_2, w_city, w_state, w_zip, w_name FROM warehouse WHERE w_id = ?;
explain (costs off) execute s1 using 1;

prepare s1 as UPDATE district SET d_ytd = d_ytd + ? WHERE d_w_id = ? AND d_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT d_street_1, d_street_2, d_city, d_state, d_zip, d_name FROM district WHERE d_w_id = ? AND d_id = ?;
explain (costs off) execute s1 using 1, 2;

prepare s1 as SELECT count(c_id) FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_id FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ? ORDER BY c_id;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city, c_state, c_zip, c_phone, c_credit, c_credit_lim, c_discount, c_balance, c_since FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ? FOR UPDATE;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_data FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as UPDATE customer SET c_balance = ?, c_data = ? WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3, 4, 5;

prepare s1 as UPDATE customer SET c_balance = ? WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3, 4;

prepare s1 as INSERT INTO history(h_c_d_id, h_c_w_id, h_c_id, h_d_id, h_w_id, h_date, h_amount, h_data) VALUES(?, ?, ?, ?, ?, ?, ?, ?);
explain (costs off) execute s1 using 1, 2, 3, 4, 5, '2024-10-10 10:10:10', 7, 8;

prepare s1 as SELECT count(c_id) FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ? ORDER BY c_first;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT o_id, o_entry_d, COALESCE(o_carrier_id,0) FROM orders WHERE o_w_id = ? AND o_d_id = ? AND o_c_id = ? AND o_id = (SELECT MAX(o_id) FROM orders WHERE o_w_id = ? AND o_d_id = ? AND o_c_id = ?);
explain (costs off) execute s1 using 1, 2, 3, 4, 5, 6;

prepare s1 as SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_delivery_d FROM order_line WHERE ol_w_id = ? AND ol_d_id = ? AND ol_o_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT COALESCE(MIN(no_o_id),0) FROM new_orders WHERE no_d_id = ? AND no_w_id = ?;
explain (costs off) execute s1 using 1, 2;

prepare s1 as DELETE FROM new_orders WHERE no_o_id = ? AND no_d_id = ? AND no_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT o_c_id FROM orders WHERE o_id = ? AND o_d_id = ? AND o_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as UPDATE orders SET o_carrier_id = ? WHERE o_id = ? AND o_d_id = ? AND o_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3, 4;

prepare s1 as UPDATE order_line SET ol_delivery_d = ? WHERE ol_o_id = ? AND ol_d_id = ? AND ol_w_id = ?;
explain (costs off) execute s1 using '2024-10-10 10:10:10', 2, 3, 4;

prepare s1 as SELECT SUM(ol_amount) FROM order_line WHERE ol_o_id = ? AND ol_d_id = ? AND ol_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as UPDATE customer SET c_balance = c_balance + ? , c_delivery_cnt = c_delivery_cnt + 1 WHERE c_id = ? AND c_d_id = ? AND c_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3, 4;

prepare s1 as SELECT d_next_o_id FROM district WHERE d_id = ? AND d_w_id = ?;
explain (costs off) execute s1 using 1, 2;

prepare s1 as SELECT DISTINCT ol_i_id FROM order_line WHERE ol_w_id = ? AND ol_d_id = ? AND ol_o_id < ? AND ol_o_id >= (? - 20);
explain (costs off) execute s1 using 1, 2, 3, 4;

prepare s1 as SELECT count(*) FROM stock WHERE s_w_id = ? AND s_i_id = ? AND s_quantity < ?;
explain (costs off) execute s1 using 1, 2, 3;



-- with  foreign key and FOREIGN_KEY_CHECKS
CREATE INDEX idx_customer ON customer (c_w_id,c_d_id,c_last,c_first);
CREATE INDEX idx_orders ON orders (o_w_id,o_d_id,o_c_id,o_id);
CREATE INDEX fkey_stock_2 ON stock (s_i_id);
CREATE INDEX fkey_order_line_2 ON order_line (ol_supply_w_id,ol_i_id);

ALTER TABLE district  ADD CONSTRAINT fkey_district_1 FOREIGN KEY(d_w_id) REFERENCES warehouse(w_id);
ALTER TABLE customer ADD CONSTRAINT fkey_customer_1 FOREIGN KEY(c_w_id,c_d_id) REFERENCES district(d_w_id,d_id);
ALTER TABLE history  ADD CONSTRAINT fkey_history_1 FOREIGN KEY(h_c_w_id,h_c_d_id,h_c_id) REFERENCES customer(c_w_id,c_d_id,c_id);
ALTER TABLE history  ADD CONSTRAINT fkey_history_2 FOREIGN KEY(h_w_id,h_d_id) REFERENCES district(d_w_id,d_id);
ALTER TABLE new_orders ADD CONSTRAINT fkey_new_orders_1 FOREIGN KEY(no_w_id,no_d_id,no_o_id) REFERENCES orders(o_w_id,o_d_id,o_id);
ALTER TABLE orders ADD CONSTRAINT fkey_orders_1 FOREIGN KEY(o_w_id,o_d_id,o_c_id) REFERENCES customer(c_w_id,c_d_id,c_id);
ALTER TABLE order_line ADD CONSTRAINT fkey_order_line_1 FOREIGN KEY(ol_w_id,ol_d_id,ol_o_id) REFERENCES orders(o_w_id,o_d_id,o_id);
ALTER TABLE order_line ADD CONSTRAINT fkey_order_line_2 FOREIGN KEY(ol_supply_w_id,ol_i_id) REFERENCES stock(s_w_id,s_i_id);
ALTER TABLE stock ADD CONSTRAINT fkey_stock_1 FOREIGN KEY(s_w_id) REFERENCES warehouse(w_id);
ALTER TABLE stock ADD CONSTRAINT fkey_stock_2 FOREIGN KEY(s_i_id) REFERENCES item(i_id);


set FOREIGN_KEY_CHECKS = 0;
set enable_beta_opfusion = on;
set opfusion_debug_mode = 'log';

prepare s1 as SELECT c_discount, c_last, c_credit, w_tax FROM customer, warehouse WHERE w_id = ? AND c_w_id = w_id AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3;


prepare s1 as SELECT d_next_o_id, d_tax FROM district WHERE d_id = ? AND d_w_id = ? FOR UPDATE;
explain (costs off) execute s1 using 1, 2;

prepare s1 as UPDATE district SET d_next_o_id = ? + 1 WHERE d_id = ? AND d_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d, o_ol_cnt, o_all_local) VALUES(?, ?, ?, ?, ?, ?, ?);
explain (costs off) execute s1 using 1, 2, 3, 4, '2024-10-10 10:10:10', 6, 7;


prepare s1 as INSERT INTO new_orders (no_o_id, no_d_id, no_w_id) VALUES (?,?,?);
explain (costs off) execute s1 using 1, 2, 3;


prepare s1 as SELECT i_price, i_name, i_data FROM item WHERE i_id = ?;
explain (costs off) execute s1 using 1;

prepare s1 as SELECT s_quantity, s_data, s_dist_01, s_dist_02, s_dist_03, s_dist_04, s_dist_05, s_dist_06, s_dist_07, s_dist_08, s_dist_09, s_dist_10 FROM stock WHERE s_i_id = ? AND s_w_id = ? FOR UPDATE;
explain (costs off) execute s1 using 1, 2;

prepare s1 as UPDATE stock SET s_quantity = ? WHERE s_i_id = ? AND s_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_dist_info) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);
explain (costs off) execute s1 using 1, 2, 3, 4, 5, 6, 7, 8, 9;

prepare s1 as UPDATE warehouse SET w_ytd = w_ytd + ? WHERE w_id = ?;
explain (costs off) execute s1 using 1, 2;

prepare s1 as SELECT w_street_1, w_street_2, w_city, w_state, w_zip, w_name FROM warehouse WHERE w_id = ?;
explain (costs off) execute s1 using 1;

prepare s1 as UPDATE district SET d_ytd = d_ytd + ? WHERE d_w_id = ? AND d_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT d_street_1, d_street_2, d_city, d_state, d_zip, d_name FROM district WHERE d_w_id = ? AND d_id = ?;
explain (costs off) execute s1 using 1, 2;

prepare s1 as SELECT count(c_id) FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_id FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ? ORDER BY c_id;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city, c_state, c_zip, c_phone, c_credit, c_credit_lim, c_discount, c_balance, c_since FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ? FOR UPDATE;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_data FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as UPDATE customer SET c_balance = ?, c_data = ? WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3, 4, 5;

prepare s1 as UPDATE customer SET c_balance = ? WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3, 4;

prepare s1 as INSERT INTO history(h_c_d_id, h_c_w_id, h_c_id, h_d_id, h_w_id, h_date, h_amount, h_data) VALUES(?, ?, ?, ?, ?, ?, ?, ?);
explain (costs off) execute s1 using 1, 2, 3, 4, 5, '2024-10-10 10:10:10', 7, 8;

prepare s1 as SELECT count(c_id) FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ? ORDER BY c_first;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT o_id, o_entry_d, COALESCE(o_carrier_id,0) FROM orders WHERE o_w_id = ? AND o_d_id = ? AND o_c_id = ? AND o_id = (SELECT MAX(o_id) FROM orders WHERE o_w_id = ? AND o_d_id = ? AND o_c_id = ?);
explain (costs off) execute s1 using 1, 2, 3, 4, 5, 6;

prepare s1 as SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_delivery_d FROM order_line WHERE ol_w_id = ? AND ol_d_id = ? AND ol_o_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT COALESCE(MIN(no_o_id),0) FROM new_orders WHERE no_d_id = ? AND no_w_id = ?;
explain (costs off) execute s1 using 1, 2;

prepare s1 as DELETE FROM new_orders WHERE no_o_id = ? AND no_d_id = ? AND no_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as SELECT o_c_id FROM orders WHERE o_id = ? AND o_d_id = ? AND o_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as UPDATE orders SET o_carrier_id = ? WHERE o_id = ? AND o_d_id = ? AND o_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3, 4;

prepare s1 as UPDATE order_line SET ol_delivery_d = ? WHERE ol_o_id = ? AND ol_d_id = ? AND ol_w_id = ?;
explain (costs off) execute s1 using '2024-10-10 10:10:10', 2, 3, 4;

prepare s1 as SELECT SUM(ol_amount) FROM order_line WHERE ol_o_id = ? AND ol_d_id = ? AND ol_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3;

prepare s1 as UPDATE customer SET c_balance = c_balance + ? , c_delivery_cnt = c_delivery_cnt + 1 WHERE c_id = ? AND c_d_id = ? AND c_w_id = ?;
explain (costs off) execute s1 using 1, 2, 3, 4;

prepare s1 as SELECT d_next_o_id FROM district WHERE d_id = ? AND d_w_id = ?;
explain (costs off) execute s1 using 1, 2;

prepare s1 as SELECT DISTINCT ol_i_id FROM order_line WHERE ol_w_id = ? AND ol_d_id = ? AND ol_o_id < ? AND ol_o_id >= (? - 20);
explain (costs off) execute s1 using 1, 2, 3, 4;

prepare s1 as SELECT count(*) FROM stock WHERE s_w_id = ? AND s_i_id = ? AND s_quantity < ?;
explain (costs off) execute s1 using 1, 2, 3;


drop table history;
drop table order_line;
drop table new_orders;
drop table orders;
drop table customer;  
drop table district; 
drop table stock;
drop table item;
drop table warehouse;

drop schema tpcc_plan cascade;
reset current_schema;

