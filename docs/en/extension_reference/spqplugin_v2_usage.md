# spqplugin_v2 Usage

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-13T03:20:58.119Z pushedAt=2026-09-29T02:34:56.710Z -->

## Prerequisites

1. The servers hosting a CN and several DN nodes are running properly, and openGauss can be started normally.

2. All nodes are interconnected over the network.

3. The thread pool is disabled (currently not supported).

## Deployment

### 1. openGauss Deployment

#### 1.1 CN Node

Add the following parameters to `cn node data directory/postgresql.conf`:

```shell
password_encryption_type = 1
enable_pbe_optimization = OFF
shared_preload_libraries = 'spq'
port = cn node port
```

After modifying the preceding configuration, restart the openGauss process. For example, run the following command (if openGauss is not launched, simply start it. The command is an example, and other launch methods also work.)

```shell
gs_ctl restart -D CN node data directory
```

#### 1.2 DN node

Add the following parameters to `dn node data directory/postgresql.conf`:

```shell
password_encryption_type = 1
shared_preload_libraries = 'spq'
port = dn node port
listen_addresses = 'localhost,current DN node NIC IP'
```

Add the following line to `dn node data directory/pg_hba.conf` to accept all user connections from the CN node (controlled by code logic):

```shell
# TYPE  DATABASE        USER            ADDRESS          METHOD
host    all             all             CN node IP/32  trust
```

Note that in single-machine multi-Docker deployment (the DN and CN nodes are on the same machine but in different Docker containers), the CN node IP in the `pg_hba.conf` file of the DN node must be set to the docker0 IP in the `ifconfig` command output, for example, `172.17.0.1` in the following config.

```shell
$ ifconfig
docker0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 172.17.0.1  netmask 255.255.0.0  broadcast 172.17.255.255
        inet6 fe80::42:a6ff:fe5e:19cb  prefixlen 64  scopeid 0x20<link>
        ether 02:42:a6:5e:19:cb  txqueuelen 0  (Ethernet)
        RX packets 23556  bytes 1778105 (1.6 MiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 23323  bytes 2091561 (1.9 MiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

After the preceding configurations, restart the openGauss process, for example, by running the following command (if openGauss is not launched, start it; the command is an example, and other launch methods also work):

```shell
gs_ctl restart -D current DN node data directory
```

Check whether the firewall is enabled on each DN node. If yes, allow the CN node to access the openGauss listening port of the DN nodes.

```shell
# Check whether firewalld is enabled. If the output contains Active: active (running), firewalld is running. Otherwise, allow the CN node IP to access the openGauss listening port.
systemctl status firewalld

# Allow the port for the CN node IP on the DN node.
firewall-cmd --permanent --add-rich-rule='rule family=ipv4 source address=CN node IP port port=Current DN node openGauss port protocol="tcp" accept'
firewall-cmd --reload
```

Note that the preceding configuration must be performed on each DN node.

### 2. spq Setup

#### 2.1 Loading the Plugin

Connect to the openGauss instances on the CN node and all DN nodes and run the following SQL statement to load the spq plugin. Note that this SQL statement must be executed on openGauss of all nodes.

```
create extension spq;
```

#### 2.2 Node Configuration

Connect to the CN node and add node configurations.

```sql
-- Set the current node as the CN node.
SELECT * from spq_set_coordinator_host('127.0.0.1', CN node port number);
-- Add all DN nodes.
SELECT * from spq_add_node('DN1 IP', DN1 port number);
SELECT * from spq_add_node('DN2 IP', DN2 port number);
...

