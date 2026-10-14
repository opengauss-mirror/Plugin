# spqplugin

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-13T03:30:51.405Z pushedAt=2026-09-29T02:34:56.715Z -->

## Overview

openGauss provides the spqplugin extension (spqplugin-1.0.0). SPQ (SharedEverything Parallel Query) is a multi-server parallel query framework built on openGauss, deployed in resource pools where a cluster adopts the single-write-multi-read architecture. The cluster has one write/read node and multiple read-only nodes. Currently, the cluster only supports queries on one read-only node at a time, so the query performance of the cluster is highly limited.
SPQ is based on the SharedEverything distributed architecture, in which all nodes share the resources in a cluster. Through execution plan splitting, execution, and aggregation, all read nodes perform parallel queries, fully leveraging the OLAP capability of the cluster. This enables resource pooling to deliver both robust TP and AP performance.

## Limitations

- Currently, the extension can be loaded only using [shared_preload_libraries](../database_reference/kernel_resource_usage.md). If the extension is loaded using [CREATE Extension](../sql_reference/create_extension.md), it cannot be deleted.
- The extension depends on xerces-c 3.0 or later.
- If the reserved disk space is insufficient, statement execution fails.

## Installation

The extension is automatically installed and does not require manual installation. To manually compile and load the extension, perform the following steps:

### Compilation and Installation

1. [Compile and install openGauss](https://atomgit.com/opengauss/openGauss-server#%E7%BC%96%E8%AF%91).

2. Copy the [spqplugin source code](https://atomgit.com/opengauss/Plugin/tree/master/contrib/spq_plugin) to the `contrib` directory of the openGauss-server source code.

3. Enter the `spq_plugin` directory and run `make install`.

### OM Installation

1. Install openGauss using OM.

2. Copy the files required by the extension: `spqplugin.so` to `app/lib/postgresql/`; `spqplugin.control` and `spqplugin--1.0.sql` to `app/share/postgresql/extension`.

## Guidance

### Loading the Plugin

The multi-server query feature is disabled by default. Add the following GUC configuration parameters to enable the extension:

node1:

```
port = 12300
pgxc_node_name = 'node1'
comm_sctp_port = 12330
comm_control_port = 12340
shared_preload_libraries = 'spqplugin'
spqplugin.cluster_map = 'node1|x.x.x.x|12300,node2|x.x.x.x|12300'
spqplugin.enable_spq = on
```

node2:

```
port = 12300
pgxc_node_name = 'node2'
comm_sctp_port = 12330
comm_control_port = 12340
shared_preload_libraries = 'spqplugin'
spqplugin.cluster_map = 'node1|x.x.x.x|12300,node2|x.x.x.x|12300'
spqplugin.enable_spq = on
```

### Using the Plugin

The multi-server parallel query feature currently supports certain DQL, DDL, and DML statements. For details, see:

[SELECT](spqplugin-SELECT.md)

[CREATE INDEX](spqplugin-CREATE-INDEX.md)

[INSERT](spqplugin-INSERT.md)

[DELETE](spqplugin-DELETE.md)

[UPDATE](spqplugin-UPDATE.md)

## SQL Reference

- [SQL](spqplugin-SQL.md)
