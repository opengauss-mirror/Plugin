create database vec_engine_test dbcompatibility 'b';
\c vec_engine_test
CREATE TABLE customer (
    c_custkey integer NOT NULL,
    c_name character varying(25) NOT NULL,
    c_address character varying(40) NOT NULL,
    c_nationkey integer NOT NULL,
    c_phone character(15) NOT NULL,
    c_acctbal numeric(15,2) NOT NULL,
    c_mktsegment character(10) NOT NULL,
    c_comment character varying(117) NOT NULL
)
with (orientation = column)
;
CREATE TABLE orders (
    o_orderkey integer NOT NULL,
    o_custkey integer NOT NULL,
    o_orderstatus character(1) NOT NULL,
    o_totalprice numeric(15,2) NOT NULL,
    o_orderdate date NOT NULL,
    o_orderpriority character(15) NOT NULL,
    o_clerk character(15) NOT NULL,
    o_shippriority integer NOT NULL,
    o_comment character varying(79) NOT NULL
)
with (orientation = column)
;
CREATE TABLE lineitem (
    l_orderkey integer NOT NULL,
    l_partkey integer NOT NULL,
    l_suppkey integer NOT NULL,
    l_linenumber integer NOT NULL,
    l_quantity numeric(15,2) NOT NULL,
    l_extendedprice numeric(15,2) NOT NULL,
    l_discount numeric(15,2) NOT NULL,
    l_tax numeric(15,2) NOT NULL,
    l_returnflag character(1) NOT NULL,
    l_linestatus character(1) NOT NULL,
    l_shipdate date NOT NULL,
    l_commitdate date NOT NULL,
    l_receiptdate date NOT NULL,
    l_shipinstruct character(25) NOT NULL,
    l_shipmode character(10) NOT NULL,
    l_comment character varying(44) NOT NULL
)
with (orientation = column)
;
CREATE TABLE supplier (
    s_suppkey integer NOT NULL,
    s_name character(25) NOT NULL,
    s_address character varying(40) NOT NULL,
    s_nationkey integer NOT NULL,
    s_phone character(15) NOT NULL,
    s_acctbal numeric(15,2) NOT NULL,
    s_comment character varying(101) NOT NULL
)
with (orientation = column)
;
CREATE TABLE nation (
    n_nationkey integer NOT NULL,
    n_name character(25) NOT NULL,
    n_regionkey integer NOT NULL,
    n_comment character varying(152)
)
with (orientation = column)
;
CREATE TABLE region (
    r_regionkey integer NOT NULL,
    r_name character(25) NOT NULL,
    r_comment character varying(152)
)
with (orientation = column)
;

set enable_vector_engine=on;
set enable_early_free=on;
set enable_nestloop=off;
set enable_mergejoin=off;
set enable_hashjoin=on;
set codegen_cost_threshold=0;
explain (costs off) select
                n_name,
                sum(l_extendedprice * (1 - l_discount)) as revenue
        from
                customer,
                orders,
                lineitem,
                supplier,
                nation,
                region
        where
                c_custkey = o_custkey
                and l_orderkey = o_orderkey
                and l_suppkey = s_suppkey
                and c_nationkey = s_nationkey
                and s_nationkey = n_nationkey
                and n_regionkey = r_regionkey
                and r_name = 'ASIA'
                and o_orderdate >= '1994-01-01'::date
                and o_orderdate < '1994-01-01'::date + interval '1 year'
        group by
                n_name
        order by
                revenue desc;
\c postgres
drop database vec_engine_test;
