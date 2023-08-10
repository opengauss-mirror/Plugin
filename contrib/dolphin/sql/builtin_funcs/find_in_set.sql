create schema db_find_in_set;
set current_schema to 'db_find_in_set';
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
select find_in_set('','a,b,c,,dx');
select find_in_set('','');
select find_in_set('a','a');
select find_in_set(1,'1');
-- ignore cast when collation is xxx_ci
set b_format_behavior_compat_options = 'enable_multi_charset';
set collation_connection = 'utf8mb4_general_ci';
select find_in_set('a','A');

drop schema db_find_in_set cascade;
reset current_schema;
