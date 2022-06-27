Appendix A  Correspondence with Oracle Databases
----

This appendix explains the correspondence between OpenGauss and Oracle databases.

### A.1 Hint Clauses

**Description**

An execution plan specified for a query can be controlled per SQL statement by hints without any change in the settings for the entire server.

#### A.1.1  Hint Clause Correspondence

The table below lists the pg_hint_plan hints that correspond to Oracle database hints.

**Correspondence between Oracle database hints and pg_hint_plan**

|Hint (Oracle database)|Hint (OpenGauss)|
|:---|:---|
|FIRST_ROWS hint	|Rows	|
|FULL hint	|Seqscan	|
|INDEX hint	|IndexScan	|
|LEADING hint	|Leading	|
|NO_INDEX hint	|NoIndexScan	|
|NO_USE_HASH hint	|NoHashJoin	|
|NO_USE_MERGE hint	|NoMergeJoin	|
|NO_USE_NL hint	|NoNestLoop	|
|ORDERED hint	|Leading	|
|USE_HASH hint	|HashJoin	|
|USE_MERGE hint	|MergeJoin	|
|USE_NL hint	|NestLoop	|

**Note**

---

The optimizer operates differently for each database. Therefore, hint statements that are migrated as is may not have the same effect after migration. Be sure to verify operation at migration.

---

**Migration example**

The example below shows migration of a hint clause (INDEX hint).

<table>
<thead>
<tr>
<th align="center">Oracle database</th>
<th align="center">OpenGauss</th>
</tr>
</thead>
<tbody>
<tr>
<td align="left">
<pre><code>SELECT /*+INDEX(inventory_table idx1)*/ *
 FROM inventory_table WHERE i_number = 110;</code></pre>
</td>

<td align="left">
<pre><code>SELECT /*+IndexScan(inventory_table idx1)*/ *
 FROM inventory_table WHERE i_number = 110; </code></pre>
</td>
</tr>
</tbody>
</table>

Note: The string idx1 is the index name defined for the i_number row of inventory_table.

**Note**

----

The pg_hint_plan hint, which is an extended feature of OpenGauss, cannot be used to specify a column name or set a query block before the table name specification.

----

### A.2 Dynamic Performance Views

**Description**

Dynamic performance views are views that can reference information mainly relating to database performance.

#### A.2.1 Alternatives for Dynamic Performance Views

OpenGauss does not contain any dynamic performance views. Consider using the OpenGauss system catalogs or statistics views instead.

The table below lists the alternative system catalogs and statistics views that correspond to the dynamic performance views.

Alternatives for dynamic performance views

|Dynamic performance view<br> (Oracle database)	|System catalog or statistics view <br>(OpenGauss)|
|:---|:---|
|V$ACCESS	|	pg_locks	|
|V$ACTIVE_SERVICES	|	pg_stat_activity	|
|V$ARCHIVED_LOG	|	pg_stat_archiver	|
|V$CLIENT_STATS	|	pg_stat_activity	|
|V$CONTEXT	|	pg_settings	|
|V$DATABASE	|	pg_database	|
|V$EMX_USAGE_STATS	|	pg_stat_user_functions	|
|V$ENABLEDPRIVS	|	pg_authid	|
|V$ENQUEUE_LOCK	|	pg_locks	|
|V$FILESTAT	|	pg_statio_all_tables	|
|V$FIXED_TABLE	|	pg_views	|
|V$FIXED_VIEW_DEFINITION	|	pg_views	|
|V$GES_BLOCKING_ENQUEUE	|	pg_locks	|
|V$GLOBAL_BLOCKED_LOCKS	|	pg_locks	|
|V$GLOBAL_TRANSACTION	|	pg_locks	|
|V$LOCK	|	pg_locks	|
|V$LOCKED_OBJECT	|	pg_locks	|
|V$MVREFRESH	|	pg_matviews	|
|V$MYSTAT	|	pg_stat_all_tables or other view	|
|V$NLS_PARAMETERS	|	pg_settings	|
|V$NLS_VALID_VALUES	|	pg_proc <br>pg_ts_config <br>pg_collation<br>pg_type	|
|V$OBJECT_PRIVILEGE	|	pg_default_acl	|
|V$OBJECT_USAGE	|	pg_stat_all_indexes	|
|V$OPEN_CURSOR	|	pg_cursors	|
|V$OPTION	|	pg_settings	|
|V$PARAMETER	|	pg_settings	|
|V$PARAMETER2	|	pg_settings	|
|V$PROCESS	|	pg_stat_activity	|
|V$PWFILE_USERS	|	pg_users	|
|V$REPLPROP	|	pg_replication_origin <br>pg_replication_origin_status	|
|V$SESSION	|	pg_stat_activity 	|
|V$SESSTAT	|	pg_stat_all_tables or other view	|
|V$SQLFN_METADATA	|	pg_proc,pg_aggrgate	|
|V$SYSTEM_PARAMETER	|	pg_settings	|
|V$SYSTEM_PARAMETER2	|	pg_settings	|
|V$TABLESPACE	|	pg_tablespace	|
|V$TEMPSTAT	|	pg_stat_database	|
|V$TIMEZONE_NAMES	|	pg_timezone_names	|
|V$TRANSACTION	|	pg_locks	|
|V$TRANSACTION_ENQUEUE	|	pg_locks	|

**Note**

----

Not all dynamic performance view information can be obtained from the system catalogs and statistics views. Each user should determine whether the obtained information can be used.

----


### A.3 Formats

**Description**

Specifying formats in data type formatting functions makes it possible to convert numeric and date and time data types to formatted strings and to convert formatted strings to specific data types.

#### A.3.1 Number Format Correspondence

The table below indicates which Oracle database number formats are available in OpenGauss.

**Number format correspondence**

|Number format|TO_CHAR | TO_NUMBER | Remarks|
|:---|:---:|:---:|:---|
|	, (comma)	|	Y	|	Y	|		|
|	. (period)	|	Y	|	Y	|		|
|	$	|	YR	|	YR	|	If a dollar sign ($) is specified in any position other than the first character of a number format, move it to the front of the number format.	|
|	0	|	Y	|	Y	|		|
|	9	|	Y	|	Y	|		|
|	B	|	Y	|	Y	|		|
|	C	|	N	|	N	|		|
|	D	|	Y	|	Y	|		|
|	EEEE	|	Y	|	Y	|		|
|	G	|	Y	|	Y	|		|
|	L	|	Y	|	Y	|		|
|	MI	|	Y	|	Y	|		|
|	PR	|	Y	|	Y	|		|
|	RN	|	Y	|	-	|		|
|	Rn	|	Y	|	-	|		|
|	S	|	Y	|	Y	|		|
|	TM	|	N	|	-	|		|
|	U	|	N	|	N	|		|
|	V	|	Y	|	-	|		|
|	X	|	N	|	N	|		|
|	X	|	N	|	N	|		|

Y: Available

YR: Available with restrictions

N: Cannot be migrated

-: Does not need to be migrated (because it is not available in Oracle databases)

