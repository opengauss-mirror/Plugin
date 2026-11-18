# spqplugin_v2 Overview and Installation

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-13T03:28:15.161Z pushedAt=2026-09-29T02:34:56.713Z -->

## Overview

spqplugin_v2 is a lightweight distributed database extension plugin based on openGauss. It has been available starting from openGauss 7.0.0-RC2. The database cluster is built with one CN node and multiple DN nodes, supporting sharded storage of large tables for parallel queries and horizontal scaling.

## Constraints

- Supported only in A-mode databases.
- To install the plugin, configure shared_preload_libraries first, and then run create extension.
- Deleting the plugin is not supported.
- Complex queries on distributed tables are not supported, including JOIN, subquery, CTE, and window function.
- Partitioned tables are not supported.
- MySQL compatibility plugin, thread pools, and PBE optimization are not supported.

## Installation

spqplugin_v2 is included in the openGauss package by default. Perform the following steps to use it:

1. Add the following configuration to `postgresql.conf`:

```
shared_preload_libraries = 'spq'
```

2. Create a database and connect to it to start using the plugin.

```
openGauss=# create extension spq;
CREATE EXTENSION
```
