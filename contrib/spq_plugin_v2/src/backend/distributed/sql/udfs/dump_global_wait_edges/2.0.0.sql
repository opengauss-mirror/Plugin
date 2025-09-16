CREATE FUNCTION pg_catalog.dump_global_wait_edges(
                    OUT waiting_pid int4,
                    OUT waiting_node_id int4,
                    OUT waiting_transaction_num int8,
                    OUT waiting_transaction_stamp timestamptz,
                    OUT blocking_pid int4,
                    OUT blocking_node_id int4,
                    OUT blocking_transaction_num int8,
                    OUT blocking_transaction_stamp timestamptz,
                    OUT blocking_transaction_waiting bool)
RETURNS SETOF RECORD
LANGUAGE 'c' STRICT
AS $$MODULE_PATHNAME$$, $$dump_global_wait_edges$$;
COMMENT ON FUNCTION pg_catalog.dump_global_wait_edges()
IS 'returns a global list of blocked transactions originating from this node';
