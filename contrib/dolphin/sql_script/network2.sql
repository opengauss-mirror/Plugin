DROP FUNCTION IF EXISTS pg_catalog.inet_aton(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet_ntoa(int) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_aton(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.inet6_ntoa(bytea) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.inet_aton (
text
) RETURNS int8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'inetaton';

CREATE OR REPLACE FUNCTION pg_catalog.inet_ntoa (
int8
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'inetntoa';

CREATE OR REPLACE FUNCTION pg_catalog.inet6_aton (
text
) RETURNS bytea LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'inetpton';

CREATE OR REPLACE FUNCTION pg_catalog.inet6_ntoa (
text
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'inetntop';

CREATE OR REPLACE FUNCTION pg_catalog.inet6_ntoa (
bytea
) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'inetntop';
