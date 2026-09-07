/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION age" to load this file. \quit

--
-- Ensure ag_catalog is created and owned by the installing role.
--
-- CREATE EXTENSION places all of AGE's objects in ag_catalog. A normal install
-- creates that schema, owned by the installer. If ag_catalog already exists and
-- is owned by a different role, that role would retain control over the schema
-- that holds AGE's catalog objects. To keep ownership well-defined, refuse to
-- install into a pre-existing ag_catalog owned by another role. Ownership is
-- compared directly (not via role membership) so the check is exact even for a
-- superuser, who is otherwise considered a member of every role.
--
DO $age_install_guard$
BEGIN
    IF EXISTS (
        SELECT 1
        FROM pg_catalog.pg_namespace n
        WHERE n.nspname = 'ag_catalog'
          AND n.nspowner <> (SELECT r.oid
                             FROM pg_catalog.pg_roles r
                             WHERE r.rolname = current_user)
    ) THEN
        RAISE EXCEPTION 'schema "ag_catalog" already exists and is not owned by the installing role "%"', current_user
            USING HINT = 'Apache AGE will not install into a pre-existing ag_catalog owned by another role. Drop it (DROP SCHEMA ag_catalog CASCADE) or transfer its ownership to the installing role, then retry CREATE EXTENSION age.';
    END IF;
END
$age_install_guard$;

--
-- catalog tables
--

CREATE TABLE ag_graph (
                          graphid oid NOT NULL,
                          name name NOT NULL,
                          namespace regnamespace NOT NULL
);

CREATE UNIQUE INDEX ag_graph_graphid_index ON ag_graph USING btree (graphid);

-- include content of the ag_graph table into the pg_dump output
SELECT pg_catalog.pg_extension_config_dump('ag_graph', '');

CREATE UNIQUE INDEX ag_graph_name_index ON ag_graph USING btree (name);

CREATE UNIQUE INDEX ag_graph_namespace_index
    ON ag_graph
    USING btree (namespace);

-- 0 is an invalid label ID
CREATE DOMAIN label_id AS int NOT NULL CHECK (VALUE > 0 AND VALUE <= 65535);

CREATE DOMAIN label_kind AS "char" NOT NULL CHECK (VALUE = 'v' OR VALUE = 'e');

CREATE TABLE ag_label (
                          name name NOT NULL,
                          graph oid NOT NULL,
                          id label_id,
                          kind label_kind,
                          relation regclass NOT NULL,
                          seq_name name NOT NULL,
                          CONSTRAINT fk_graph_oid
                              FOREIGN KEY(graph)
                                  REFERENCES ag_graph(graphid)
);

-- include content of the ag_label table into the pg_dump output
SELECT pg_catalog.pg_extension_config_dump('ag_label', '');

CREATE UNIQUE INDEX ag_label_name_graph_index
    ON ag_label
    USING btree (name, graph);

CREATE UNIQUE INDEX ag_label_graph_oid_index
    ON ag_label
    USING btree (graph, id);

CREATE UNIQUE INDEX ag_label_relation_index ON ag_label USING btree (relation);

CREATE UNIQUE INDEX ag_label_seq_name_graph_index
    ON ag_label
    USING btree (seq_name, graph);

--
-- catalog lookup functions
--

CREATE FUNCTION ag_catalog._label_id(graph_name name, label_name name)
    RETURNS label_id
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- Internal selftest for the agehash open-addressing hashtable. Returns "OK"
-- on success or "FAIL: ..." with a diagnostic message. Intended for the
-- agehash regression test only.
CREATE FUNCTION ag_catalog._agehash_self_test()
    RETURNS text
    LANGUAGE c
    VOLATILE
    PARALLEL UNSAFE
AS 'MODULE_PATHNAME';

--
-- utility functions
--

CREATE FUNCTION ag_catalog.create_graph(graph_name name)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.drop_graph(graph_name name, cascade boolean = false)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.create_vlabel(graph_name cstring, label_name cstring)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.create_elabel(graph_name cstring, label_name cstring)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.alter_graph(graph_name name, operation cstring,
                                       new_value name)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.drop_label(graph_name name, label_name name,
                                      force boolean = false)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

--
-- If `load_as_agtype` is true, property values are loaded as agtype; otherwise
-- loaded as string.
--
CREATE FUNCTION ag_catalog.load_labels_from_file(graph_name name,
                                                 label_name name,
                                                 file_path text,
                                                 id_field_exists bool default true,
                                                 load_as_agtype bool default false,
                                                 delimiter text default ',')
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.load_edges_from_file(graph_name name,
                                                label_name name,
                                                file_path text,
                                                load_as_agtype bool default false,
                                                delimiter text default ',')
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

--
-- graphid type
--

-- define graphid as a shell type first
CREATE TYPE graphid;

CREATE FUNCTION ag_catalog.graphid_in(cstring)
    RETURNS graphid
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.graphid_out(graphid)
    RETURNS cstring
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- binary I/O functions
CREATE FUNCTION ag_catalog.graphid_send(graphid)
    RETURNS bytea
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.graphid_recv(internal)
    RETURNS graphid
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE TYPE graphid (
  INPUT = ag_catalog.graphid_in,
  OUTPUT = ag_catalog.graphid_out,
  SEND = ag_catalog.graphid_send,
  RECEIVE = ag_catalog.graphid_recv,
  INTERNALLENGTH = 8,
  PASSEDBYVALUE,
  ALIGNMENT = float8,
  STORAGE = plain
);

--
-- graphid - comparison operators (=, <>, <, >, <=, >=)
--

CREATE FUNCTION ag_catalog.graphid_eq(graphid, graphid)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.graphid_eq,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  HASHES,
  MERGES
);

CREATE FUNCTION ag_catalog.graphid_ne(graphid, graphid)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.graphid_ne,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.graphid_lt(graphid, graphid)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.graphid_lt,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.graphid_gt(graphid, graphid)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.graphid_gt,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.graphid_le(graphid, graphid)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.graphid_le,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.graphid_ge(graphid, graphid)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.graphid_ge,
  LEFTARG = graphid,
  RIGHTARG = graphid,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

--
-- graphid - B-tree support functions
--

-- comparison support
CREATE FUNCTION ag_catalog.graphid_btree_cmp(graphid, graphid)
    RETURNS int
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- sort support
CREATE FUNCTION ag_catalog.graphid_btree_sort(internal)
    RETURNS void
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- define operator classes for graphid
--

-- B-tree strategies
--   1: less than
--   2: less than or equal
--   3: equal
--   4: greater than or equal
--   5: greater than
--
-- B-tree support functions
--   1: compare two keys and return an integer less than zero, zero, or greater
--      than zero, indicating whether the first key is less than, equal to, or
--      greater than the second
--   2: return the addresses of C-callable sort support function(s) (optional)
--   3: compare a test value to a base value plus/minus an offset, and return
--      true or false according to the comparison result (optional)
CREATE OPERATOR CLASS graphid_ops DEFAULT FOR TYPE graphid USING btree AS
  OPERATOR 1 <,
  OPERATOR 2 <=,
  OPERATOR 3 =,
  OPERATOR 4 >=,
  OPERATOR 5 >,
  FUNCTION 1 ag_catalog.graphid_btree_cmp (graphid, graphid),
  FUNCTION 2 ag_catalog.graphid_btree_sort (internal);

--
-- graphid functions
--

CREATE FUNCTION ag_catalog._graphid(label_id int, entry_id bigint)
    RETURNS graphid
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._extract_label_id(graphid)
    RETURNS label_id
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- VLE cache invalidation trigger function.
-- Installed on graph label tables to catch SQL-level mutations
-- (INSERT/UPDATE/DELETE/TRUNCATE) and increment the graph's
-- version counter so VLE caches are properly invalidated.
--
CREATE FUNCTION ag_catalog.age_invalidate_graph_cache()
    RETURNS trigger
    LANGUAGE c
AS 'MODULE_PATHNAME';
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- agtype type and its support functions
--

-- define agtype as a shell type first
CREATE TYPE agtype;

CREATE FUNCTION ag_catalog.agtype_in(cstring)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_out(agtype)
    RETURNS cstring
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- binary I/O functions
CREATE FUNCTION ag_catalog.agtype_send(agtype)
    RETURNS bytea
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_recv(internal)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE TYPE agtype (
  INPUT = ag_catalog.agtype_in,
  OUTPUT = ag_catalog.agtype_out,
  SEND = ag_catalog.agtype_send,
  RECEIVE = ag_catalog.agtype_recv,
  LIKE = jsonb
);

--
-- agtype - mathematical operators (+, -, *, /, %, ^)
--

