CREATE FUNCTION pg_catalog.spq_json_concatenate(state json, val json)
 RETURNS json
 LANGUAGE SQL
AS $function$
 SELECT CASE
  WHEN val IS NULL THEN state
  WHEN json_typeof(state) = 'null' THEN val
  WHEN json_typeof(state) = 'object' THEN
    (SELECT json_object_agg(key, value) FROM (
     SELECT * FROM json_each(state)
     UNION ALL
     SELECT * FROM json_each(val)
    ) t)
  ELSE
    (SELECT json_agg(a) FROM (
     SELECT json_array_elements(state) AS a
     UNION ALL
     SELECT json_array_elements(val) AS a
    ) t)
 END;
$function$;
