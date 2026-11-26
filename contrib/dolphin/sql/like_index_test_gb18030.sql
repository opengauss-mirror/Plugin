set enable_indexscan = off;
--GB18030
set dolphin.b_compatibility_mode to off;
drop database if exists test_gb18030;
Create database test_gb18030 dbcompatibility='b' ENCODING=36 LC_COLLATE='zh_CN.gb18030' LC_CTYPE='zh_CN.gb18030';
\c test_gb18030

--GB18030_CHINESE_CI
--char
create table test_gb18030_chinese_ci_char(c1 char(10))charset gb18030 collate gb18030_chinese_ci;
create index on test_gb18030_chinese_ci_char(c1);
insert into test_gb18030_chinese_ci_char values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_chinese_ci_char where c1 like 'sde%';
select * from test_gb18030_chinese_ci_char where c1 like 'sde%';
insert into test_gb18030_chinese_ci_char values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_chinese_ci_char(c1) select convert_from('\xD0BBFE40FE40', 'GB18030');
explain (costs off) select * from test_gb18030_chinese_ci_char where c1 like 'Ğ»%';
select * from test_gb18030_chinese_ci_char where c1 like 'Ğ»%';

--varchar with length without prefix key
create table test_gb18030_chinese_ci_varchar_length_nprefix(c1 varchar(10))charset gb18030 collate gb18030_chinese_ci;
create index on test_gb18030_chinese_ci_varchar_length_nprefix(c1);
insert into test_gb18030_chinese_ci_varchar_length_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_chinese_ci_varchar_length_nprefix where c1 like 'sde%';
select * from test_gb18030_chinese_ci_varchar_length_nprefix where c1 like 'sde%';
insert into test_gb18030_chinese_ci_varchar_length_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_chinese_ci_varchar_length_nprefix(c1) select convert_from('\xD0BBFE40FE40', 'GB18030');
explain (costs off) select * from test_gb18030_chinese_ci_varchar_length_nprefix where c1 like 'Ğ»%';
select * from test_gb18030_chinese_ci_varchar_length_nprefix where c1 like 'Ğ»%';

--varchar without length without prefix key
create table test_gb18030_chinese_ci_varchar_nlength_nprefix(c1 varchar)charset gb18030 collate gb18030_chinese_ci;
create index on test_gb18030_chinese_ci_varchar_nlength_nprefix(c1);
insert into test_gb18030_chinese_ci_varchar_nlength_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_chinese_ci_varchar_nlength_nprefix where c1 like 'sde%';
select * from test_gb18030_chinese_ci_varchar_nlength_nprefix where c1 like 'sde%';
insert into test_gb18030_chinese_ci_varchar_nlength_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_chinese_ci_varchar_nlength_nprefix(c1) select convert_from('\xD0BBFE40FE40', 'GB18030');
explain (costs off) select * from test_gb18030_chinese_ci_varchar_nlength_nprefix where c1 like 'Ğ»%';
select * from test_gb18030_chinese_ci_varchar_nlength_nprefix where c1 like 'Ğ»%';

--varchar without length with prefix key
create table test_gb18030_chinese_ci_varchar_nlength_prefix(c1 varchar)charset gb18030 collate gb18030_chinese_ci;
create index on test_gb18030_chinese_ci_varchar_nlength_prefix(c1(10));
insert into test_gb18030_chinese_ci_varchar_nlength_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_chinese_ci_varchar_nlength_prefix where c1 like 'sde%';
select * from test_gb18030_chinese_ci_varchar_nlength_prefix where c1 like 'sde%';
insert into test_gb18030_chinese_ci_varchar_nlength_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_chinese_ci_varchar_nlength_prefix(c1) select convert_from('\xD0BBFE40FE40', 'GB18030');
explain (costs off) select * from test_gb18030_chinese_ci_varchar_nlength_prefix where c1 like 'Ğ»%';
select * from test_gb18030_chinese_ci_varchar_nlength_prefix where c1 like 'Ğ»%';

