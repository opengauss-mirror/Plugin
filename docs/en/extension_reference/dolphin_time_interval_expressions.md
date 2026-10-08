# Time Interval Expressions

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:22:22.561Z pushedAt=2026-09-12T03:52:13.549Z -->

- interval_expr is a time interval expression, and its syntax is as follows:

```
INTERVAL expr unit
```

NOTE
expr is an expression, and unit is the unit of expr. For the allowed values of unit and the corresponding expr constant value formats, see [Table 1](#intervalexpr_unit_expr_tbl). expr supports arithmetic expressions, column references, and bind parameter usage.

NOTE
If the number of components in the expr value is fewer than that in the standard form corresponding to unit, the components of the expr value are filled from right to left into the standard form corresponding to unit. For example, for the time unit DAY_SECOND, if expr is '1 10', the final time interval value is '0 00:01:10'.

Time interval expressions are typically used in certain time calculation functions, such as DATE_ADD() and DATE_SUB().

**Table 1** Value of unit and corresponding expr format in temporal interval expressions

<a name="intervalexpr_unit_expr_tbl"></a>
<table>
<thead align="left">
<tr>
<th class="cellrowborder" valign="top" width="50%"><p>unit Value</p></th>
<th class="cellrowborder" valign="top" width="50%"><p>expr Value Format</p></th>
</tr>
</thead>
<tbody>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>MICROSECOND</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>MICROSECONDS</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>SECOND</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>SECONDS</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>MINUTE</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>MINUTES</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>HOUR</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>HOURS</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>DAY</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>DAYS</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>WEEK</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>WEEKS</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>MONTH</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>MONTHS</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>QUARTER</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>QUARTERS</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>YEAR</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>YEARS</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>SECOND_MICROSECOND</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'SECONDS.MICROSECONDS'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>MINUTE_MICROSECOND</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'MINUTES:SECONDS.MICROSECONDS'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>MINUTE_SECOND</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'MINUTES:SECONDS'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>HOUR_MICROSECOND</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'HOURS:MINUTES:SECONDS.MICROSECONDS'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>HOUR_SECOND</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'HOURS:MINUTES:SECONDS'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>HOUR_MINUTE</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'HOURS:MINUTES'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>DAY_MICROSECOND</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'DAYS HOURS:MINUTES:SECONDS.MICROSECONDS'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>DAY_SECOND</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'DAYS HOURS:MINUTES:SECONDS'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>DAY_MINUTE</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'DAYS HOURS:MINUTES'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>DAY_HOUR</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'DAYS HOURS'</p></td>
</tr>
<tr>
<td class="cellrowborder" valign="top" width="50%"><p>YEAR_MONTH</p></td>
<td class="cellrowborder" valign="top" width="50%"><p>'YEARS-MONTHS'</p></td>
</tr>
</tbody>
</table>

## Examples

```
openGauss=# create database test_db dbcompatibility 'B';
CREATE DATABASE
openGauss=# \c test_db 
test_db=# set dolphin.b_compatibility_mode to on;
SET
test_db=# select date_add('1997-12-31 23:59:59',INTERVAL 1 MICROSECOND);
          date_add          
----------------------------
 1997-12-31 23:59:59.000001
(1 row)

test_db=# select date_add('1997-12-31 23:59:59',INTERVAL '1.111' SECOND_MICROSECOND);
        date_add         
-------------------------
 1998-01-01 00:00:00.111
(1 row)

test_db=# select '1997-12-11' + interval '1 10' day_second;
      ?column?       
---------------------
 1997-12-11 00:01:10
(1 row)

-- Arithmetic expression.
test_db=# select '1997-12-11' + interval 1 + 2 * 3 day;
  ?column?  
------------
 1997-12-18
(1 row)

-- Column reference.
test_db=# create table t1 (c1 int);
CREATE TABLE
test_db=# insert into t1 values(1);
INSERT 0 1
test_db=# select '1997-12-11' + interval c1 + 2 year from t1;
  ?column?  
------------
 2000-12-11
(1 row)

-- Bind parameter.
test_db=# prepare stmt as 'select ? + interval ? hour';
PREPARE
test_db=# execute stmt('1997-12-11', 20);
      ?column?       
---------------------
 1997-12-11 20:00:00
(1 row)

```