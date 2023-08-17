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
 * OpenGaussConnection.h
 *    OpenGauss Connection tools
 *
 * IDENTIFICATION
 *    contrib/assessment/OpenGaussConnection.cpp
 *
 * -------------------------------------------------------------------------
 */

#ifndef OPENGAUSS_OPENGAUSSCONNECTION_H
#define OPENGAUSS_OPENGAUSSCONNECTION_H

#include "libpq/libpq-fe.h"
#include "string"

class OpenGaussConnection {
public:
    bool ConnectDB();

    bool Exit();

    bool ExecCommand(const char* sql);

    std::string ExecQuery(const char* sql);

    std::string GetExecError();

    bool ExecExplainCommand(const char* sql);

    bool ExecDDLCommand(const char* sql);

    /* single get_set function */
    void SetDB(char* dbname);

    void SetHost(char* host);

    void SetPort(char* port);

    void SetPassword(char* password);

    void SetUsername(char* username);

    const char* GetDB();

    /* create assessment database */
    bool CreateAssessmentDB(char* compatibility);

    ~OpenGaussConnection();

private:
    PGconn* connection = nullptr;
    char* host = nullptr;
    char* port = nullptr;
    char* username = nullptr;
    char* password = nullptr;
    char* dbname = nullptr;
    const char* applicationName = "gs_assessment";
    const char* clientEncoding = "auto";
    const char* connectTimeout = "300";

};


#endif // OPENGAUSS_OPENGAUSSCONNECTION_H