create schema compress_modify_flag;
set current_schema = compress_modify_flag;
create table t_grammar0006_01
(id NUMBER CONSTRAINT t_partition_list_0016_01_id_nn NOT NULL,
use_filename VARCHAR2(20),
filename VARCHAR2(255),
text VARCHAR2(2000))
engine ='分区表' compress tablespace pg_default nocompress
with(fillfactor=10,orientation=row,storage_type=astore)
partition by list(id)
(partition p1 values(10,50),
partition p2 values(20,100));
drop table t_grammar0006_01;
reset current_schema;
drop schema compress_modify_flag;
