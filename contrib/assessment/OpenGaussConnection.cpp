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
 * OpenGaussConnection.cpp
 *    OpenGauss Connection tools
 *
 * IDENTIFICATION
 *    contrib/assessment/OpenGaussConnection.cpp
 *
 * -------------------------------------------------------------------------
 */

#include "OpenGaussConnection.h"
#include "securec.h"
#include "securec_check.h"
#include <unistd.h>
#include <sys/time.h>
/* pset */
#include "Scan.h"

using namespace std;

OpenGaussConnection::~OpenGaussConnection()
{
    if (host != nullptr) {
        free(host);
    }
    if (username != nullptr) {
        free(username);
    }
    if (password != nullptr) {
        free(password);
    }
    if (port != nullptr) {
        free(port);
    }
    if (dbname != nullptr) {
        free(dbname);
    }
    this->Exit();
}

bool OpenGaussConnection::CreateAssessmentDB(char* compatibility)
{

    struct timeval tv;
    gettimeofday(&tv, NULL);
    string dbname = "assessment_" + to_string(tv.tv_usec);
    string sqlCommand = "create database " + dbname + " dbcompatibility '" + compatibility + "'";
    if (!this->ExecCommand(sqlCommand.c_str())) {
        return false;
    }
    this->SetDB((char*) dbname.c_str());
    return true;
}

bool OpenGaussConnection::ConnectDB()
{
    string connect =
            "application_name=assessment port=" + string(this->port) + " connect_timeout=" + this->connectTimeout;

    if (this->username) {
        connect = connect + " user=" + this->username;
    }
    if (this->host) {
        connect = connect + " host=" + this->host;
    }
    if (this->dbname) {
        connect = connect + " dbname=" + this->dbname;
    }

    if (this->password) {
        connect = connect + " password=" + this->password;
        securec_check_c(memset_s(this->password, strlen(this->password), 0, strlen(this->password)), "", "");
    }

    if ((this->connection = PQconnectdb(connect.c_str())) != nullptr && PQstatus(this->connection) == CONNECTION_OK) {
        return true;
    }
    return false;
}

string OpenGaussConnection::GetExecError()
{
    return PQerrorMessage(this->connection);
}

bool OpenGaussConnection::Exit()
{
    if (this->connection != nullptr) {
        PQfinish(this->connection);
        this->connection = nullptr;
    }
    return true;
}

bool OpenGaussConnection::ExecCommand(const char* sql)
{
    PGresult* res = PQexec(this->connection, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
}

std::string OpenGaussConnection::ExecQuery(const char* sql)
{
    PGresult* res = PQexec(this->connection, sql);
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) == 1) {
        auto result = string(PQgetvalue(res, 0, 0));
        PQclear(res);
        return result;
    }
    PQclear(res);
    return "";
}

bool OpenGaussConnection::ExecExplainCommand(const char* sql)
{
    PGresult* res = PQexec(this->connection, sql);
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        PQclear(res);
        return true;
    }
    PQclear(res);
    return false;
}

bool OpenGaussConnection::ExecDDLCommand(const char* sql)
{
    PGresult* res = PQexec(this->connection, sql);
    /* PGRES_EMPTY_QUERY for comments */
    if (PQresultStatus(res) == PGRES_COMMAND_OK || PQresultStatus(res) == PGRES_EMPTY_QUERY) {
        PQclear(res);
        return true;
    }
    PQclear(res);
    return false;
}

static void SetValue(char** dst, char* value)
{
    if (*dst != nullptr) {
        free(*dst);
    }
    if (value == nullptr) {
        *dst = nullptr;
        return;
    }
    auto newLen = strlen(value) + 1;
    *dst = (char*) malloc(newLen);
    auto ret = memcpy_s(*dst, newLen, value, newLen);
    securec_check_c(ret, "\0", "\0");
    (*dst)[newLen - 1] = '\0';
}

void OpenGaussConnection::SetDB(char* dbname)
{
    SetValue(&(this->dbname), dbname);
}

void OpenGaussConnection::SetHost(char* host)
{
    SetValue(&(this->host), host);
}

void OpenGaussConnection::SetPassword(char* password)
{
    SetValue(&(this->password), password);
}

void OpenGaussConnection::SetUsername(char* username)
{
    SetValue(&(this->username), username);
}

void OpenGaussConnection::SetPort(char* port)
{
    SetValue(&(this->port), port);
}

const char* OpenGaussConnection::GetDB()
{
    return dbname;
}
