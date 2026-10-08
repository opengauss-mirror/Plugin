# spqplugin.enable_spq

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-13T02:59:26.716Z pushedAt=2026-09-29T02:34:56.707Z -->

**Description**: The parameter value is of Boolean type. This parameter enables or disables the plugin.

**Value**: Boolean type

- `on`: compatibility features enabled.
- `off`: compatibility features disabled. Original kernel functions are used.

**Default Value**: off

## spqplugin.cluster_map

**Description**: The parameter value is of string type. This parameter specifies the IP address and libcomm port of compute nodes. Configured nodes receive and execute multi-node plans dispatched by the primary node and return the computation results to the primary node. The primary node aggregates the final results and returns them to the user.

**Value**: string

Format of spqplugin.cluster_map:
pgxc_node_name|ip|port

- `pgxc_node_name`: Name of the current compute node. It must be the same as the GUC parameter [pgxc_node_name](../database_reference/opengauss_transaction.md). The node names configured on different compute nodes must be different from each other.
- `ip`: IP address used by the current compute node for libcomm communication. Configure it in a network segment different from that of the cluster communication IP to avoid mutual network impact.
- `port`: Current database port.

The following example uses two nodes. The parameters are configured as follows:

```
spqplugin.cluster_map = 'node1|x.x.x.x|12300,node2|x.x.x.x|12300'
```

**Default Value**: NULL (empty by default, indicating that no multi-server nodes are specified)

## spqplugin.spq_optimizer_log

**Description**: The parameter value is of Boolean type. This parameter enables or disables the multi-server parallel optimizer logging.

**Value**: Boolean type

- `on`: Multi-server parallelism optimizer logging enabled.
- `off`: Multi-server parallelism optimizer logging disabled.

**Default Value**: off

## spqplugin.spq_optimizer_print_plan

**Description**: The parameter value is of the Boolean type. This parameter enables or disables multi-server parallel printing of plan tree information.

**Value**: Boolean type

- `on`: Enables multi-server parallel printing of plan tree information.
- `off`: Disables multi-server parallel printing of plan tree information.

**Default Value**: off

## spqplugin.spq_optimizer_print_xform

**Description**: The parameter value is of Boolean type. This parameter enables or disables multi-server parallel xform information printing.

**Value**: Boolean

- `on`: Enables multi-server parallel xform information printing.
- `off`: Disables multi-server parallel xform information printing.

**Default Value**: off

## spqplugin.spq_optimizer_print_xform_results

**Description**: The parameter value is of Boolean type. This parameter controls whether to print multi-server parallel xform input/output result information.

**Value**: Boolean type

- `on`: Enables multi-server parallel xorm input/output result information printing.
- `off`: Disables multi-server parallel xform input/output result information printing.

**Default Value**: off

## spqplugin.spq_optimizer_print_memo_after_exploration

**Description**: The parameter value is of Boolean type. This parameter controls whether to print the multi-server parallel memo optimization process.

**Value**: Boolean type

- `on`: Enables the printing of the multi-server parallel memo optimization process.
- `off`: Disables the printing of the multi-server parallel memo optimization process.

**Default Value**: off

## spqplugin.spq_optimizer_print_memo_after_implementation

**Description**: The parameter value is of the Boolean type. This parameter enables or disables printing the replacement plan of the multi-server parallel memo optimization process.

**Value**: Boolean type

- `on`: Prints the replacement plan of the multi-server parallel memo optimization process.
- `off`: Does not print the replacement plan of the multi-server parallel memo optimization process.

**Default Value**: off

## spqplugin.spq_optimizer_print_memo_after_optimization

**Description**: The parameter value is of Boolean type. This parameter controls whether to print the final plan of the multi-server parallel memo optimization process.

**Value**: Boolean type

- `on`: Prints the final plan of the multi-server parallel memo optimization process.
- `off`: Does not print the final plan of the multi-server parallel memo optimization process.

**Default Value**: off

## spqplugin.spq_optimizer_print_job_scheduler

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel task scheduling information printing.

**Value**: Boolean type

- `on`: Enables multi-server parallel task scheduling information printing.
- `off`: Disables multi-server parallel task scheduling information printing.

**Default Value**: off

## spqplugin.spq_optimizer_print_group_properties

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel operator attribute information printing.

**Value**: Boolean type

- `on`: Enables multi-server parallel operator attribute information printing.
- `off`: Disables multi-server parallel operator attribute information printing.

**Default Value**: off

## spqplugin.spq_optimizer_print_optimization_cost

**Description**: The parameter value is of Boolean type. This parameter controls printing of multi-server parallel optimization cost information.

