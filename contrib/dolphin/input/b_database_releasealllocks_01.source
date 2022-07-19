drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

select lockname from get_all_locks();
select release_all_locks();
select get_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',1);
select get_lock('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',1);

select get_lock('获取一个锁2022',1);

select get_lock('1234567890',1);
select get_lock(1,1);
select get_lock(1234567890,1);

select get_lock('`',1);
select get_lock('~',1);
select get_lock('!',1);
select get_lock('@',1);
select get_lock('#',1);
select get_lock('$',1);
select get_lock('%',1);
select get_lock('^',1);
select get_lock('&',1);
select get_lock('*',1);
select get_lock('(',1);
select get_lock(')',1);
select get_lock('[',1);
select get_lock(']',1);
select get_lock('{',1);
select get_lock('}',1);
select get_lock('-',1);
select get_lock('_',1);
select get_lock('=',1);
select get_lock('+',1);
select get_lock('\',1);
select get_lock('|',1);
select get_lock(';',1);
select get_lock(':',1);
select get_lock('""""',1);
select get_lock(',',1);
select get_lock('.',1);
select get_lock('/',1);
select get_lock('<',1);
select get_lock('>',1);
select get_lock('?',1);

select get_lock('·',1);
select get_lock('~',1);
select get_lock('！',1);
select get_lock('@',1);
select get_lock('#',1);
select get_lock('￥',1);
select get_lock('%',1);
select get_lock('……',1);
select get_lock('&',1);
select get_lock('*',1);
select get_lock('（',1);
select get_lock('）',1);
select get_lock('-',1);
select get_lock('——',1);
select get_lock('=',1);
select get_lock('+',1);
select get_lock('【',1);
select get_lock('】',1);
select get_lock('{',1);
select get_lock('}',1);
select get_lock('、',1);
select get_lock('|',1);
select get_lock('；',1);
select get_lock('：',1);
select get_lock('‘',1);
select get_lock('“',1);
select get_lock('”',1);
select get_lock('，',1);
select get_lock('《',1);
select get_lock('。',1);
select get_lock('》',1);
select get_lock('/',1);
select get_lock('？',1);

select get_lock('a                                                             a');
select get_lock('a                                                              a');
select get_lock('1                                                              1');
select get_lock('a                                                              1');
select get_lock(' ');

select release_all_locks();
\c postgres
drop database if exists b_databaselock;
