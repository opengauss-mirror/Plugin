drop database if exists test_connection_id;
create database test_connection_id dbcompatibility 'b';
\c test_connection_id

-- 测试返回连接的ID
SELECT CONNECTION_ID();

\c postgres
drop database test_connection_id;