**Value**: Boolean

- `on`: Enables printing of multi-server parallel optimization cost information.
- `off`: Disables printing of multi-server parallel optimization cost information.

**Default Value**: off

## spqplugin.spq_optimizer_print_optimization_stats

**Description**: The parameter value is of Boolean type. This parameter enables or disables multi-server parallel optimization status information printing.

**Value**: Boolean type

- `on`: Enables multi-server parallel optimization status information printing.
- `off`: Disables multi-server parallel optimization status information printing.

**Default Value**: off

## spqplugin.spq_optimizer_enable_nljoin

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the nested loop operator.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the nested loop operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the nested loop operator.

**Default value**: on

## spqplugin.spq_optimizer_enable_indexjoin

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of index join operators.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate index join operators.
- `off`: Does not allow the multi-server parallelism optimizer to generate index join operators.

**Default value**: on

## spqplugin.spq_optimizer_enable_motions

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of motion operators.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate motion operators.
- `off`: Does not allow the multi-server parallelism optimizer to generate motion operators.

**Default Value**: on

## spqplugin.spq_optimizer_enable_motion_broadcast

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the motion broadcast operator.

**Value**: Boolean

- `on`: Allows the multi-server parallelism optimizer to generate the motion broadcast operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the motion broadcast operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_motion_gather

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the motion gather operator.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the motion gather operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the motion gather operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_motion_redistribute

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the motion redistribute operator.

**Value**: Boolean

- `on`: Allows the multi-server parallelism optimizer to generate the motion redistribute operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the motion redistribute operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_sort

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the sort operator.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the sort operators.
- `off`: Does not allow the multi-server parallelism optimizer to generate the sort operators.

**Default Value**: on

## spqplugin.spq_optimizer_enable_materialize

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the materialize operator.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the materialize operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the materialize operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_partition_propagation

**Description**: The Parameter Value is of the Boolean type. This parameter controls the multi-server parallel generation of the partition propagation operator.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the partition propagation operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the partition propagation operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_partition_selection

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the partition selection operator.

**Value**: Boolean

- `on`: Allows the multi-server parallelism optimizer to generate the partition selection operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the partition selection operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_outerjoin_rewrite

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel outer join rewrites.

**Value**: Boolean type

- `on`: Enables multi-server parallel outer join rewrites.
- `off`: Disables multi-server parallel outer join rewrites.

**Default Value**: on

## spqplugin.spq_optimizer_enable_multiple_distinct_aggs

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel distinct aggs.

**Value**: Boolean type

- `on`: Allows multiple distinct aggs in multi-server parallelism plans.
- `off`: Does not allow multiple distinct aggs in multi-server parallelism plans.

**Default Value**: off

## spqplugin.spq_optimizer_enable_direct_dispatch

**Description**: The parameter value is of Boolean type. This parameter controls task assignment by the multi-server parallelism optimizer.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to directly assign tasks.
- `off`: Does not allow the multi-server parallelism optimizer to directly assign tasks.

**Default Value**: on

## spqplugin.spq_optimizer_enable_hashjoin_redistribute_broadcast_children

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the left and right subtrees of the hash join operator.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism hash join operator subtrees to cover `redistribute` and `broadcast`.
- `off`: Does not allow the multi-server parallelism hash join operator subtrees to cover `redistribute` and `broadcast`.

**Default Value**: off

## spqplugin.spq_optimizer_discard_redistribute_hashjoin

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the redistribute hash join operator.

**Value**: Boolean

- `on`: Discards the multi-server parallelism redistribute hash join operator.
- `off`: Does not discard the multi-server parallelism redistribute hash join operator.

**Default Value**: off

## spqplugin.spq_optimizer_enable_broadcast_nestloop_outer_child

**Description**: The parameter value is of the Boolean type. This parameter controls the multi-server parallel generation of nested loop operator subtrees.

**Value**: Boolean type

- `on`: Allows broadcasts in nested loop operator outer joins in multi-server parallelism.
- `off`: Does not allow broadcasts in nested loop operator outer joins in multi-server parallelism.

**Default Value**: on

## spqplugin.spq_optimizer_enable_streaming_material

**Description**: The parameter value is of the Boolean type. This parameter controls the multi-server parallel generation of nested loop operator subtrees.

**Value**: Boolean type

- `on`: Allows broadcasts in the multi-server parallel nested loop operator outer joins.
- `off`: Does not allow broadcasts in the multi-server parallel nested loop operator outer joins.

**Default Value**: on