CREATE FUNCTION ag_catalog.agtype_add(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_add,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, smallint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  smallint,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(smallint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = smallint,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, integer)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  integer,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(integer, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = integer,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, bigint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  bigint,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(bigint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = bigint,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, real)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  real,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(real, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = real,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, double precision)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  double precision,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(double precision, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = double precision,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(agtype, numeric)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = agtype,
  RIGHTARG =  numeric,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_any_add(numeric, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR + (
  FUNCTION = ag_catalog.agtype_any_add,
  LEFTARG = numeric,
  RIGHTARG =  agtype,
  COMMUTATOR = +
);

CREATE FUNCTION ag_catalog.agtype_sub(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_sub,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, smallint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  smallint
);

CREATE FUNCTION ag_catalog.agtype_any_sub(smallint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = smallint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, integer)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  integer
);

CREATE FUNCTION ag_catalog.agtype_any_sub(integer, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = integer,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, bigint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  bigint
);

CREATE FUNCTION ag_catalog.agtype_any_sub(bigint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = bigint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, real)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  real
);

CREATE FUNCTION ag_catalog.agtype_any_sub(real, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = real,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, double precision)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  double precision
);

CREATE FUNCTION ag_catalog.agtype_any_sub(double precision, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = double precision,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_sub(agtype, numeric)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = agtype,
  RIGHTARG =  numeric
);

CREATE FUNCTION ag_catalog.agtype_any_sub(numeric, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_any_sub,
  LEFTARG = numeric,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_neg(agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR - (
  FUNCTION = ag_catalog.agtype_neg,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_mul(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_mul,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, smallint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  smallint,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(smallint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = smallint,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, integer)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  integer,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(integer, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = integer,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, bigint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  bigint,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(bigint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = bigint,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, real)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  real,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(real, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = real,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, double precision)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  double precision,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(double precision, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = double precision,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(agtype, numeric)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = agtype,
  RIGHTARG =  numeric,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_any_mul(numeric, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR * (
  FUNCTION = ag_catalog.agtype_any_mul,
  LEFTARG = numeric,
  RIGHTARG =  agtype,
  COMMUTATOR = *
);

CREATE FUNCTION ag_catalog.agtype_div(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_div,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, smallint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  smallint
);

CREATE FUNCTION ag_catalog.agtype_any_div(smallint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = smallint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, integer)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  integer
);

CREATE FUNCTION ag_catalog.agtype_any_div(integer, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = integer,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, bigint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  bigint
);

CREATE FUNCTION ag_catalog.agtype_any_div(bigint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = bigint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, real)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  real
);

CREATE FUNCTION ag_catalog.agtype_any_div(real, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = real,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, double precision)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  double precision
);

CREATE FUNCTION ag_catalog.agtype_any_div(double precision, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = double precision,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_div(agtype, numeric)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = agtype,
  RIGHTARG =  numeric
);

CREATE FUNCTION ag_catalog.agtype_any_div(numeric, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR / (
  FUNCTION = ag_catalog.agtype_any_div,
  LEFTARG = numeric,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_mod(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_mod,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, smallint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  smallint
);

CREATE FUNCTION ag_catalog.agtype_any_mod(smallint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = smallint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, integer)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  integer
);

CREATE FUNCTION ag_catalog.agtype_any_mod(integer, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = integer,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, bigint)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  bigint
);

CREATE FUNCTION ag_catalog.agtype_any_mod(bigint, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = bigint,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, real)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  real
);

CREATE FUNCTION ag_catalog.agtype_any_mod(real, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = real,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, double precision)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  double precision
);

CREATE FUNCTION ag_catalog.agtype_any_mod(double precision, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = double precision,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_any_mod(agtype, numeric)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = agtype,
  RIGHTARG =  numeric
);

CREATE FUNCTION ag_catalog.agtype_any_mod(numeric, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR % (
  FUNCTION = ag_catalog.agtype_any_mod,
  LEFTARG = numeric,
  RIGHTARG =  agtype
);

CREATE FUNCTION ag_catalog.agtype_pow(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR ^ (
  FUNCTION = ag_catalog.agtype_pow,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.agtype_concat(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR || (
  FUNCTION = ag_catalog.agtype_concat,
  LEFTARG = agtype,
  RIGHTARG = agtype
);

CREATE FUNCTION ag_catalog.graphid_hash_cmp(graphid)
    RETURNS INTEGER
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR CLASS graphid_ops_hash
  DEFAULT
  FOR TYPE graphid
  USING hash AS
  OPERATOR 1 =,
  FUNCTION 1 ag_catalog.graphid_hash_cmp(graphid);

CREATE FUNCTION ag_catalog.graph_exists(graph_name name)
    RETURNS agtype
    LANGUAGE c
    AS 'MODULE_PATHNAME', 'age_graph_exists';
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- agtype - comparison operators (=, <>, <, >, <=, >=)
--

CREATE FUNCTION ag_catalog.agtype_eq(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_eq,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  HASHES
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, smallint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(smallint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, integer)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(integer, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, bigint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(bigint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, real)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(real, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, double precision)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(double precision, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(agtype, numeric)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_eq(numeric, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR = (
  FUNCTION = ag_catalog.agtype_any_eq,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = =,
  NEGATOR = <>,
  RESTRICT = eqsel,
  JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_ne(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_ne,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, smallint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(smallint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, integer)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(integer, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, bigint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(bigint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, real)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(real, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, double precision)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(double precision, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(agtype, numeric)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ne(numeric, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <> (
  FUNCTION = ag_catalog.agtype_any_ne,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = <>,
  NEGATOR = =,
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.agtype_lt(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_lt,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, smallint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(smallint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, integer)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(integer, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, bigint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(bigint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, real)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(real, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, double precision)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(double precision, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(agtype, numeric)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_lt(numeric, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR < (
  FUNCTION = ag_catalog.agtype_any_lt,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = >,
  NEGATOR = >=,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE FUNCTION ag_catalog.agtype_gt(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_gt,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, smallint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(smallint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, integer)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(integer, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, bigint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(bigint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, real)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(real, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, double precision)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(double precision, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(agtype, numeric)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_gt(numeric, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR > (
  FUNCTION = ag_catalog.agtype_any_gt,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = <,
  NEGATOR = <=,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE FUNCTION ag_catalog.agtype_le(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_le,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, smallint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(smallint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, integer)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(integer, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, bigint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(bigint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, real)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(real, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, double precision)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(double precision, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(agtype, numeric)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_le(numeric, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <= (
  FUNCTION = ag_catalog.agtype_any_le,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = >=,
  NEGATOR = >,
  RESTRICT = scalarlesel,
  JOIN = scalarlejoinsel
);

CREATE FUNCTION ag_catalog.agtype_ge(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_ge,
  LEFTARG = agtype,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, smallint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = smallint,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(smallint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = smallint,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, integer)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = integer,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(integer, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = integer,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, bigint)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = bigint,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(bigint, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = bigint,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, real)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = real,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(real, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = real,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, double precision)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = double precision,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(double precision, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = double precision,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(agtype, numeric)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = agtype,
  RIGHTARG = numeric,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_any_ge(numeric, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR >= (
  FUNCTION = ag_catalog.agtype_any_ge,
  LEFTARG = numeric,
  RIGHTARG = agtype,
  COMMUTATOR = <=,
  NEGATOR = <,
  RESTRICT = scalargesel,
  JOIN = scalargejoinsel
);

CREATE FUNCTION ag_catalog.agtype_btree_cmp(agtype, agtype)
    RETURNS INTEGER
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR CLASS agtype_ops_btree
  DEFAULT
  FOR TYPE agtype
  USING btree AS
  OPERATOR 1 <,
  OPERATOR 2 <=,
  OPERATOR 3 =,
  OPERATOR 4 >,
  OPERATOR 5 >=,
  FUNCTION 1 ag_catalog.agtype_btree_cmp(agtype, agtype);

CREATE FUNCTION ag_catalog.agtype_hash_cmp(agtype)
    RETURNS INTEGER
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR CLASS agtype_ops_hash
  DEFAULT
  FOR TYPE agtype
  USING hash AS
  OPERATOR 1 =,
  FUNCTION 1 ag_catalog.agtype_hash_cmp(agtype);

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- agtype - access operators ( ->, ->> )
--

CREATE FUNCTION ag_catalog.agtype_object_field(agtype, text)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- get agtype object field
CREATE OPERATOR -> (
  LEFTARG = agtype,
  RIGHTARG = text,
  FUNCTION = ag_catalog.agtype_object_field
);

CREATE FUNCTION ag_catalog.agtype_object_field_text(agtype, text)
    RETURNS text
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- get agtype object field as text
CREATE OPERATOR ->> (
  LEFTARG = agtype,
  RIGHTARG = text,
  FUNCTION = ag_catalog.agtype_object_field_text
);

CREATE FUNCTION ag_catalog.agtype_object_field_agtype(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- get agtype object field or array element
CREATE OPERATOR -> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_object_field_agtype
);

CREATE FUNCTION ag_catalog.agtype_object_field_text_agtype(agtype, agtype)
    RETURNS text
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- get agtype object field or array element as text
CREATE OPERATOR ->> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_object_field_text_agtype
);

CREATE FUNCTION ag_catalog.agtype_array_element(agtype, int4)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- get agtype array element
CREATE OPERATOR -> (
  LEFTARG = agtype,
  RIGHTARG = int4,
  FUNCTION = ag_catalog.agtype_array_element
);

CREATE FUNCTION ag_catalog.agtype_array_element_text(agtype, int4)
    RETURNS text
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- get agtype array element as text
CREATE OPERATOR ->> (
  LEFTARG = agtype,
  RIGHTARG = int4,
  FUNCTION = ag_catalog.agtype_array_element_text
);

CREATE FUNCTION ag_catalog.agtype_extract_path(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- return the extracted path as agtype
CREATE OPERATOR #> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_extract_path
);

CREATE FUNCTION ag_catalog.agtype_extract_path_text(agtype, agtype)
    RETURNS text
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- return the extracted path as text
CREATE OPERATOR #>> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_extract_path_text
);
       
--
-- agtype - access operators
--

-- for series of `map.key` and `container[expr]`
CREATE FUNCTION ag_catalog.agtype_access_operator(VARIADIC agtype[])
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_access_slice(agtype, agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_in_operator(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- Contains operators @> <@
--
CREATE FUNCTION ag_catalog.agtype_contains(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR @> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_contains,
  COMMUTATOR = '<@',
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_contained_by(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <@ (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_contained_by,
  COMMUTATOR = '@>',
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_contains_top_level(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR @>> (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_contains_top_level,
  COMMUTATOR = '<<@',
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_contained_by_top_level(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR <<@ (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_contained_by_top_level,
  COMMUTATOR = '@>>',
  RESTRICT = contsel,
  JOIN = contjoinsel
);/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- Key Existence Operators ? ?| ?&
--
CREATE FUNCTION ag_catalog.agtype_exists(agtype, text)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR ? (
  LEFTARG = agtype,
  RIGHTARG = text,
  FUNCTION = ag_catalog.agtype_exists,
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_agtype(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR ? (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_exists_agtype,
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_any(agtype, text[])
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR ?| (
  LEFTARG = agtype,
  RIGHTARG = text[],
  FUNCTION = ag_catalog.agtype_exists_any,
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_any_agtype(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR ?| (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_exists_any_agtype,
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_all(agtype, text[])
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR ?& (
  LEFTARG = agtype,
  RIGHTARG = text[],
  FUNCTION = ag_catalog.agtype_exists_all,
  RESTRICT = contsel,
  JOIN = contjoinsel
);

CREATE FUNCTION ag_catalog.agtype_exists_all_agtype(agtype, agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR ?& (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.agtype_exists_all_agtype,
  RESTRICT = contsel,
  JOIN = contjoinsel
);
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- agtype GIN support
--
CREATE FUNCTION ag_catalog.gin_compare_agtype(text, text)
    RETURNS int
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT
PARALLEL SAFE;

CREATE FUNCTION gin_extract_agtype(agtype, internal)
    RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT
PARALLEL SAFE;

CREATE FUNCTION ag_catalog.gin_extract_agtype_query(agtype, internal, int2,
                                                    internal, internal)
    RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT
PARALLEL SAFE;

CREATE FUNCTION ag_catalog.gin_consistent_agtype(internal, int2, agtype, int4,
                                                 internal, internal)
    RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT
PARALLEL SAFE;

CREATE FUNCTION ag_catalog.gin_triconsistent_agtype(internal, int2, agtype, int4,
                                                    internal, internal, internal)
    RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C
IMMUTABLE
STRICT
PARALLEL SAFE;

CREATE OPERATOR CLASS ag_catalog.gin_agtype_ops
DEFAULT FOR TYPE agtype USING gin AS
  OPERATOR 7 @>(agtype, agtype),
  OPERATOR 8 <@(agtype, agtype),
  OPERATOR 9 ?(agtype, agtype),
  OPERATOR 10 ?|(agtype, agtype),
  OPERATOR 11 ?&(agtype, agtype),
  OPERATOR 12 @>>(agtype, agtype),
  OPERATOR 13 <<@(agtype, agtype),
  FUNCTION 1 ag_catalog.gin_compare_agtype(text,text),
  FUNCTION 2 ag_catalog.gin_extract_agtype(agtype, internal),
  FUNCTION 3 ag_catalog.gin_extract_agtype_query(agtype, internal, int2,
                                                 internal, internal),
  FUNCTION 4 ag_catalog.gin_consistent_agtype(internal, int2, agtype, int4,
                                              internal, internal),
  FUNCTION 6 ag_catalog.gin_triconsistent_agtype(internal, int2, agtype, int4,
                                                 internal, internal, internal),
STORAGE text;
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- graph id conversion function
--
CREATE FUNCTION ag_catalog.graphid_to_agtype(graphid)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (graphid AS agtype)
    WITH FUNCTION ag_catalog.graphid_to_agtype(graphid);

CREATE FUNCTION ag_catalog.agtype_to_graphid(agtype)
    RETURNS graphid
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS graphid)
    WITH FUNCTION ag_catalog.agtype_to_graphid(agtype)
AS IMPLICIT;

--
-- Composite types for vertex and edge
--
CREATE TYPE ag_catalog.vertex AS (
    id graphid,
    label agtype,
    properties agtype
);

CREATE TYPE ag_catalog.edge AS (
    id graphid,
    label agtype,
    end_id graphid,
    start_id graphid,
    properties agtype
);

--
-- Label name function
--
CREATE FUNCTION ag_catalog._label_name(graph_oid oid, graphid)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- agtype - path
--
CREATE FUNCTION ag_catalog._agtype_build_path(VARIADIC "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
CALLED ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- agtype - vertex
--
CREATE FUNCTION ag_catalog._agtype_build_vertex(graphid, agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
CALLED ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- agtype - edge
--
CREATE FUNCTION ag_catalog._agtype_build_edge(graphid, graphid, graphid,
                                              agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
CALLED ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- vertex/edge to agtype cast functions
--
CREATE FUNCTION ag_catalog.vertex_to_agtype(vertex)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.edge_to_agtype(edge)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- Implicit casts from vertex/edge to agtype
--
CREATE CAST (vertex AS agtype)
    WITH FUNCTION ag_catalog.vertex_to_agtype(vertex)
AS IMPLICIT;

CREATE CAST (edge AS agtype)
    WITH FUNCTION ag_catalog.edge_to_agtype(edge)
AS IMPLICIT;

CREATE FUNCTION ag_catalog.vertex_to_json(vertex)
    RETURNS json
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.edge_to_json(edge)
    RETURNS json
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (vertex AS json)
    WITH FUNCTION ag_catalog.vertex_to_json(vertex);

CREATE CAST (edge AS json)
    WITH FUNCTION ag_catalog.edge_to_json(edge);

CREATE FUNCTION ag_catalog.vertex_to_jsonb(vertex)
    RETURNS jsonb
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.edge_to_jsonb(edge)
    RETURNS jsonb
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (vertex AS jsonb)
    WITH FUNCTION ag_catalog.vertex_to_jsonb(vertex);

CREATE CAST (edge AS jsonb)
    WITH FUNCTION ag_catalog.edge_to_jsonb(edge);

--
-- Equality operators for vertex and edge (compare by id)
--
CREATE FUNCTION ag_catalog.vertex_eq(vertex, vertex)
    RETURNS boolean
    LANGUAGE sql
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS $$ SELECT $1.id = $2.id $$;

CREATE OPERATOR = (
    FUNCTION = ag_catalog.vertex_eq,
    LEFTARG = vertex,
    RIGHTARG = vertex,
    COMMUTATOR = =,
    NEGATOR = <>,
    RESTRICT = eqsel,
    JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.vertex_ne(vertex, vertex)
    RETURNS boolean
    LANGUAGE sql
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS $$ SELECT $1.id <> $2.id $$;

CREATE OPERATOR <> (
    FUNCTION = ag_catalog.vertex_ne,
    LEFTARG = vertex,
    RIGHTARG = vertex,
    COMMUTATOR = <>,
    NEGATOR = =,
    RESTRICT = neqsel,
    JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog.edge_eq(edge, edge)
    RETURNS boolean
    LANGUAGE sql
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS $$ SELECT $1.id = $2.id $$;

CREATE OPERATOR = (
    FUNCTION = ag_catalog.edge_eq,
    LEFTARG = edge,
    RIGHTARG = edge,
    COMMUTATOR = =,
    NEGATOR = <>,
    RESTRICT = eqsel,
    JOIN = eqjoinsel
);

CREATE FUNCTION ag_catalog.edge_ne(edge, edge)
    RETURNS boolean
    LANGUAGE sql
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS $$ SELECT $1.id <> $2.id $$;

CREATE OPERATOR <> (
    FUNCTION = ag_catalog.edge_ne,
    LEFTARG = edge,
    RIGHTARG = edge,
    COMMUTATOR = <>,
    NEGATOR = =,
    RESTRICT = neqsel,
    JOIN = neqjoinsel
);

CREATE FUNCTION ag_catalog._ag_enforce_edge_uniqueness2(graphid, graphid)
    RETURNS bool
    LANGUAGE c
    STABLE
PARALLEL SAFE
as 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._ag_enforce_edge_uniqueness3(graphid, graphid, graphid)
    RETURNS bool
    LANGUAGE c
    STABLE
PARALLEL SAFE
as 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._ag_enforce_edge_uniqueness4(graphid, graphid, graphid, graphid)
    RETURNS bool
    LANGUAGE c
    STABLE
PARALLEL SAFE
as 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._ag_enforce_edge_uniqueness(VARIADIC "any")
    RETURNS bool
    LANGUAGE c
    STABLE
PARALLEL SAFE
as 'MODULE_PATHNAME';

--
-- agtype - map literal (`{key: expr, ...}`)
--

CREATE FUNCTION ag_catalog.agtype_build_map(VARIADIC "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
CALLED ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_build_map()
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
CALLED ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME', 'agtype_build_map_noargs';

CREATE FUNCTION ag_catalog.agtype_build_map_nonull(VARIADIC "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
CALLED ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- There are times when the optimizer might eliminate
-- functions we need. Wrap the function with this to
-- prevent that from happening
--

CREATE FUNCTION ag_catalog.agtype_volatile_wrapper("any")
    RETURNS agtype
    LANGUAGE c
    VOLATILE
CALLED ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- agtype - list literal (`[expr, ...]`)
--

CREATE FUNCTION ag_catalog.agtype_build_list(VARIADIC "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
CALLED ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_build_list()
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
CALLED ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME', 'agtype_build_list_noargs';

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- agtype - type coercions
--
-- agtype -> text (explicit)
CREATE FUNCTION ag_catalog.agtype_to_text(agtype)
    RETURNS text
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS text)
    WITH FUNCTION ag_catalog.agtype_to_text(agtype);

-- text -> agtype
CREATE FUNCTION ag_catalog.text_to_agtype(text)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- agtype -> boolean (implicit)
CREATE FUNCTION ag_catalog.agtype_to_bool(agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS boolean)
    WITH FUNCTION ag_catalog.agtype_to_bool(agtype)
AS IMPLICIT;

-- boolean -> agtype (explicit)
CREATE FUNCTION ag_catalog.bool_to_agtype(boolean)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (boolean AS agtype)
    WITH FUNCTION ag_catalog.bool_to_agtype(boolean);

-- float8 -> agtype (explicit)
CREATE FUNCTION ag_catalog.float8_to_agtype(float8)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (float8 AS agtype)
    WITH FUNCTION ag_catalog.float8_to_agtype(float8);

-- agtype -> float8 (explicit)
CREATE FUNCTION ag_catalog.agtype_to_float8(agtype)
    RETURNS float8
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS float8)
    WITH FUNCTION ag_catalog.agtype_to_float8(agtype);

-- int8 -> agtype (explicit)
CREATE FUNCTION ag_catalog.int8_to_agtype(int8)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (int8 AS agtype)
    WITH FUNCTION ag_catalog.int8_to_agtype(int8);

-- agtype -> int8
CREATE FUNCTION ag_catalog.agtype_to_int8(variadic "any")
    RETURNS bigint
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS bigint)
    WITH FUNCTION ag_catalog.agtype_to_int8(variadic "any")
AS ASSIGNMENT;

-- int4 -> agtype (explicit)
CREATE FUNCTION ag_catalog.int4_to_agtype(int4)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (int4 AS agtype)
    WITH FUNCTION ag_catalog.int4_to_agtype(int4);

-- agtype -> int4
CREATE FUNCTION ag_catalog.agtype_to_int4(variadic "any")
    RETURNS int
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS int)
    WITH FUNCTION ag_catalog.agtype_to_int4(variadic "any");

-- agtype -> int2
CREATE FUNCTION ag_catalog.agtype_to_int2(variadic "any")
    RETURNS smallint
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS smallint)
    WITH FUNCTION ag_catalog.agtype_to_int2(variadic "any");

-- agtype -> int4[]
CREATE FUNCTION ag_catalog.agtype_to_int4_array(variadic "any")
    RETURNS int[]
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS int[])
    WITH FUNCTION ag_catalog.agtype_to_int4_array(variadic "any");

CREATE FUNCTION ag_catalog.agtype_to_json(agtype)
    RETURNS json
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype AS json)
    WITH FUNCTION ag_catalog.agtype_to_json(agtype);

-- agtype -> jsonb (explicit)
-- Uses json intermediate (agtype_to_json -> json::jsonb) because agtype
-- extends jsonb's binary format with types (AGTV_INTEGER, AGTV_FLOAT,
-- AGTV_VERTEX, AGTV_EDGE, AGTV_PATH) that jsonb does not recognize.
CREATE FUNCTION ag_catalog.agtype_to_jsonb(agtype)
    RETURNS jsonb
    LANGUAGE sql
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'SELECT ag_catalog.agtype_to_json($1)::jsonb';

CREATE CAST (agtype AS jsonb)
    WITH FUNCTION ag_catalog.agtype_to_jsonb(agtype);

-- jsonb -> agtype (explicit)
CREATE FUNCTION ag_catalog.jsonb_to_agtype(jsonb)
    RETURNS agtype
    LANGUAGE sql
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'SELECT $1::text::agtype';

CREATE CAST (jsonb AS agtype)
    WITH FUNCTION ag_catalog.jsonb_to_agtype(jsonb);

CREATE FUNCTION ag_catalog.agtype_array_to_agtype(agtype[])
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE CAST (agtype[] AS agtype)
    WITH FUNCTION ag_catalog.agtype_array_to_agtype(agtype[]);
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- agtype - string matching (`STARTS WITH`, `ENDS WITH`, `CONTAINS`, & =~)
--

CREATE FUNCTION ag_catalog.agtype_string_match_starts_with(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_string_match_ends_with(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_string_match_contains(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_eq_tilde(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE OPERATOR =~ (
  LEFTARG = agtype,
  RIGHTARG = agtype,
  FUNCTION = ag_catalog.age_eq_tilde
);

CREATE FUNCTION ag_catalog.age_is_valid_label_name(agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- functions for updating clauses
--

-- This function is defined as a VOLATILE function to prevent the optimizer
-- from pulling up Query's for CREATE clauses.
CREATE FUNCTION ag_catalog._cypher_create_clause(internal)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._cypher_set_clause(internal)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._cypher_delete_clause(internal)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog._cypher_merge_clause(internal)
    RETURNS void
    LANGUAGE c
    AS 'MODULE_PATHNAME';

--
-- query functions
--
CREATE FUNCTION ag_catalog.cypher(graph_name name = NULL,
                                  query_string cstring = NULL,
                                  params agtype = NULL)
    RETURNS SETOF record
LANGUAGE c
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.get_cypher_keywords(OUT word text, OUT catcode "char",
                                               OUT catdesc text)
    RETURNS SETOF record
LANGUAGE c
STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
COST 10
ROWS 60
AS 'MODULE_PATHNAME';
--
-- End
--
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- Scalar Functions
--

CREATE FUNCTION ag_catalog.age_id(agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_start_id(agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_end_id(agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_head(agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_last(agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tail(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_properties(agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_startnode(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_endnode(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- Helper function for optimized startNode/endNode
CREATE FUNCTION ag_catalog._get_vertex_by_graphid(text, graphid)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_length(agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_toboolean(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tobooleanlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tofloat(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tofloatlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tointeger(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tointegerlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tostring("any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tostringlist(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_size(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_type(agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_exists(agtype)
    RETURNS boolean
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_isempty(agtype)
    RETURNS boolean
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_label(agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- String functions
--
CREATE FUNCTION ag_catalog.age_reverse(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_toupper(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tolower(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_ltrim(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_rtrim(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_trim(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_right(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_left(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_substring(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_split(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_replace(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- Trig functions - radian input
--
CREATE FUNCTION ag_catalog.age_sin(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_cos(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_tan(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_cot(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_asin(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_acos(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_atan(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_atan2(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_degrees(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_radians(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_round(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_ceil(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_floor(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_abs(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_sign(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_log(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_log10(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_e()
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_pi()
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_rand()
    RETURNS agtype
    LANGUAGE c
    PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_exp(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_sqrt(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_timestamp()
    RETURNS agtype
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- aggregate function components for stdev(internal, agtype)
-- and stdevp(internal, agtype)
--
-- wrapper for the stdev final function to pass 0 instead of null
CREATE FUNCTION ag_catalog.age_float8_stddev_samp_aggfinalfn(_float8)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- wrapper for the float8_accum to use agtype input
CREATE FUNCTION ag_catalog.age_agtype_float8_accum(_float8, agtype)
    RETURNS _float8
    LANGUAGE c
    IMMUTABLE
STRICT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- aggregate definition for age_stdev(agtype)
CREATE AGGREGATE ag_catalog.age_stdev(agtype)
(
   stype = _float8,
   sfunc = ag_catalog.age_agtype_float8_accum,
   finalfunc = ag_catalog.age_float8_stddev_samp_aggfinalfn,
   combinefunc = float8_combine,
   finalfunc_modify = read_only,
   initcond = '{0,0,0}',
   parallel = safe
);

-- wrapper for the stdevp final function to pass 0 instead of null
CREATE FUNCTION ag_catalog.age_float8_stddev_pop_aggfinalfn(_float8)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- aggregate definition for age_stdevp(agtype)
CREATE AGGREGATE ag_catalog.age_stdevp(agtype)
(
   stype = _float8,
   sfunc = age_agtype_float8_accum,
   finalfunc = ag_catalog.age_float8_stddev_pop_aggfinalfn,
   combinefunc = float8_combine,
   finalfunc_modify = read_only,
   initcond = '{0,0,0}',
   parallel = safe
);

--
-- aggregate function components for avg(agtype) and sum(agtype)
--
-- aggregate definition for avg(agytpe)
CREATE AGGREGATE ag_catalog.age_avg(agtype)
(
   stype = _float8,
   sfunc = ag_catalog.age_agtype_float8_accum,
   finalfunc = float8_avg,
   combinefunc = float8_combine,
   finalfunc_modify = read_only,
   initcond = '{0,0,0}',
   parallel = safe
);

-- sum aggtransfn
CREATE FUNCTION ag_catalog.age_agtype_sum(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
STRICT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- aggregate definition for sum(agytpe)
CREATE AGGREGATE ag_catalog.age_sum(agtype)
(
   stype = agtype,
   sfunc = ag_catalog.age_agtype_sum,
   combinefunc = ag_catalog.age_agtype_sum,
   finalfunc_modify = read_only,
   parallel = safe
);

--
-- aggregate functions for min(variadic "any") and max(variadic "any")
--
-- max transfer function
CREATE FUNCTION ag_catalog.age_agtype_larger_aggtransfn(agtype, variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- aggregate definition for max(variadic "any")
CREATE AGGREGATE ag_catalog.age_max(variadic "any")
(
   stype = agtype,
   sfunc = ag_catalog.age_agtype_larger_aggtransfn,
   combinefunc = ag_catalog.age_agtype_larger_aggtransfn,
   finalfunc_modify = read_only,
   parallel = safe
);

-- min transfer function
CREATE FUNCTION ag_catalog.age_agtype_smaller_aggtransfn(agtype, variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- aggregate definition for min(variadic "any")
CREATE AGGREGATE ag_catalog.age_min(variadic "any")
(
   stype = agtype,
   sfunc = ag_catalog.age_agtype_smaller_aggtransfn,
   combinefunc = ag_catalog.age_agtype_smaller_aggtransfn,
   finalfunc_modify = read_only,
   parallel = safe
);

--
-- aggregate functions percentileCont(internal, agtype) and
-- percentileDisc(internal, agtype)
--
-- percentile transfer function
CREATE FUNCTION ag_catalog.age_percentile_aggtransfn(internal, agtype, agtype)
    RETURNS internal
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- percentile_cont final function
CREATE FUNCTION ag_catalog.age_percentile_cont_aggfinalfn(internal)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- percentile_disc final function
CREATE FUNCTION ag_catalog.age_percentile_disc_aggfinalfn(internal)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- aggregate definition for _percentilecont(agtype, agytpe)
CREATE AGGREGATE ag_catalog.age_percentilecont(agtype, agtype)
(
    stype = internal,
    sfunc = ag_catalog.age_percentile_aggtransfn,
    finalfunc = ag_catalog.age_percentile_cont_aggfinalfn,
    parallel = safe
);

-- aggregate definition for percentiledisc(agtype, agytpe)
CREATE AGGREGATE ag_catalog.age_percentiledisc(agtype, agtype)
(
    stype = internal,
    sfunc = ag_catalog.age_percentile_aggtransfn,
    finalfunc = ag_catalog.age_percentile_disc_aggfinalfn,
    parallel = safe
);

--
-- aggregate functions for collect(variadic "any")
--
-- collect transfer function
CREATE FUNCTION ag_catalog.age_collect_aggtransfn(internal, variadic "any")
    RETURNS internal
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- collect final function
CREATE FUNCTION ag_catalog.age_collect_aggfinalfn(internal)
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- aggregate definition for age_collect(variadic "any")
CREATE AGGREGATE ag_catalog.age_collect(variadic "any")
(
    stype = internal,
    sfunc = ag_catalog.age_collect_aggtransfn,
    finalfunc = ag_catalog.age_collect_aggfinalfn,
    parallel = safe
);

--
-- reduce(acc = init, var IN list | body) fold support
--
-- Transition function for the age_reduce aggregate. The fold body is compiled
-- by transform_cypher_reduce() with the accumulator and element rewritten to
-- PARAM_EXEC params 0 and 1 and serialized into the text argument; the
-- transition evaluates it for each element in list order. The trailing
-- agtype[] argument carries the loop-invariant outer values (outer-query
-- variables and cypher() parameters) referenced by the body, bound to
-- PARAM_EXEC params 2, 3, ... It must be callable with a NULL transition state
-- (no initcond), so it is intentionally not STRICT.
CREATE FUNCTION ag_catalog.age_reduce_transfn(agtype, agtype, text, agtype, agtype[])
    RETURNS agtype
    LANGUAGE c
PARALLEL UNSAFE
AS 'MODULE_PATHNAME';

-- aggregate definition for reduce(); direct arguments are
-- (init, serialized-body, element, captured-outer-values), with the element
-- fed ORDER BY ordinality.
CREATE AGGREGATE ag_catalog.age_reduce(agtype, text, agtype, agtype[])
(
    stype = agtype,
    sfunc = ag_catalog.age_reduce_transfn
);
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- function for typecasting an agtype value to another agtype value
--
CREATE FUNCTION ag_catalog.agtype_typecast_int(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_numeric(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_float(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_bool(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_vertex(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_edge(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.agtype_typecast_path(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- original VLE function definition
-- S4: emit start_id/end_id as scalar columns to enable transformer rewrite
-- of terminal-edge quals as integer equalities (see PERF_VLE_TERMINAL_QUAL_PLAN).
CREATE FUNCTION ag_catalog.age_vle(IN agtype, IN agtype, IN agtype, IN agtype,
                                   IN agtype, IN agtype, IN agtype,
                                   OUT edges    agtype,
                                   OUT start_id graphid,
                                   OUT end_id   graphid)
    RETURNS SETOF record
LANGUAGE C
STABLE
CALLED ON NULL INPUT
PARALLEL UNSAFE -- might be safe
AS 'MODULE_PATHNAME';

-- This is an overloaded function definition to allow for the VLE local context
-- caching mechanism to coexist with the previous VLE version.
CREATE FUNCTION ag_catalog.age_vle(IN agtype, IN agtype, IN agtype, IN agtype,
                                   IN agtype, IN agtype, IN agtype, IN agtype,
                                   OUT edges    agtype,
                                   OUT start_id graphid,
                                   OUT end_id   graphid)
    RETURNS SETOF record
LANGUAGE C
STABLE
CALLED ON NULL INPUT
PARALLEL UNSAFE -- might be safe
AS 'MODULE_PATHNAME';

-- Unweighted (hop-count) shortest path between two vertices, computed over the
-- cached global graph adjacency via BFS. Returns a single path (0 or 1 rows).
-- Argument order mirrors the Cypher shortestPath() pattern
-- (a)-[:type*min_hops..max_hops]->(b):
--   (graph_name, start, end, edge_types, direction, min_hops, max_hops)
CREATE FUNCTION ag_catalog.age_shortest_path(IN agtype, IN agtype, IN agtype,
                                             IN agtype DEFAULT NULL,
                                             IN agtype DEFAULT NULL,
                                             IN agtype DEFAULT NULL,
                                             IN agtype DEFAULT NULL)
    RETURNS SETOF agtype
LANGUAGE C
STABLE
CALLED ON NULL INPUT
PARALLEL UNSAFE
AS 'MODULE_PATHNAME';

-- All unweighted shortest paths between two vertices (one path per row).
-- Same argument order as age_shortest_path.
CREATE FUNCTION ag_catalog.age_all_shortest_paths(IN agtype, IN agtype, IN agtype,
                                                  IN agtype DEFAULT NULL,
                                                  IN agtype DEFAULT NULL,
                                                  IN agtype DEFAULT NULL,
                                                  IN agtype DEFAULT NULL)
    RETURNS SETOF agtype
LANGUAGE C
STABLE
CALLED ON NULL INPUT
PARALLEL UNSAFE
AS 'MODULE_PATHNAME';

-- function to build an edge for a VLE match
CREATE FUNCTION ag_catalog.age_build_vle_match_edge(agtype, agtype)
    RETURNS agtype
    LANGUAGE C
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- function to create an AGTV_PATH from a VLE_path_container
CREATE FUNCTION ag_catalog.age_materialize_vle_path(agtype)
    RETURNS agtype
    LANGUAGE C
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- function to create an AGTV_ARRAY of edges from a VLE_path_container
CREATE FUNCTION ag_catalog.age_materialize_vle_edges(agtype)
    RETURNS agtype
    LANGUAGE C
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_match_vle_edge_to_id_qual(variadic "any")
    RETURNS boolean
    LANGUAGE C
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

-- list functions
CREATE FUNCTION ag_catalog.age_keys(agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_labels(agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_nodes(agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
RETURNS NULL ON NULL INPUT
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_relationships(agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_range(variadic "any")
    RETURNS agtype
    LANGUAGE c
    IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_unnest(agtype)
    RETURNS SETOF agtype
LANGUAGE c
IMMUTABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_vertex_stats(agtype, agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_graph_stats(agtype)
    RETURNS agtype
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_delete_global_graphs(agtype)
    RETURNS boolean
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.create_complete_graph(graph_name name, nodes int,
                                                 edge_label name,
                                                 node_label name = NULL)
    RETURNS void
    LANGUAGE c
    CALLED ON NULL INPUT
    PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_create_barbell_graph(graph_name name,
                                                    graph_size int,
                                                    bridge_size int,
                                                    node_label name = NULL,
                                                    node_properties agtype = NULL,
                                                    edge_label name = NULL,
                                                    edge_properties agtype = NULL)
    RETURNS void
    LANGUAGE c
    CALLED ON NULL INPUT
    PARALLEL SAFE
AS 'MODULE_PATHNAME';

CREATE FUNCTION ag_catalog.age_prepare_cypher(cstring, cstring)
    RETURNS boolean
    LANGUAGE c
    STABLE
PARALLEL SAFE
AS 'MODULE_PATHNAME';

--
-- End
--
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- pg_upgrade support functions
--
-- These functions help users upgrade PostgreSQL major versions using pg_upgrade
-- while preserving Apache AGE graph data. The ag_graph.namespace column uses
-- the regnamespace type which is not supported by pg_upgrade. These functions
-- temporarily convert the schema to be pg_upgrade compatible, then restore it
-- to the original state after the upgrade completes.
--
-- Usage:
--   1. Before pg_upgrade: SELECT age_prepare_pg_upgrade();
--   2. Run pg_upgrade as normal
--   3. After pg_upgrade:  SELECT age_finish_pg_upgrade();
--
-- To cancel an upgrade after preparation (before running pg_upgrade):
--   SELECT age_revert_pg_upgrade_changes();
--

--
-- age_prepare_pg_upgrade()
--
-- Prepares an AGE database for pg_upgrade by converting the ag_graph.namespace
-- column from regnamespace to oid type. This is necessary because pg_upgrade
-- does not support the regnamespace type in user tables.
--
-- This function:
--   1. Creates a backup table with graph name to namespace name mappings
--   2. Drops the existing namespace index
--   3. Converts the namespace column from regnamespace to oid
--   4. Recreates the namespace index with oid type
--   5. Creates a view for backward-compatible namespace display
--
-- Returns: void
-- Side effects: Modifies ag_graph table structure
--
CREATE FUNCTION ag_catalog.age_prepare_pg_upgrade()
    RETURNS void
    LANGUAGE plpgsql
    -- Resolve built-in functions and operators from pg_catalog first so they
    -- are not overridden by same-named objects defined in ag_catalog. The
    -- ag_catalog objects referenced here are schema-qualified.
    SET search_path = pg_catalog, ag_catalog
    AS $function$
DECLARE
    graph_count integer;
BEGIN
    -- Check if namespace column is already oid type (already prepared)
    IF EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_schema = 'ag_catalog'
          AND table_name = 'ag_graph'
          AND column_name = 'namespace'
          AND data_type = 'oid'
    ) THEN
        RAISE NOTICE 'Database already prepared for pg_upgrade (namespace is oid type).';
        RETURN;
    END IF;

    -- Drop existing backup table if it exists (from a previous failed attempt)
    DROP TABLE IF EXISTS public._age_pg_upgrade_backup;

    -- Create backup table with graph names mapped to namespace names
    -- We store nspname directly (not regnamespace::text) to avoid quoting issues
    -- Names survive pg_upgrade while OIDs don't
    CREATE TABLE public._age_pg_upgrade_backup AS
    SELECT
        g.graphid AS old_graphid,
        g.name AS graph_name,
        n.nspname AS namespace_name
    FROM ag_catalog.ag_graph g
    JOIN pg_namespace n ON n.oid = g.namespace::oid;

    SELECT count(*) INTO graph_count FROM public._age_pg_upgrade_backup;

    RAISE NOTICE 'Created backup table public._age_pg_upgrade_backup with % graph(s)', graph_count;

    -- Even with zero graphs, we still need to convert the column type
    -- because the regnamespace type itself blocks pg_upgrade

    -- Drop the existing regnamespace-based index
    DROP INDEX IF EXISTS ag_catalog.ag_graph_namespace_index;

    -- Convert namespace column from regnamespace to oid
    ALTER TABLE ag_catalog.ag_graph
        ALTER COLUMN namespace TYPE oid USING namespace::oid;

    -- Recreate the index with oid type
    CREATE UNIQUE INDEX ag_graph_namespace_index
        ON ag_catalog.ag_graph USING btree (namespace);

    -- Create a view for backward-compatible display of namespace as schema name
    CREATE OR REPLACE VIEW ag_catalog.ag_graph_view AS
    SELECT graphid, name, namespace::regnamespace AS namespace
    FROM ag_catalog.ag_graph;

    RAISE NOTICE 'Successfully prepared database for pg_upgrade.';
    RAISE NOTICE 'The ag_graph.namespace column has been converted from regnamespace to oid.';
    RAISE NOTICE 'You can now run pg_upgrade.';
    RAISE NOTICE 'After pg_upgrade completes, run: SELECT age_finish_pg_upgrade();';
END;
$function$;

COMMENT ON FUNCTION ag_catalog.age_prepare_pg_upgrade() IS 
'Prepares an AGE database for pg_upgrade by converting ag_graph.namespace from regnamespace to oid type. Run this before pg_upgrade.';

--
-- age_finish_pg_upgrade()
--
-- Completes the pg_upgrade process by remapping stale OIDs in ag_graph and
-- ag_label tables to their new values, then restores the original schema.
-- After pg_upgrade, the namespace OIDs stored in these tables no longer match
-- the actual pg_namespace OIDs because PostgreSQL assigns new OIDs to schemas
-- during the upgrade.
--
-- This function:
--   1. Reads the backup table created by age_prepare_pg_upgrade()
--   2. Looks up new namespace OIDs by schema name
--   3. Updates ag_label.graph references
--   4. Updates ag_graph.graphid and ag_graph.namespace
--   5. Restores namespace column to regnamespace type
--   6. Cleans up the backup table and view
--   7. Invalidates AGE caches to ensure cypher queries work immediately
--
-- Returns: void
-- Side effects: Updates OIDs in ag_graph and ag_label tables, restores schema
--
CREATE FUNCTION ag_catalog.age_finish_pg_upgrade()
    RETURNS void
    LANGUAGE plpgsql
    -- Resolve built-in functions and operators from pg_catalog first so they
    -- are not overridden by same-named objects defined in ag_catalog. The
    -- ag_catalog objects referenced here are schema-qualified.
    SET search_path = pg_catalog, ag_catalog
    AS $function$
DECLARE
    mapping_count integer;
    updated_labels integer;
    updated_graphs integer;
BEGIN
    -- Check if backup table exists
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.tables
        WHERE table_schema = 'public'
          AND table_name = '_age_pg_upgrade_backup'
    ) THEN
        RAISE EXCEPTION 'Backup table public._age_pg_upgrade_backup not found. '
            'Did you run age_prepare_pg_upgrade() before pg_upgrade?';
    END IF;

    -- Check if namespace column is oid type (was properly prepared)
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_schema = 'ag_catalog'
          AND table_name = 'ag_graph'
          AND column_name = 'namespace'
          AND data_type = 'oid'
    ) THEN
        RAISE EXCEPTION 'ag_graph.namespace is not oid type. '
            'Did you run age_prepare_pg_upgrade() before pg_upgrade?';
    END IF;

    -- Create temporary mapping table with old and new OIDs
    CREATE TEMP TABLE _graphid_mapping AS
    SELECT
        b.old_graphid,
        b.graph_name,
        n.oid AS new_graphid
    FROM public._age_pg_upgrade_backup b
    JOIN pg_namespace n ON n.nspname = b.namespace_name;

    GET DIAGNOSTICS mapping_count = ROW_COUNT;

    -- Verify all backup rows were mapped (detect missing schemas)
    DECLARE
        backup_count integer;
    BEGIN
        SELECT count(*) INTO backup_count FROM public._age_pg_upgrade_backup;
        IF mapping_count < backup_count THEN
            RAISE EXCEPTION 'Only % of % graphs could be mapped. Some schema names may have changed or been dropped.',
                mapping_count, backup_count;
        END IF;
    END;

    -- Handle zero-graph case (still need to restore schema)
    IF mapping_count = 0 THEN
        RAISE NOTICE 'No graphs to remap (empty backup table).';
        DROP TABLE _graphid_mapping;
        -- Skip to schema restoration
    ELSE
        RAISE NOTICE 'Found % graph(s) to remap', mapping_count;

        -- Temporarily drop foreign key constraint
        ALTER TABLE ag_catalog.ag_label DROP CONSTRAINT IF EXISTS fk_graph_oid;

        -- Update ag_label.graph references to use new OIDs
        UPDATE ag_catalog.ag_label l
        SET graph = m.new_graphid
        FROM _graphid_mapping m
        WHERE l.graph = m.old_graphid;

        GET DIAGNOSTICS updated_labels = ROW_COUNT;
        RAISE NOTICE 'Updated % label record(s)', updated_labels;

        -- Update ag_graph.graphid and ag_graph.namespace to new OIDs
        UPDATE ag_catalog.ag_graph g
        SET graphid = m.new_graphid,
            namespace = m.new_graphid
        FROM _graphid_mapping m
        WHERE g.graphid = m.old_graphid;

        GET DIAGNOSTICS updated_graphs = ROW_COUNT;
        RAISE NOTICE 'Updated % graph record(s)', updated_graphs;

        -- Restore foreign key constraint
        ALTER TABLE ag_catalog.ag_label
        ADD CONSTRAINT fk_graph_oid
            FOREIGN KEY(graph) REFERENCES ag_catalog.ag_graph(graphid);

        -- Clean up temporary mapping table
        DROP TABLE _graphid_mapping;

        RAISE NOTICE 'Successfully completed pg_upgrade OID remapping.';
    END IF;

    --
    -- Restore original schema (revert namespace to regnamespace)
    --
    RAISE NOTICE 'Restoring original schema...';

    -- Drop the view (no longer needed with regnamespace)
    DROP VIEW IF EXISTS ag_catalog.ag_graph_view;

    -- Drop the existing oid-based index
    DROP INDEX IF EXISTS ag_catalog.ag_graph_namespace_index;

    -- Convert namespace column back to regnamespace
    ALTER TABLE ag_catalog.ag_graph
        ALTER COLUMN namespace TYPE regnamespace USING namespace::regnamespace;

    -- Recreate the index with regnamespace type
    CREATE UNIQUE INDEX ag_graph_namespace_index
        ON ag_catalog.ag_graph USING btree (namespace);

    RAISE NOTICE 'Successfully restored ag_graph.namespace to regnamespace type.';

    --
    -- Invalidate AGE's internal caches by touching each graph's namespace
    -- AGE registers a syscache callback on NAMESPACEOID, so altering a schema
    -- triggers cache invalidation. This ensures cypher queries work immediately
    -- without requiring a session reconnect.
    --
    -- We use xact-level advisory lock (auto-released at transaction end)
    -- and preserve original schema ownership.
    --
    RAISE NOTICE 'Invalidating AGE caches...';
    PERFORM pg_catalog.pg_advisory_xact_lock(pg_catalog.hashtext('age_finish_pg_upgrade'));
    DECLARE
        graph_rec RECORD;
        cache_invalidated boolean := false;
    BEGIN
        FOR graph_rec IN
            SELECT n.nspname AS ns_name, r.rolname AS owner_name
            FROM ag_catalog.ag_graph g
            JOIN pg_namespace n ON n.oid = g.namespace
            JOIN pg_roles r ON r.oid = n.nspowner
        LOOP
            BEGIN
                -- Touch schema by changing owner to current_user then back to original
                -- This triggers cache invalidation without permanently changing ownership
                EXECUTE pg_catalog.format('ALTER SCHEMA %I OWNER TO %I', graph_rec.ns_name, current_user);
                EXECUTE pg_catalog.format('ALTER SCHEMA %I OWNER TO %I', graph_rec.ns_name, graph_rec.owner_name);
                cache_invalidated := true;
            EXCEPTION WHEN insufficient_privilege THEN
                -- If we can't change ownership, skip this schema
                -- The cache will be invalidated on first use anyway
                RAISE NOTICE 'Could not invalidate cache for schema % (insufficient privileges)', graph_rec.ns_name;
            END;
        END LOOP;
        IF NOT cache_invalidated AND (SELECT count(*) FROM ag_catalog.ag_graph) > 0 THEN
            RAISE NOTICE 'Cache invalidation skipped. You may need to reconnect for cypher queries to work.';
        END IF;
    END;

    -- Now that all steps succeeded, clean up the backup table
    DROP TABLE IF EXISTS public._age_pg_upgrade_backup;

    RAISE NOTICE '';
    RAISE NOTICE 'pg_upgrade complete. All graph data has been preserved.';
END;
$function$;

COMMENT ON FUNCTION ag_catalog.age_finish_pg_upgrade() IS
'Completes pg_upgrade by remapping stale OIDs and restoring the original schema. Run this after pg_upgrade.';

--
-- age_revert_pg_upgrade_changes()
--
-- Reverts the schema changes made by age_prepare_pg_upgrade() if you need to
-- cancel the upgrade process before running pg_upgrade. This restores the
-- namespace column to its original regnamespace type.
--
-- NOTE: This function is NOT needed after age_finish_pg_upgrade(), which
-- automatically restores the original schema. Use this only if you called
-- age_prepare_pg_upgrade() but decided not to proceed with pg_upgrade.
--
-- This function:
--   1. Drops the ag_graph_view (no longer needed)
--   2. Drops the oid-based namespace index
--   3. Converts namespace column back to regnamespace
--   4. Recreates the namespace index with regnamespace type
--   5. Invalidates AGE caches to ensure cypher queries work immediately
--   6. Does NOT clean up the backup table (manual cleanup may be needed)
--
-- Returns: void
-- Side effects: Modifies ag_graph table structure
--
CREATE FUNCTION ag_catalog.age_revert_pg_upgrade_changes()
    RETURNS void
    LANGUAGE plpgsql
    -- Resolve built-in functions and operators from pg_catalog first so they
    -- are not overridden by same-named objects defined in ag_catalog. The
    -- ag_catalog objects referenced here are schema-qualified.
    SET search_path = pg_catalog, ag_catalog
    AS $function$
BEGIN
    -- Check if namespace column is oid type (needs reverting)
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_schema = 'ag_catalog'
          AND table_name = 'ag_graph'
          AND column_name = 'namespace'
          AND data_type = 'oid'
    ) THEN
        RAISE NOTICE 'ag_graph.namespace is already regnamespace type. Nothing to revert.';
        RETURN;
    END IF;

    -- Drop the view (no longer needed with regnamespace)
    DROP VIEW IF EXISTS ag_catalog.ag_graph_view;

    -- Drop the existing oid-based index
    DROP INDEX IF EXISTS ag_catalog.ag_graph_namespace_index;

    -- Convert namespace column back to regnamespace
    ALTER TABLE ag_catalog.ag_graph
        ALTER COLUMN namespace TYPE regnamespace USING namespace::regnamespace;

    -- Recreate the index with regnamespace type
    CREATE UNIQUE INDEX ag_graph_namespace_index
        ON ag_catalog.ag_graph USING btree (namespace);

    --
    -- Invalidate AGE's internal caches by touching each graph's namespace
    -- We use xact-level advisory lock and preserve original ownership
    --
    PERFORM pg_catalog.pg_advisory_xact_lock(pg_catalog.hashtext('age_revert_pg_upgrade'));
    DECLARE
        graph_rec RECORD;
    BEGIN
        FOR graph_rec IN
            SELECT n.nspname AS ns_name, r.rolname AS owner_name
            FROM ag_catalog.ag_graph g
            JOIN pg_namespace n ON n.oid = g.namespace
            JOIN pg_roles r ON r.oid = n.nspowner
        LOOP
            BEGIN
                -- Touch schema by changing owner to current_user then back to original
                EXECUTE pg_catalog.format('ALTER SCHEMA %I OWNER TO %I', graph_rec.ns_name, current_user);
                EXECUTE pg_catalog.format('ALTER SCHEMA %I OWNER TO %I', graph_rec.ns_name, graph_rec.owner_name);
            EXCEPTION WHEN insufficient_privilege THEN
                RAISE NOTICE 'Could not invalidate cache for schema % (insufficient privileges)', graph_rec.ns_name;
            END;
        END LOOP;
    END;

    RAISE NOTICE 'Successfully reverted ag_graph.namespace to regnamespace type.';
    RAISE NOTICE '';
    RAISE NOTICE 'Upgrade preparation has been cancelled.';
    RAISE NOTICE 'You may want to drop the backup table: DROP TABLE IF EXISTS public._age_pg_upgrade_backup;';
END;
$function$;

COMMENT ON FUNCTION ag_catalog.age_revert_pg_upgrade_changes() IS
'Reverts schema changes if you need to cancel after age_prepare_pg_upgrade() but before pg_upgrade. Not needed after age_finish_pg_upgrade().';

--
-- age_pg_upgrade_status()
--
-- Returns the current pg_upgrade readiness status of the AGE installation.
-- Useful for checking whether the database needs preparation before pg_upgrade.
--
-- Returns: TABLE with status information
--
CREATE FUNCTION ag_catalog.age_pg_upgrade_status()
    RETURNS TABLE (
        status text,
        namespace_type text,
        graph_count bigint,
        backup_exists boolean,
        message text
    )
    LANGUAGE plpgsql
    -- Resolve built-in functions and operators from pg_catalog first so they
    -- are not overridden by same-named objects defined in ag_catalog. The
    -- ag_catalog objects referenced here are schema-qualified.
    SET search_path = pg_catalog, ag_catalog
    AS $function$
DECLARE
    ns_type text;
    g_count bigint;
    backup_exists boolean;
BEGIN
    -- Get namespace column type
    SELECT data_type INTO ns_type
    FROM information_schema.columns
    WHERE table_schema = 'ag_catalog'
      AND table_name = 'ag_graph'
      AND column_name = 'namespace';

    -- Get graph count
    SELECT count(*) INTO g_count FROM ag_catalog.ag_graph;

    -- Check for backup table
    SELECT EXISTS(
        SELECT 1 FROM information_schema.tables
        WHERE table_schema = 'public'
          AND table_name = '_age_pg_upgrade_backup'
    ) INTO backup_exists;

    -- Determine status and message
    IF ns_type = 'regnamespace' AND NOT backup_exists THEN
        -- Normal state - ready for use, needs prep before pg_upgrade
        RETURN QUERY SELECT
            'NORMAL'::text,
            ns_type,
            g_count,
            backup_exists,
            'Run SELECT age_prepare_pg_upgrade(); before pg_upgrade'::text;
    ELSIF ns_type = 'regnamespace' AND backup_exists THEN
        -- Unusual state - backup exists but schema wasn't converted
        RETURN QUERY SELECT
            'WARNING'::text,
            ns_type,
            g_count,
            backup_exists,
            'Backup table exists but schema not converted. Run age_prepare_pg_upgrade() again.'::text;
    ELSIF ns_type = 'oid' AND backup_exists THEN
        -- Prepared and ready for pg_upgrade, or awaiting finish after pg_upgrade
        RETURN QUERY SELECT
            'PREPARED - AWAITING FINISH'::text,
            ns_type,
            g_count,
            backup_exists,
            'After pg_upgrade, run SELECT age_finish_pg_upgrade();'::text;
    ELSE
        -- oid type without backup - manually converted or partial state
        RETURN QUERY SELECT
            'CONVERTED'::text,
            ns_type,
            g_count,
            backup_exists,
            'Namespace is oid type. If upgrading, ensure backup table exists.'::text;
    END IF;
END;
$function$;

COMMENT ON FUNCTION ag_catalog.age_pg_upgrade_status() IS
'Returns the current pg_upgrade readiness status of the AGE installation.';
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

--
-- create_subgraph(): materialized subgraph extraction.
--
-- Builds a new, persistent AGE graph that is the subgraph of an existing graph
-- selected by a node predicate and a relationship predicate. The semantics
-- follow the graph-theory "induced subgraph" definition as operationalized by
-- Neo4j GDS gds.graph.filter():
--
--   * a vertex is kept iff node_filter evaluates true ('*' keeps all);
--   * an edge is kept iff relationship_filter evaluates true AND BOTH of its
--     endpoints were kept (the induced rule -- no dangling edges).
--
-- Unlike the Neo4j in-memory projection, the result is a real, ACID,
-- fully-Cypher-queryable AGE graph; properties of any agtype are preserved, and
-- self-loops / parallel edges (multigraph structure) are kept.
--
-- node_filter / relationship_filter are Cypher predicates bound to a single
-- entity -- the node variable is `n`, the relationship variable is `r` -- or
-- the literal '*' to keep all. They are evaluated by AGE's own Cypher engine
-- against the source graph, so the full Cypher predicate language is available.
--
-- Internal entity ids (graphids) are reassigned in the new graph (a graphid
-- encodes the source graph's label id, which differs in the destination), and
-- edge endpoints are remapped accordingly. Properties are copied verbatim.
--
CREATE FUNCTION ag_catalog.create_subgraph(new_graph name,
                                           from_graph name,
                                           node_filter text DEFAULT '*',
                                           relationship_filter text DEFAULT '*')
    RETURNS TABLE(node_count bigint, relationship_count bigint)
    LANGUAGE plpgsql
    VOLATILE
    SET search_path = ag_catalog, pg_catalog
    AS $function$
DECLARE
    from_oid oid;
    new_oid  oid;
    v_node_count bigint := 0;
    v_rel_count  bigint := 0;
    rec RECORD;
    cypher_q text;
    where_clause text;
    dst_label_id int;
    dst_seq_fqn text;
    dst_relation text;
    inserted bigint;
    has_rows boolean;
BEGIN
    -- Argument validation.
    IF new_graph IS NULL THEN
        RAISE EXCEPTION 'new graph name must not be NULL';
    END IF;
    IF from_graph IS NULL THEN
        RAISE EXCEPTION 'source graph name must not be NULL';
    END IF;
    IF new_graph = from_graph THEN
        RAISE EXCEPTION 'cannot extract a subgraph of "%" into itself', from_graph;
    END IF;

    -- NULL predicate is treated as the '*' wildcard (keep all).
    IF node_filter IS NULL THEN
        node_filter := '*';
    END IF;
    IF relationship_filter IS NULL THEN
        relationship_filter := '*';
    END IF;

    -- The predicates are embedded into a dollar-quoted cypher() query using the
    -- $age_subgraph$ tag; reject predicates that contain the tag to keep the
    -- quoting unambiguous.
    IF position('$age_subgraph$' IN node_filter) > 0
       OR position('$age_subgraph$' IN relationship_filter) > 0 THEN
        RAISE EXCEPTION 'filter predicate must not contain the reserved token $age_subgraph$';
    END IF;

    -- Validate source graph exists.
    SELECT graphid INTO from_oid
    FROM ag_catalog.ag_graph WHERE name = from_graph;
    IF from_oid IS NULL THEN
        RAISE EXCEPTION 'graph "%" does not exist', from_graph;
    END IF;

    -- Validate destination graph does not exist (create_graph also enforces
    -- naming rules and uniqueness, but we give a clear early error).
    IF EXISTS (SELECT 1 FROM ag_catalog.ag_graph WHERE name = new_graph) THEN
        RAISE EXCEPTION 'graph "%" already exists', new_graph;
    END IF;

    -- Create the destination graph (default labels are created automatically).
    PERFORM ag_catalog.create_graph(new_graph);

    SELECT graphid INTO new_oid
    FROM ag_catalog.ag_graph WHERE name = new_graph;

    -- Working sets / mapping (uniquely named to avoid colliding with user temps).
    DROP TABLE IF EXISTS _ag_sg_kept_v;
    DROP TABLE IF EXISTS _ag_sg_kept_e;
    DROP TABLE IF EXISTS _ag_sg_vmap;
    DROP TABLE IF EXISTS _ag_sg_vstage;
    DROP TABLE IF EXISTS _ag_sg_estage;

    --
    -- Kept vertices: evaluate node_filter with AGE's Cypher engine. The node
    -- variable `n` is bound exactly as in the spec; '*' selects all vertices.
    --
    IF node_filter IS NULL OR btrim(node_filter) = '*' THEN
        where_clause := '';
    ELSE
        where_clause := ' WHERE ' || node_filter;
    END IF;
    cypher_q := 'MATCH (n)' || where_clause || ' RETURN id(n)';

    EXECUTE format(
        'CREATE TEMP TABLE _ag_sg_kept_v ON COMMIT DROP AS '
        'SELECT DISTINCT ag_catalog.agtype_to_graphid(vid) AS gid '
        'FROM ag_catalog.cypher(%L, $age_subgraph$%s$age_subgraph$) AS (vid agtype)',
        from_graph, cypher_q);
    CREATE INDEX ON _ag_sg_kept_v (gid);

    --
    -- Kept edges: evaluate relationship_filter with AGE's Cypher engine. The
    -- relationship variable `r` is bound exactly as in the spec.
    --
    IF relationship_filter IS NULL OR btrim(relationship_filter) = '*' THEN
        where_clause := '';
    ELSE
        where_clause := ' WHERE ' || relationship_filter;
    END IF;
    cypher_q := 'MATCH ()-[r]->()' || where_clause || ' RETURN id(r)';

    EXECUTE format(
        'CREATE TEMP TABLE _ag_sg_kept_e ON COMMIT DROP AS '
        'SELECT DISTINCT ag_catalog.agtype_to_graphid(eid) AS gid '
        'FROM ag_catalog.cypher(%L, $age_subgraph$%s$age_subgraph$) AS (eid agtype)',
        from_graph, cypher_q);
    CREATE INDEX ON _ag_sg_kept_e (gid);

    -- old -> new vertex id mapping (graphid is unique within a graph).
    CREATE TEMP TABLE _ag_sg_vmap (old_id graphid PRIMARY KEY,
                                   new_id graphid NOT NULL) ON COMMIT DROP;

    --
    -- PASS 1: copy kept vertices, label by label, assigning new graphids and
    -- recording the old->new mapping for edge remapping.
    --
    FOR rec IN
        SELECT name, id, relation, seq_name
        FROM ag_catalog.ag_label
        WHERE graph = from_oid AND kind = 'v'
        ORDER BY id
    LOOP
        -- Skip labels with no surviving vertices. Read ONLY this label's own
        -- rows: AGE label tables use table inheritance (custom labels inherit
        -- from _ag_label_vertex), so a plain scan of a parent would also return
        -- its children and copy them twice.
        EXECUTE format(
            'SELECT EXISTS (SELECT 1 FROM ONLY %s t '
            'WHERE EXISTS (SELECT 1 FROM _ag_sg_kept_v k WHERE k.gid = t.id))',
            rec.relation::regclass::text)
        INTO has_rows;
        IF NOT has_rows THEN
            CONTINUE;
        END IF;

        -- Ensure the label exists in the destination graph.
        IF rec.name <> '_ag_label_vertex' THEN
            PERFORM 1 FROM ag_catalog.ag_label
            WHERE graph = new_oid AND name = rec.name;
            IF NOT FOUND THEN
                EXECUTE format('SELECT ag_catalog.create_vlabel(%L, %L)',
                               new_graph, rec.name);
            END IF;
        END IF;

        SELECT id, seq_name, relation::regclass::text
        INTO dst_label_id, dst_seq_fqn, dst_relation
        FROM ag_catalog.ag_label
        WHERE graph = new_oid AND name = rec.name;
        dst_seq_fqn := format('%I.%I', new_graph, dst_seq_fqn);

        -- Stage surviving vertices with freshly generated ids in a real temp
        -- table (single evaluation), then copy to the label table and record
        -- the old->new mapping. A materialized stage avoids any ambiguity from
        -- referencing a nextval-bearing CTE more than once.
        DROP TABLE IF EXISTS _ag_sg_vstage;
        EXECUTE format(
            'CREATE TEMP TABLE _ag_sg_vstage ON COMMIT DROP AS '
            'SELECT t.id AS old_id, '
            '       ag_catalog._graphid(%s, nextval(%L::regclass)) AS new_id, '
            '       t.properties AS props '
            'FROM ONLY %s t '
            'WHERE EXISTS (SELECT 1 FROM _ag_sg_kept_v k WHERE k.gid = t.id)',
            dst_label_id, dst_seq_fqn, rec.relation::regclass::text);

        EXECUTE format('INSERT INTO %s (id, properties) '
                       'SELECT new_id, props FROM _ag_sg_vstage', dst_relation);

        INSERT INTO _ag_sg_vmap (old_id, new_id)
            SELECT old_id, new_id FROM _ag_sg_vstage;

        DROP TABLE _ag_sg_vstage;
    END LOOP;

    SELECT count(*) INTO v_node_count FROM _ag_sg_vmap;

    --
    -- PASS 2: copy kept edges, remapping endpoints. The joins to _ag_sg_vmap
    -- enforce the induced rule (an edge survives only if BOTH endpoints were
    -- kept); membership in _ag_sg_kept_e applies relationship_filter.
    --
    FOR rec IN
        SELECT name, id, relation, seq_name
        FROM ag_catalog.ag_label
        WHERE graph = from_oid AND kind = 'e'
        ORDER BY id
    LOOP
        -- Skip labels with no surviving edges. Read ONLY this label's own rows
        -- (see the vertex pass for why inheritance requires ONLY).
        EXECUTE format(
            'SELECT EXISTS ('
            '  SELECT 1 FROM ONLY %s x '
            '  JOIN _ag_sg_vmap vs ON vs.old_id = x.start_id '
            '  JOIN _ag_sg_vmap ve ON ve.old_id = x.end_id '
            '  WHERE EXISTS (SELECT 1 FROM _ag_sg_kept_e k WHERE k.gid = x.id))',
            rec.relation::regclass::text)
        INTO has_rows;
        IF NOT has_rows THEN
            CONTINUE;
        END IF;

        IF rec.name <> '_ag_label_edge' THEN
            PERFORM 1 FROM ag_catalog.ag_label
            WHERE graph = new_oid AND name = rec.name;
            IF NOT FOUND THEN
                EXECUTE format('SELECT ag_catalog.create_elabel(%L, %L)',
                               new_graph, rec.name);
            END IF;
        END IF;

        SELECT id, seq_name, relation::regclass::text
        INTO dst_label_id, dst_seq_fqn, dst_relation
        FROM ag_catalog.ag_label
        WHERE graph = new_oid AND name = rec.name;
        dst_seq_fqn := format('%I.%I', new_graph, dst_seq_fqn);

        -- Stage surviving edges, remapping endpoints through _ag_sg_vmap. The
        -- joins enforce the induced rule (both endpoints kept); membership in
        -- _ag_sg_kept_e applies relationship_filter.
        DROP TABLE IF EXISTS _ag_sg_estage;
        EXECUTE format(
            'CREATE TEMP TABLE _ag_sg_estage ON COMMIT DROP AS '
            'SELECT ag_catalog._graphid(%s, nextval(%L::regclass)) AS new_id, '
            '       vs.new_id AS new_start, ve.new_id AS new_end, '
            '       x.properties AS props '
            'FROM ONLY %s x '
            'JOIN _ag_sg_vmap vs ON vs.old_id = x.start_id '
            'JOIN _ag_sg_vmap ve ON ve.old_id = x.end_id '
            'WHERE EXISTS (SELECT 1 FROM _ag_sg_kept_e k WHERE k.gid = x.id)',
            dst_label_id, dst_seq_fqn, rec.relation::regclass::text);

        EXECUTE format('INSERT INTO %s (id, start_id, end_id, properties) '
                       'SELECT new_id, new_start, new_end, props '
                       'FROM _ag_sg_estage', dst_relation);
        GET DIAGNOSTICS inserted = ROW_COUNT;
        v_rel_count := v_rel_count + inserted;

        DROP TABLE _ag_sg_estage;
    END LOOP;

    RETURN QUERY SELECT v_node_count, v_rel_count;
END;
$function$;

COMMENT ON FUNCTION ag_catalog.create_subgraph(name, name, text, text) IS
'Materializes a new persistent graph as the induced subgraph of from_graph selected by a Cypher node predicate (on n) and relationship predicate (on r); ''*'' keeps all. An edge is kept only if its predicate holds and both endpoints are kept. Returns (node_count, relationship_count).';
