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
 * Scan.cpp
 *    File Scanner
 *
 * IDENTIFICATION
 *    contrib/assessment/Scan.h
 *
 * -------------------------------------------------------------------------
 */

#ifndef OPENGAUSS_SCAN_H
#define OPENGAUSS_SCAN_H

#include <cstddef>
#include "psqlscan.h"
#include "settings.h"
#include <vector>
#include <string>


/**
 * init some variable needed by scan.l
 */
extern void PSqlPostInit(void);

extern PsqlSettings pset;

class Scan {
public:
    explicit Scan(FILE* fd);

    vector <std::string> GetNextSql();

    void FreeScan();

private:
    PsqlScanState scanState = nullptr;
    FILE* fd = nullptr;
};


#endif //OPENGAUSS_SCAN_H