## spqplugin.spq_optimizer_enable_gather_on_segment_for_dml

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel DML optimization.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to enforce gather on segments to enable DML optimization.
- `off`: Does not allow the multi-server parallelism optimizer to enforce gather on segments to enable DML optimization.

**Default Value**: on

## spqplugin.spq_optimizer_enable_assert_maxonerow

**Description**: The parameter value is of Boolean type. This parameter controls the row count check in parallel across servers.

**Value**: Boolean

- `on`: Enables the Assert MaxOneRow plan in parallel across servers to check the row count at runtime.
- `off`: Does not enable the Assert MaxOneRow plan in parallel across servers to check the row count at runtime.

**Default Value**: on

## spqplugin.spq_optimizer_enable_constant_expression_evaluation

**Description**: The parameter value is of the Boolean type. This parameter controls constant expression evaluation in multi-server parallelism.

**Value**: Boolean type

- `on`: Allows constant expression evaluation in multi-server parallelism.
- `off`: Does not allow constant expression evaluation in multi-server parallelism.

**Default Value**: on

## spqplugin.spq_optimizer_enable_bitmapscan

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the bitmap scan operators.

**Value**: Boolean

- `on`: Allows the multi-server parallelism optimizer to generate the bitmap scan operators.
- `off`: Does not allow the multi-server parallelism optimizer to generate the bitmap scan operators.

**Default Value**: on

## spqplugin.spq_optimizer_enable_outerjoin_to_unionall_rewrite

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel outer join rewrites as unionall.

**Value**: Boolean type

- `on`: Allows multi-server parallel outer join rewrites as unionall.
- `off`: Does not allow multi-server parallel outer join rewrites as unionall.

**Default Value**: off

## spqplugin.spq_optimizer_enable_ctas

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel generation of CTAS plans.

**Value**: Boolean

- `on`: Allows multi-server parallel generation of CTAS plans.
- `off`: Does not allow multi-server parallel generation of CTAS plans.

**Default value**: off

## spqplugin.spq_optimizer_enable_partial_index

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel generation of partial index plans.

**Value**: Boolean type

- on: Allows multi-server parallel generation of partial index plans.
- off: Does not allow multi-server parallel generation of partial index plans.

**Default Value**: on

## spqplugin.spq_optimizer_enable_dml

**Description**: The parameter value is of Boolean type. This parameter controls whether DML multi-server execution is allowed.

**Value**: Boolean type

- on: Allows DML multi-server parallelism.
- off: Does not allow DML multi-server parallelism.

**Default Value**: off

## spqplugin.spq_optimizer_enable_dml_triggers

**Description**: The parameter value is of Boolean type. This parameter controls whether DML multi-server execution is allowed.

**Value**: Boolean type

- `on`: Allows DML multi-server parallelism.
- `off`: Does not allow DML multi-server parallelism.

**Default Value**: off

## spqplugin.spq_optimizer_enable_dml_constraints

**Description**: The parameter value is of Boolean type. This parameter controls whether DML multi-server execution is allowed.

**Value**: Boolean

- `on`: Allows DML with check constraints and NOT NULL constraints for multi-server parallelism.
- `off`: Does not allow DML with check constraints and NOT NULL constraints for multi-server parallelism.

**Default Value**: off

## spqplugin.spq_optimizer_enable_master_only_queries

**Description**: The parameter value is of Boolean type. This parameter controls whether to allow multi-server execution for catalog table queries.

**Value**: Boolean

- `on`: Allows multi-server execution for catalog table queries.
- `off`: Does not allow multi-server execution for catalog table queries.

**Default Value**: off

## spqplugin.spq_optimizer_enable_hashjoin

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel hash join operator generation.

**Value**: Boolean

- `on`: Allows the multi-server parallelism optimizer to generate the hash join operators.
- `off`: Does not allow the multi-server parallelism optimizer to generate the hash join operators.

**Default**: on

## spqplugin.spq_optimizer_enable_dynamictablescan

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel dynamictablescan operator generation.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the dynamictablescan operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the dynamictablescan operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_indexscan

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel indexscan operator generation.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the indexscan operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the indexscan operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_indexonlyscan

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel indexonlyscan operator generation.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the indexonlyscan operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the indexonlyscan operator.

**Default value**: on

## spqplugin.spq_optimizer_enable_tablescan

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel tablescan operator generation.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the tablescan operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the tablescan operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_seqsharescan

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel seqsharescan operator generation.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the seqsharescan operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the seqsharescan operator.

**Default**: on

## spqplugin.spq_optimizer_enable_shareindexscan

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel shareindexscan operator generation.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the shareindexscan operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the shareindexscan operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_hashagg

