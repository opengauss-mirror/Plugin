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
