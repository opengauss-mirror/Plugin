/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
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
 * assessment.h
 *    extension common header
 *
 * IDENTIFICATION
 *    contrib/assessment/assessment.h
 *
 * -------------------------------------------------------------------------
 */

#ifndef OPENGAUSS_ASSESSMENT_H
#define OPENGAUSS_ASSESSMENT_H

enum AssessmentType {
    DDL, DML, DCL, EXPLAIN, COMMENT, /* comments or empty input */
    DATABASE_COMMAND, TRANSACTION, /* start, start transaction, commit, etc */
    SET_VARIABLE, TODO, UNSUPPORTED
};

enum CompatibilityType {
    COMPATIBLE, AST_COMPATIBLE, INCOMPATIBLE, UNSUPPORTED_COMPATIBLE, SKIP_COMMAND,
};

#endif //OPENGAUSS_ASSESSMENT_H
