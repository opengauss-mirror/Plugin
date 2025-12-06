CREATE OR REPLACE FUNCTION pg_catalog.datediff(date, date) RETURNS int4 LANGUAGE C STABLE RETURNS NULL ON NULL INPUT as '$libdir/dolphin', 'datediff_date_date';

do $$
DECLARE
ans boolean;
BEGIN
    for ans in select case when count(*)=0 then true else false end as ans  from (select opcmethod from pg_opclass where opcname = 'uint1_ops_1')
    LOOP
        if ans = true then
            CREATE OPERATOR CLASS uint1_ops_1
               FOR TYPE uint1 USING hash family integer_ops AS
                 OPERATOR        1       =(int2, uint1),
                 OPERATOR        1       =(int, uint1);
        end if;
    exit;
    END LOOP;
END$$;


CREATE OR REPLACE FUNCTION pg_catalog.date_add_time_interval_return_time (time, interval, boolean) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'adddate_time_interval';

CREATE OR REPLACE FUNCTION pg_catalog.date_sub_time_interval_return_time (time, interval, boolean) RETURNS time LANGUAGE C STABLE STRICT as '$libdir/dolphin', 'subdate_time_interval';
