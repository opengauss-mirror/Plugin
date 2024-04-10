DROP CAST IF EXISTS (bool AS bit) CASCADE;
DROP CAST IF EXISTS (bool AS float4) CASCADE;
DROP CAST IF EXISTS (bool AS float8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.booltobit(bool, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat4(bool) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat8(bool) CASCADE;

do $$
begin
    update pg_cast set castcontext='e', castowner=10 where castsource=1560 and casttarget=20 and castcontext='a';
    update pg_cast set castcontext='e', castowner=10 where castsource=1560 and casttarget=23 and castcontext='a';
    update pg_cast set castcontext='e', castowner=10 where castsource=20 and casttarget=1560 and castcontext='a';
    update pg_cast set castcontext='e', castowner=10 where castsource=23 and casttarget=1560 and castcontext='a';
end
$$;
drop CAST IF EXISTS (uint4 AS bit);
drop CAST IF EXISTS (uint8 AS bit);
CREATE CAST (uint4 AS bit) WITH FUNCTION bitfromuint4(uint4, int4);
CREATE CAST (uint8 AS bit) WITH FUNCTION bitfromuint8(uint8, int4);

DROP FUNCTION IF EXISTS pg_catalog.bit_length(boolean);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(year);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(blob);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(anyenum);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(json);
DROP FUNCTION IF EXISTS pg_catalog.bit_length(binary);