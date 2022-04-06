drop database if exists mysql_test;
create database mysql_test dbcompatibility 'b';
\c mysql_test
create extension b_sql_plugin;
select 'bbbbb' regexp '^([bc])\1*$' as t, 'bbbbb' not regexp '^([bc])\1*$' as t2, 'bbbbb' rlike '^([bc])\1*$' as t;
select 'ccc' regexp '^([bc])\1*$' as t, 'ccc' not regexp '^([bc])\1*$' as t2, 'ccc' rlike '^([bc])\1*$' as t;
select 'xxx' regexp '^([bc])\1*$' as f, 'xxx' not regexp '^([bc])\1*$' as f2, 'xxx' rlike '^([bc])\1*$' as f;
select 'bbc' regexp '^([bc])\1*$' as f, 'bbc' not regexp '^([bc])\1*$' as f2, 'bbc' rlike '^([bc])\1*$' as f;
select 'b' regexp '^([bc])\1*$' as t, 'b' not regexp '^([bc])\1*$' as t2, 'b' rlike '^([bc])\1*$' as t;
select 'abc abc abc' regexp '^(\w+)( \1)+$' as t, 'abc abc abc' not regexp '^(\w+)( \1)+$' as t, 'abc abc abc' rlike '^(\w+)( \1)+$' as t;
select 'abc abc abc' regexp '^(.+)( \1)+$' as t, 'abc abc abc' not regexp '^(.+)( \1)+$' as t, 'abc abc abc' rlike '^(.+)( \1)+$' as t;
select 'a' regexp '($|^)*', 'a' not regexp '($|^)*', 'a' rlike '($|^)*';
select 'a' regexp '(^)+^', 'a' not regexp '(^)+^', 'a' rlike '(^)+^';
select 'a' regexp '$($$)+', 'a' not regexp '$($$)+', 'a' rlike '$($$)+';
select 'a' regexp '($^)+', 'a' not regexp '($^)+', 'a' rlike '($^)+';
select 'a' regexp '(^$)*', 'a' not regexp '(^$)*', 'a' rlike '(^$)*';
select 'aa bb cc' regexp '(^(?!aa))+', 'aa bb cc' not regexp '(^(?!aa))+', 'aa bb cc' rlike '(^(?!aa))+';
select 'aa x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'aa x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'aa x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'bb x' regexp '(^(?!aa)(?!bb)(?!cc))+', 'bb x' not regexp '(^(?!aa)(?!bb)(?!cc))+', 'bb x' rlike '(^(?!aa)(?!bb)(?!cc))+';
select 'x' regexp 'abcd(\m)+xyz', 'x' not regexp 'abcd(\m)+xyz', 'x' rlike 'abcd(\m)+xyz';
select 'x' regexp 'a^(^)bcd*xy(((((($a+|)+|)+|)+$|)+|)+|)^$', 'x' not regexp 'a^(^)bcd*xy(((((($a+|)+|)+|)+$|)+|)+|)^$', 'x' rlike 'a^(^)bcd*xy(((((($a+|)+|)+|)+$|)+|)+|)^$';
select 'a' regexp '((((((a)*)*)*)*)*)*', 'a' not regexp '((((((a)*)*)*)*)*)*', 'a' rlike '((((((a)*)*)*)*)*)*';
select 'a' regexp '((((((a+|)+|)+|)+|)+|)+|)', 'a' not regexp '((((((a+|)+|)+|)+|)+|)+|)', 'a' rlike '((((((a+|)+|)+|)+|)+|)+|)';
select 'a' regexp '\\x7fffffff', 'a' not regexp '\\x7fffffff', 'a' rlike '\\x7fffffff';
select 'a' regexp '$()|^\1', 'a' not regexp '$()|^\1', 'a' rlike '$()|^\1';
select 'a' regexp '.. ()|\1', 'a' not regexp '.. ()|\1', 'a' rlike '.. ()|\1';
select 'a' regexp '()*\1', 'a' not regexp '()*\1', 'a' rlike '()*\1';--()*\1匹配一切字符
select 'a' regexp '()+\1', 'a' not regexp '()+\1', 'a' rlike '()+\1';

select '-12.3abc' regexp null;
select '-12.3abc' regexp -100.1;
select '-12.3abc' regexp 0;
select '-12.3abc' regexp 5;
select '-12.3abc' regexp 158.3;
select '-12.3abc' regexp -8.222e4;
select '-12.3abc' regexp true;
select '-12.3abc' regexp false;
select '-12.3abc' regexp 'null';
select '-12.3abc' regexp '12';
 select 'xxxyyy' regexp '^xx';
 select 'xxxyyy' regexp 'x*';
 select 'fonfo' regexp '^fo$';
 select '-12.3abc' regexp '中文';

select '-12.3abc' not regexp null;
select '-12.3abc' not regexp -100.1;
select '-12.3abc' not regexp 0;
select '-12.3abc' not regexp 5;
select '-12.3abc' not regexp 158.3;
select '-12.3abc' not regexp -8.222e4;
select '-12.3abc' not regexp true;
select '-12.3abc' not regexp false;
select '-12.3abc' not regexp 'null';
select 'fonfo' not REGEXP '^fo$';
select '-12.3abc' not regexp '中文';

select '-12.3abc' rlike null;
select '-12.3abc' rlike -100.1;
select '-12.3abc' rlike 0;
select '-12.3abc' rlike 5;
select '-12.3abc' rlike 158.3;
select '-12.3abc' rlike -8.222e4;
select '-12.3abc' rlike '中文';
select '-12.3abc' rlike true;
select '-12.3abc' rlike false;
select '-12.3abc' rlike 'null';

\c postgres
drop database if exists mysql_test;