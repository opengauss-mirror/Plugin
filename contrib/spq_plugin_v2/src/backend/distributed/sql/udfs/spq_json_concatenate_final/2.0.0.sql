CREATE FUNCTION pg_catalog.spq_json_concatenate_final(state json)
 RETURNS json
 LANGUAGE SQL
AS $function$
 SELECT CASE WHEN json_typeof(state) = 'null' THEN NULL ELSE state END;
$function$;
