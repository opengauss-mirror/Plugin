--
-- test LPAD family of functions
--

/* cases where one or more arguments are of type CHAR */
SELECT '|' || orafce.lpad('あbcd'::char(8), 10) || '|';
SELECT '|' || orafce.lpad('あbcd'::char(8),  5) || '|';
SELECT '|' || orafce.lpad('あbcd'::char(8), 1) || '|';

SELECT '|' || orafce.lpad('あbcd'::char(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || orafce.lpad('あbcd'::char(5),  5, 'xい'::char(3)) || '|';

SELECT '|' || orafce.lpad('あbcd'::char(5), 10, 'xい'::text) || '|';
SELECT '|' || orafce.lpad('あbcd'::char(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || orafce.lpad('あbcd'::char(5), 10, 'xい'::nvarchar2(3)) || '|';

SELECT '|' || orafce.lpad('あbcd'::text, 10, 'xい'::char(3)) || '|';
SELECT '|' || orafce.lpad('あbcd'::text,  5, 'xい'::char(3)) || '|';

SELECT '|' || orafce.lpad('あbcd'::varchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || orafce.lpad('あbcd'::varchar2(5),  5, 'xい'::char(3)) || '|';

SELECT '|' || orafce.lpad('あbcd'::nvarchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || orafce.lpad('あbcd'::nvarchar2(5),  5, 'xい'::char(3)) || '|';

/* test orafce.lpad(text, int [, text]) */
SELECT '|' || orafce.lpad('あbcd'::text, 10) || '|';
SELECT '|' || orafce.lpad('あbcd'::text,  5) || '|';

SELECT '|' || orafce.lpad('あbcd'::varchar2(10), 10) || '|';
SELECT '|' || orafce.lpad('あbcd'::varchar2(10), 5) || '|';

SELECT '|' || orafce.lpad('あbcd'::nvarchar2(10), 10) || '|';
SELECT '|' || orafce.lpad('あbcd'::nvarchar2(10), 5) || '|';

SELECT '|' || orafce.lpad('あbcd'::text, 10, 'xい'::text) || '|';
SELECT '|' || orafce.lpad('あbcd'::text, 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || orafce.lpad('あbcd'::text, 10, 'xい'::nvarchar2(3)) || '|';

SELECT '|' || orafce.lpad('あbcd'::varchar2(5), 10, 'xい'::text) || '|';
SELECT '|' || orafce.lpad('あbcd'::varchar2(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || orafce.lpad('あbcd'::varchar2(5), 10, 'xい'::nvarchar2(5)) || '|';

SELECT '|' || orafce.lpad('あbcd'::nvarchar2(5), 10, 'xい'::text) || '|';
SELECT '|' || orafce.lpad('あbcd'::nvarchar2(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || orafce.lpad('あbcd'::nvarchar2(5), 10, 'xい'::nvarchar2(5)) || '|';

--
-- test RPAD family of functions
--

/* cases where one or more arguments are of type CHAR */
SELECT '|' || orafce.rpad('あbcd'::char(8), 10) || '|';
SELECT '|' || orafce.rpad('あbcd'::char(8),  5) || '|';
SELECT '|' || orafce.rpad('あbcd'::char(8), 1) || '|';

SELECT '|' || orafce.rpad('あbcd'::char(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || orafce.rpad('あbcd'::char(5),  5, 'xい'::char(3)) || '|';

SELECT '|' || orafce.rpad('あbcd'::char(5), 10, 'xい'::text) || '|';
SELECT '|' || orafce.rpad('あbcd'::char(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || orafce.rpad('あbcd'::char(5), 10, 'xい'::nvarchar2(3)) || '|';

SELECT '|' || orafce.rpad('あbcd'::text, 10, 'xい'::char(3)) || '|';
SELECT '|' || orafce.rpad('あbcd'::text,  5, 'xい'::char(3)) || '|';

SELECT '|' || orafce.rpad('あbcd'::varchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || orafce.rpad('あbcd'::varchar2(5),  5, 'xい'::char(3)) || '|';

SELECT '|' || orafce.rpad('あbcd'::nvarchar2(5), 10, 'xい'::char(3)) || '|';
SELECT '|' || orafce.rpad('あbcd'::nvarchar2(5),  5, 'xい'::char(3)) || '|';

/* test orafce.lpad(text, int [, text]) */
SELECT '|' || orafce.rpad('あbcd'::text, 10) || '|';
SELECT '|' || orafce.rpad('あbcd'::text,  5) || '|';

SELECT '|' || orafce.rpad('あbcd'::varchar2(10), 10) || '|';
SELECT '|' || orafce.rpad('あbcd'::varchar2(10), 5) || '|';

SELECT '|' || orafce.rpad('あbcd'::nvarchar2(10), 10) || '|';
SELECT '|' || orafce.rpad('あbcd'::nvarchar2(10), 5) || '|';

SELECT '|' || orafce.rpad('あbcd'::text, 10, 'xい'::text) || '|';
SELECT '|' || orafce.rpad('あbcd'::text, 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || orafce.rpad('あbcd'::text, 10, 'xい'::nvarchar2(3)) || '|';

SELECT '|' || orafce.rpad('あbcd'::varchar2(5), 10, 'xい'::text) || '|';
SELECT '|' || orafce.rpad('あbcd'::varchar2(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || orafce.rpad('あbcd'::varchar2(5), 10, 'xい'::nvarchar2(5)) || '|';

SELECT '|' || orafce.rpad('あbcd'::nvarchar2(5), 10, 'xい'::text) || '|';
SELECT '|' || orafce.rpad('あbcd'::nvarchar2(5), 10, 'xい'::varchar2(5)) || '|';
SELECT '|' || orafce.rpad('あbcd'::nvarchar2(5), 10, 'xい'::nvarchar2(5)) || '|';

--
-- test orafce.length()
--

/* test that trailing blanks are not ignored */
SELECT orafce.length('あbb'::char(6));
SELECT orafce.length(''::char(6));
