\set ECHO none
SET client_min_messages = warning;
DROP DATABASE IF EXISTS pg_test;
CREATE DATABASE pg_test dbcompatibility 'PG';
\c pg_test
SET DATESTYLE TO ISO;
SET client_encoding = utf8;
\pset null '<NULL>'
\set ECHO all

SET client_min_messages = error;
CREATE EXTENSION orafce;
SET client_min_messages = default;

-- returns 1 (start of the source string)
SELECT strposb('ABCありがとう', '');
-- returns 0
SELECT lengthb('');
/* cases where one or more arguments are of type CHAR */
/*
SELECT '|' || oracle.lpad('あbcd'::char(8), 10) || '|';
SELECT '|' || oracle.lpad('あbcd'::char(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.lpad('あbcd'::char(5),  5, 'xい'::char(3)) || '|';

SELECT '|' || oracle.lpad('あbcd'::char(5), 10, 'xい'::text) || '|';
SELECT '|' || oracle.lpad('あbcd'::char(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.lpad('あbcd'::char(5), 10, 'xい'::nvarchar2(3)) || '|';

SELECT '|' || oracle.lpad('あbcd'::text, 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.lpad('あbcd'::nvarchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5),  5, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5),  5, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5), 10, 'xい'::text) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5), 10, 'xい'::text) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5), 10, 'xい'::nvarchar2(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::text, 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::varchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::nvarchar2(5), 10, 'xい'::char(3)) || '|';
*/

CREATE TABLE dbms_output_test (buff VARCHAR(20), status INTEGER);

CREATE FUNCTION dbms_output_test() RETURNS VOID AS $$
DECLARE
	buff1	VARCHAR(20) := 'orafce';
BEGIN
	PERFORM DBMS_OUTPUT.DISABLE();
	PERFORM DBMS_OUTPUT.ENABLE();
	PERFORM DBMS_OUTPUT.SERVEROUTPUT ('t');
	PERFORM DBMS_OUTPUT.PUT_LINE ('ORAFCE');
	PERFORM DBMS_OUTPUT.PUT_LINE (buff1);
	PERFORM DBMS_OUTPUT.PUT ('ABC');
	PERFORM DBMS_OUTPUT.PUT_LINE ('');
END;
$$ LANGUAGE plpgsql;
SELECT dbms_output_test();
DROP FUNCTION dbms_output_test();

CREATE FUNCTION dbms_output_test() RETURNS VOID AS $$
DECLARE
	buff1	VARCHAR(20) := 'ora';
	buff2	VARCHAR(20) := 'f';
	buff3	VARCHAR(20) := 'ce';
BEGIN
	PERFORM DBMS_OUTPUT.DISABLE();
	PERFORM DBMS_OUTPUT.ENABLE();
	PERFORM DBMS_OUTPUT.SERVEROUTPUT ('t');
	PERFORM DBMS_OUTPUT.PUT ('ORA');
	PERFORM DBMS_OUTPUT.PUT ('F');
	PERFORM DBMS_OUTPUT.PUT ('CE');
	PERFORM DBMS_OUTPUT.PUT_LINE ('');
	PERFORM DBMS_OUTPUT.PUT ('ABC');
	PERFORM DBMS_OUTPUT.PUT_LINE ('');
END;
$$ LANGUAGE plpgsql;
SELECT dbms_output_test();
DROP FUNCTION dbms_output_test();

CREATE FUNCTION dbms_output_test() RETURNS VOID AS $$
BEGIN
	PERFORM DBMS_OUTPUT.DISABLE();
	PERFORM DBMS_OUTPUT.ENABLE();
	PERFORM DBMS_OUTPUT.SERVEROUTPUT ('t');
	PERFORM DBMS_OUTPUT.PUT ('ORA
F
CE');
	PERFORM DBMS_OUTPUT.PUT_LINE ('');
END;
$$ LANGUAGE plpgsql;
SELECT dbms_output_test();
DROP FUNCTION dbms_output_test();

SELECT oracle.REGEXP_LIKE('1234', '\d+', NULL);
SELECT oracle.REGEXP_LIKE('1234', '\d+', '');
select plvsubst.string('My name is %s.', '');
select plvsubst.string('My name is empty.', '');
select 1 = instr('abc', '');
select oracle.substr('TechOnTheNet', -8, 0) =  '';
