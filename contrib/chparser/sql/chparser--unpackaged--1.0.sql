-- Portions Copyright (c) 2012-2013, Jov(amutu@amutu.com)
-- SPDX-License-Identifier: PostgreSQL
-- Adapted from zhparser--unpackaged--1.0.sql in zhparser. See ../COPYRIGHT for the original notice.

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION chparser" to load this file. \quit

ALTER EXTENSION chparser ADD function chprs_start(internal,integer);
ALTER EXTENSION chparser ADD function chprs_getlexeme(internal,internal,internal);
ALTER EXTENSION chparser ADD function chprs_end(internal);
ALTER EXTENSION chparser ADD function chprs_lextype(internal);
ALTER EXTENSION chparser ADD text search parser chparser;