--Query all nodes.
SELECT * from pg_dist_node;
SELECT * FROM spq_get_active_worker_nodes();
```

#### 2.3 Distributed Table Example

Create a local table:

```sql
CREATE TABLE IF NOT EXISTS public.pg_vector_collection (id BIGINT PRIMARY KEY, embedding vector(4));
ALTER TABLE public.pg_vector_collection ALTER COLUMN embedding SET STORAGE PLAIN;
ALTER TABLE public.pg_vector_collection SET (parallel_workers = 32);
-- Query the shard distribution of the table:
SELECT * from pg_dist_shard;
```

Convert the table to a distributed table:

```sql
SELECT create_distributed_table('pg_vector_collection', 'id', shard_count:=4);
-- Query the shard distribution of the table:
SELECT * from pg_dist_shard;
```

Insert data:

```sql
INSERT INTO public.pg_vector_collection 
VALUES(1,'[1,2,3,4]'),(2,'[1,2,3,4]'),(3,'[1,2,3,4]'),(4,'[1,2,3,4]'),(5,'[1,2,3,4]'),(6,'[1,2,3,4]'),(7,'[1,2,3,4]');
```

Create an index:

```sql
CREATE INDEX IF NOT EXISTS  "opengauss_index"  ON public. pg_vector_collection USING  "hnsw"  (embedding  "vector_cosine_ops" ) WITH ( "m" = "16", "ef_construction" = "200" );
```

Query:

```sql
SELECT * FROM public.pg_vector_collection ORDER BY embedding <=> '[1,2,3,4]' LIMIT 5;
```

Drop the table:

```sql
drop table pg_vector_collection;
```

## Function Reference

### Distributed Table Functions

- create_distributed_table(table_name, distribution_column, shard_count)

    Description: Converts a local table into a distributed table. Note that columns other than the specified distribution column cannot have PRIMARY KEY or UNIQUE constraints. For the distribution column, the collation used for comparison must be deterministic, that is, the same value must always have the same hash value.

    Parameters:
    - table_name: Name of the table to be converted into a distributed table
    - distribution_column: Name of the specified shard column
    - shard_count: Shard count, which defaults to spq.shard_count if not specified

    Return value type: N/A

    Example:

    ```sql
    -- Create a local table.
    openGauss=# create table t1(id int primary key, b varchar(10));
    -- Convert the local table to a distributed table
    openGauss=# select create_distributed_table('t1', 'id', shard_count:=4);
    ```

- alter_distributed_table(table_name, distribution_column, shard_count)

    Description: Modifies the distribution column and shard count of a distributed table.

    Parameters:
    - table_name: Table name of the distributed table
    - distribution_column: Name of the specified shard column
    - shard_count: Shard count, which defaults to spq.shard_count if not specified

    Return value type: N/A

    Example:

    ```sql
    -- Modify the shard count
    openGauss=# select alter_distributed_table('t1', shard_count:=8);
    ```

- undistribute_table(table_name)

    Description: Converts a distributed table back to a local table.

    Parameters:
    - table_name: Table name of the distributed table.

    Return value type: N/A

    Example:

    ```sql
    -- Modify the shard count.
    openGauss=# select undistribute_table('t1');
    ```

- rebalance_table_shards()

    Description: Rebalances the shards of a distributed table to balance the shard count on each DN node.

    Parameters: N/A

    Return value type: N/A

    Example:

    ```sql
    -- Add a DN node.
    select spq_add_node('127.0.01', 5432);
    -- Rebalance table shards.
    openGauss=# select rebalance_table_shards();
    ```

- spq_rebalance_start()

    Description: Works the same as rebalance_table_shards. Returns immediately after the call, and the data rebalancing task runs asynchronously in the background.

    Parameters: N/A

    Return value type: N/A

    Example:

    ```sql
    -- Add a DN node.
    select spq_add_node('127.0.01', 5432);
    -- Rebalance shards.
    openGauss=# select spq_rebalance_start();
    ```

### Node Management Functions

- spq_set_coordinator_host(host text, port integer default current_setting('port')::int)

    Description: Sets a node as the CN node. Each cluster requires only one read/write CN.

    Parameters:
    - host: IP address, domain name, or endpoint of the current CN
    - port: openGauss port number of the CN node

    Return value type: N/A

    Example:

    ```sql
    openGauss=# select spq_set_coordinator_host('127.0.01', 5432);
    ```

- spq_add_node(nodename text, nodeport integer)

    Description: Adds a DN node. The node is activated immediately and ready to use.

    Parameters:
    - nodename: IP address, domain name, or endpoint of the current DN
    - nodeport: openGauss port number of the DN node

    Return value type: N/A

    Example:

    ```sql
    openGauss=# select spq_add_node('127.0.01', 5432);
    ```   

- spq_add_inactive_node(nodename text, nodeport integer)

    Description: Adds a DN node. The node is not activated immediately, meaning that shards of distributed tables will not be placed on this node.

    Parameters:
    - nodename: IP address, domain name, or endpoint of the current DN
    - nodeport: openGauss port number of the DN node

    Return value type: N/A

    Example:

    ```sql
    openGauss=# select spq_add_inactive_node('127.0.01', 5432);
    ```

- spq_activate_node(nodename text, nodeport integer)

    Description: Activates an inactive node. Used after spq_add_inactive_node.

    Parameters:
    - nodename: IP address, domain name, or endpoint of the current DN
    - nodeport: openGauss port number of the DN node

    Return value type: N/A

    Example:

    ```sql
    openGauss=# select spq_activate_node('127.0.01', 5432);
    ```

- spq_disable_node(nodename text, nodeport integer)

    Description: The reverse operation of spq_activate_node, which changes an active node to an inactive node.

    Parameters:
    - nodename: IP address, domain name, or endpoint of the current DN
    - nodeport: openGauss port number of the DN node

    Return value type: N/A

    Example:

    ```sql
    openGauss=# select spq_disable_node('127.0.01', 5432);
    ```

- spq_update_node(node_id int, new_node_name text, new_node_port integer)

    Description: Updates the IP address and port of a node.

    Parameters:
    - node_id: Node ID of the node in the pg_dist_node table
    - new_node_name: New IP address, domain name, or endpoint of the node
    - new_node_port: New openGauss port number of the node

    Return value type: N/A

    Example:

    ```sql
    openGauss=# select spq_update_node(1, '127.0.01', 5432);
    ```

- spq_remove_node(nodename text, nodeport integer)

    Description: Removes a node. The node must not contain any data shards.

    Parameters:
    - nodename: IP address, domain name, or endpoint of the current DN
    - nodeport: openGauss port number of the DN node

    Return value type: N/A

    Example:

    ```sql
    openGauss=# select spq_remove_node('127.0.01', 5432);
    ```

- spq_is_coordinator()

    Description: Queries whether the current node is a CN node.

    Parameters: N/A

    Return value type: bool

    Example:

    ```sql
    openGauss=# select spq_is_coordinator();
     spq_is_coordinator
    --------------------
     t
    ```

- spq_get_active_worker_nodes()

    Description: Obtains information about active DN nodes in the cluster.

    Parameters: N/A

    Return value type: record

    Example:

    ```sql
    openGauss=# select spq_get_active_worker_nodes();
     spq_get_active_worker_nodes
    -----------------------------
     (127.0.0.1,5432)
     (127.0.0.1,5532)
    ```

- spq_check_connection_to_node(nodename text, nodeport integer)

    Description: Checks the connectivity of a node.

    Parameters:
    - nodename: IP address, domain name, or endpoint of the current DN
    - nodeport: openGauss port number of the DN node

    Return value type: bool

    Example:

    ```sql
    openGauss=# select spq_check_connection_to_node('127.0.0.1', 5432);
     spq_check_connection_to_node
    ------------------------------
     t
    ```

## System Tables and Views

- pg_dist_node

    Description: The system table that stores information about all nodes in the cluster.

    <table><thead align="left"><tr id="row2994153425611">
    <th class="cellrowborder" valign="top" width="11.97%" id="mcps1.2.5.1.2"><p id="p3994113411568"><a name="p3994113411568"></a><a name="p3994113411568"></a>Column Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="15.32%" id="mcps1.2.5.1.3"><p id="p189946342567"><a name="p189946342567"></a><a name="p189946342567"></a>Type</p>
    </th>
    <th class="cellrowborder" valign="top" width="61.019999999999996%" id="mcps1.2.5.1.4"><p id="p899410346569"><a name="p899410346569"></a><a name="p899410346569"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row16994143445619">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p114021312413"><a name="p114021312413"></a><a name="p114021312413"></a>nodeid</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p16994183413569"><a name="p16994183413569"></a><a name="p16994183413569"></a>integer</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Unique node identifier.
    </td>
    </tr>
    <tr id="row17994123411562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p53919138411"><a name="p53919138411"></a><a name="p53919138411"></a>groupid</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p12994103420564"><a name="p12994103420564"></a><a name="p12994103420564"></a>integer</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Unique identifier of a group of nodes.
    </td>
    </tr>
    <tr id="row29944347562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p0371613945"><a name="p0371613945"></a><a name="p0371613945"></a>nodename</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p188631948943"><a name="p188631948943"></a><a name="p188631948943"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Node IP address or domain name.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">nodeport
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>integer</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Node port.
    </td>
    </tr>
    
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">noderack
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Not used in the current version.
    </td>
    </tr>
    
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">hasmetadata
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>boolean</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Whether metadata exists. `true` for CN nodes and `false` for DN nodes.
    </td>
    </tr>
    
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">isactive
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>boolean</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Whether the node is used.
    </td>
    </tr>
    
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">nodecluster
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>name</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Name of the spq cluster to which the node belongs.
    </td>
    </tr>
    
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">metadatasynced
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>boolean</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Whether metadata is synchronized. `true` for CN nodes and `false` for DN nodes.
    </td>
    </tr>
    
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">shouldhaveshards
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>boolean</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Whether shard data can be stored. `false` for CN nodes and `true` for DN nodes.
    </td>
    </tr>
    </tbody>
    </table>

    Examples:

    ```sql
    openGauss=# select * from pg_dist_node;
    nodeid | groupid | nodename  | nodeport | noderack | hasmetadata | isactive | noderole | nodecluster | metadatasynced | shouldhaveshards 
    --------+---------+-----------+----------+----------+-------------+----------+----------+-------------+----------------+------------------
         1 |       0 | 127.0.0.1 |    15432 | default  | t           | t        | primary  | default     | t              | f
         2 |       1 | 127.0.0.1 |    15532 | default  | f           | t        | primary  | default     | f              | t
         3 |       2 | 127.0.0.1 |    15632 | default  | f           | t        | primary  | default     | f              | t
    (3 rows)
    ```

- pg_dist_partition

    Description: The system table that stores the definitions of distributed tables

    <table><thead align="left"><tr id="row2994153425611">
    <th class="cellrowborder" valign="top" width="11.97%" id="mcps1.2.5.1.2"><p id="p3994113411568"><a name="p3994113411568"></a><a name="p3994113411568"></a>Column Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="15.32%" id="mcps1.2.5.1.3"><p id="p189946342567"><a name="p189946342567"></a><a name="p189946342567"></a>Type</p>
    </th>
    <th class="cellrowborder" valign="top" width="61.019999999999996%" id="mcps1.2.5.1.4"><p id="p899410346569"><a name="p899410346569"></a><a name="p899410346569"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row16994143445619">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p114021312413"><a name="p114021312413"></a><a name="p114021312413"></a>logicalrelid</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p16994183413569"><a name="p16994183413569"></a><a name="p16994183413569"></a>regclass</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Table name of the distributed table.
    </td>
    </tr>
    <tr id="row17994123411562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p53919138411"><a name="p53919138411"></a><a name="p53919138411"></a>partmethod</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p12994103420564"><a name="p12994103420564"></a><a name="p12994103420564"></a>char</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    In the current version, the only value is 'h', indicating hash partitioning.
    </td>
    </tr>
    <tr id="row29944347562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p0371613945"><a name="p0371613945"></a><a name="p0371613945"></a>partkey</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p188631948943"><a name="p188631948943"></a><a name="p188631948943"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Information about the partition column.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">colocationid
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>integer</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Not used in the current version.
    </td>
    </tr>
    
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">repmodel
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>char</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Not used in the current version.
    </td>
    </tr>
    
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">autoconverted
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>boolean</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Not used in the current version.
    </td>
    </tr>
    
    </tbody>
    </table>

    Example:

    ```sql
    openGauss=# select * from pg_dist_partition;
    logicalrelid | partmethod |                                                        partkey                                                         | colocationid | repmodel | autoconverted 
    --------------+------------+------------------------------------------------------------------------------------------------------------------------+--------------+----------+---------------
    t1           | h          | {VAR :varno 1 :varattno 1 :vartype 23 :vartypmod -1 :varcollid 0 :varlevelsup 0 :varnoold 1 :varoattno 1 :location -1} |            1 | s        | f
    (1 row)
    ```

- pg_dist_shard

    Description: The system table that stores information about each shard of a distributed table

    <table><thead align="left"><tr id="row2994153425611">
    <th class="cellrowborder" valign="top" width="11.97%" id="mcps1.2.5.1.2"><p id="p3994113411568"><a name="p3994113411568"></a><a name="p3994113411568"></a>Column Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="15.32%" id="mcps1.2.5.1.3"><p id="p189946342567"><a name="p189946342567"></a><a name="p189946342567"></a>Type</p>
    </th>
    <th class="cellrowborder" valign="top" width="61.019999999999996%" id="mcps1.2.5.1.4"><p id="p899410346569"><a name="p899410346569"></a><a name="p899410346569"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row16994143445619">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p114021312413"><a name="p114021312413"></a><a name="p114021312413"></a>logicalrelid</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p16994183413569"><a name="p16994183413569"></a><a name="p16994183413569"></a>regclass</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Distributed table name.
    </td>
    </tr>
    <tr id="row17994123411562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p53919138411"><a name="p53919138411"></a><a name="p53919138411"></a>shardid</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p12994103420564"><a name="p12994103420564"></a><a name="p12994103420564"></a>bigint</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Shard ID.
    </td>
    </tr>
    <tr id="row29944347562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p0371613945"><a name="p0371613945"></a><a name="p0371613945"></a>shardstorage</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p188631948943"><a name="p188631948943"></a><a name="p188631948943"></a>char</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Storage type. Currently, only 't' (ordinary table) is supported.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">shardminvalue
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Minimum hash value of the hash partition of the current shard.
    </td>
    </tr>
    
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">shardmaxvalue
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Maximum hash value of the hash partition of the current shard.
    </td>
    </tr>
    
    </tbody>
    </table>

    Example:

    ```sql
    openGauss=# select * from pg_dist_shard;
     logicalrelid | shardid | shardstorage | shardminvalue | shardmaxvalue 
    --------------+---------+--------------+---------------+---------------
     t1           |   62025 | t            | -2147483648   | -1073741825
     t1           |   62026 | t            | -1073741824   | -1
     t1           |   62027 | t            | 0             | 1073741823
     t1           |   62028 | t            | 1073741824    | 2147483647
    (4 rows)
    ```

- pg_dist_placement

    Description: The system table that stores the placement information of each shard of a distributed table

    <table><thead align="left"><tr id="row2994153425611">
    <th class="cellrowborder" valign="top" width="11.97%" id="mcps1.2.5.1.2"><p id="p3994113411568"><a name="p3994113411568"></a><a name="p3994113411568"></a>Column Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="15.32%" id="mcps1.2.5.1.3"><p id="p189946342567"><a name="p189946342567"></a><a name="p189946342567"></a>Type</p>
    </th>
    <th class="cellrowborder" valign="top" width="61.019999999999996%" id="mcps1.2.5.1.4"><p id="p899410346569"><a name="p899410346569"></a><a name="p899410346569"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">placementid
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>bigint</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Auto-increment unique identifier.
    </td>
    </tr>
    <tr id="row17994123411562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p53919138411"><a name="p53919138411"></a><a name="p53919138411"></a>shardid</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p12994103420564"><a name="p12994103420564"></a><a name="p12994103420564"></a>bigint</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Shard ID.
    </td>
    </tr>
    <tr id="row29944347562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p0371613945"><a name="p0371613945"></a><a name="p0371613945"></a>shardstate</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p188631948943"><a name="p188631948943"></a><a name="p188631948943"></a>int</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Shard state.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">shardlength
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>bigint</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Always 0 for hash partitions.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">groupid
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>integer</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Unique identifier of a group of nodes.
    </td>
    </tr>
    
    </tbody>
    </table>

    Example:

    ```sql
    openGauss=# select * from pg_dist_placement;
     placementid | shardid | shardstate | shardlength | groupid 
    -------------+---------+------------+-------------+---------
               1 |   62025 |          1 |           0 |       1
               2 |   62026 |          1 |           0 |       2
               3 |   62027 |          1 |           0 |       1
               4 |   62028 |          1 |           0 |       2
    (4 rows)
    
    ```

- spq_shards

    Description: The view that displays information about all shards

    <table><thead align="left"><tr id="row2994153425611">
    <th class="cellrowborder" valign="top" width="11.97%" id="mcps1.2.5.1.2"><p id="p3994113411568"><a name="p3994113411568"></a><a name="p3994113411568"></a>Column Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="15.32%" id="mcps1.2.5.1.3"><p id="p189946342567"><a name="p189946342567"></a><a name="p189946342567"></a>Type</p>
    </th>
    <th class="cellrowborder" valign="top" width="61.019999999999996%" id="mcps1.2.5.1.4"><p id="p899410346569"><a name="p899410346569"></a><a name="p899410346569"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">table_name
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>regclass</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Table name.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">shardid
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>bigint</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Shard ID.
    </td>
    </tr>
    <tr id="row17994123411562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p53919138411"><a name="p53919138411"></a><a name="p53919138411"></a>shard_name</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p12994103420564"><a name="p12994103420564"></a><a name="p12994103420564"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Shard name.
    </td>
    </tr>
    <tr id="row29944347562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p0371613945"><a name="p0371613945"></a><a name="p0371613945"></a>table_type</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p188631948943"><a name="p188631948943"></a><a name="p188631948943"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Table type.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">nodename</td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>text</p></td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Node IP address or domain name.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">nodeport</td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>integer</p></td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Node port.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">shard_size</td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>bigint</p></td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Shard size.
    </td>
    </tr>
    
    </tbody>
    </table>

    Example:

    ```sql
    openGauss=# select * from spq_shards;
     table_name | shardid | shard_name | table_type  | nodename  | nodeport | shard_size 
    ------------+---------+------------+-------------+-----------+----------+------------
     t1         |   62025 | t1_62025   | distributed | 127.0.0.1 |    15532 |       8192
     t1         |   62026 | t1_62026   | distributed | 127.0.0.1 |    15632 |       8192
     t1         |   62027 | t1_62027   | distributed | 127.0.0.1 |    15532 |          0
     t1         |   62028 | t1_62028   | distributed | 127.0.0.1 |    15632 |       8192
    (4 rows)
    ```

- spq_tables

    Description: The view that displays information about all distributed tables

    <table><thead align="left"><tr id="row2994153425611">
    <th class="cellrowborder" valign="top" width="11.97%" id="mcps1.2.5.1.2"><p id="p3994113411568"><a name="p3994113411568"></a><a name="p3994113411568"></a>Column Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="15.32%" id="mcps1.2.5.1.3"><p id="p189946342567"><a name="p189946342567"></a><a name="p189946342567"></a>Type</p>
    </th>
    <th class="cellrowborder" valign="top" width="61.019999999999996%" id="mcps1.2.5.1.4"><p id="p899410346569"><a name="p899410346569"></a><a name="p899410346569"></a>Description</p>
    </th>
    </tr>
    </thead>
    <tbody>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">table_name
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>regclass</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Table name.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">table_type
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Table type.
    </td>
    </tr>
    <tr id="row17994123411562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p53919138411"><a name="p53919138411"></a><a name="p53919138411"></a>distribution_column</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p12994103420564"><a name="p12994103420564"></a><a name="p12994103420564"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Shard column.
    </td>
    </tr>
    <tr id="row29944347562">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 "><p id="p0371613945"><a name="p0371613945"></a><a name="p0371613945"></a>table_size</p>
    </td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p188631948943"><a name="p188631948943"></a><a name="p188631948943"></a>text</p>
    </td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 ">    Table size.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">shard_count</td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>bigint</p></td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Shard count.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">table_owner</td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>name</p></td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Owner.
    </td>
    </tr>
    <tr id="row179951634105614">
    <td class="cellrowborder" valign="top" width="11.97%" headers="mcps1.2.5.1.2 ">access_method</td>
    <td class="cellrowborder" valign="top" width="15.32%" headers="mcps1.2.5.1.3 "><p id="p1199573415619"><a name="p1199573415619"></a><a name="p1199573415619"></a>name</p></td>
    <td class="cellrowborder" valign="top" width="61.019999999999996%" headers="mcps1.2.5.1.4 "><a name="ul13872183415441"></a><a name="ul13872183415441"></a>
    Not used in the current version.
    </td>
    </tr>
    
    </tbody>
    </table>

    Example:

    ```sql
    openGauss=# select * from spq_tables;
     table_name | table_type  | distribution_column | table_size | shard_count | table_owner | access_method 
    ------------+-------------+---------------------+------------+-------------+-------------+---------------
     t1         | distributed | a                   | 24 kB      |           4 | openGauss   | 
    (1 row)
    ```
