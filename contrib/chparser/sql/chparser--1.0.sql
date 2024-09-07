CREATE FUNCTION chprs_start(internal, int4)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION chprs_getlexeme(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION chprs_end(internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION chprs_lextype(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE TEXT SEARCH PARSER chparser (
    START    = chprs_start,
    GETTOKEN = chprs_getlexeme,
    END      = chprs_end,
    HEADLINE = pg_catalog.prsd_headline,
    LEXTYPES = chprs_lextype
);


CREATE SCHEMA chparser;
CREATE TABLE chparser.chprs_custom_word(word text primary key, tf float default '1.0', idf float default '1.0', attr char default '@', check(attr = '@' or attr = '!'));

CREATE FUNCTION sync_chprs_custom_word() RETURNS void LANGUAGE plpgsql AS
$$
declare
	data_dir text;
	dict_path text;
	time_tag_path text;
	query text;
begin
	select setting from pg_settings where name='data_directory' into data_dir;

	select data_dir || '/base' || '/chprs_dict_' || current_database() || '.txt' into dict_path;
	select data_dir || '/base' || '/chprs_dict_' || current_database() || '.tag' into time_tag_path;

	query = 'copy (select word, tf, idf, attr from chparser.chprs_custom_word) to ' || chr(39) || dict_path || chr(39) || ' encoding ' || chr(39) || 'utf8' || chr(39) ;
	execute query;
	query = 'copy (select now()) to ' || chr(39) || time_tag_path || chr(39) ;
	execute query;
end;
$$;

-- do not created custom dict files when fresh installed
-- select sync_chprs_custom_word();
