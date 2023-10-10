/*
 * This file and its contents are licensed under the Apache License 2.0.
 * Please see the included NOTICE for copyright information and
 * LICENSE-APACHE for a copy of the license.
 */
#ifndef TIMESCALEDB_CACHE_H
#define TIMESCALEDB_CACHE_H

#include <postgres.h>
#include <utils/memutils.h>
#include <utils/hsearch.h>

#include "tsdb_head.h"
#include "export.h"
#include "commands/extension.h"


typedef enum TelemetryLevel
{
	TELEMETRY_OFF,
	TELEMETRY_BASIC,
} TelemetryLevel;

typedef enum CacheQueryFlags
{
	CACHE_FLAG_NONE = 0,
	CACHE_FLAG_MISSING_OK = 1 << 0,
	CACHE_FLAG_NOCREATE = 1 << 1,
} CacheQueryFlags;

#define CACHE_FLAG_CHECK (CACHE_FLAG_MISSING_OK | CACHE_FLAG_NOCREATE)

typedef struct CacheQuery
{
	/* CacheQueryFlags as defined above */
	const unsigned int flags;
	void *result;
	void *data;
} CacheQuery;

typedef struct CacheStats
{
	long numelements;
	uint64 hits;
	uint64 misses;
} CacheStats;

typedef struct Cache
{
	HASHCTL hctl;
	HTAB *htab;
	int refcount;
	const char *name;
	long numelements;
	int flags;
	CacheStats stats;
	void *(*get_key)(struct CacheQuery *);
	void *(*create_entry)(struct Cache *, CacheQuery *);
	void *(*update_entry)(struct Cache *, CacheQuery *);
	void (*missing_error)(const struct Cache *, const CacheQuery *);
	bool (*valid_result)(const void *);
	void (*pre_destroy_hook)(struct Cache *);
	bool release_on_commit; /* This should be false if doing
							 * cross-commit operations like CLUSTER or
							 * VACUUM */
} Cache;

typedef struct tsdb_session_context {
    List *tsdb_pinned_caches;
    MemoryContext tsdb_pinned_caches_mctx;
	Cache *tsdb_hypertable_cache_current;
	List *tsdb_planner_hcaches;
	const char *tsdb_TS_CTE_EXPAND;
	ExtensiblePathMethods tsdb_constraint_aware_append_path_methods;
	ExtensiblePlanMethods tsdb_constraint_aware_append_plan_methods;
	ExtensibleExecMethods tsdb_constraint_aware_append_state_methods;
	ExtensiblePlanMethods tsdb_chunk_append_plan_methods;
	bool tsdb_expect_chunk_modification;

	
	struct config_enum_entry tsdb_telemetry_level_options[3];
	TelemetryLevel tsdb_on_level;
	bool tsdb_first_start;
} tsdb_session_context; 



extern void ts_cache_init(Cache *cache);
extern void ts_cache_invalidate(Cache *cache);
extern void *ts_cache_fetch(Cache *cache, CacheQuery *query);
extern bool ts_cache_remove(Cache *cache, void *key);

extern MemoryContext ts_cache_memory_ctx(Cache *cache);

extern Cache *ts_cache_pin(Cache *cache);
extern TSDLLEXPORT int ts_cache_release(Cache *cache);



extern void _cache_init(void);
extern void _cache_fini(void);

extern "C" void set_extension_index(uint32 index);
extern "C" void init_session_vars(void);

extern tsdb_session_context* get_session_context(bool is_from_PG_init=false);

#endif /* TIMESCALEDB_CACHE_H */
