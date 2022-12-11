DROP FUNCTION IF EXISTS pg_catalog.bit_count(numeric) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (numeric)  RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_numeric';
DROP FUNCTION IF EXISTS pg_catalog.bit_count(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (text)  RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_text';
DROP FUNCTION IF EXISTS pg_catalog.bit_count_bit(text) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count_bit (text) RETURNS int8 LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'bit_count_bit';
DROP FUNCTION IF EXISTS pg_catalog.bit_count(bit) CASCADE;
CREATE OR REPLACE FUNCTION pg_catalog.bit_count (bit) RETURNS int8 AS $$ SELECT pg_catalog.bit_count_bit(cast($1 as text)) $$ LANGUAGE SQL;

-- support bit_xor aggregate for varbit

DROP FUNCTION IF EXISTS pg_catalog.varbit_bit_xor_transfn(varbit, varbit) CASCADE;
CREATE FUNCTION pg_catalog.varbit_bit_xor_transfn (
t1 varbit ,t2 varbit
) RETURNS varbit AS
$$
BEGIN
    IF t1 is null THEN
        RETURN t2;
    ELSEIF t2 is null THEN
        RETURN t1;
    ELSE
        RETURN (SELECT bitxor(t1, t2));
    END IF;
END;
$$
LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS pg_catalog.varbit_bit_xor_finalfn(varbit) CASCADE;
CREATE FUNCTION pg_catalog.varbit_bit_xor_finalfn (
t varbit
) RETURNS int8 AS
$$
BEGIN
    IF t is null THEN
        RETURN 0;
    ELSE
        RETURN (SELECT int8(t));
    END IF;
END;
$$
LANGUAGE plpgsql;

drop aggregate if exists pg_catalog.bit_xor(varbit);
create aggregate pg_catalog.bit_xor(varbit) (SFUNC=varbit_bit_xor_transfn, finalfunc = varbit_bit_xor_finalfn ,STYPE= varbit);
