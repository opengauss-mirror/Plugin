CREATE OR REPLACE FUNCTION pg_catalog.export_set (numeric, text, text, text, numeric) RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_5args';
CREATE OR REPLACE FUNCTION pg_catalog.export_set (numeric, text, text, text) RETURNS text LANGUAGE C STABLE STRICT  as '$libdir/dolphin', 'export_set_4args';
CREATE OR REPLACE FUNCTION pg_catalog.export_set (numeric, text, text)  RETURNS text LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'export_set_3args';