--varchar with length with prefix key
create table test_gb18030_chinese_ci_varchar_length_prefix(c1 varchar(10))charset gb18030 collate gb18030_chinese_ci;
create index on test_gb18030_chinese_ci_varchar_length_prefix(c1(2));
insert into test_gb18030_chinese_ci_varchar_length_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_chinese_ci_varchar_length_prefix where c1 like 's%';
select * from test_gb18030_chinese_ci_varchar_length_prefix where c1 like 's%';
insert into test_gb18030_chinese_ci_varchar_length_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_chinese_ci_varchar_length_prefix(c1) select convert_from('\xD0BBFE40FE40', 'GB18030');
explain (costs off) select * from test_gb18030_chinese_ci_varchar_length_prefix where c1 like 'Ğ»%';
select * from test_gb18030_chinese_ci_varchar_length_prefix where c1 like 'Ğ»%';

--text without prefix key
create table test_gb18030_chinese_ci_text_nprefix(c1 text)charset gb18030 collate gb18030_chinese_ci;
create index on test_gb18030_chinese_ci_text_nprefix(c1);
insert into test_gb18030_chinese_ci_text_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_chinese_ci_text_nprefix where c1 like 's%';
select * from test_gb18030_chinese_ci_text_nprefix where c1 like 's%';
insert into test_gb18030_chinese_ci_text_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_chinese_ci_text_nprefix(c1) select convert_from('\xD0BBFE40FE40', 'GB18030');
explain (costs off) select * from test_gb18030_chinese_ci_text_nprefix where c1 like 'Ğ»%';
select * from test_gb18030_chinese_ci_text_nprefix where c1 like 'Ğ»%';

--text with prefix key
create table test_gb18030_chinese_ci_text_prefix(c1 text)charset gb18030 collate gb18030_chinese_ci;
create index on test_gb18030_chinese_ci_text_prefix(c1(10));
insert into test_gb18030_chinese_ci_text_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_chinese_ci_text_prefix where c1 like 's%';
select * from test_gb18030_chinese_ci_text_prefix where c1 like 's%';
insert into test_gb18030_chinese_ci_text_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_chinese_ci_text_prefix(c1) select convert_from('\xD0BBFE40FE40', 'GB18030');
explain (costs off) select * from test_gb18030_chinese_ci_text_prefix where c1 like 'Ğ»%';
select * from test_gb18030_chinese_ci_text_prefix where c1 like 'Ğ»%';

--GB18030_BIN
--char
create table test_gb18030_bin_char(c1 char(10))charset gb18030 collate gb18030_bin;
create index on test_gb18030_bin_char(c1);
insert into test_gb18030_bin_char values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_bin_char where c1 like 'sde%';
select * from test_gb18030_bin_char where c1 like 'sde%';
insert into test_gb18030_bin_char values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_bin_char(c1) select convert_from('\xD0BBFEFEFEFE', 'GB18030');
explain (costs off) select * from test_gb18030_bin_char where c1 like 'Ğ»%';
select * from test_gb18030_bin_char where c1 like 'Ğ»%';

--varchar with length without prefix key
create table test_gb18030_bin_varchar_length_nprefix(c1 varchar(10))charset gb18030 collate gb18030_bin;
create index on test_gb18030_bin_varchar_length_nprefix(c1);
insert into test_gb18030_bin_varchar_length_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_bin_varchar_length_nprefix where c1 like 'sde%';
select * from test_gb18030_bin_varchar_length_nprefix where c1 like 'sde%';
insert into test_gb18030_bin_varchar_length_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_bin_varchar_length_nprefix(c1) select convert_from('\xD0BBFEFEFEFE', 'GB18030');
explain (costs off) select * from test_gb18030_bin_varchar_length_nprefix where c1 like 'Ğ»%';
select * from test_gb18030_bin_varchar_length_nprefix where c1 like 'Ğ»%';

