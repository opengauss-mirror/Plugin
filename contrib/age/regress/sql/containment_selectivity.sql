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

/*
 * Planner selectivity binding regression for the agtype containment and
 * key-existence operators (issue #2356).
 *
 * NOTE (openGauss): the upstream Apache AGE suite also replays the
 * age--1.7.0--y.y.y.sql upgrade block, which flips the operators onto
 * matchingsel / matchingjoinsel and back. openGauss does not provide the
 * matchingsel / matchingjoinsel selectivity functions (they are a later
 * PostgreSQL addition), and this plugin ships a single age--1.0.0.sql without a
 * version upgrade chain, so that PostgreSQL-specific upgrade-path assertion is
 * intentionally omitted here. The fresh-install checks below fully cover the
 * openGauss delivery.
 */

LOAD 'age';
SET search_path TO ag_catalog;

-- Selectivity helpers for the four containment operators.
SELECT o.oprname,
       pg_catalog.format_type(o.oprleft,  NULL) AS lhs,
       pg_catalog.format_type(o.oprright, NULL) AS rhs,
       o.oprrest::text  AS restrict_fn,
       o.oprjoin::text  AS join_fn
FROM   pg_catalog.pg_operator o
JOIN   pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
WHERE  n.nspname = 'ag_catalog'
  AND  o.oprname IN ('@>', '<@', '@>>', '<<@')
ORDER  BY o.oprname, lhs, rhs;

-- Selectivity helpers for all key-existence operator overloads
-- (right-hand side may be text, text[], or agtype).
SELECT o.oprname,
       pg_catalog.format_type(o.oprleft,  NULL) AS lhs,
       pg_catalog.format_type(o.oprright, NULL) AS rhs,
       o.oprrest::text  AS restrict_fn,
       o.oprjoin::text  AS join_fn
FROM   pg_catalog.pg_operator o
JOIN   pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
WHERE  n.nspname = 'ag_catalog'
  AND  o.oprname IN ('?', '?|', '?&')
ORDER  BY o.oprname, lhs, rhs;

-- Scoped guard for issue #2356: assert that none of the specific containment
-- and key-existence operators on agtype are bound to matchingsel /
-- matchingjoinsel. We deliberately limit the check to these operator names
-- (rather than every operator in ag_catalog) so unrelated operators that
-- legitimately use matchingsel for their own semantics are not affected by
-- this regression test.
SELECT COUNT(*) AS leaked_matchingsel_bindings
FROM   pg_catalog.pg_operator o
JOIN   pg_catalog.pg_namespace n ON n.oid = o.oprnamespace
WHERE  n.nspname = 'ag_catalog'
  AND  o.oprname IN ('@>', '<@', '@>>', '<<@', '?', '?|', '?&')
  AND  (o.oprrest::text  = 'matchingsel'
        OR o.oprjoin::text = 'matchingjoinsel');

-- Smoke test: each operator still works functionally. Selectivity binding
-- only affects the planner; this guards against an inadvertent operator
-- removal as part of any future cleanup.
SELECT '{"a":1,"b":2}'::agtype @>  '{"a":1}'::agtype             AS contains_yes;
SELECT '{"a":1}'::agtype       <@  '{"a":1,"b":2}'::agtype       AS contained_yes;
SELECT '{"a":{"b":1}}'::agtype @>> '{"a":{"b":1}}'::agtype       AS top_contains_yes;
SELECT '{"a":{"b":1}}'::agtype <<@ '{"a":{"b":1}}'::agtype       AS top_contained_yes;
SELECT '{"a":1}'::agtype       ?   'a'::text                     AS exists_text_yes;
SELECT '{"a":1}'::agtype       ?   '"a"'::agtype                 AS exists_agtype_yes;
SELECT '{"a":1,"b":2}'::agtype ?|  ARRAY['a','c']                AS exists_any_text_yes;
SELECT '{"a":1,"b":2}'::agtype ?|  '["a","c"]'::agtype           AS exists_any_agtype_yes;
SELECT '{"a":1,"b":2}'::agtype ?&  ARRAY['a','b']                AS exists_all_text_yes;
SELECT '{"a":1,"b":2}'::agtype ?&  '["a","b"]'::agtype           AS exists_all_agtype_yes;