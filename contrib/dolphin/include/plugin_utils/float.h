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
 * float.h
 *    Declarations for operations on float types.
 *
 * -------------------------------------------------------------------------
 */

#ifndef PLUGIN_FLOAT_H
#define PLUGIN_FLOAT_H

#include "utils/float.h"

/*
 * Converts float8 number to a string
 * using a standard output format
 *
 * @param ascii The buffer to output conversion result.
 * @param ascii_size The size of `ascii`.
 */
template <std::size_t ascii_size>
void dolphin_dtoa(double num, char* ascii)
{
    errno_t rc = memset_sp(ascii, ascii_size, 0, ascii_size);
    securec_check(rc, "\0", "\0");

    if (std::isnan(num)) {
        if (DB_IS_CMPT(A_FORMAT) && u_sess->attr.attr_sql.enable_binary_special_a_format && !is_req_from_jdbc()) {
            rc = strcpy_sp(ascii, ascii_size, "Nan");
        } else {
            rc = strcpy_sp(ascii, ascii_size, "NaN");
        }
        securec_check(rc, "\0", "\0");
        return;
    } else if (std::isinf(num)) {
        if (num > 0) {
        if (DB_IS_CMPT(A_FORMAT) && u_sess->attr.attr_sql.enable_binary_special_a_format && !is_req_from_jdbc()) {
                rc = strcpy_sp(ascii, ascii_size, "Inf");
            } else {
                rc = strcpy_sp(ascii, ascii_size, "Infinity");
            }
        } else {
        if (DB_IS_CMPT(A_FORMAT) && u_sess->attr.attr_sql.enable_binary_special_a_format && !is_req_from_jdbc()) {
                rc = strcpy_sp(ascii, ascii_size, "-Inf");
            } else {
                rc = strcpy_sp(ascii, ascii_size, "-Infinity");
            }
        }
        securec_check(rc, "\0", "\0");
        return;
    }

    if (u_sess->attr.attr_common.extra_float_digits > 0) {
        (void)double_to_shortest_decimal_buf(num, ascii);
    } else {
#ifdef DOLPHIN
        int ndig = std::numeric_limits<double>::digits10 + u_sess->attr.attr_common.extra_float_digits + 1;
#else
        int ndig = std::numeric_limits<double>::digits10 + u_sess->attr.attr_common.extra_float_digits;
#endif
        if (ndig < 1) {
            ndig = 1;
        }

        rc = snprintf_s(ascii, ascii_size, ascii_size - 1, "%.*g", ndig, num);
        securec_check_ss(rc, "\0", "\0");
    }

    if (DISPLAY_LEADING_ZERO) {
        return;
    }

    if ((num > 0 && num < 1) || (num > -1 && num < 0)) {
        detail::delete_leading_zero(ascii, ascii_size);
    }
}

#endif
