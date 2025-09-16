DO $proc$
BEGIN
  EXECUTE $$
    CREATE AGGREGATE array_cat_agg(anyarray) (SFUNC = array_cat, STYPE = anyarray);
    COMMENT ON AGGREGATE array_cat_agg(anyarray) IS 'concatenate input arrays into a single array';
  $$;
END$proc$;
