create schema test_connection_id;
set current_schema to 'test_connection_id';

-- 测试返回连接的ID
SELECT CONNECTION_ID();

drop schema test_connection_id cascade;
reset current_schema;