-- tinyblob
DROP TYPE IF EXISTS pg_catalog.tinyblob CASCADE;
DROP TYPE IF EXISTS pg_catalog._tinyblob CASCADE;

CREATE TYPE pg_catalog.tinyblob;

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_rawin(cstring) CASCADE;

CREATE FUNCTION pg_catalog.tinyblob_rawin (
cstring
) RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin', 'tinyblob_rawin';

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_rawout(tinyblob) CASCADE;

CREATE FUNCTION pg_catalog.tinyblob_rawout (
tinyblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'rawout';

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_recv(internal) CASCADE;

CREATE FUNCTION pg_catalog.tinyblob_recv (
internal
) RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'tinyblob_recv';

DROP FUNCTION IF EXISTS pg_catalog.tinyblob_send(tinyblob) CASCADE;

CREATE FUNCTION pg_catalog.tinyblob_send (
tinyblob
) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteasend';

CREATE TYPE pg_catalog.tinyblob (input=tinyblob_rawin, output=tinyblob_rawout, RECEIVE = tinyblob_recv, SEND = tinyblob_send, STORAGE=EXTENDED, category='C');


--mediumblob
DROP TYPE IF EXISTS pg_catalog.mediumblob CASCADE;
DROP TYPE IF EXISTS pg_catalog._mediumblob CASCADE;

CREATE TYPE pg_catalog.mediumblob;

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_rawin(cstring) CASCADE;

CREATE FUNCTION pg_catalog.mediumblob_rawin (
cstring
) RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'mediumblob_rawin';

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_rawout(mediumblob) CASCADE;

CREATE FUNCTION pg_catalog.mediumblob_rawout (
mediumblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'rawout';

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_recv(internal) CASCADE;

CREATE FUNCTION pg_catalog.mediumblob_recv (
internal
) RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'mediumblob_recv';

DROP FUNCTION IF EXISTS pg_catalog.mediumblob_send(mediumblob) CASCADE;

CREATE FUNCTION pg_catalog.mediumblob_send (
mediumblob
) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteasend';

CREATE TYPE pg_catalog.mediumblob (input=mediumblob_rawin, output=mediumblob_rawout, RECEIVE = mediumblob_recv, SEND = mediumblob_send, STORAGE=EXTENDED, category='U');

-- longblob
DROP TYPE IF EXISTS pg_catalog.longblob CASCADE;
DROP TYPE IF EXISTS pg_catalog._longblob CASCADE;

CREATE TYPE pg_catalog.longblob;

DROP FUNCTION IF EXISTS pg_catalog.longblob_rawin(cstring) CASCADE;

CREATE FUNCTION pg_catalog.longblob_rawin (
cstring
) RETURNS longblob LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'longblob_rawin';

DROP FUNCTION IF EXISTS pg_catalog.longblob_rawout(longblob) CASCADE;

CREATE FUNCTION pg_catalog.longblob_rawout (
longblob
) RETURNS cstring LANGUAGE INTERNAL IMMUTABLE STRICT as 'rawout';

DROP FUNCTION IF EXISTS pg_catalog.longblob_recv(internal) CASCADE;

CREATE FUNCTION pg_catalog.longblob_recv (
internal
) RETURNS longblob LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'longblob_recv';

DROP FUNCTION IF EXISTS pg_catalog.longblob_send(longblob) CASCADE;

CREATE FUNCTION pg_catalog.longblob_send (
longblob
) RETURNS bytea LANGUAGE INTERNAL IMMUTABLE STRICT as 'byteasend';

CREATE TYPE pg_catalog.longblob (input=longblob_rawin, output=longblob_rawout, RECEIVE = longblob_recv, SEND = longblob_send, STORAGE=EXTENDED, category='U');

-- to_tinyblob
DROP FUNCTION IF EXISTS pg_catalog.to_tinyblob(longblob) CASCADE;

CREATE FUNCTION pg_catalog.to_tinyblob (
longblob
) RETURNS tinyblob LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'longblob2tinyblob';

-- to_mediumblob
DROP FUNCTION IF EXISTS pg_catalog.to_mediumblob(longblob) CASCADE;

CREATE FUNCTION pg_catalog.to_mediumblob (
longblob
) RETURNS mediumblob LANGUAGE C IMMUTABLE STRICT as '$libdir/b_sql_plugin',  'longblob2mediumblob';

CREATE CAST (tinyblob AS longblob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (mediumblob AS longblob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (blob AS longblob) WITHOUT FUNCTION AS IMPLICIT;

CREATE CAST (blob AS tinyblob) WITH FUNCTION to_tinyblob(longblob) AS IMPLICIT;
CREATE CAST (mediumblob AS tinyblob) WITH FUNCTION to_tinyblob(longblob) AS IMPLICIT;
CREATE CAST (longblob AS tinyblob) WITH FUNCTION to_tinyblob(longblob) AS IMPLICIT;

CREATE CAST (tinyblob AS blob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (mediumblob AS blob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (longblob AS blob) WITHOUT FUNCTION AS IMPLICIT;

CREATE CAST (tinyblob AS mediumblob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (blob AS mediumblob) WITH FUNCTION to_mediumblob(longblob) AS IMPLICIT;
CREATE CAST (longblob AS mediumblob) WITH FUNCTION to_mediumblob(longblob) AS IMPLICIT;

CREATE CAST (tinyblob AS raw) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (mediumblob AS raw) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (longblob AS raw) WITHOUT FUNCTION AS IMPLICIT;

CREATE CAST (raw AS longblob) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (raw AS tinyblob) WITH FUNCTION to_tinyblob(longblob) AS IMPLICIT;
CREATE CAST (raw AS mediumblob) WITH FUNCTION to_mediumblob(longblob) AS IMPLICIT;
