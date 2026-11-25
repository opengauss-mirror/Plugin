create schema test_binary_like;
set search_path = test_binary_like;

set enable_indexscan = off;

create table test_binary(a binary(10));
create index on test_binary(a);
insert into test_binary values('aaa');
explain(costs off)select * from test_binary where a like 'a%';
select * from test_binary where a like 'a%';

create table test_varbinary(a varbinary(10));
create index on test_varbinary(a);
insert into test_varbinary values('aaa');
explain(costs off) select * from test_varbinary where a like 'a%';
select * from test_varbinary where a like 'a%';

set b_format_behavior_compat_options='enable_multi_charset';
CREATE TABLE t_diff_charset_columns(
    futf8_bin varchar(16) CHARSET utf8mb4 COLLATE 'utf8mb4_bin',
    futf8_uni varchar(16) CHARSET utf8mb4 COLLATE 'utf8mb4_unicode_ci',
    futf8_gen varchar(16) CHARSET utf8mb4 COLLATE 'utf8mb4_general_ci',
    fgbk_bin varchar(16) CHARSET gbk COLLATE 'gbk_bin',
    fgbk_chi varchar(16) CHARSET gbk COLLATE 'gbk_chinese_ci',
    fgb18030_bin varchar(16) CHARSET gb18030 COLLATE 'gb18030_bin',
    fgb18030_chi varchar(16) CHARSET gb18030 COLLATE 'gb18030_chinese_ci',
    fbin varbinary(16),
    fblob blob
);
INSERT INTO t_diff_charset_columns(futf8_bin, futf8_uni, futf8_gen, fgbk_bin, fgbk_chi, fgb18030_bin, fgb18030_chi, fbin, fblob)
    VALUES('高斯DB', '高斯db', '高斯db', '高斯DB', '高斯db', '高斯DB', '高斯db', '高斯DB', 'E9AB98E696AF');

set enable_seqscan=off;
CREATE INDEX idx_prefixkey_fgbk_bin on t_diff_charset_columns (fgbk_bin(2) text_pattern_ops);
EXPLAIN (costs false)
SELECT /*+ indexscan(t_diff_charset_columns idx_prefixkey_fgbk_bin) */ fgbk_bin FROM t_diff_charset_columns WHERE fgbk_bin LIKE '高斯\%';
EXPLAIN (costs false)
SELECT /*+ indexscan(t_diff_charset_columns idx_prefixkey_fgbk_bin) */ fgbk_bin FROM t_diff_charset_columns WHERE fgbk_bin LIKE '高斯@%' ESCAPE '@';

reset b_format_behavior_compat_options;
reset enable_seqscan;
reset search_path;
drop schema test_binary_like cascade;