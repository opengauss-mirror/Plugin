CREATE OR REPLACE FUNCTION pg_get_basic_value(typename text)
RETURNS text
AS
$$
BEGIN
    IF typename = 'timestamp' then
        return 'now';
    elsif typename = 'time' or typename = 'timetz' or typename = 'interval' or typename = 'reltime' then
        return '00:00:00';
    elsif typename = 'date' then
        return '1970-01-01';
    elsif typename = 'smalldatetime' then
        return '1970-01-01 08:00:00';
    elsif typename = 'abstime' then
        return '1970-01-01 08:00:00+08';
    elsif typename = 'uuid' then
        return '00000000-0000-0000-0000-000000000000';
    elsif typename = 'bool' then
        return 'false';
    elsif typename = 'point' or typename = 'polygon' then
        return '(0,0)';
    elsif typename = 'path' then
        return '((0,0))';
    elsif typename = 'circle' then
        return '(0,0),0';
    elsif typename = 'lseg' or typename = 'box' then
        return '(0,0),(0,0)';
    elsif typename = 'tinterval' then
        return '["1970-01-01 00:00:00+08" "1970-01-01 00:00:00+08"]';
    else
        return '0 or empty';
    end if;
end;
$$
LANGUAGE plpgsql;

CREATE VIEW pg_type_basic_value AS
    SELECT
            t.typname As typename,
            pg_get_basic_value(t.typname) As basic_value

    FROM pg_type t;