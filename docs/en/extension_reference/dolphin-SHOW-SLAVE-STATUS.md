# SHOW SLAVE STATUS

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:24:47.438Z pushedAt=2026-09-12T05:36:32.642Z -->

## Function Description<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s86b6c9741c7741d3976c5e358e8d5486"></a>

Displays WAL (xlog) log synchronization status information, such as the log position sent by the sender and the log position received by the receiver.

## NOTE<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_sdd2da7fe44624eb99ee77013ff96c6bd"></a>

- This command is effective only when executed on the primary node.
- The displayed result is consistent with that of `select * from pg_stat_get_wal_senders();`.

## Syntax Format<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_se242be9719f44731b261539dbd42d7b9"></a>

```
SHOW {SLAVE | REPLICA} STATUS [FOR CHANNEL channel]
```

## Parameter Description<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s06dfa4f09bfd4e0d9826a80e6a91b0a6"></a>

- **{SLAVE | REPLICA}**

       Both are equivalent.

- **[FOR CHANNEL channel]**

       Filtering by the channel field is supported. The filter value can be all or part of the channel field, such as a port number or IP address, for fuzzy query.

## Field Description<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_s06dfa4f09bfd4e0d9826a80e6a91b0a6"></a>

- **pid**

       Thread number of the walsender.

- **sender_pid**

       Lightweight thread number corresponding to the PID of the walsender.

- **local_role**

       Primary node type.

- **peer_role**

       Standby node type.

- **peer_state**

       Standby node status.

- **state**

       Walsender status.

- **catchup_start**

       Catchup start time.

- **catchup_end**

       Catchup end time.

- **sender_sent_location**

      Location sent by the primary node.

- **sender_write_location**

       Disk location on the primary node.

- **sender_flush_location**

       Flush disk location on the primary node.

- **sender_replay_location**

       Redo location on the primary node.

- **receiver_received_location**

       Receive location on the standby node.

- **receiver_write_location**

       Write-to-disk location on the standby node.

- **receiver_flush_location**

       The disk location where the standby node flushes data.

- **receiver_replay_location**

       The disk location where the standby node performs redo.

- **sync_percent**

       Synchronization percentage.

- **sync_state**

       Synchronization state.

- **sync_priority**

       Priority of synchronous replication.

- **sync_most_available**

       Maximum availability mode settings.

- **channel**

       Walsender channel information.

## Example<a name="zh-cn_topic_0283137542_zh-cn_topic_0237122167_zh-cn_topic_0059778902_sfff14489321642278317cf06cd89810d"></a>

```
openGauss=# show slave status;
-[ RECORD 1 ]--------------+-------------------------------------------
pid                        | 139766487054080
sender_pid                 | 129111
local_role                 | Primary
peer_role                  | Standby
peer_state                 | Normal
state                      | Streaming
catchup_start              | 2023-05-17 17:23:58.6677+08
catchup_end                | 2023-05-17 17:23:58.736808+08
sender_sent_location       | 0/44A27C8
sender_write_location      | 0/44A27C8
sender_flush_location      | 0/44A27C8
sender_replay_location     | 0/44A27C8
receiver_received_location | 0/44A27C8
receiver_write_location    | 0/44A27C8
receiver_flush_location    | 0/44A27C8
receiver_replay_location   | 0/44A27C8
sync_percent               | 100%
sync_state                 | Sync
sync_priority              | 1
sync_most_available        | Off
channel                    | 10.248.192.249:5433-->10.248.192.249:46984

openGauss=# show slave status for channel '249';
-[ RECORD 1 ]--------------+-------------------------------------------
pid                        | 139766487054080
sender_pid                 | 129111
local_role                 | Primary
peer_role                  | Standby
peer_state                 | Normal
state                      | Streaming
catchup_start              | 2023-05-17 17:23:58.6677+08
catchup_end                | 2023-05-17 17:23:58.736808+08
sender_sent_location       | 0/44A27C8
sender_write_location      | 0/44A27C8
sender_flush_location      | 0/44A27C8
sender_replay_location     | 0/44A27C8
receiver_received_location | 0/44A27C8
receiver_write_location    | 0/44A27C8
receiver_flush_location    | 0/44A27C8
receiver_replay_location   | 0/44A27C8
sync_percent               | 100%
sync_state                 | Sync
sync_priority              | 1
sync_most_available        | Off
channel                    | 10.248.192.249:5433-->10.248.192.249:46984
```