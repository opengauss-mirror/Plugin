CREATE OR REPLACE FUNCTION get_current_transaction_id(OUT database_id oid, OUT process_id int, OUT initiator_node_identifier int4, OUT transaction_number int8, OUT transaction_stamp timestamptz)
     RETURNS RECORD
     LANGUAGE C STRICT
     AS 'MODULE_PATHNAME',$$get_current_transaction_id$$;
 COMMENT ON FUNCTION get_current_transaction_id(OUT database_id oid, OUT process_id int, OUT initiator_node_identifier int4, OUT transaction_number int8, OUT transaction_stamp timestamptz)
     IS 'returns the current backend data including distributed transaction id';

