DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv6(text) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(inet) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv6(inet) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4(cstring) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv6(cstring) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4(
text
) RETURNS int4 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'network_is_ipv4';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv6(
text
) RETURNS int4 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'network_is_ipv6';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4(
inet
) RETURNS int4 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'network_is_ipv4';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv6(
inet
) RETURNS int4 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'network_is_ipv6';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4(
cstring
) RETURNS int4 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'network_is_ipv4';
CREATE OR REPLACE FUNCTION pg_catalog.is_ipv6(
cstring
) RETURNS int4 LANGUAGE C IMMUTABLE as '$libdir/dolphin', 'network_is_ipv6';

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

DROP FUNCTION IF EXISTS pg_catalog.is_ipv4_compat(bytea) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.is_ipv4_mapped(bytea) CASCADE;

CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4_compat (
bytea
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'is_v4compat';

CREATE OR REPLACE FUNCTION pg_catalog.is_ipv4_mapped (
bytea
) RETURNS int LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'is_v4mapped';