**Description**: The parameter value is of the Boolean type. This parameter controls the multi-server parallel hashagg operator generation.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the hashagg operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the hashagg operator.

**Default Value**: on

## spqplugin.spq_optimizer_enable_groupagg

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel groupagg operator generation.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to generate the groupagg operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the groupagg operator.

**Default Value**: on

## spqplugin.spq_optimizer_expand_fulljoin

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel `union all` for expanding outer joins.

**Value**: Boolean type

- `on`: Allows multi-server parallel `union all` for expanding outer joins.
- `off`: Does not allow multi-server parallel `union all` for expanding outer joins.

**Default Value**: off

## spqplugin.spq_optimizer_enable_mergejoin

**Description**: The parameter value is of Boolean type. This parameter controls the multi-server parallel generation of the mergejoin operator.

**Value**: Boolean

- `on`: Allows the multi-server parallelism optimizer to generate the mergejoin operator.
- `off`: Does not allow the multi-server parallelism optimizer to generate the mergejoin operator.

**Default Value**: on

## spqplugin.spq_optimizer_prune_unused_columns

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism optimization plans.

**Value**: Boolean type

- `on`: Allows the multi-server parallelism optimizer to prune unused columns.
- `off`: Does not allow the multi-server parallelism optimizer to prune unused columns.

**Default Value**: on

## spqplugin.spq_optimizer_enable_redistribute_nestloop_loj_inner_child

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism optimization plans.

**Value**: Boolean

- `on`: Enables `redistribute` for the multi-server parallelism nested loop left join.
- `off`: Disables `redistribute` for the multi-server parallelism nested loop left join.

**Default Value**: on

## spqplugin.spq_optimizer_force_comprehensive_join_implementation

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism optimization plans.

**Value**: Boolean type

- `on`: Enables comprehensive join for multi-server parallelism.
- `off`: Disables comprehensive join for multi-server parallelism.

**Default Value**: off

## spqplugin.spq_optimizer_enable_replicated_table

**Description**: The parameter value is of Boolean type. This parameter controls whether to allow multi-server execution of replicated table queries.

**Value**: Boolean type

- `on`: Allows multi-server execution of replicated table queries.
- `off`: Does not allow multi-server execution of replicated table queries.

**Default Value**: on

## spqplugin.spq_optimizer_enumerate_plans

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism optimization plans.

**Value**: Boolean type

- `on`: Allows multi-server parallel plan enumeration.
- `off`: Does not allow multi-server parallel plan enumeration.

**Default Value**: off

## spqplugin.spq_optimizer_sample_plans

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism optimization plans.

**Value**: Boolean

- `on`: Allows multi-server parallel sampling plans.
- `off`: Does not allow multi-server parallel sampling plans.

**Default Value**: off

## spqplugin.spq_optimizer_extract_dxl_stats

**Description**: The parameter value is of Boolean type. This parameter controls whether plan statistics are extracted in DXL in parallel across servers.

**Value**: Boolean type

- `on`: Extracts plan statistics in DXL in parallel across servers.
- `off`: Does not extract plan statistics in DXL in parallel across servers.

**Default Value**: off

## spqplugin.spq_optimizer_extract_dxl_stats_all_nodes

**Description**: The parameter value is of Boolean type. This parameter controls whether plan statistics are extracted in DXL in parallel across servers.

**Value**: Boolean type

- `on`: Extracts plan statistics in DXL in parallel across servers on all nodes.
- `off`: Does not extract plan statistics in DXL in parallel across servers on all nodes.

**Default value**: off

## spqplugin.spq_optimizer_print_missing_stats

**Description**: The parameter value is of the Boolean type. This parameter controls whether to print columns that lack statistics in parallel across servers.

**Value**: Boolean type

- `on`: Prints columns that lack statistics in parallel across servers.
- `off`: Does not allow printing columns that lack statistics in parallel across servers.

**Default Value**: on

## spqplugin.spq_optimizer_dpe_stats

**Description**: The parameter value is of Boolean type. This parameter controls whether multi-server parallelism enables statistics derivation for partitioned tables with dynamic partition elimination.

**Value**: Boolean type

- `on`: Enables statistics derivation for partitioned tables with dynamic partition elimination.
- `off`: Disables statistics derivation for partitioned tables with dynamic partition elimination.

**Default Value**: on

## spqplugin.spq_optimizer_enable_derive_stats_all_groups

**Description**: The parameter value is of Boolean type. This parameter controls whether multi-server parallelism enables statistics derivation for all groups.

**Value**: Boolean

