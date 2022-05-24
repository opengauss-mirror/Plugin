/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#ifndef VECFUNC_PLUGIN_H
#define VECFUNC_PLUGIN_H

#include "utils/builtins.h"
#include "utils/fmgrtab.h"
#include "mb/pg_wchar.h"

typedef Datum (*sub_Array_Plugin)(Datum str, int32 start, int32 length, bool* isnull, mblen_converter fun_mblen);

extern sub_Array_Plugin substr_Array_Plugin[32];

extern void InitVecsubarrayPlugin(void);

#endif /* VECFUNC_PLUGIN_H */
