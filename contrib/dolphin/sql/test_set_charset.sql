drop database if exists db_charset;
create database db_charset dbcompatibility 'b';
\c db_charset
show client_encoding;
set charset gbk;
show client_encoding;
set charset default;
show client_encoding;
set character set 'gbk';
show client_encoding;
set character set default;
show client_encoding;
\c postgres
drop database if exists db_charset;
