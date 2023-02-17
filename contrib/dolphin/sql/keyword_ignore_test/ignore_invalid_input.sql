-- test for insert/update ignore.
create schema sql_ignore_invalid_input_test;
set current_schema to 'sql_ignore_invalid_input_test';
set timezone to 'PRC';

-- type: tinyint
drop table if exists t_tinyint;
create table t_tinyint(c tinyint);
insert ignore into t_tinyint values('12a34');
insert ignore into t_tinyint values('12555a34');
insert ignore into t_tinyint values('-12555a34');
insert ignore into t_tinyint values('aaa123a34');
insert ignore into t_tinyint values('abcde');
insert ignore into t_tinyint values('');
select * from t_tinyint;
update ignore t_tinyint set c = '12a34';

-- type: tinyint unsigned
drop table if exists t_tinyint_unsigned;
create table t_tinyint_unsigned(c tinyint unsigned);
insert ignore into t_tinyint_unsigned values('12a34');
insert ignore into t_tinyint_unsigned values('12555a34');
insert ignore into t_tinyint_unsigned values('-12555a34');
insert ignore into t_tinyint_unsigned values('aaa123a34');
insert ignore into t_tinyint_unsigned values('abcde');
insert ignore into t_tinyint_unsigned values('');
select * from t_tinyint_unsigned;
update ignore t_tinyint_unsigned set c = '12a34';

-- type: smallint
drop table if exists t_smallint;
create table t_smallint(c smallint);
insert ignore into t_smallint values ('12a34');
insert ignore into t_smallint values ('123333333333333a34');
insert ignore into t_smallint values ('-123333333333333a34');
insert ignore into t_smallint values ('aaa1234a5');
insert ignore into t_smallint values ('abcde');
insert ignore into t_smallint values ('');
select * from t_smallint;
update ignore t_smallint set c = '12a34';

-- type: smallint unsigned
drop table if exists t_smallint_unsigned;
create table t_smallint_unsigned(c smallint unsigned);
insert ignore into t_smallint_unsigned values ('12a34');
insert ignore into t_smallint_unsigned values ('123333333333333a34');
insert ignore into t_smallint_unsigned values ('-123333333333333a34');
insert ignore into t_smallint_unsigned values ('aaa1234a5');
insert ignore into t_smallint_unsigned values ('abcde');
insert ignore into t_smallint_unsigned values ('');
select * from t_smallint_unsigned;
update ignore t_smallint_unsigned set c = '12a34';

-- type: int4
drop table if exists t_int;
create table t_int(c int);
insert ignore into t_int values ('12a34');
insert ignore into t_int values ('123333333333333333333333333a34');
insert ignore into t_int values ('-123333333333333333333333333a34');
insert ignore into t_int values ('aaa123a45');
insert ignore into t_int values ('abcde');
insert ignore into t_int values ('');
select * from t_int;
update ignore t_int set c = '12a34';

-- type: int4 unsigned
drop table if exists t_int_unsigned;
create table t_int_unsigned(c int unsigned);
insert ignore into t_int_unsigned values ('12a34');
insert ignore into t_int_unsigned values ('123333333333333333333333333a34');
insert ignore into t_int_unsigned values ('-123333333333333333333333333a34');
insert ignore into t_int_unsigned values ('aaa123a45');
insert ignore into t_int_unsigned values ('abcde');
insert ignore into t_int_unsigned values ('');
select * from t_int_unsigned;
update ignore t_int_unsigned set c = '12a34';

-- type: bigint
drop table if exists t_bigint;
create table t_bigint(c bigint);
insert ignore into t_bigint values ('12a34');
insert ignore into t_bigint values ('123333333333333333333333333333333333333333333333333333333333333333333333333333333333333a34');
insert ignore into t_bigint values ('-123333333333333333333333333333333333333333333333333333333333333333333333333333333333333a34');
insert ignore into t_bigint values ('aaa123a45');
insert ignore into t_bigint values ('abcde');
insert ignore into t_bigint values ('');
select * from t_bigint;
update ignore t_bigint set c = '12a34';

