CREATE FUNCTION worker_apply_sequence_command(text)
    RETURNS VOID
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$worker_apply_sequence_command$$;
COMMENT ON FUNCTION worker_apply_sequence_command(text)
    IS 'create a sequence which products globally unique values';

CREATE FUNCTION worker_apply_sequence_command(create_sequence_command text,
                                              sequence_type_id regtype DEFAULT 'bigint'::regtype)
    RETURNS VOID
    LANGUAGE C STRICT
    AS 'MODULE_PATHNAME', $$worker_apply_sequence_command$$;
COMMENT ON FUNCTION worker_apply_sequence_command(text,regtype)
    IS 'create a sequence which produces globally unique values';
