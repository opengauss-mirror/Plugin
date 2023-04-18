create schema test_json_unquote;
set current_schema to 'test_json_unquote';

select json_unquote('""');
select json_unquote('"abc"');
select json_unquote('abc');
select json_unquote('"abc');
select json_unquote('"abc\t');
select json_unquote('"abc\\t');
select json_unquote('"abc\\t"');
select json_unquote('"ab\\tc"');
select json_unquote('"abc"\t');
select json_unquote('"abc"\\t');
select json_unquote('"ab"djoada"c"');
select json_unquote('a"bdw\tc');
select json_unquote('"abdmad\\tc"');
select json_unquote('abc\\t"dwaj');
select json_unquote('"abcmdw\tmdkw"');
select json_unquote('"abs""c"\t');
select json_unquote('abc\\t');
select json_unquote('abcdw\t');
select json_unquote('"ab"c');
select json_unquote('a"bc');
select json_unquote('ab\tc"');
select json_unquote('ab\\tc"');
select json_unquote('\t"abc"');
select json_unquote('\\t"abc"');
select json_unquote('a\\t"bc"');
SELECT JSON_UNQUOTE('"\\t\\u0032"');
select json_unquote('"\\u0032a"');
select json_unquote('" \\u554a kbhjfuk..."');
select json_unquote('"abcd\\t kjh \\u554 kbhjfuk"');


create table data(name VARCHAR(20) PRIMARY KEY);
insert into data (name) value(json_unquote('"sjy"'));
select name from data;
drop table data;

drop schema test_json_unquote cascade;
reset current_schema;
