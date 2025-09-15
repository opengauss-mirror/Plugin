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
 * thread_cache.cc
 *
 * --------------------------------------------------------------------------------------
 */

#include "distributed/context/thread_context.h"
#include "access/hash.h"
#include "utils/hashutils.h"

namespace Spq {

THR_LOCAL ThreadContext* ThdCtx = nullptr;

void ThreadContext::Initialize()
{
    auto mem = AllocSetContextCreate(TopMemoryContext, "Spq thread Context",
                                     ALLOCSET_DEFAULT_MINSIZE, ALLOCSET_DEFAULT_INITSIZE,
                                     ALLOCSET_DEFAULT_MAXSIZE);

    ThdCtx = new (MemoryContextAlloc(mem, sizeof(ThreadContext))) ThreadContext(mem);
}

uint32 ThreadContext::ConnectionHashHash(const void* key, Size keysize)
{
    ConnectionHashKey* entry = (ConnectionHashKey*)key;

    uint32 hash = string_hash(entry->hostname, NAMEDATALEN);
    hash = hash_combine(hash, hash_uint32(entry->port));
    hash = hash_combine(hash, string_hash(entry->user, NAMEDATALEN));
    hash = hash_combine(hash, string_hash(entry->database, NAMEDATALEN));
    hash = hash_combine(hash, hash_uint32(entry->replicationConnParam));

    return hash;
}

int ThreadContext::ConnectionHashCompare(const void* a, const void* b, Size keysize)
{
    ConnectionHashKey* ca = (ConnectionHashKey*)a;
    ConnectionHashKey* cb = (ConnectionHashKey*)b;

    if (strncmp(ca->hostname, cb->hostname, MAX_NODE_LENGTH) != 0 ||
        ca->port != cb->port || ca->replicationConnParam != cb->replicationConnParam ||
        strncmp(ca->user, cb->user, NAMEDATALEN) != 0 ||
        strncmp(ca->database, cb->database, NAMEDATALEN) != 0) {
        return 1;
    } else {
        return 0;
    }
}

void ThreadContext::CreateParamsCache()
{
    HASHCTL connParamsInfo;
    uint32 hashFlags = (HASH_ELEM | HASH_FUNCTION | HASH_CONTEXT | HASH_COMPARE);
    memset(&connParamsInfo, 0, sizeof(connParamsInfo));
    connParamsInfo.keysize = sizeof(ConnectionHashKey);
    connParamsInfo.hash = ConnectionHashHash;
    connParamsInfo.match = ConnectionHashCompare;
    connParamsInfo.hcxt = m_mem;
    connParamsInfo.entrysize = sizeof(ConnParamsElem);

    m_connParamsHash = hash_create("spq connparams cache (host,port,user,database)", 64,
                                   &connParamsInfo, hashFlags);
}

void ThreadContext::FreeConnParamsHashEntryFields(ConnParamsElem* elem)
{
    if (elem->keywords != NULL) {
        char** keyword = &elem->keywords[elem->runtimeParamStart];
        while (*keyword != NULL) {
            pfree(*keyword);
            keyword++;
        }
        pfree(elem->keywords);
        elem->keywords = NULL;
    }

    if (elem->values != NULL) {
        char** value = &elem->values[elem->runtimeParamStart];
        while (*value != NULL) {
            pfree(*value);
            value++;
        }
        pfree(elem->values);
        elem->values = NULL;
    }

    elem->runtimeParamStart = 0;
}

void ThreadContext::InvalidateParamsCache()
{
    ConnParamsElem* elem = nullptr;
    HASH_SEQ_STATUS hash_seq;

    if (m_connParamsHash == nullptr) {
        return;
    }

    hash_seq_init(&hash_seq, m_connParamsHash);
    while ((elem = (ConnParamsElem*)hash_seq_search(&hash_seq)) != nullptr) {
        FreeConnParamsHashEntryFields(elem);
        hash_search(m_connParamsHash, &elem->key, HASH_REMOVE, nullptr);
    }
}

ConnParamsElem* ThreadContext::FindOrCreateConnParams(ConnectionHashKey* key)
{
    bool found = false;

    if (m_connParamsHash == nullptr) {
        CreateParamsCache();
    }

    /* search our cache for precomputed connection settings */
    ConnParamsElem* elem =
        (ConnParamsElem*)hash_search(m_connParamsHash, key, HASH_ENTER, &found);
    if (!found) {
        memset(((char*)elem) + sizeof(ConnectionHashKey), 0,
               sizeof(ConnParamsElem) - sizeof(ConnectionHashKey));

        /* if not found or not valid, compute them from GUC, runtime, etc. */
        GetConnParams(key, &elem->keywords, &elem->values, &elem->runtimeParamStart,
                      m_mem);
    }

    return elem;
}

}  // namespace Spq