-- type: bigint unsigned
drop table if exists t_bigint_unsigned;
create table t_bigint_unsigned(c bigint unsigned);
insert ignore into t_bigint_unsigned values ('12a34');
insert ignore into t_bigint_unsigned values ('123333333333333333333333333333333333333333333333333333333333333333333333333333333333333a34');
insert ignore into t_bigint_unsigned values ('-123333333333333333333333333333333333333333333333333333333333333333333333333333333333333a34');
insert ignore into t_bigint_unsigned values ('aaa123a45');
insert ignore into t_bigint_unsigned values ('abcde');
insert ignore into t_bigint_unsigned values ('');
select * from t_bigint_unsigned;
update ignore t_bigint_unsigned set c = '12a34';

-- type: float4
drop table if exists t_float4;
create table t_float4(c float4);
insert ignore into t_float4 values ('12.123a34');
insert ignore into t_float4 values ('123333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333.123a34');
insert ignore into t_float4 values ('-123333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333.123a34');
insert ignore into t_float4 values ('aaa123.12a45');
insert ignore into t_float4 values ('abcde');
insert ignore into t_float4 values ('');
select * from t_float4;
update ignore t_float4 set c = '12a34';

-- type: float8
drop table if exists t_float8;
create table t_float8(c float8);
insert ignore into t_float8 values ('12.123a34');
insert ignore into t_float8 values ('3333333331892038097432987589432759843769348605436304758493758943758943758943759843756983760945860948605948765487689547893475893475918920380974329875894327598437693486054363047584937589437589437589437598437569837609458609486059487654876895478934758934759627346378267863475863875648365843734895749837589437589473988.18920380974329875894327598437693486054363047584937589437589437589437598437569837609458609486059487654876895478934758934759189203809743298758943275984376934860543630475849375894375894375894375984375698376094586094860594876548768954789347589347593874894375984aaa34');
insert ignore into t_float8 values ('-3333333331892038097432987589432759843769348605436304758493758943758943758943759843756983760945860948605948765487689547893475893475918920380974329875894327598437693486054363047584937589437589437589437598437569837609458609486059487654876895478934758934759627346378267863475863875648365843734895749837589437589473988.18920380974329875894327598437693486054363047584937589437589437589437598437569837609458609486059487654876895478934758934759189203809743298758943275984376934860543630475849375894375894375894375984375698376094586094860594876548768954789347589347593874894375984aaa34');
insert ignore into t_float8 values ('aaa123.12a45');
insert ignore into t_float8 values ('abcde');
insert ignore into t_float8 values ('');
select * from t_float8;
update ignore t_float8 set c = '12a34';

-- type: numeric
create table t_numeric(c numeric(1000, 100));
insert ignore into t_numeric values ('12.123a34');
insert ignore into t_numeric values ('333331892038097432987589432759843769348605436304758493758943758943758943759843756983760945860948605948765487689547893475893475918920380974329875894327598437693486054363047584937589437589437589437598437569837609458609486059487654876895478934758934759627346378267863475863875648365843734895749837589437589473988.18920380974329875894327598437693486054363047584937589437589437589437598437569837609458609486059487654876895478934758934759189203809743298758943275984376934860543630475849375894375894375894375984375698376094586094860594876548768954789347589347593874894375984aaa34');
insert ignore into t_numeric values ('-333331892038097432987589432759843769348605436304758493758943758943758943759843756983760945860948605948765487689547893475893475918920380974329875894327598437693486054363047584937589437589437589437598437569837609458609486059487654876895478934758934759627346378267863475863875648365843734895749837589437589473988.18920380974329875894327598437693486054363047584937589437589437589437598437569837609458609486059487654876895478934758934759189203809743298758943275984376934860543630475849375894375894375894375984375698376094586094860594876548768954789347589347593874894375984aaa34');
insert ignore into t_numeric values ('aaa123.12a45');
insert ignore into t_numeric values ('abcde');
insert ignore into t_numeric values ('');
select * from t_numeric;
update ignore t_numeric set c = '12a34';

