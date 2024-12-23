/*
 * Portions Copyright (c) 2024 Huawei Technologies Co.,Ltd.
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
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
 * -------------------------------------------------------------------------
 *
 * numutils.h
 *    Declarations for operations on integer types.
 *
 * -------------------------------------------------------------------------
 */

#ifndef PLUGIN_NUMUTILS_H
#define PLUGIN_NUMUTILS_H

#include "utils/numutils.h"

inline char* pg_ultostr_zeropad_min_width_2(char* str, uint32 value)
{
    if (likely(value < 100)) {
        str = pg_ultostr_zeropad_width_2(str, value);
        return str;
    }

    return pg_ultostr_zeropad(str, value, 2);
}


#endif
