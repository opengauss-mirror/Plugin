drop schema if exists select_into_var cascade;
create schema select_into_var;
set current_schema to 'select_into_var';

set enable_set_variable_b_format = on;

create table test(a int,b varchar(100), c varchar(100), d varchar(100));

INSERT INTO test (a, b, c, d) VALUES ('100', 'Hello World', 'Tom', 'Cat');
INSERT INTO test (a, b, c, d) VALUES ('200', 'Good Morning', 'Jack', 'Dog');
INSERT INTO test (a, b, c, d) VALUES ('300', 'Happy New Year', 'Jenny', 'Monkey');


begin
select a,b into @num,@str from test;
RAISE INFO '成功获取数据: num=%, str=%', @num, @str;
end;
/


declare
num int;
str1 varchar(100);
begin
select a,b,c,d into num,@str,str1,@str2 from test;
RAISE INFO '成功获取数据: num=%, str=%, str1=%, str2=%', num, @str, str1,@str2;
end;
/



declare
num int;
str varchar(100);
begin
select a,b,c,d into num, str, @str1, @str2 from test;
RAISE INFO '成功获取数据: num=%, str=%, str1=%, str2=%', num, str, @str1, @str2;
end;
/


declare
str1 varchar(100);
str2 varchar(100);
begin
select a,b,c,d into @num, @str, str1, str2 from test;
RAISE INFO '成功获取数据: num=%, str=%, str1=%, str2=%', @num, @str, str1, str2;
end;
/

set b_format_behavior_compat_options = '';

declare
  num int;
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into num, @str, @str1, @str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', num, @str, @str1, @str2;
  end loop;
  close c_test;
end;
/


declare
  str varchar(100);
  str2 varchar(100);
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into @num, str, @str1, str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', @num, str, @str1, str2;
  end loop;
  close c_test;
end;
/


declare
  num int;
  str1 varchar(100);
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into num, @str, str1, @str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', num, @str, str1, @str2;
  end loop;
  close c_test;
end;
/


declare
  num int;
  str varchar(100);
  str1 varchar(100);
  str2 varchar(100);
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into num, str, str1, str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', num, str, str1, str2;
  end loop;
  close c_test;
end;
/


declare
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into @num, @str, @str1, @str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', @num, @str, @str1, @str2;
  end loop;
  close c_test;
end;
/


set b_format_behavior_compat_options = 'fetch';
declare
  num int;
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into num, @str, @str1, @str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', num, @str, @str1, @str2;
  end loop;
  close c_test;
end;
/


declare
  str varchar(100);
  str2 varchar(100);
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into @num, str, @str1, str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', @num, str, @str1, str2;
  end loop;
  close c_test;
end;
/


declare
  num int;
  str1 varchar(100);
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into num, @str, str1, @str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', num, @str, str1, @str2;
  end loop;
  close c_test;
end;
/


declare
  num int;
  str varchar(100);
  str1 varchar(100);
  str2 varchar(100);
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into num, str, str1, str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', num, str, str1, str2;
  end loop;
  close c_test;
end;
/


declare
  cursor c_test is select a,b,c,d from test;
begin
  open c_test;
  loop
    fetch c_test into @num, @str, @str1, @str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str2=%, str2=%', @num, @str, @str1, @str2;
  end loop;
  close c_test;
end;
/


create table test2(a int,b varchar(100), c varchar(100), d varchar(100));

INSERT INTO test2 (a, b, c, d) VALUES ('100', 'Hello World', 'Tom', 'Cat');
INSERT INTO test2 (a, b, c, d) VALUES ('200', NULL, NULL, NULL);

declare
  num int;
  cursor c_test is select a, b,c,d from test2;
begin
  open c_test;
  loop
    fetch c_test into num, @str, @str1, @str2;
    exit when c_test%notfound;
    RAISE INFO '获取到数据: num=%, str=%, str1=%, str2=%', num, @str, @str1, @str2;
  end loop;
  close c_test;
end;
/


create table test3(a int,b varchar(100));
INSERT INTO test3 (a, b) VALUES ('100', 'Hello World');

begin
select a,b into @num,@str from test3;
RAISE INFO '成功获取数据: num=%, str=%', @num, @str;
end;
/

declare
str varchar(100);
begin
select a,b into str,@num from test3;
RAISE INFO '成功获取数据: num=%, str=%', @num, str;
end;
/

drop table test;
drop table test2;
drop table test3;

drop schema select_into_var cascade;
reset current_schema;
