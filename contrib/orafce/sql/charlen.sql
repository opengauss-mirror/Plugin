--
-- test LPAD family of functions
--

/* cases where one or more arguments are of type CHAR */
SELECT '|' || oracle.lpad('あbcd'::char(8), 10) || '|';
SELECT '|' || oracle.lpad('あbcd'::char(8),  5) || '|';
SELECT '|' || oracle.lpad('あbcd'::char(8), 1) || '|';

SELECT '|' || oracle.lpad('あbcd'::char(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.lpad('あbcd'::char(5),  5, 'xい'::char(3)) || '|';

SELECT '|' || oracle.lpad('あbcd'::char(5), 10, 'xい'::text) || '|';
SELECT '|' || oracle.lpad('あbcd'::char(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.lpad('あbcd'::char(5), 10, 'xい'::nvarchar2(3)) || '|';

SELECT '|' || oracle.lpad('あbcd'::text, 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.lpad('あbcd'::text,  5, 'xい'::char(3)) || '|';

SELECT '|' || oracle.lpad('あbcd'::varchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.lpad('あbcd'::varchar2(5),  5, 'xい'::char(3)) || '|';

SELECT '|' || oracle.lpad('あbcd'::nvarchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.lpad('あbcd'::nvarchar2(5),  5, 'xい'::char(3)) || '|';

/* test oracle.lpad(text, int [, text]) */
SELECT '|' || oracle.lpad('あbcd'::text, 10) || '|';
SELECT '|' || oracle.lpad('あbcd'::text,  5) || '|';

SELECT '|' || oracle.lpad('あbcd'::varchar2(10), 10) || '|';
SELECT '|' || oracle.lpad('あbcd'::varchar2(10), 5) || '|';

SELECT '|' || oracle.lpad('あbcd'::nvarchar2(10), 10) || '|';
SELECT '|' || oracle.lpad('あbcd'::nvarchar2(10), 5) || '|';

SELECT '|' || oracle.lpad('あbcd'::text, 10, 'xい'::text) || '|';
SELECT '|' || oracle.lpad('あbcd'::text, 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.lpad('あbcd'::text, 10, 'xい'::nvarchar2(3)) || '|';

SELECT '|' || oracle.lpad('あbcd'::varchar2(5), 10, 'xい'::text) || '|';
SELECT '|' || oracle.lpad('あbcd'::varchar2(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.lpad('あbcd'::varchar2(5), 10, 'xい'::nvarchar2(5)) || '|';

SELECT '|' || oracle.lpad('あbcd'::nvarchar2(5), 10, 'xい'::text) || '|';
SELECT '|' || oracle.lpad('あbcd'::nvarchar2(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.lpad('あbcd'::nvarchar2(5), 10, 'xい'::nvarchar2(5)) || '|';

--
-- test RPAD family of functions
--

/* cases where one or more arguments are of type CHAR */
SELECT '|' || oracle.rpad('あbcd'::char(8), 10) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(8),  5) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(8), 1) || '|';

SELECT '|' || oracle.rpad('あbcd'::char(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5),  5, 'xい'::char(3)) || '|';

SELECT '|' || oracle.rpad('あbcd'::char(5), 10, 'xい'::text) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.rpad('あbcd'::char(5), 10, 'xい'::nvarchar2(3)) || '|';

SELECT '|' || oracle.rpad('あbcd'::text, 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::text,  5, 'xい'::char(3)) || '|';

SELECT '|' || oracle.rpad('あbcd'::varchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::varchar2(5),  5, 'xい'::char(3)) || '|';

SELECT '|' || oracle.rpad('あbcd'::nvarchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || oracle.rpad('あbcd'::nvarchar2(5),  5, 'xい'::char(3)) || '|';

/* test oracle.lpad(text, int [, text]) */
SELECT '|' || oracle.rpad('あbcd'::text, 10) || '|';
SELECT '|' || oracle.rpad('あbcd'::text,  5) || '|';

SELECT '|' || oracle.rpad('あbcd'::varchar2(10), 10) || '|';
SELECT '|' || oracle.rpad('あbcd'::varchar2(10), 5) || '|';

SELECT '|' || oracle.rpad('あbcd'::nvarchar2(10), 10) || '|';
SELECT '|' || oracle.rpad('あbcd'::nvarchar2(10), 5) || '|';

SELECT '|' || oracle.rpad('あbcd'::text, 10, 'xい'::text) || '|';
SELECT '|' || oracle.rpad('あbcd'::text, 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.rpad('あbcd'::text, 10, 'xい'::nvarchar2(3)) || '|';

SELECT '|' || oracle.rpad('あbcd'::varchar2(5), 10, 'xい'::text) || '|';
SELECT '|' || oracle.rpad('あbcd'::varchar2(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.rpad('あbcd'::varchar2(5), 10, 'xい'::nvarchar2(5)) || '|';

SELECT '|' || oracle.rpad('あbcd'::nvarchar2(5), 10, 'xい'::text) || '|';
SELECT '|' || oracle.rpad('あbcd'::nvarchar2(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || oracle.rpad('あbcd'::nvarchar2(5), 10, 'xい'::nvarchar2(5)) || '|';

--
-- test oracle.length()
--

/* test that trailing blanks are not ignored */
SELECT oracle.length('あbb'::char(6));
SELECT oracle.length(''::char(6));