-- type: date
create table t_date(c date);
insert ignore into t_date values('12a34');
insert ignore into t_date values('123a34');
insert ignore into t_date values('12aaaaaaa34');
insert ignore into t_date values('aaaaaaa12aaaaaaa34');
insert ignore into t_date values('abcde');
insert ignore into t_date values('');
select * from t_date;
update ignore t_date set c = '12a34';

-- type: time
create table t_time(c time);
insert ignore into t_time values('12a34');
insert ignore into t_time values('123a34');
insert ignore into t_time values('12aaaaaaa34');
insert ignore into t_time values('aaaaaaa12aaaaaaa34');
insert ignore into t_time values('abcde');
insert ignore into t_time values('');
select * from t_time;
update ignore t_time set c = '12a34';

-- type: timestamp
create table t_timestamp(c timestamp);
insert  ignore into t_timestamp values('12a34');
insert  ignore into t_timestamp values('abcde');
insert  ignore into t_timestamp values('aaaaaa12a34');
insert  ignore into t_timestamp values('');
select * from t_timestamp;
update ignore t_timestamp set c = '12a34';

-- type: timestamptz
create table t_timestamptz(c timestamptz);
insert  ignore into t_timestamptz values('12a34');
insert  ignore into t_timestamptz values('abcde');
insert  ignore into t_timestamptz values('aaaaaa12a34');
insert  ignore into t_timestamptz values('');
select * from t_timestamptz;
update ignore t_timestamptz set c = '12a34';

-- type: timetz
create table t_timetz(c timetz);
insert ignore into t_timetz values('12a34');
insert ignore into t_timetz values('123a34');
insert ignore into t_timetz values('12aaaaaaa34');
insert ignore into t_timetz values('aaaaaaa12aaaaaaa34');
insert ignore into t_timetz values('abcde');
insert ignore into t_timetz values('');
select * from t_timetz;
update ignore t_timetz set c = '12a34';

-- type: interval
create table t_interval(c interval);
insert ignore into t_interval values('12a34');
insert ignore into t_interval values('123a34');
insert ignore into t_interval values('12aaaaaaa34');
insert ignore into t_interval values('aaaaaaa12aaaaaaa34');
insert ignore into t_interval values('abcde');
insert ignore into t_interval values('');
select * from t_interval;
update ignore t_interval set c = '12a34';

-- type: tinterval
create table t_tinterval(c tinterval);
insert ignore into t_tinterval values('12a34');
insert ignore into t_tinterval values('123a34');
insert ignore into t_tinterval values('12aaaaaaa34');
insert ignore into t_tinterval values('aaaaaaa12aaaaaaa34');
insert ignore into t_tinterval values('abcde');
insert ignore into t_tinterval values('');
select * from t_tinterval;
update ignore t_tinterval set c = '12a34';

-- type: smalldatetime
create table t_smalldatetime(c smalldatetime);
insert ignore into t_smalldatetime values('12a34');
insert ignore into t_smalldatetime values('123a34');
insert ignore into t_smalldatetime values('12aaaaaaa34');
insert ignore into t_smalldatetime values('aaaaaaa12aaaaaaa34');
insert ignore into t_smalldatetime values('abcde');
insert ignore into t_smalldatetime values('');
select * from t_smalldatetime;
update ignore t_smalldatetime set c = '12a34';

-- type: uuid
create table t_uuid(c uuid);
insert ignore into t_uuid values('12a34');
insert ignore into t_uuid values('');
update ignore t_uuid set c = '12a34';
select * from t_uuid;

-- type: point
create table t_point(c point);
insert ignore into t_point values('12a34');
insert ignore into t_point values('');
select * from t_point;
update ignore t_point set c = '12a34';
select * from t_point;

-- type: path
create table t_path(c path);
insert ignore into t_path values('12a34');
insert ignore into t_path values('');
select * from t_path;
update ignore t_path set c = '12a34';
select * from t_path;