--varchar without length without prefix key
create table test_gb18030_bin_varchar_nlength_nprefix(c1 varchar)charset gb18030 collate gb18030_bin;
create index on test_gb18030_bin_varchar_nlength_nprefix(c1);
insert into test_gb18030_bin_varchar_nlength_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_bin_varchar_nlength_nprefix where c1 like 'sde%';
select * from test_gb18030_bin_varchar_nlength_nprefix where c1 like 'sde%';
insert into test_gb18030_bin_varchar_nlength_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_bin_varchar_nlength_nprefix(c1) select convert_from('\xD0BBFEFEFEFE', 'GB18030');
explain (costs off) select * from test_gb18030_bin_varchar_nlength_nprefix where c1 like 'Ğ»%';
select * from test_gb18030_bin_varchar_nlength_nprefix where c1 like 'Ğ»%';

--varchar without length with prefix key
create table test_gb18030_bin_varchar_nlength_prefix(c1 varchar)charset gb18030 collate gb18030_bin;
create index on test_gb18030_bin_varchar_nlength_prefix(c1(10));
insert into test_gb18030_bin_varchar_nlength_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_bin_varchar_nlength_prefix where c1 like 'sde%';
select * from test_gb18030_bin_varchar_nlength_prefix where c1 like 'sde%';
insert into test_gb18030_bin_varchar_nlength_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_bin_varchar_nlength_prefix(c1) select convert_from('\xD0BBFEFEFEFE', 'GB18030');
explain (costs off) select * from test_gb18030_bin_varchar_nlength_prefix where c1 like 'Ğ»%';
select * from test_gb18030_bin_varchar_nlength_prefix where c1 like 'Ğ»%';

--varchar with length with prefix key
create table test_gb18030_bin_varchar_length_prefix(c1 varchar(10))charset gb18030 collate gb18030_bin;
create index on test_gb18030_bin_varchar_length_prefix(c1(2));
insert into test_gb18030_bin_varchar_length_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_bin_varchar_length_prefix where c1 like 's%';
select * from test_gb18030_bin_varchar_length_prefix where c1 like 's%';
insert into test_gb18030_bin_varchar_length_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_bin_varchar_length_prefix(c1) select convert_from('\xD0BBFEFEFEFE', 'GB18030');
explain (costs off) select * from test_gb18030_bin_varchar_length_prefix where c1 like 'Ğ»%';
select * from test_gb18030_bin_varchar_length_prefix where c1 like 'Ğ»%';

--text without prefix key
create table test_gb18030_bin_text_nprefix(c1 text)charset gb18030 collate gb18030_bin;
create index on test_gb18030_bin_text_nprefix(c1);
insert into test_gb18030_bin_text_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_bin_text_nprefix where c1 like 's%';
select * from test_gb18030_bin_text_nprefix where c1 like 's%';
insert into test_gb18030_bin_text_nprefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_bin_text_nprefix(c1) select convert_from('\xD0BBFEFEFEFE', 'GB18030');
explain (costs off) select * from test_gb18030_bin_text_nprefix where c1 like 'Ğ»%';
select * from test_gb18030_bin_text_nprefix where c1 like 'Ğ»%';

--text with prefix key
create table test_gb18030_bin_text_prefix(c1 text)charset gb18030 collate gb18030_bin;
create index on test_gb18030_bin_text_prefix(c1(10));
insert into test_gb18030_bin_text_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_gb18030_bin_text_prefix where c1 like 's%';
select * from test_gb18030_bin_text_prefix where c1 like 's%';
insert into test_gb18030_bin_text_prefix values('Ğ»Ğ»ÄãÑ½'), ('ĞÂÄê¿ìÀÖ');
insert into test_gb18030_bin_text_prefix(c1) select convert_from('\xD0BBFEFEFEFE', 'GB18030');
explain (costs off) select * from test_gb18030_bin_text_prefix where c1 like 'Ğ»%';
select * from test_gb18030_bin_text_prefix where c1 like 'Ğ»%';