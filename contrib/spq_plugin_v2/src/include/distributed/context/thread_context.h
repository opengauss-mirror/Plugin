/*
 * Copyright (c) 2025 Huawei Technologies Co.,Ltd.
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * --------------------------------------------------------------------------------------
 *
 * thread_cache.h
 *
 * --------------------------------------------------------------------------------------
 */
#ifndef SPQ_THREAD_CONTEXT_H
#define SPQ_THREAD_CONTEXT_H

#include "postgres.h"
#include "utils/hsearch.h"

#include "distributed/connection_management.h"

namespace Spq {

/* hash entry for cached connection parameters */
struct ConnParamsElem {
    ConnectionHashKey key;
    Index runtimeParamStart;
    char** keywords;
    char** values;
};

/** Simple class for all thread context, including thread cache
  @TODO: refactor it for other caches. */
class ThreadContext {
public:
    /** Create the thread context. */
    static void Initialize();

    /** Invalidate all connection parameters caches. */
    void InvalidateParamsCache();

    /** find a connection parameter*/
    ConnParamsElem* FindOrCreateConnParams(ConnectionHashKey* key);

private:
    ThreadContext(MemoryContext mem) : m_mem(mem)
    {}

    ~ThreadContext()
    {}

    void CreateParamsCache();

    static uint32 ConnectionHashHash(const void* key, Size keysize);

    static int ConnectionHashCompare(const void* a, const void* b, Size keysize);

    void FreeConnParamsHashEntryFields(ConnParamsElem* elem);

    MemoryContext m_mem{nullptr};

    HTAB* m_connParamsHash{nullptr};
};

extern THR_LOCAL ThreadContext* ThdCtx;
}  // namespace Spq
#endif