DROP FUNCTION IF EXISTS pg_catalog.enum2float8(anyenum) cascade;
CREATE OR REPLACE FUNCTION pg_catalog.enum2float8(anyenum) RETURNS float8 LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin',  'Enum2Float8';

do $$
DECLARE
ans boolean;
BEGIN
    for ans in select case when count(*)=1 then true else false end as ans  from (select * from pg_catalog.pg_cast where castsource=3500 and casttarget=701)
    LOOP
        if ans = true then
            UPDATE pg_catalog.pg_cast SET castfunc = (SELECT oid FROM pg_proc WHERE proname = 'enum2float8' AND pronamespace = 11 AND proargtypes='3500' AND prorettype = 701),castcontext='i',castmethod='f',castowner=10;
        else
            INSERT INTO pg_catalog.pg_cast
            -- castsource is anyenum(3500), casttarget(701) is float8, castowner is 10(superuser)
            SELECT 3500, 701, oid, 'i', 'f', 10
            FROM pg_proc WHERE 
            proname = 'enum2float8' AND
            -- namespace is pg_catalog
            pronamespace = 11 AND
            -- input arg is anyenum
            proargtypes='3500' AND
            -- return type is float8
            prorettype = 701;
        end if;
        exit;
    END LOOP;
END$$;

DROP FUNCTION IF EXISTS pg_catalog.enumtextlt(anyenum, text) CASCADE; 
CREATE FUNCTION pg_catalog.enumtextlt ( 
anyenum, text 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextlt';
DROP FUNCTION IF EXISTS pg_catalog.enumtextle(anyenum, text) CASCADE; 
CREATE FUNCTION pg_catalog.enumtextle ( 
anyenum, text 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextle';
DROP FUNCTION IF EXISTS pg_catalog.enumtextne(anyenum, text) CASCADE; 
CREATE FUNCTION pg_catalog.enumtextne ( 
anyenum, text 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextne';
DROP FUNCTION IF EXISTS pg_catalog.enumtexteq(anyenum, text) CASCADE; 
CREATE FUNCTION pg_catalog.enumtexteq ( 
anyenum, text 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtexteq';
DROP FUNCTION IF EXISTS pg_catalog.enumtextgt(anyenum, text) CASCADE; 
CREATE FUNCTION pg_catalog.enumtextgt ( 
anyenum, text 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextgt';
DROP FUNCTION IF EXISTS pg_catalog.enumtextge(anyenum, text) CASCADE; 
CREATE FUNCTION pg_catalog.enumtextge ( 
anyenum, text 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'enumtextge';
DROP FUNCTION IF EXISTS pg_catalog.textenumlt(text, anyenum) CASCADE; 
CREATE FUNCTION pg_catalog.textenumlt ( 
text, anyenum 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumlt';
DROP FUNCTION IF EXISTS pg_catalog.textenumle(text, anyenum) CASCADE; 
CREATE FUNCTION pg_catalog.textenumle ( 
text, anyenum 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumle';
DROP FUNCTION IF EXISTS pg_catalog.textenumne(text, anyenum) CASCADE; 
CREATE FUNCTION pg_catalog.textenumne ( 
text, anyenum 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumne';
DROP FUNCTION IF EXISTS pg_catalog.textenumeq(text, anyenum) CASCADE; 
CREATE FUNCTION pg_catalog.textenumeq ( 
text, anyenum 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumeq';
DROP FUNCTION IF EXISTS pg_catalog.textenumgt(text, anyenum) CASCADE; 
CREATE FUNCTION pg_catalog.textenumgt ( 
text, anyenum 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumgt';
DROP FUNCTION IF EXISTS pg_catalog.textenumge(text, anyenum) CASCADE; 
CREATE FUNCTION pg_catalog.textenumge ( 
text, anyenum 
) RETURNS bool LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'textenumge';

CREATE OPERATOR pg_catalog.=( 
 PROCEDURE = enumtexteq, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.=), NEGATOR = OPERATOR(pg_catalog.<>), RESTRICT = eqsel, JOIN = eqjoinsel, HASHES
);

CREATE OPERATOR pg_catalog.<>( 
 PROCEDURE = enumtextne, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.<>), NEGATOR = OPERATOR(pg_catalog.=), RESTRICT = neqsel, JOIN = neqjoinsel
);

CREATE OPERATOR pg_catalog.<( 
 PROCEDURE = enumtextlt, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.>), NEGATOR = OPERATOR(pg_catalog.>=), RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);

CREATE OPERATOR pg_catalog.>( 
 PROCEDURE = enumtextgt, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.<), NEGATOR = OPERATOR(pg_catalog.<=), RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);

CREATE OPERATOR pg_catalog.<=( 
 PROCEDURE = enumtextle, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.>=), NEGATOR = OPERATOR(pg_catalog.>), RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);

CREATE OPERATOR pg_catalog.>=( 
 PROCEDURE = enumtextge, LEFTARG = anyenum, RIGHTARG = text, COMMUTATOR = OPERATOR(pg_catalog.<=), NEGATOR = OPERATOR(pg_catalog.<), RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);

CREATE OPERATOR pg_catalog.=( 
 PROCEDURE = textenumeq, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.=), NEGATOR = OPERATOR(pg_catalog.<>), RESTRICT = eqsel, JOIN = eqjoinsel, HASHES
);

CREATE OPERATOR pg_catalog.<>( 
 PROCEDURE = textenumne, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.<>), NEGATOR = OPERATOR(pg_catalog.=), RESTRICT = neqsel, JOIN = neqjoinsel
);

CREATE OPERATOR pg_catalog.<( 
 PROCEDURE = textenumlt, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.>), NEGATOR = OPERATOR(pg_catalog.>=), RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);

CREATE OPERATOR pg_catalog.>( 
 PROCEDURE = textenumgt, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.<), NEGATOR = OPERATOR(pg_catalog.<=), RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);

CREATE OPERATOR pg_catalog.<=( 
 PROCEDURE = textenumle, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.>=), NEGATOR = OPERATOR(pg_catalog.>), RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);

CREATE OPERATOR pg_catalog.>=( 
 PROCEDURE = textenumge, LEFTARG = text, RIGHTARG = anyenum, COMMUTATOR = OPERATOR(pg_catalog.<=), NEGATOR = OPERATOR(pg_catalog.<), RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);
