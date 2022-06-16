Chapter 3 Migrating Functions
---

This chapter explains how to migrate SQL functions.

### 3.1 CONVERT

**Description**

CONVERT converts a string from one character set to another.

**Functional differences**

 - **Oracle database**
    - The string is converted from the character set identified in the third argument to the character set identified in the second argument.
 - **OpenGauss**
     - The string is converted from the character set identified in the second argument to the character set identified in the third argument.

**Migration procedure**

Use the following procedure to perform migration:

 1. Search for the keyword CONVERT and identify where it is used.
 2. Switch the second and third arguments.
 3. Change the character sets in the second and third arguments to names that are valid under the OpenGauss encoding system.

**Migration example**

The example below shows migration when strings are changed to the character set of the target database.

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
<pre><code>SELECT <b>CONVERT( 'abc', 
 'JA16EUC', 
 'AL32UTF8' )</b> 
 FROM DUAL;</code></pre>
</td>

<td align="left">
<pre><code>SELECT <b>CONVERT( CAST( 'abc' AS BYTEA ), 
  'UTF8', 
 'EUC_JP' )</b> 
 FROM DUAL;</code></pre>
</td>
</tr>
</tbody>
</table>

### 3.2 LEAD

**Description**

LEAD obtains the value of the column specified in the arguments from the record that is the specified number of lines below.

**Functional differences**

 - **Oracle database**
     - A NULL value in the column specified in the arguments can be excluded from the calculation.
 - **OpenGauss**
     - A NULL value in the column specified in the arguments cannot be excluded from the calculation.

**Migration procedure**

Use the following procedure to perform migration:

 1. Search for the keyword LEAD and identify where it is used.
 2. If the IGNORE NULLS clause is specified, check the following values to create a subquery that excludes NULL values:
     - Arguments of LEAD (before IGNORE NULLS)
     - Tables targeted by IGNORE NULLS
     - Columns targeted by IGNORE NULLS
     - Columns to be sorted
 3. Change the table in the FROM clause to a subquery to match the format shown below.
 4. Replace LEAD in the select list with MAX. Specify LEAD_IGNLS in the arguments of MAX, and PARTITION BY CNT in the OVER clause.

~~~
FROM ( SELECT columnBeingUsed,
              CASE
               WHEN ignoreNullsTargetColumn IS NOT NULL THEN
                LEAD( leadFunctionArguments )
                 OVER( PARTITION BY NVL2( ignoreNullsTargetColumn, '0', '1' )
                        ORDER BY sortTargetColumn )
              END AS LEAD_IGNLS,
              COUNT( ignoreNullsTargetColumn )
               OVER( ORDER BY sortTargetColumn
                      ROWS BETWEEN 1 FOLLOWING AND UNBOUNDED FOLLOWING )
               AS CNT
        FROM ignoreNullsTargetTable ) AS T1;
~~~

**Migration example**

The example below shows migration when NULL values are not included in the calculation of column values.

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
<pre><code>SELECT staff_id, 
       name, 
       job, 
 <b>LEAD( job, 1 ) IGNORE NULLS 
 OVER( ORDER BY staff_id DESC) 
 AS "LEAD_IGNORE_NULLS"</b> 
 FROM <b>staff_table 
 ORDER BY staff_id DESC</b>;
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
</code></pre>
</td>

<td align="left">
<pre><code>SELECT staff_id, 
       name, 
       job, 
 <b>MAX( LEAD_IGNLS ) 
 OVER( PARTITION BY CNT ) 
 AS "LEAD_IGNORE_NULLS"</b> 
 FROM <b>( SELECT staff_id, 
       name, 
       job, 
 CASE 
 WHEN job IS NOT NULL THEN 
 LEAD( job, 1 ) 
 OVER( PARTITION BY NVL2( 
       job, 
       '0', 
       '1' ) 
 ORDER BY staff_id DESC) 
 END AS LEAD_IGNLS, 
 COUNT( job ) 
 OVER( ORDER BY staff_id DESC 
  ROWS BETWEEN 1 FOLLOWING 
  AND UNBOUNDED FOLLOWING ) 
 AS CNT 
 FROM staff_table ) AS T1 
 ORDER BY staff_id DESC</b>;</code></pre>
</td>
</tr>
</tbody>
</table>

**Information**

----

If the IGNORE NULLS clause is not specified or if the RESPECT NULLS clause is specified, NULL is included in the calculation, so operation is the same as the LEAD function of OpenGauss. Therefore, if the IGNORE NULLS clause is not specified, no changes need to be made. If the RESPECT NULLS clause is specified, delete the RESPECT NULLS clause.

The example below shows migration when RESPECT NULLS is specified in the Oracle database.

**LEAD migration example (when RESPECT NULLS is specified)**

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
<pre><code>SELECT staff_id, 
       name, 
       job, 
 LEAD( job, 1 ) 
 <b>RESPECT NULLS</b> 
 OVER( ORDER BY staff_id DESC ) 
 AS "LEAD_RESPECT_NULLS" 
 FROM staff_table 
 ORDER BY staff_id DESC;</code></pre>
</td>

<td align="left">
<pre><code>SELECT staff_id, 
       name, 
       job, 
 LEAD( job, 1 ) 
 OVER( ORDER BY staff_id DESC ) 
 AS "LEAD_RESPECT_NULLS" 
 FROM staff_table 
 ORDER BY staff_id DESC; 
 </code></pre>
</td>
</tr>
</tbody>
</table>

----

### 3.3 TO_TIMESTAMP

**Description**

TO_TIMESTAMP converts a string value to the TIMESTAMP data type.

**Functional differences**

 - **Oracle database**
     - The language to be used for returning the month and day of the week can be specified.
 - **OpenGauss**
     - The language to be used for returning the month and day of the week cannot be specified.

**Migration procedure**

Use the following procedure to perform migration:

 1. Search for the keyword TO_TIMESTAMP and identify where it is used.
 2. If the third argument of TO_TIMESTAMP is specified, delete it.
 3. If the a string in the first argument contains a national character string, it is replaced with a datetime keyword supported by OpenGauss.

**Migration example**

The example below shows migration when a string value is converted to the TIMESTAMP data type. One string specifies the month in Japanese as a national character, so it is replaced with the date keyword 'JULY'.

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
<pre><code>SELECT TO_TIMESTAMP('2016/**/21 14:15:30', 
        'YYYY/MONTH/DD HH24:MI:SS', 
        'NLS_DATE_LANGUAGE = Japanese') 
 FROM DUAL;</code></pre>
</td>

<td align="left">
<pre><code>SELECT TO_TIMESTAMP('2016/JULY/21 14:15:30', 
        'YYYY/MONTH/DD HH24:MI:SS') 
<br>
 FROM DUAL;</code></pre>
</td>
</tr>
</tbody>
</table>

\*\*: The July in Japanese

