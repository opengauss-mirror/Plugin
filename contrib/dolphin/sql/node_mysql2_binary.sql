\pset format unaligned
-- node-mysql2 query serializes an empty Buffer as the bare literal X''.
SELECT octet_length(X''::blob) AS empty_blob,
       octet_length(X''::tinyblob) AS empty_tinyblob,
       octet_length(X''::mediumblob) AS empty_mediumblob,
       octet_length(X''::longblob) AS empty_longblob;
SELECT octet_length(X'00'::blob) AS nul_length, hex(X'00'::blob) AS nul_hex;
SELECT octet_length(X'0011ff'::blob) AS bytes_length, hex(X'0011ff'::blob) AS bytes_hex;
SELECT X''::blob IS NULL AS empty_is_null, NULL::blob IS NULL AS null_is_null;
