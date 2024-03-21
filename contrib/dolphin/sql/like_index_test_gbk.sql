set enable_indexscan = off;
--GBK
set dolphin.b_compatibility_mode to off;
drop database if exists test_gbk;
create database test_gbk dbcompatibility='B' encoding='gbk' LC_CTYPE='zh_CN.gbk' LC_COLLATE='zh_CN.gbk';
\c test_gbk

-- set client_encoding to 'utf8';
--GBK_BIN
--char
create table test_gbk_table_bin_char(c1 char(10))charset GBK collate gbk_bin;
create index on test_gbk_table_bin_char(c1);
insert into test_gbk_table_bin_char values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_table_bin_char where c1 like 'sde%';
select * from test_gbk_table_bin_char where c1 like 'sde%';
insert into test_gbk_table_bin_char values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_table_bin_char(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_table_bin_char where c1 like 'Ğ»%';
select * from test_gbk_table_bin_char where c1 like 'Ğ»%';

--varchar with length without prefix key
create table test_gbk_table_bin_varchar_length_nprefix(c1 varchar(10))charset GBK collate gbk_bin;
create index on test_gbk_table_bin_varchar_length_nprefix(c1);
insert into test_gbk_table_bin_varchar_length_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_table_bin_varchar_length_nprefix where c1 like 'sde%';
select * from test_gbk_table_bin_varchar_length_nprefix where c1 like 'sde%';
insert into test_gbk_table_bin_varchar_length_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_table_bin_varchar_length_nprefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_table_bin_varchar_length_nprefix where c1 like 'Ğ»%';
select * from test_gbk_table_bin_varchar_length_nprefix where c1 like 'Ğ»%';

--varchar without length without prefix key
create table test_gbk_table_bin_varchar_nlength_nprefix(c1 varchar)charset GBK collate gbk_bin;
create index on test_gbk_table_bin_varchar_nlength_nprefix(c1);
insert into test_gbk_table_bin_varchar_nlength_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_table_bin_varchar_nlength_nprefix where c1 like 'sde%';
select * from test_gbk_table_bin_varchar_nlength_nprefix where c1 like 'sde%';
insert into test_gbk_table_bin_varchar_nlength_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_table_bin_varchar_nlength_nprefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_table_bin_varchar_nlength_nprefix where c1 like 'Ğ»%';
select * from test_gbk_table_bin_varchar_nlength_nprefix where c1 like 'Ğ»%';

--varchar without length with prefix key
create table test_gbk_table_bin_varchar_nlength_prefix(c1 varchar)charset GBK collate gbk_bin;
create index on test_gbk_table_bin_varchar_nlength_prefix(c1(10));
insert into test_gbk_table_bin_varchar_nlength_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_table_bin_varchar_nlength_prefix where c1 like 'sde%';
select * from test_gbk_table_bin_varchar_nlength_prefix where c1 like 'sde%';
insert into test_gbk_table_bin_varchar_nlength_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_table_bin_varchar_nlength_prefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_table_bin_varchar_nlength_prefix where c1 like 'Ğ»%';
select * from test_gbk_table_bin_varchar_nlength_prefix where c1 like 'Ğ»%';

--varchar with length with prefix key
create table test_gbk_table_bin_varchar_length_prefix(c1 varchar(10))charset GBK collate gbk_bin;
create index on test_gbk_table_bin_varchar_length_prefix(c1(2));
insert into test_gbk_table_bin_varchar_length_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_table_bin_varchar_length_prefix where c1 like 's%';
select * from test_gbk_table_bin_varchar_length_prefix where c1 like 's%';
insert into test_gbk_table_bin_varchar_length_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_table_bin_varchar_length_prefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_table_bin_varchar_length_prefix where c1 like 'Ğ»%';
select * from test_gbk_table_bin_varchar_length_prefix where c1 like 'Ğ»%';

--text without prefix key
create table test_gbk_table_bin_text_nprefix(c1 text)charset GBK collate gbk_bin;
create index on test_gbk_table_bin_text_nprefix(c1);
insert into test_gbk_table_bin_text_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_table_bin_text_nprefix where c1 like 's%';
select * from test_gbk_table_bin_text_nprefix where c1 like 's%';
insert into test_gbk_table_bin_text_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_table_bin_text_nprefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_table_bin_text_nprefix where c1 like 'Ğ»%';
select * from test_gbk_table_bin_text_nprefix where c1 like 'Ğ»%';

--text with prefix key
create table test_gbk_table_bin_text_prefix(c1 text)charset GBK collate gbk_bin;
create index on test_gbk_table_bin_text_prefix(c1(10));
insert into test_gbk_table_bin_text_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_table_bin_text_prefix where c1 like 's%';
select * from test_gbk_table_bin_text_prefix where c1 like 's%';
insert into test_gbk_table_bin_text_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_table_bin_text_prefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_table_bin_text_prefix where c1 like 'Ğ»%';
select * from test_gbk_table_bin_text_prefix where c1 like 'Ğ»%';


--GBK_CHINESE_CI
--char
create table test_gbk_chinese_ci_char(c1 char(10))charset GBK collate gbk_chinese_ci;
create index on test_gbk_chinese_ci_char(c1);
insert into test_gbk_chinese_ci_char values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_chinese_ci_char where c1 like 'sde%';
select * from test_gbk_chinese_ci_char where c1 like 'sde%';
insert into test_gbk_chinese_ci_char values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_chinese_ci_char(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_chinese_ci_char where c1 like 'Ğ»%';
select * from test_gbk_chinese_ci_char where c1 like 'Ğ»%';

--varchar with length without prefix key
create table test_gbk_chinese_ci_varchar_length_nprefix(c1 varchar(10))charset GBK collate gbk_chinese_ci;
create index on test_gbk_chinese_ci_varchar_length_nprefix(c1);
insert into test_gbk_chinese_ci_varchar_length_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_chinese_ci_varchar_length_nprefix where c1 like 'sde%';
select * from test_gbk_chinese_ci_varchar_length_nprefix where c1 like 'sde%';
insert into test_gbk_chinese_ci_varchar_length_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_chinese_ci_varchar_length_nprefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_chinese_ci_varchar_length_nprefix where c1 like 'Ğ»%';
select * from test_gbk_chinese_ci_varchar_length_nprefix where c1 like 'Ğ»%';

--varchar without length without prefix key
create table test_gbk_chinese_ci_varchar_nlength_nprefix(c1 varchar)charset GBK collate gbk_chinese_ci;
create index on test_gbk_chinese_ci_varchar_nlength_nprefix(c1);
insert into test_gbk_chinese_ci_varchar_nlength_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_chinese_ci_varchar_nlength_nprefix where c1 like 'sde%';
select * from test_gbk_chinese_ci_varchar_nlength_nprefix where c1 like 'sde%';
insert into test_gbk_chinese_ci_varchar_nlength_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_chinese_ci_varchar_nlength_nprefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_chinese_ci_varchar_nlength_nprefix where c1 like 'Ğ»%';
select * from test_gbk_chinese_ci_varchar_nlength_nprefix where c1 like 'Ğ»%';

--varchar without length with prefix key
create table test_gbk_chinese_ci_varchar_nlength_prefix(c1 varchar)charset GBK collate gbk_chinese_ci;
create index on test_gbk_chinese_ci_varchar_nlength_prefix(c1(10));
insert into test_gbk_chinese_ci_varchar_nlength_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_chinese_ci_varchar_nlength_prefix where c1 like 'sde%';
select * from test_gbk_chinese_ci_varchar_nlength_prefix where c1 like 'sde%';
insert into test_gbk_chinese_ci_varchar_nlength_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_chinese_ci_varchar_nlength_prefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_chinese_ci_varchar_nlength_prefix where c1 like 'Ğ»%';
select * from test_gbk_chinese_ci_varchar_nlength_prefix where c1 like 'Ğ»%';

--varchar with length with prefix key
create table test_gbk_chinese_ci_varchar_length_prefix(c1 varchar(10))charset GBK collate gbk_chinese_ci;
create index on test_gbk_chinese_ci_varchar_length_prefix(c1(2));
insert into test_gbk_chinese_ci_varchar_length_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_chinese_ci_varchar_length_prefix where c1 like 's%';
select * from test_gbk_chinese_ci_varchar_length_prefix where c1 like 's%';
insert into test_gbk_chinese_ci_varchar_length_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_chinese_ci_varchar_length_prefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_chinese_ci_varchar_length_prefix where c1 like 'Ğ»%';
select * from test_gbk_chinese_ci_varchar_length_prefix where c1 like 'Ğ»%';

--text without prefix key
create table test_gbk_chinese_ci_text_nprefix(c1 text)charset GBK collate gbk_chinese_ci;
create index on test_gbk_chinese_ci_text_nprefix(c1);
insert into test_gbk_chinese_ci_text_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_chinese_ci_text_nprefix where c1 like 's%';
select * from test_gbk_chinese_ci_text_nprefix where c1 like 's%';
insert into test_gbk_chinese_ci_text_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_chinese_ci_text_nprefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_chinese_ci_text_nprefix where c1 like 'Ğ»%';
select * from test_gbk_chinese_ci_text_nprefix where c1 like 'Ğ»%';

--text with prefix key
create table test_gbk_chinese_ci_text_prefix(c1 text)charset GBK collate gbk_chinese_ci;
create index on test_gbk_chinese_ci_text_prefix(c1(10));
insert into test_gbk_chinese_ci_text_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gbk_table_bin_text_prefix where c1 like 's%';
select * from test_gbk_chinese_ci_text_prefix where c1 like 's%';
insert into test_gbk_chinese_ci_text_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gbk_chinese_ci_text_nprefix(c1) select convert_from('\xD0BBFEFE', 'GBK');
explain (costs off) select * from test_gbk_chinese_ci_text_prefix where c1 like 'Ğ»%';
select * from test_gbk_chinese_ci_text_prefix where c1 like 'Ğ»%';
reset client_encoding;