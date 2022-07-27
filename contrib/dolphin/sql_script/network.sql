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