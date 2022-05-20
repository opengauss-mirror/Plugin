drop database if exists mysqllock;
create database mysqllock dbcompatibility 'b';
\c mysqllock
create extension dolphin;
select get_lock('База данных');
select get_lock('Полиция');
select get_lock('Воплощение света');
select get_lock('Облепиховый сок');
select get_lock('Прилагатель души дракона');
select lockname from get_all_locks() order by lockname;

select is_free_lock('База данных');
select is_free_lock('Полиция');
select is_free_lock('Воплощение света');
select is_free_lock('Облепиховый сок');
select is_free_lock('Прилагатель души дракона');

select release_lock('База данных');
select release_lock('Полиция');
select release_lock('Воплощение света');
select release_lock('Облепиховый сок');
select release_lock('Прилагатель души дракона');
\c postgres
drop database if exists mysqllock;
