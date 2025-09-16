-- Enable SSL to encrypt all trafic by default
-- create temporary UDF that has the power to change settings within postgres and drop it
-- after ssl has been setup.
CREATE FUNCTION citus_setup_ssl()
    RETURNS void
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$citus_setup_ssl$$;

DO LANGUAGE plpgsql
$$
BEGIN
 -- setup ssl when postgres is OpenSSL-enabled
 IF current_setting('ssl_ciphers') != 'none' THEN
   PERFORM citus_setup_ssl();
 END IF;
END;
$$;
DROP FUNCTION citus_setup_ssl();
