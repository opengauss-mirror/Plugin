DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp without time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp without time zone, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ;
DROP FUNCTION IF EXISTS pg_catalog.date_format (timestamp with time zone, text);
CREATE OR REPLACE FUNCTION pg_catalog.date_format (timestamp with time zone, text) RETURNS TEXT LANGUAGE SQL STABLE STRICT as  $$ SELECT pg_catalog.date_format($1::text, $2); $$ ;

CREATE OR REPLACE FUNCTION pg_catalog.db_b_format(number, name) RETURNS text LANGUAGE SQL IMMUTABLE STRICT as 'select db_b_format($1::"any", $2::bigint)';
DROP FUNCTION IF EXISTS pg_catalog.db_b_format(varchar, varchar);
DROP FUNCTION IF EXISTS pg_catalog.db_b_format(unknown, int4);
DROP FUNCTION IF EXISTS pg_catalog.db_b_format(unknown, number);
DROP FUNCTION IF EXISTS pg_catalog.db_b_format(unknown, int8);
DROP FUNCTION IF EXISTS pg_catalog.db_b_format(number, number);

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

DROP OPERATOR IF EXISTS pg_catalog.<>(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.<>(binary, text);
DROP OPERATOR IF EXISTS pg_catalog.>(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.>(binary, text);
DROP OPERATOR IF EXISTS pg_catalog.<(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.<(binary, text);
DROP OPERATOR IF EXISTS pg_catalog.>=(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.>=(binary, text);
DROP OPERATOR IF EXISTS pg_catalog.<=(text, binary);
DROP OPERATOR IF EXISTS pg_catalog.<=(binary, text);

CREATE OR REPLACE FUNCTION pg_catalog.text_binary_eq(text, binary) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT ($1::binary = $2)';

CREATE OR REPLACE FUNCTION pg_catalog.binary_text_eq(binary, text) RETURNS bool LANGUAGE SQL IMMUTABLE STRICT AS
'SELECT ($1 = $2::binary)';

DROP FUNCTION IF EXISTS pg_catalog.text_binary_ne(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_ne(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.text_binary_gt(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_gt(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.text_binary_lt(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_lt(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.text_binary_ge(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_ge(binary, text);
DROP FUNCTION IF EXISTS pg_catalog.text_binary_le(text, binary);
DROP FUNCTION IF EXISTS pg_catalog.binary_text_le(binary, text);
