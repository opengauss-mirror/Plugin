DROP CAST IF EXISTS (bool AS bit) CASCADE;
DROP CAST IF EXISTS (bool AS float4) CASCADE;
DROP CAST IF EXISTS (bool AS float8) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.booltobit(bool, int4) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat4(bool) CASCADE;
DROP FUNCTION IF EXISTS pg_catalog.booltofloat8(bool) CASCADE;

DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan2 (boolean, float8);
DROP FUNCTION IF EXISTS pg_catalog.atan2 (float8, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, float8);
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean, float8);
DROP FUNCTION IF EXISTS pg_catalog.atan (float8, boolean);
DROP FUNCTION IF EXISTS pg_catalog.atan (boolean);

drop CAST IF EXISTS (timestamptz as boolean);
drop CAST IF EXISTS (timestamp(0) without time zone as boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamptz_bool(timestamptz);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_bool(timestamp(0) without time zone);

DROP FUNCTION IF EXISTS pg_catalog.ord(varbit);
DROP FUNCTION IF EXISTS pg_catalog.oct(bit);
DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", text);
DROP FUNCTION IF EXISTS pg_catalog.substring_index ("any", "any", numeric);

DO $for_og_310$
BEGIN
    if working_version_num() > 92780 then
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, text, numeric) CASCADE;
        CREATE FUNCTION pg_catalog.substring_index (
        text,
        text,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index';
        
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, text, numeric) CASCADE;
        CREATE FUNCTION pg_catalog.substring_index (
        boolean,
        text,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_1';
        
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (text, boolean, numeric) CASCADE;
        CREATE FUNCTION pg_catalog.substring_index (
        text,
        boolean,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_bool_2';
        
        DROP FUNCTION IF EXISTS pg_catalog.substring_index (boolean, boolean, numeric) CASCADE;
        CREATE FUNCTION pg_catalog.substring_index (
        boolean,
        boolean,
        numeric
        ) RETURNS text LANGUAGE C IMMUTABLE STRICT as '$libdir/dolphin', 'substring_index_2bool';
    end if;
END
$for_og_310$;

DROP FUNCTION IF EXISTS pg_catalog.date_cast(cstring, boolean);
DROP FUNCTION IF EXISTS pg_catalog.timestamp_cast(cstring, oid, integer, boolean);

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


drop function IF EXISTS pg_catalog."user"();

DROP OPERATOR IF EXISTS pg_catalog.~~(varbinary, varbinary);
DROP OPERATOR IF EXISTS pg_catalog.~~(varbinary, text);
DROP OPERATOR IF EXISTS pg_catalog.~~(text, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.varbinarylike(varbinary, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.textvarbinarylike(text, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.varbinarytextlike(varbinary, text);
DROP OPERATOR IF EXISTS pg_catalog.~~(blob, blob);
DROP FUNCTION IF EXISTS pg_catalog.bloblike(blob, blob);

DROP OPERATOR CLASS IF EXISTS pg_catalog.varbinary_ops USING BTREE;
DROP OPERATOR CLASS IF EXISTS pg_catalog.varbinary_ops USING HASH;
DROP OPERATOR CLASS IF EXISTS pg_catalog.binary_ops USING BTREE;
DROP OPERATOR CLASS IF EXISTS pg_catalog.binary_ops USING HASH;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.varbinary_ops USING BTREE;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.varbinary_ops USING HASH;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.binary_ops USING BTREE;
DROP OPERATOR FAMILY IF EXISTS pg_catalog.binary_ops USING HASH;
DROP FUNCTION IF EXISTS pg_catalog.varbinary_cmp(varbinary, varbinary);
DROP FUNCTION IF EXISTS pg_catalog.binary_cmp(binary, binary);

