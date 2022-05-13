/* the bug is OpenGauss DCH_from_char() Bug, pg is right but opengauss is wrong! */
set orafce.nls_date_format='YY-MonDD HH24:MI:SS';
select to_date('14-Jan08 11:44:49+05:30');
set orafce.nls_date_format='YY-DDMon HH24:MI:SS';
select to_date('14-08Jan 11:44:49+05:30');
set orafce.nls_date_format='DDMMYYYY HH24:MI:SS';
select to_date('21052014 12:13:44+05:30');
set orafce.nls_date_format='DDMMYY HH24:MI:SS';
select to_date('210514 12:13:44+05:30');

select to_date('14-Jan08 11:44:49+05:30' ,'YY-MonDD HH24:MI:SS');
select to_date('14-08Jan 11:44:49+05:30','YY-DDMon HH24:MI:SS');
select to_date('21052014 12:13:44+05:30','DDMMYYYY HH24:MI:SS');
select to_date('210514 12:13:44+05:30','DDMMYY HH24:MI:SS');

set orafce.nls_date_format='YY-MonDD HH24:MI:SS';
select oracle.to_char(orafce.to_date('14-Jan08 11:44:49+05:30'));
set orafce.nls_date_format='YY-DDMon HH24:MI:SS';
select oracle.to_char(orafce.to_date('14-08Jan 11:44:49+05:30'));
set orafce.nls_date_format='DDMMYYYY HH24:MI:SS';
select oracle.to_char(orafce.to_date('21052014 12:13:44+05:30'));
set orafce.nls_date_format='DDMMYY HH24:MI:SS';
select oracle.to_char(orafce.to_date('210514 12:13:44+05:30'));

set orafce.nls_date_format='DDMMYY HH24:MI:SS';
select oracle.to_char(to_date('14-Jan08 11:44:49+05:30' ,'YY-MonDD HH24:MI:SS'));
set orafce.nls_date_format='DDMMYYYY HH24:MI:SS';
select oracle.to_char(to_date('14-08Jan 11:44:49+05:30','YY-DDMon HH24:MI:SS'));
set orafce.nls_date_format='YY-MonDD HH24:MI:SS';
select oracle.to_char(to_date('21052014 12:13:44+05:30','DDMMYYYY HH24:MI:SS'));
set orafce.nls_date_format='DDMMYY HH24:MI:SS';
select oracle.to_char(to_date('210514 12:13:44+05:30','DDMMYY HH24:MI:SS'));

SELECT next_day ('2008-01-01', 'sunAAA');
SELECT next_day ('2008-01-01 121212', 'sunAAA');
