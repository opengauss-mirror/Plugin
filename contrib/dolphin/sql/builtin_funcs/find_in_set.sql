drop database if exists db_find_in_set;
create database db_find_in_set dbcompatibility 'B';
\c db_find_in_set
select find_in_set(1,'a,1,c');
select find_in_set(1,'true,1,c');
select find_in_set(1.2,'a,1.2,c');
select find_in_set('bac','a,bac,c');
select find_in_set('bac');
select find_in_set('1','a,1,c');
select find_in_set('1.1','a,1.1,c');
select find_in_set('qw','a,1.2,c,qwee,1212,1.1,12,qw');
select find_in_set('qw','a,1.2,null,c,qwee,1212,1.1,12,qw');
select find_in_set(1,'true,1,a,1.2,null,c,qwee,1212,1.1,12,qw');
select find_in_set('测试','a,测试,1.2,c,qwee,1212,1.1');
select find_in_set('1','1,1.2,c,qwee,1212,1.1,12,qw');
select find_in_set(1,'1,1.2,c,qwee,1212,1.1,12,qw');
select find_in_set(1,'1.1,1.2,c,qwee,1212,1.1,12,1');
select find_in_set(1.1,'a,1.2,c,qwee,1212,1.1');
\c postgres
drop database if exists db_find_in_set;
