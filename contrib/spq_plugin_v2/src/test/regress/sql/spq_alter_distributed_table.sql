CREATE TABLE users_table (user_id int, time timestamp, value_1 int, value_2 int, value_3 float, value_4 bigint);
SELECT create_distributed_table('users_table', 'user_id');

CREATE TABLE events_table (user_id int, time timestamp, event_type int, value_2 int, value_3 float, value_4 bigint);
SELECT create_distributed_table('events_table', 'user_id');

CREATE INDEX is_index1 ON users_table(user_id);
CREATE INDEX is_index2 ON events_table(user_id);
CREATE INDEX is_index3 ON users_table(value_1);
CREATE INDEX is_index4 ON events_table(event_type);
CREATE INDEX is_index5 ON users_table(value_2);
CREATE INDEX is_index6 ON events_table(value_2);

SELECT count(distinct(colocationid)) FROM pg_dist_partition;
SELECT * FROM alter_distributed_table('users_table','user_id', shard_count:=1);
SELECT * FROM alter_distributed_table('events_table','user_id', shard_count:=1);
SELECT count(distinct(colocationid)) FROM pg_dist_partition;
SELECT * FROM alter_distributed_table('users_table','user_id', shard_count:=2);

DROP TABLE users_table;
DROP TABLE events_table;
