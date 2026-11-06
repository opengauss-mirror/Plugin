-- Dolphin 5.3 to 6.0 version transition.

CREATE OR REPLACE VIEW public.index_statistic AS
SELECT
  i.namespace AS "namespace",
  (SELECT relname FROM pg_class tc WHERE tc.oid = i.indrelid) AS "table",
  NOT i.indisunique AS "non_unique",
  (CASE WHEN EXISTS (
    SELECT 1 FROM pg_index pi WHERE pi.indexrelid = i.indexrelid AND pi.indisprimary
  ) THEN 'PRIMARY'::name ELSE c.relname END) AS "key_name",
  i.seq_in_index AS "seq_in_index",
  a.attname AS "column_name",
  (CASE WHEN m.amcanorder
    THEN (
      CASE WHEN i.collation & 1 THEN 'D' ELSE 'A' END
    ) ELSE NULL END
  ) AS "collation",
  (SELECT
      (CASE WHEN ts.stadistinct = 0
        THEN NULL ELSE (
          CASE WHEN ts.stadistinct > 0 THEN ts.stadistinct ELSE ts.stadistinct * tc.reltuples * -1 END
        ) END
      )
    FROM pg_class tc
      LEFT JOIN pg_statistic ts ON tc.oid = ts.starelid
    WHERE
      tc.oid = i.indrelid
      AND ts.staattnum = i.attrnum
  ) AS "cardinality",
  NULL AS "sub_part",
  NULL AS "packed",
  (CASE WHEN a.attnotnull THEN '' ELSE 'YES' END) AS "null",
  m.amname AS "index_type",
  (CASE WHEN i.indisusable THEN '' ELSE 'disabled' END) AS "comment",
  (SELECT description FROM pg_description WHERE objoid = i.indexrelid) AS "index_comment"
FROM
  (SELECT * FROM get_index_columns()) i
  LEFT JOIN pg_class c ON c.oid = i.indexrelid
  LEFT JOIN pg_attribute a ON a.attrelid = i.indrelid
  AND a.attnum = i.attrnum
  LEFT JOIN pg_am m ON m.oid = c.relam
ORDER BY
  c.relname;

-- Allow TEXT expressions to be assigned to BLOB columns.
DROP CAST IF EXISTS (text AS blob);
CREATE CAST (text AS blob) WITH FUNCTION pg_catalog.text_to_blob(text) AS ASSIGNMENT;

-- round support for binary/blob types
CREATE OR REPLACE FUNCTION pg_catalog.b_double_round(float8, int8) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'b_double_round';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, binary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, varbinary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, binary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, varbinary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, tinyblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, blob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, mediumblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, longblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, tinyblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, blob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, mediumblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, longblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, tinyblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, blob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, mediumblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, longblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, tinyblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, blob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, mediumblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, longblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, int8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, int8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, int8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, int8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, int8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), $2)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, int8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), $2)';

DROP FUNCTION IF EXISTS pg_catalog.round(binary, int4);
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, int4) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), $2::int8)';
-- cross-type round overloads: blob/binary family x integer/numeric types
DROP FUNCTION IF EXISTS pg_catalog.round(binary, uint4);
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, uint4) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, tinyblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, blob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, mediumblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, longblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, tinyblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, blob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, mediumblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, longblob) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, binary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, varbinary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, binary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, varbinary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, binary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, varbinary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, binary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, varbinary) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, tinyint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, smallint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, uint1) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, uint2) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, uint8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, number) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, tinyint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, smallint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, integer) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, uint1) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, uint2) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, uint4) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, uint8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(varbinary, number) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, tinyint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, smallint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, integer) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, uint1) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, uint2) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, uint4) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, uint8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyblob, number) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, tinyint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, smallint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, integer) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, uint1) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, uint2) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, uint4) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, uint8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(blob, number) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, tinyint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, smallint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, integer) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, uint1) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, uint2) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, uint4) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, uint8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(mediumblob, number) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, tinyint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, smallint) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, integer) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, uint1) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, uint2) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, uint4) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, uint8) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(longblob, number) RETURNS float8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_double_round(cast($1 as float8), cast($2 as number)::int8)';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyint, binary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyint, varbinary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyint, tinyblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyint, blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyint, mediumblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(tinyint, longblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(smallint, binary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(smallint, varbinary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(smallint, tinyblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(smallint, blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(smallint, mediumblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(smallint, longblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(integer, binary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(integer, varbinary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(integer, tinyblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(integer, blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(integer, mediumblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(integer, longblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint1, binary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint1, varbinary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint1, tinyblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint1, blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint1, mediumblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint1, longblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint2, binary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint2, varbinary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint2, tinyblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint2, blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint2, mediumblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint2, longblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint4, binary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint4, varbinary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint4, tinyblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint4, blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint4, mediumblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint4, longblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint8, binary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint8, varbinary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint8, tinyblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint8, blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint8, mediumblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(uint8, longblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(bigint, binary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(bigint, varbinary) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(bigint, tinyblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(bigint, blob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(bigint, mediumblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(bigint, longblob) RETURNS int8 LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as number))::int8';
CREATE OR REPLACE FUNCTION pg_catalog.round(number, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round($1, cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.round(number, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round($1, cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.round(number, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round($1, cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.round(number, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round($1, cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.round(number, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round($1, cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.round(number, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round($1, cast($2 as number))';

CREATE OR REPLACE FUNCTION pg_catalog.b_numeric_mod(numeric, numeric) RETURNS numeric LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'b_numeric_mod';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
-- cross-type mod overloads: blob/binary family x integer/numeric types
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, tinyint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, smallint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, integer) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, uint1) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, uint2) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, uint8) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(binary, number) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, tinyint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, smallint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, integer) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, uint1) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, uint2) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, uint8) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(varbinary, number) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, tinyint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, smallint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, integer) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, bigint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, uint1) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, uint2) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, uint8) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyblob, number) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, tinyint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, smallint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, integer) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, bigint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, uint1) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, uint2) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, uint8) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(blob, number) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, tinyint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, smallint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, integer) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, bigint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, uint1) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, uint2) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, uint8) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(mediumblob, number) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, tinyint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, smallint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, integer) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, bigint) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, uint1) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, uint2) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, uint8) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(longblob, number) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyint, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyint, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyint, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyint, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyint, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(tinyint, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(smallint, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(smallint, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(smallint, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(smallint, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(smallint, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(smallint, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(integer, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(integer, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(integer, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(integer, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(integer, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(integer, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint1, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint1, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint1, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint1, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint1, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint1, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint2, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint2, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint2, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint2, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint2, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint2, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint4, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint4, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint4, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint4, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint4, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint4, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint8, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint8, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint8, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint8, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint8, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(uint8, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(number, binary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(number, varbinary) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(number, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(number, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(number, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(number, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(bigint, tinyblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(bigint, blob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(bigint, mediumblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
CREATE OR REPLACE FUNCTION pg_catalog.mod(bigint, longblob) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.b_numeric_mod(cast($1 as number), cast($2 as number))';
