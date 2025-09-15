CREATE FUNCTION pg_catalog.spq_jsonb_concatenate_final(state jsonb)
 RETURNS jsonb
 LANGUAGE SQL
AS $function$
 SELECT CASE WHEN jsonb_typeof(state) = 'null' THEN NULL ELSE state END;
$function$;