- `on`: Enables statistics derivation for all groups.
- `off`: Disables statistics derivation for all groups.

**Default Value**: off

## spqplugin.spq_optimizer_force_multistage_agg

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism plan generation.

**Value**: Boolean

- `on`: Forcibly generates multistage agg plans for multi-server parallelism.
- `off`: Does not forcibly generate multistage agg plans for multi-server parallelism.

**Default Value**: off

## spqplugin.spq_optimizer_force_three_stage_scalar_dqa

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism plan generation.

**Value**: Boolean type

- `on`: Forcibly generates a three-stage agg plan for distinct qualified aggregation in parallel across servers.
- `off`: Does not forcibly generate a three-stage agg plan for distinct qualified aggregation in parallel across servers.

**Default Value**: on

## spqplugin.spq_optimizer_force_expanded_distinct_aggs

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism plan generation.

**Value**: Boolean

- `on`: Forcibly expands distinct aggs in parallel across servers.
- `off`: Does not forcibly expand distinct aggs in parallel across servers.

**Default Value**: on

## spqplugin.spq_optimizer_force_agg_skew_avoidance

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism plan generation.

**Value**: Boolean type

- `on`: Forcibly selects an agg plan that avoids data skew in parallel across servers.
- `off`: Does not forcibly select an agg plan that avoids data skew in parallel across servers.

**Default Value**: on

## spqplugin.spq_optimizer_penalize_skew

**Description**: The parameter value is of the Boolean type. This parameter controls multi-server parallelism plan generation.

**Value**: Boolean

- `on`: Penalizes data skew plans in parallel across servers.
- `off`: Does not penalize data skew plans in parallel across servers.

**Default Value**: on

## spqplugin.spq_optimizer_prune_computed_columns

**Description**: The parameter value is of the Boolean type. This parameter controls whether multi-server parallelism prunes unused computed columns.

**Value**: Boolean type

- `on`: Prunes unused computed columns during preprocessing.
- `off`: Does not prune unused computed columns during preprocessing.

**Default Value**: on

## spqplugin.spq_enable_btbuild

**Description**: The parameter value is of Boolean type. This parameter creates btree indexes in parallel across servers.

**Value**: Boolean type

- `on`: Creates btree indexes in parallel across servers.
- `off`: Does not create btree indexes in parallel across servers. Original kernel functions are used.

**Default Value**: off

## spqplugin.spq_enable_btbuild_cic

**Description**: The parameter value is of Boolean type. This parameter enables multi-server parallelism for online creation of btree indexes.

**Value**: Boolean type

- `on`: Enables multi-server parallelism for online creation of btree indexes.
- `off`: Disables multi-server parallelism for online creation of btree indexes and uses the original kernel functions.

**Default Value**: off

## spqplugin.spq_enable_insert_select

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel insert operation.

**Value**: Boolean type

- `on`: Enables multi-server parallelism to insert data.
- `off`: Disables multi-server parallelism to insert data and uses the original kernel functions.

**Default Value**: off

## spqplugin.spq_enable_insert_from_tableless

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel insert operations without subqueries.

**Value**: Boolean

- `on`: Allows multi-server parallel insert operations without subqueries.
- `off`: Requires multi-server parallel insert statements to contain a subquery.

**Default Value**: off

## spqplugin.spq_enable_insert_order_sensitive

**Description**: The parameter value is of Boolean type. This parameter ensures that data is written in the order of the query result.

**Value**: Boolean type

- `on`: Writes data in the order of the query result.
- `off`: Does not guarantee the order of written data.

**Default Value**: off

## spqplugin.spq_enable_delete

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallelism for DELETE operations.

**Value**: Boolean type

- `on`: Deletes data using multi-server parallelism.
- `off`: Does not delete data using multi-server parallelism. Original kernel functions are used.

**Default Value**: off

## spqplugin.spq_enable_update

**Description**: The parameter value is of Boolean type. This parameter controls multi-server parallel UPDATE operations.

**Value**: Boolean type

- `on`: Updates data using multi-server parallelism.
- `off`: Does not update date using multi-server parallelism. Original kernel functions are used.

**Default Value**: off

## comm_max_stream

**Description**: The parameter value is an integer. This parameter limits the maximum number of streams (stream IDs) between two nodes in multi-server parallelism.

- Formula:  dop *dop* number of streams
- If connected to multiple QE nodes: (dop *dop* number of streams) x number of QE nodes
- Plans running at the same time share all current stream IDs. Therefore, the total number of streams is the sum of the number of streams in each plan.

**Value**: 1-60000

**Default Value**: 1024
