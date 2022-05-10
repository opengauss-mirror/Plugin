CREATE FUNCTION ast_support(IN sql text) RETURNS int AS 'MODULE_PATHNAME', 'AssessmentAstTree'
LANGUAGE C STRICT;
