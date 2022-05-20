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
 * Report.h
 *    Assessment Report Generator
 *
 * IDENTIFICATION
 *    contrib/assessment/Report.h
 *
 * -------------------------------------------------------------------------
 */

#ifndef OPENGAUSS_REPORT_H
#define OPENGAUSS_REPORT_H

#include "string"
#include <vector>

class SQLCompatibility {
public:
    SQLCompatibility(std::string sql, std::string compatibility, std::string errDetail) : sql(std::move(sql)),
                                                                                          compatibility(std::move(
                                                                                              
                                                                                              compatibility)),
                                                                                          errDetail(std::move(
                                                                                              errDetail)) {}

    std::string sql;
    std::string compatibility;
    std::string errDetail;
};

class CompatibilityTable {
public:
    void AppendOneSQL(std::string sql, std::string compatibility, std::string errDetail);

    bool GenerateReport();

    bool GenerateReportHeader(char* fileName, char* output, const char* dbType);

    bool GenerateReportEnd();

    ~CompatibilityTable();
private:
    FILE* fd;
    std::vector <SQLCompatibility> sqlCompatibilities;
};


#endif //OPENGAUSS_REPORT_H
