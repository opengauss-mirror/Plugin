--UTF8
create database test_utf8 dbcompatibility='B' encoding='utf8';
\c test_utf8

--utf8_bin
--char
create table test_utf8_bin_char(c1 char(10))charset utf8 collate utf8_bin;
create index on test_utf8_bin_char(c1);
insert into test_utf8_bin_char values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_bin_char where c1 like 'sde%';
select * from test_utf8_bin_char where c1 like 'sde%';
insert into test_utf8_bin_char values('谢谢你呀'), ('新年快乐');
insert into test_utf8_bin_char(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_bin_char where c1 like '谢%';
select * from test_utf8_bin_char where c1 like '谢%';

--varchar with length without prefix key
create table test_utf8_bin_varchar_length_nprefix(c1 varchar(10))charset utf8 collate utf8_bin;
create index on test_utf8_bin_varchar_length_nprefix(c1);
insert into test_utf8_bin_varchar_length_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_bin_varchar_length_nprefix where c1 like 'sde%';
select * from test_utf8_bin_varchar_length_nprefix where c1 like 'sde%';
insert into test_utf8_bin_varchar_length_nprefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_bin_varchar_length_nprefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_bin_varchar_length_nprefix where c1 like '谢%';
select * from test_utf8_bin_varchar_length_nprefix where c1 like '谢%';

--varchar without length without prefix key
create table test_utf8_bin_varchar_nlength_nprefix(c1 varchar)charset utf8 collate utf8_bin;
create index on test_utf8_bin_varchar_nlength_nprefix(c1);
insert into test_utf8_bin_varchar_nlength_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_bin_varchar_nlength_nprefix where c1 like 'sde%';
select * from test_utf8_bin_varchar_nlength_nprefix where c1 like 'sde%';
insert into test_utf8_bin_varchar_nlength_nprefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_bin_varchar_nlength_nprefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_bin_varchar_nlength_nprefix where c1 like '谢%';
select * from test_utf8_bin_varchar_nlength_nprefix where c1 like '谢%';

--varchar without length with prefix key
create table test_utf8_bin_varchar_nlength_prefix(c1 varchar)charset utf8 collate utf8_bin;
create index on test_utf8_bin_varchar_nlength_prefix(c1(10));
insert into test_utf8_bin_varchar_nlength_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_bin_varchar_nlength_prefix where c1 like 'sde%';
select * from test_utf8_bin_varchar_nlength_prefix where c1 like 'sde%';
insert into test_utf8_bin_varchar_nlength_prefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_bin_varchar_nlength_prefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_bin_varchar_nlength_prefix where c1 like '谢%';
select * from test_utf8_bin_varchar_nlength_prefix where c1 like '谢%';

--varchar with length with prefix key
create table test_utf8_bin_varchar_length_prefix(c1 varchar(10))charset utf8 collate utf8_bin;
create index on test_utf8_bin_varchar_length_prefix(c1(2));
insert into test_utf8_bin_varchar_length_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_bin_varchar_length_prefix where c1 like 's%';
select * from test_utf8_bin_varchar_length_prefix where c1 like 's%';
insert into test_utf8_bin_varchar_length_prefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_bin_varchar_length_prefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_bin_varchar_length_prefix where c1 like '谢%';
select * from test_utf8_bin_varchar_length_prefix where c1 like '谢%';

--text without prefix key
create table test_utf8_bin_text_nprefix(c1 text)charset utf8 collate utf8_bin;
create index on test_utf8_bin_text_nprefix(c1);
insert into test_utf8_bin_text_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_bin_text_nprefix where c1 like 's%';
select * from test_utf8_bin_text_nprefix where c1 like 's%';
insert into test_utf8_bin_text_nprefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_bin_text_nprefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_bin_text_nprefix where c1 like '谢%';
select * from test_utf8_bin_text_nprefix where c1 like '谢%';

--text with prefix key
create table test_utf8_bin_text_prefix(c1 text)charset utf8 collate utf8_bin;
create index on test_utf8_bin_text_prefix(c1(10));
insert into test_utf8_bin_text_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_bin_text_prefix where c1 like 's%';
select * from test_utf8_bin_text_prefix where c1 like 's%';
insert into test_utf8_bin_text_prefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_bin_text_prefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_bin_text_prefix where c1 like '谢%';
select * from test_utf8_bin_text_prefix where c1 like '谢%';

--utf8_general_ci
--char
create table test_utf8_general_ci_char(c1 char(10))charset utf8 collate utf8_general_ci;
create index on test_utf8_general_ci_char(c1);
insert into test_utf8_general_ci_char values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_general_ci_char where c1 like 'sde%';
select * from test_utf8_general_ci_char where c1 like 'sde%';
insert into test_utf8_general_ci_char values('谢谢你呀'), ('新年快乐');
insert into test_utf8_general_ci_char(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_general_ci_char where c1 like '谢%';
select * from test_utf8_general_ci_char where c1 like '谢%';

--varchar with length without prefix key
create table test_utf8_general_ci_varchar_length_nprefix(c1 varchar(10))charset utf8 collate utf8_general_ci;
create index on test_utf8_general_ci_varchar_length_nprefix(c1);
insert into test_utf8_general_ci_varchar_length_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_general_ci_varchar_length_nprefix where c1 like 'sde%';
select * from test_utf8_general_ci_varchar_length_nprefix where c1 like 'sde%';
insert into test_utf8_general_ci_varchar_length_nprefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_general_ci_varchar_length_nprefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_general_ci_varchar_length_nprefix where c1 like '谢%';
select * from test_utf8_general_ci_varchar_length_nprefix where c1 like '谢%';

--varchar without length without prefix key
create table test_utf8_general_ci_varchar_nlength_nprefix(c1 varchar)charset utf8 collate utf8_general_ci;
create index on test_utf8_general_ci_varchar_nlength_nprefix(c1);
insert into test_utf8_general_ci_varchar_nlength_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_general_ci_varchar_nlength_nprefix where c1 like 'sde%';
select * from test_utf8_general_ci_varchar_nlength_nprefix where c1 like 'sde%';
insert into test_utf8_general_ci_varchar_nlength_nprefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_general_ci_varchar_nlength_nprefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_general_ci_varchar_nlength_nprefix where c1 like '谢%';
select * from test_utf8_general_ci_varchar_nlength_nprefix where c1 like '谢%';

--varchar without length with prefix key
create table test_utf8_general_ci_varchar_nlength_prefix(c1 varchar)charset utf8 collate utf8_general_ci;
create index on test_utf8_general_ci_varchar_nlength_prefix(c1(10));
insert into test_utf8_general_ci_varchar_nlength_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_general_ci_varchar_nlength_prefix where c1 like 'sde%';
select * from test_utf8_general_ci_varchar_nlength_prefix where c1 like 'sde%';
insert into test_utf8_general_ci_varchar_nlength_prefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_general_ci_varchar_nlength_prefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_general_ci_varchar_nlength_prefix where c1 like '谢%';
select * from test_utf8_general_ci_varchar_nlength_prefix where c1 like '谢%';

--varchar with length with prefix key
create table test_utf8_general_ci_varchar_length_prefix(c1 varchar(10))charset utf8 collate utf8_general_ci;
create index on test_utf8_general_ci_varchar_length_prefix(c1(2));
insert into test_utf8_general_ci_varchar_length_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_general_ci_varchar_length_prefix where c1 like 's%';
select * from test_utf8_general_ci_varchar_length_prefix where c1 like 's%';
insert into test_utf8_general_ci_varchar_length_prefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_general_ci_varchar_length_prefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_general_ci_varchar_length_prefix where c1 like '谢%';
select * from test_utf8_general_ci_varchar_length_prefix where c1 like '谢%';

--text without prefix key
create table test_utf8_general_ci_text_nprefix(c1 text)charset utf8 collate utf8_general_ci;
create index on test_utf8_general_ci_text_nprefix(c1);
insert into test_utf8_general_ci_text_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_general_ci_text_nprefix where c1 like 's%';
select * from test_utf8_general_ci_text_nprefix where c1 like 's%';
insert into test_utf8_general_ci_text_nprefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_general_ci_text_nprefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_general_ci_text_nprefix where c1 like '谢%';
select * from test_utf8_general_ci_text_nprefix where c1 like '谢%';

--text with prefix key
create table test_utf8_general_ci_text_prefix(c1 text)charset utf8 collate utf8_general_ci;
create index on test_utf8_general_ci_text_prefix(c1(10));
insert into test_utf8_general_ci_text_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_general_ci_text_prefix where c1 like 's%';
select * from test_utf8_general_ci_text_prefix where c1 like 's%';
insert into test_utf8_general_ci_text_prefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_general_ci_text_prefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_general_ci_text_prefix where c1 like '谢%';
select * from test_utf8_general_ci_text_prefix where c1 like '谢%';

--utf8_unicode_ci
--char
create table test_utf8_unicode_ci_char(c1 char(10))charset utf8 collate utf8_unicode_ci;
create index on test_utf8_unicode_ci_char(c1);
insert into test_utf8_unicode_ci_char values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_unicode_ci_char where c1 like 'sde%';
select * from test_utf8_unicode_ci_char where c1 like 'sde%';
insert into test_utf8_unicode_ci_char values('谢谢你呀'), ('新年快乐');
insert into test_utf8_unicode_ci_char(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_unicode_ci_char where c1 like '谢%';
select * from test_utf8_unicode_ci_char where c1 like '谢%';

--varchar with length without prefix key
create table test_utf8_unicode_ci_varchar_length_nprefix(c1 varchar(10))charset utf8 collate utf8_unicode_ci;
create index on test_utf8_unicode_ci_varchar_length_nprefix(c1);
insert into test_utf8_unicode_ci_varchar_length_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_unicode_ci_varchar_length_nprefix where c1 like 'sde%';
select * from test_utf8_unicode_ci_varchar_length_nprefix where c1 like 'sde%';
insert into test_utf8_unicode_ci_varchar_length_nprefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_unicode_ci_varchar_length_nprefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_unicode_ci_varchar_length_nprefix where c1 like '谢%';
select * from test_utf8_unicode_ci_varchar_length_nprefix where c1 like '谢%';

--varchar without length without prefix key
create table test_utf8_unicode_ci_varchar_nlength_nprefix(c1 varchar)charset utf8 collate utf8_unicode_ci;
create index on test_utf8_unicode_ci_varchar_nlength_nprefix(c1);
insert into test_utf8_unicode_ci_varchar_nlength_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_unicode_ci_varchar_nlength_nprefix where c1 like 'sde%';
select * from test_utf8_unicode_ci_varchar_nlength_nprefix where c1 like 'sde%';
insert into test_utf8_unicode_ci_varchar_nlength_nprefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_unicode_ci_varchar_nlength_nprefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_unicode_ci_varchar_nlength_nprefix where c1 like '谢%';
select * from test_utf8_unicode_ci_varchar_nlength_nprefix where c1 like '谢%';

--varchar without length with prefix key
create table test_utf8_unicode_ci_varchar_nlength_prefix(c1 varchar)charset utf8 collate utf8_unicode_ci;
create index on test_utf8_unicode_ci_varchar_nlength_prefix(c1(10));
insert into test_utf8_unicode_ci_varchar_nlength_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_unicode_ci_varchar_nlength_prefix where c1 like 'sde%';
select * from test_utf8_unicode_ci_varchar_nlength_prefix where c1 like 'sde%';
insert into test_utf8_unicode_ci_varchar_nlength_prefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_unicode_ci_varchar_nlength_prefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_unicode_ci_varchar_nlength_prefix where c1 like '谢%';
select * from test_utf8_unicode_ci_varchar_nlength_prefix where c1 like '谢%';

--varchar with length with prefix key
create table test_utf8_unicode_ci_varchar_length_prefix(c1 varchar(10))charset utf8 collate utf8_unicode_ci;
create index on test_utf8_unicode_ci_varchar_length_prefix(c1(2));
insert into test_utf8_unicode_ci_varchar_length_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_unicode_ci_varchar_length_prefix where c1 like 's%';
select * from test_utf8_unicode_ci_varchar_length_prefix where c1 like 's%';
insert into test_utf8_unicode_ci_varchar_length_prefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_unicode_ci_varchar_length_prefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_unicode_ci_varchar_length_prefix where c1 like '谢%';
select * from test_utf8_unicode_ci_varchar_length_prefix where c1 like '谢%';

--text without prefix key
create table test_utf8_unicode_ci_text_nprefix(c1 text)charset utf8 collate utf8_unicode_ci;
create index on test_utf8_unicode_ci_text_nprefix(c1);
insert into test_utf8_unicode_ci_text_nprefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_unicode_ci_text_nprefix where c1 like 's%';
select * from test_utf8_unicode_ci_text_nprefix where c1 like 's%';
insert into test_utf8_unicode_ci_text_nprefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_unicode_ci_text_nprefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_unicode_ci_text_nprefix where c1 like '谢%';
select * from test_utf8_unicode_ci_text_nprefix where c1 like '谢%';

--text with prefix key
create table test_utf8_unicode_ci_text_prefix(c1 text)charset utf8 collate utf8_unicode_ci;
create index on test_utf8_unicode_ci_text_prefix(c1(10));
insert into test_utf8_unicode_ci_text_prefix values('sdeWf'), ('wertf');
explain (costs off) select * from test_utf8_unicode_ci_text_prefix where c1 like 's%';
select * from test_utf8_unicode_ci_text_prefix where c1 like 's%';
insert into test_utf8_unicode_ci_text_prefix values('谢谢你呀'), ('新年快乐');
insert into test_utf8_unicode_ci_text_prefix(c1) select convert_from('\xe8b0a2efbfbf','utf8');
explain (costs off) select * from test_utf8_unicode_ci_text_prefix where c1 like '谢%';
select * from test_utf8_unicode_ci_text_prefix where c1 like '谢%';