-- type: polygon
create table t_polygon(c polygon);
insert ignore into t_polygon values('12a34');
insert ignore into t_polygon values('');
select * from t_polygon;
update ignore t_polygon set c = '12a34';
select * from t_polygon;

-- type: circle
create table t_circle(c circle);
insert ignore into t_circle values('12a34');
insert ignore into t_circle values('');
select * from t_circle;
update ignore t_circle set c = '12a34';
select * from t_circle;

-- type: lseg
create table t_lseg(c lseg);
insert ignore into t_lseg values('12a34');
insert ignore into t_lseg values('');
select * from t_lseg;
update ignore t_lseg set c = '12a34';
select * from t_lseg;

-- type: box
create table t_box(c box);
insert ignore into t_box values('12a34');
insert ignore into t_box values('');
select * from t_box;
update ignore t_box set c = '12a34';
select * from t_box;

-- type: json
create table t_json(c json);
insert ignore into t_json values('12a34');
insert ignore into t_json values('');
insert ignore into t_json values('');
insert ignore into t_json values('');
insert ignore into t_json values('');
insert ignore into t_json values('');
insert ignore into t_json values('');
select * from t_json;
update ignore t_json set c = '12a34';
select * from t_json;

-- type: jsonb
create table t_jsonb(c jsonb);
insert ignore into t_jsonb values('12a34');
insert ignore into t_jsonb values('');
insert ignore into t_jsonb values('');
insert ignore into t_jsonb values('');
insert ignore into t_jsonb values('');
insert ignore into t_jsonb values('');
insert ignore into t_jsonb values('');
select * from t_jsonb;
update ignore t_jsonb set c = '12a34';
select * from t_jsonb;

-- type: bit
create table t_bit(c bit);
insert ignore into t_bit values('12a34');
insert ignore into t_bit values('');
select * from t_bit;
update ignore t_bit set c = '12a34';
select * from t_bit;

-- test net types
reset sql_ignore_strategy;
show sql_ignore_strategy;
create table net(
    c1 cidr not null,
    c2 inet  not null,
    c3 macaddr not null
);
insert ignore into net values('', '', '');
insert ignore into net values(null, null, null);
insert ignore into net values(1::int, 1.1::float, 'sdfdf'::time);
set sql_ignore_strategy='overwrite_null';
insert ignore into net values(null, null, null);
insert ignore into net values(1::int, 1.1::float, 'sdfdf'::time);
select * from net;
reset sql_ignore_strategy;

-- test range types
create table ran(
    c1 numrange not null,
    c2 int8range not null,
    c3 int4range not null,
    c4 tsrange not null,
    c5 tstzrange not null,
    c6 daterange not null
);
insert ignore into ran values('', '', '', '', '', '');
insert ignore into ran values(null, null, null, null, null, null);
set sql_ignore_strategy='overwrite_null';
insert ignore into ran values(null, null, null, null, null, null);
select * from ran;
reset sql_ignore_strategy;

-- test hash & tsvector types
create table hashvec(
    c1 hash16 not null,
    c2 hash32 not null,
    c3 tsvector not null
);
insert ignore into hashvec values('', '', '');
insert ignore into hashvec values(null, null, null);
insert ignore into hashvec values(1::int, 1.1::float, 'sdfdf'::time);
set sql_ignore_strategy='overwrite_null';
insert ignore into hashvec values(null, null, null);
insert ignore into hashvec values(1::int, 1.1::float, 'sdfdf'::time);
select * from hashvec;
reset sql_ignore_strategy;

-- test bit varying
create table varbit(
    c1 BIT VARYING(5) not null
);
insert ignore into varbit values('');
insert ignore into varbit values(null);
insert ignore into varbit values('sdfdf'::time);
set sql_ignore_strategy='overwrite_null';
insert ignore into varbit values(null);
insert ignore into varbit values(1::int);
insert ignore into varbit values('sdfdf'::time);
select * from varbit;
reset sql_ignore_strategy;

drop table net, ran, hashvec, varbit;

-- restore context
reset timezone;
show timezone;
drop schema sql_ignore_invalid_input_test cascade;
reset current_schema;