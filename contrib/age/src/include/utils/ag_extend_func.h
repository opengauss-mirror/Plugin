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


#include "postgres.h"
#include "fmgr.h"


extern Datum hash_any_extended(register const unsigned char *k,
                               register int keylen, uint64 seed);
// copy pg fmgrprotos.h
extern Datum hash_numeric_extended(PG_FUNCTION_ARGS);
extern Datum hashcharextended(PG_FUNCTION_ARGS);
extern Datum hashint8extended(PG_FUNCTION_ARGS);
extern Datum hashfloat8extended(PG_FUNCTION_ARGS);
extern Datum int8out(PG_FUNCTION_ARGS);
extern Datum int8in(PG_FUNCTION_ARGS);
