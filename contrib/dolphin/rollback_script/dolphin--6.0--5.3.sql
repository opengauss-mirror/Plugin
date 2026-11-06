-- Dolphin 6.0 to 5.3 version transition.

-- Exclude catalog columns newer than the rollback target from join aliases.
-- Those columns disappear after switching back to the old server binaries.
SET LOCAL skip_new_column_for_ruledef = true;

CREATE OR REPLACE VIEW public.index_statistic AS
SELECT
  i.namespace AS "namespace",
  (SELECT relname FROM pg_class tc WHERE tc.oid = i.indrelid) AS "table",
  NOT i.indisunique AS "non_unique",
  c.relname AS "key_name",
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

-- Restore explicit-only TEXT to BLOB conversion.
DROP CAST IF EXISTS (text AS blob);
CREATE CAST (text AS blob) WITH FUNCTION pg_catalog.text_to_blob(text);

-- drop round support for binary/blob types
DROP FUNCTION IF EXISTS pg_catalog.round(binary, int8);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, int8);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, int8);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, int8);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, smallint);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, uint1);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, uint2);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, uint8);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, number);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, smallint);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, integer);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, uint1);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, uint2);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, uint4);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, uint8);
DROP FUNCTION IF EXISTS pg_catalog.round(varbinary, number);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, smallint);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, integer);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, uint1);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, uint2);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, uint4);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyblob, number);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, smallint);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, integer);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, uint1);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, uint2);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, uint4);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.round(blob, number);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, smallint);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, integer);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, uint1);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, uint2);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, uint4);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.round(mediumblob, number);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, smallint);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, integer);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, uint1);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, uint2);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, uint4);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.round(longblob, number);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyint, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyint, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyint, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyint, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyint, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(tinyint, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(smallint, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(smallint, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(smallint, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(smallint, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(smallint, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(smallint, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(integer, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(integer, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(integer, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(integer, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(integer, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(integer, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint1, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(uint1, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(uint1, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint1, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint1, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint1, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint2, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(uint2, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(uint2, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint2, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint2, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint2, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint4, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(uint4, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(uint4, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint4, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint4, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint4, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint8, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(uint8, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(uint8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint8, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(uint8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(bigint, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(bigint, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(bigint, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(bigint, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(bigint, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(bigint, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(number, binary);
DROP FUNCTION IF EXISTS pg_catalog.round(number, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.round(number, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.round(number, blob);
DROP FUNCTION IF EXISTS pg_catalog.round(number, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.round(number, longblob);
DROP FUNCTION IF EXISTS pg_catalog.round(binary, uint4);
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, uint4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), cast($2 as int4))';
DROP FUNCTION IF EXISTS pg_catalog.b_double_round(float8, int8);

DROP FUNCTION IF EXISTS pg_catalog.round(binary, int4);
CREATE OR REPLACE FUNCTION pg_catalog.round(binary, int4) RETURNS number LANGUAGE SQL IMMUTABLE STRICT as 'select pg_catalog.round(cast($1 as number), $2)';

DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, smallint);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, integer);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, uint1);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, uint2);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, uint4);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, uint8);
DROP FUNCTION IF EXISTS pg_catalog.mod(binary, number);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, smallint);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, integer);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, uint1);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, uint2);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, uint4);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, uint8);
DROP FUNCTION IF EXISTS pg_catalog.mod(varbinary, number);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, smallint);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, integer);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, bigint);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, uint1);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, uint2);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, uint4);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyblob, number);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, smallint);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, integer);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, bigint);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, uint1);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, uint2);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, uint4);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.mod(blob, number);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, smallint);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, integer);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, bigint);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, uint1);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, uint2);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, uint4);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.mod(mediumblob, number);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, tinyint);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, smallint);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, integer);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, bigint);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, uint1);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, uint2);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, uint4);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, uint8);
DROP FUNCTION IF EXISTS pg_catalog.mod(longblob, number);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyint, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyint, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyint, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyint, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyint, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(tinyint, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(smallint, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(smallint, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(smallint, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(smallint, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(smallint, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(smallint, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(integer, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(integer, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(integer, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(integer, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(integer, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(integer, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint1, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint1, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint1, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint1, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint1, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint1, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint2, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint2, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint2, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint2, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint2, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint2, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint4, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint4, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint4, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint4, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint4, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint4, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint8, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint8, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint8, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint8, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint8, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(uint8, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(number, binary);
DROP FUNCTION IF EXISTS pg_catalog.mod(number, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.mod(number, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(number, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(number, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(number, longblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(bigint, tinyblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(bigint, blob);
DROP FUNCTION IF EXISTS pg_catalog.mod(bigint, mediumblob);
DROP FUNCTION IF EXISTS pg_catalog.mod(bigint, longblob);
DROP FUNCTION IF EXISTS pg_catalog.b_numeric_mod(numeric, numeric);
