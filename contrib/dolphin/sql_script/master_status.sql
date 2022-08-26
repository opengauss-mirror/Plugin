DROP FUNCTION IF EXISTS pg_catalog.gs_master_status(OUT "Xlog_File_Name" text, OUT "Xlog_File_Offset" int4, OUT "Xlog_Lsn" text) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.gs_master_status (
OUT "Xlog_File_Name" text, OUT "Xlog_File_Offset" int4, OUT "Xlog_Lsn" text
) RETURNS setof record LANGUAGE C VOLATILE STRICT as '$libdir/dolphin', 'gs_master_status';