CREATE FUNCTION assign_distributed_transaction_id(initiator_node_identifier int4, transaction_number int8, transaction_stamp timestamptz)
     RETURNS void
     LANGUAGE C STRICT
     AS 'MODULE_PATHNAME',$$assign_distributed_transaction_id$$;
 COMMENT ON FUNCTION assign_distributed_transaction_id(initiator_node_identifier int4, transaction_number int8, transaction_stamp timestamptz)
     IS 'Only intended for internal use, users should not call this. The function sets the distributed transaction id';

