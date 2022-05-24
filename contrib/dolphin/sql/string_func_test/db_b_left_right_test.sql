SELECT left('abcdefg', 3);
SELECT left('abcdefg', -3);
SELECT right('abcdefg', 3);
SELECT right('abcdefg', -3);

drop database if exists test;
create database test dbcompatibility 'B';
\c test
create extension dolphin;

SELECT left('abcdefg', 3);
SELECT left('abcdefg', -3), left('abcdefg', -3) is null ;
SELECT right('abcdefg', 3);
SELECT right('abcdefg', -3), right('abcdefg', -3) is null;

SELECT left('', 3), left('', 3) is null;
SELECT left('', -3), left('', -3) is null;
SELECT right('', 3), right('', 3) is null;
SELECT right('', -3), right('', -3) is null;

SELECT left('abcdefg', 0), left('abcdefg', 0) is null;
SELECT right('abcdefg', 0), right('abcdefg', 0) is null;


SELECT left('abcdefg'::bytea, 3);
SELECT left('abcdefg'::bytea, -3), left('abcdefg'::bytea, -3) is null ;
SELECT right('abcdefg'::bytea, 3);
SELECT right('abcdefg'::bytea, -3), right('abcdefg'::bytea, -3) is null;

SELECT left('你是我的好兄弟', 3);
SELECT left('你是我的好兄弟', -3), left('你是我的好兄弟', -3) is null ;
SELECT right('你是我的好兄弟', 3);
SELECT right('你是我的好兄弟', -3), right('你是我的好兄弟', -3) is null;

SELECT left('你是我的好兄弟', 0), left('你是我的好兄弟', 0) is null;
SELECT right('你是我的好兄弟', 0), right('你是我的好兄弟', 0) is null;

SELECT left(''::bytea, 3), left(''::bytea, 3) is null;
SELECT left(''::bytea, -3), left(''::bytea, -3) is null;
SELECT right(''::bytea, 3), right(''::bytea, 3) is null;
SELECT right(''::bytea, -3), right(''::bytea, -3) is null;

SELECT left('abcdefg', 0), left('abcdefg', 0) is null;
SELECT right('abcdefg', 0), right('abcdefg', 0) is null;

SELECT left(null, 3), left(null, 3) is null;
SELECT right(null, 3), right(null, 3) is null;

\c postgres
drop database test;