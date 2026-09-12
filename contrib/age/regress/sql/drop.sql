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

LOAD 'age';
SET search_path TO ag_catalog;

SELECT create_graph('drop');

DROP EXTENSION age;

SELECT nspname FROM pg_catalog.pg_namespace WHERE nspname = 'drop';

SELECT tablename FROM pg_catalog.pg_tables WHERE schemaname = 'ag_catalog';

-- A loaded library must leave parser, planner, and object hooks idle while the
-- extension catalog entry and ag_catalog schema are absent.
SELECT EXISTS (
    SELECT 1 FROM pg_catalog.pg_namespace WHERE nspname = 'pg_catalog'
) AS hooks_are_idle;
CREATE SCHEMA _regress_drop;
DROP SCHEMA _regress_drop;

-- Recreate the extension and validate we can recreate a graph
CREATE EXTENSION age;

SELECT create_graph('drop');

-- Create a schema that uses the agtype, so we can't just drop age.
CREATE SCHEMA other_schema;

CREATE TABLE other_schema.tbl (id agtype);

-- Should Fail because agtype can't be dropped
DROP EXTENSION age;

-- Check the graph still exist, because the DROP command failed
SELECT nspname FROM pg_catalog.pg_namespace WHERE nspname = 'drop';

-- A failed extension drop must restore the graph-protection object hook.
DROP TABLE drop._ag_label_vertex;

-- Should succeed, delete the 'drop' schema and leave 'other_schema'
DROP EXTENSION age CASCADE;

-- 'other_schema' should exist, 'drop' should be deleted
SELECT nspname FROM pg_catalog.pg_namespace WHERE nspname IN ('other_schema', 'drop');
