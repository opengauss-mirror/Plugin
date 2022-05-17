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
 * main.cpp
 *    Assessment Entry
 *
 * IDENTIFICATION
 *    contrib/assessment/main.cpp
 *
 * -------------------------------------------------------------------------
 */

#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <regex>

#include "assessment.h"
#include "libpq/libpq-fe.h"
#include "securec.h"
#include "Scan.h"
#include "securec_check.h"
#include "Report.h"
#include "OpenGaussConnection.h"
#include "miscadmin.h"
#include "getopt_long.h"

using namespace std;


static regex pattern("'");

/* g_dbCompatArray */
struct AssessmentSettings {
    bool needCreateDatabase = false;
    bool plugin = false;
    bool extension = false;
    /* input database type */
    int database = -1;

    /* connect option */
    char* host;
    char* port;
    char* username;
    char* password;
    char* dbname;
    /* file name */
    char* inputFile = nullptr;
    char* outputFile = nullptr;
};

int g_globalDatabaseType = -1;

static AssessmentSettings g_assessmentSettings{};

vector <string> SplitSQLFile(FILE* file);

void InitParam(char* argv[], int argc);

void CheckOutputPrivilege();

FILE* CheckAndOpenFile(char* path, const char* mode);

void CreateAssessmentDatabase();

vector <string> SplitSQLFile(const FILE* file);

void PrintProcess(size_t sqlSize, size_t i);

DB_CompatibilityAttr g_dbCompatArray[] = {{DB_CMPT_A, "A"},
        {DB_CMPT_B, "B"},
        {DB_CMPT_C, "C"},
        {DB_CMPT_PG, "PG"}};

char* g_dbPlugins[] = {nullptr, "dolphin", nullptr, nullptr};

#define NO_ARGUMENT 0
#define REQUIRED_ARGUMENT 1
#define OPTIONAL_ARGUMENT 2

void SanityCheck()
{
    if ((g_assessmentSettings.username != nullptr) ^ (g_assessmentSettings.password != nullptr)) {
        fprintf(stderr, _("%s: -U and -W must be specified both or not\n"), pset.progname);
        exit(EXIT_FAILURE);
    }
    if (!g_assessmentSettings.port) {
        fprintf(stderr, _("%s: -p must be specified\n"), pset.progname);
        exit(EXIT_FAILURE);
    }

    if (!g_assessmentSettings.dbname && g_assessmentSettings.database == -1) {
        fprintf(stderr, _("%s: -d or -c must be specified\n"), pset.progname);
        exit(EXIT_FAILURE);
    }

    if (!g_assessmentSettings.dbname) {
        g_assessmentSettings.needCreateDatabase = true;
    }
}

void InstallPlugins(OpenGaussConnection* conn)
{
    /* if ths user specifies the database, read id from database connection */
    auto globalDatabaseType = g_assessmentSettings.database;
    if (globalDatabaseType < 0) {
        string extensionSql = "show sql_compatibility";
        string result = conn->ExecQuery(extensionSql.c_str());
        for (int i = DB_CMPT_A; i <= DB_CMPT_PG; i++) {
            if (pg_strcasecmp(result.c_str(), g_dbCompatArray[i].name) == 0) {
                globalDatabaseType = i;
                break;
            }
        }
        if (globalDatabaseType < 0) {
            fprintf(stderr, _("%s: compatibility: only support A\\B\\C\\PG, current is %s \n"), pset.progname,
                    result.c_str());
            exit(EXIT_FAILURE);
        }
    }
    if (g_dbPlugins[globalDatabaseType] != nullptr) {
        auto pluginName = g_dbPlugins[globalDatabaseType];
        string extensionSql = "select installed_version is not null from pg_available_extensions where name = ";
        extensionSql = extensionSql + "\'" + pluginName + "\'";
        const char* sql = extensionSql.c_str();
        string result = conn->ExecQuery(sql);
        if (result.compare("f") == 0) {
            g_assessmentSettings.plugin = true;
            fprintf(stdout, _("%s: Create plugin[%s] automatically.\n"), pset.progname, pluginName);
            if (!conn->ExecDDLCommand((string("create extension ") + pluginName).c_str())) {
                fprintf(stderr, "%s", conn->GetExecError().c_str());
                exit(EXIT_FAILURE);
            }
        } else if (result.compare("t") == 0) {
            g_assessmentSettings.plugin = true;
            // do nothing
        } else {
            fprintf(stdout, _("%s: %s is recommendeded in database %s.\n"), pset.progname,
                    g_dbPlugins[g_assessmentSettings.database], conn->GetDB());
        }
    }
    string extensionSql = "select installed_version is not null from pg_available_extensions where name = 'assessment'";
    string result = conn->ExecQuery(extensionSql.c_str());
    if (result.compare("") == 0) {
        fprintf(stderr, _("%s: \"assessment\" extension is needed.\n"), pset.progname);
        exit(EXIT_FAILURE);
    } else if (result.compare("f") == 0) {
        fprintf(stdout, _("%s: Create extension[assessment] automatically.\n"), pset.progname);
        g_assessmentSettings.extension = true;
        if (!conn->ExecDDLCommand("create extension assessment")) {
            fprintf(stderr, "%s", conn->GetExecError().c_str());
            exit(EXIT_FAILURE);
        }
    } else if (result.compare("t") == 0) {
        g_assessmentSettings.extension = true;
    }

}

int main(int argc, char* argv[])
{
    PSqlPostInit();

    InitParam(argv, argc);

    /* check output privilege */
    CheckOutputPrivilege();

    SanityCheck();

    /* data from file */
    FILE* file = CheckAndOpenFile(g_assessmentSettings.inputFile, "r");

    /* create assessment Database */
    if (g_assessmentSettings.needCreateDatabase) {
        CreateAssessmentDatabase();
    }

    /* connect to assessment database */
    OpenGaussConnection* conn = new OpenGaussConnection();
    conn->SetDB(g_assessmentSettings.dbname);
    conn->SetHost(g_assessmentSettings.host);
    conn->SetPort(g_assessmentSettings.port);
    conn->SetPassword(g_assessmentSettings.password);
    conn->SetUsername(g_assessmentSettings.username);

    /* free password on g_assessmentSettings.password */
    if (g_assessmentSettings.password) {
        char* passwdPointer = g_assessmentSettings.password;
        securec_check_c(memset_s(passwdPointer, strlen(passwdPointer), 0, strlen(passwdPointer)), "", "");
    }

    /* connect to openGauss assessment database */
    if (!conn->ConnectDB()) {
        fprintf(stderr, "%s", conn->GetExecError().c_str());
        exit(EXIT_FAILURE);
    }

    /* create plugin and extension */
    InstallPlugins(conn);

    /* suspend notice when exec command like `drop table if exists xxx` */
    if (!conn->ExecDDLCommand("set client_min_messages=error;")) {
        fprintf(stderr, "%s", conn->GetExecError().c_str());
        exit(EXIT_FAILURE);
    }

    vector <string> allSql = SplitSQLFile(file);

    CompatibilityTable* compatibilityTable = new CompatibilityTable();
    if (!compatibilityTable->GenerateReportHeader(g_assessmentSettings.inputFile, g_assessmentSettings.outputFile,
                                                  g_dbCompatArray[g_assessmentSettings.database].name)) {
        fprintf(stderr, "%s: can not write to file \"%s\": %s", pset.progname, g_assessmentSettings.outputFile,
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < allSql.size(); i++) {
        auto &str = allSql[i];
        bool gramTest = true;
        CompatibilityType compatibilityType = UNSUPPORTED_COMPATIBLE;
        string errorResult = "";
        /* there is `AST` grammar in sql plugin */
        if (g_assessmentSettings.plugin) {
            string querySql = "ast " + str;
            if (!conn->ExecDDLCommand(querySql.c_str())) {
                compatibilityType = INCOMPATIBLE;
                errorResult = conn->GetExecError();
                gramTest = false;
            }
        }
        if (gramTest) {
            string querySql = "select ast_support('" + regex_replace(str, pattern, "''") + "')";
            auto result = conn->ExecQuery(querySql.c_str());
            if (result.length() == 0) {
                compatibilityType = INCOMPATIBLE;
                errorResult = conn->GetExecError();
            } else {
                AssessmentType type = AssessmentType(stoi(result));

                switch (type) {
                    case DDL:
                        if (conn->ExecDDLCommand(str.c_str())) {
                            compatibilityType = COMPATIBLE;
                        } else {
                            compatibilityType = AST_COMPATIBLE;
                            errorResult = conn->GetExecError();
                        }
                        break;
                    case DML:
                        if (conn->ExecExplainCommand(("explain " + str).c_str())) {
                            compatibilityType = COMPATIBLE;
                        } else {
                            compatibilityType = AST_COMPATIBLE;
                            errorResult = conn->GetExecError();
                        }
                        break;
                    case EXPLAIN:
                        if (conn->ExecExplainCommand(str.c_str())) {
                            compatibilityType = COMPATIBLE;
                        } else {
                            compatibilityType = AST_COMPATIBLE;
                            errorResult = conn->GetExecError();
                        }
                        break;
                    case COMMENT:
                        compatibilityType = SKIP_COMMAND;
                        /* do nothing */
                        break;
                    case UNSUPPORTED:
                    default:
                        compatibilityType = UNSUPPORTED_COMPATIBLE;
                        errorResult = conn->GetExecError();
                }
            }
        }
        if (compatibilityType == COMPATIBLE) {
            compatibilityTable->AppendOneSQL(str, "完全兼容", errorResult);
        } else if (compatibilityType == AST_COMPATIBLE) {
            compatibilityTable->AppendOneSQL(str, "语法兼容", errorResult);
        } else if (compatibilityType == INCOMPATIBLE) {
            compatibilityTable->AppendOneSQL(str, "不兼容", errorResult);
        } else if (compatibilityType == UNSUPPORTED_COMPATIBLE) {
            compatibilityTable->AppendOneSQL(str, "暂不支持兼容性评估", errorResult);
        }
        PrintProcess(allSql.size(), i);
    }
    fprintf(stdout, _("%s"), "\n");
    if (!compatibilityTable->GenerateReport() || !compatibilityTable->GenerateReportEnd()) {
        fprintf(stderr, "%s: can not write to file \"%s\": %s", pset.progname, g_assessmentSettings.outputFile,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (g_assessmentSettings.database >= 0) {
        fprintf(stdout, _("%s: Create database %s automatically, clear it manually!\n"), pset.progname, conn->GetDB());
    }

    delete compatibilityTable;
    delete conn;
}

void PrintProcess(size_t sqlSize, size_t i)
{
    auto curIndex = i + 1;
    auto value = curIndex * 100.0 / (sqlSize - 1);
    fprintf(stdout, _("%s: 解析[%.2lf%%]:%lu/%lu"), pset.progname, value > 100 ? 100 : value, curIndex, sqlSize);
    if (curIndex < sqlSize) {
        fprintf(stdout, "%s", "\r");
    }
}

vector <string> SplitSQLFile(FILE* file)
{
    Scan* scan = new Scan(file);
    vector <string> vec;
    vector <string> allSql;
    while ((vec = scan->GetNextSql()).size() != 0) {
        allSql.insert(allSql.end(), vec.begin(), vec.end());
    }
    scan->FreeScan();
    delete scan;
    return allSql;
}

void CreateAssessmentDatabase()
{
    OpenGaussConnection* createDbConn = new OpenGaussConnection();
    createDbConn->SetDB((char*) "postgres");
    createDbConn->SetHost(g_assessmentSettings.host);
    createDbConn->SetPort(g_assessmentSettings.port);
    createDbConn->SetPassword(g_assessmentSettings.password);
    createDbConn->SetUsername(g_assessmentSettings.username);
    if (!createDbConn->ConnectDB()) {
        fprintf(stderr, "%s", createDbConn->GetExecError().c_str());
        exit(EXIT_FAILURE);
    }
    if (!createDbConn->CreateAssessmentDB(g_dbCompatArray[g_assessmentSettings.database].name)) {
        fprintf(stderr, "%s", createDbConn->GetExecError().c_str());
        exit(EXIT_FAILURE);
    }
    g_assessmentSettings.dbname = strdup(createDbConn->GetDB());
    fprintf(stdout, _("%s: create database \"%s\" automatically.\n"), pset.progname, g_assessmentSettings.dbname);
    delete createDbConn;
}

void CheckOutputPrivilege()
{
    if (g_assessmentSettings.outputFile == nullptr) {
        fprintf(stderr, _("%s: -o must be specified\n"), pset.progname);
        exit(EXIT_FAILURE);
    } else {
        auto outputFd = fopen(g_assessmentSettings.outputFile, "wb+");
        if (outputFd == nullptr) {
            fprintf(stderr, _("%s: can`t open %s : %s \n"), pset.progname, g_assessmentSettings.outputFile,
                    strerror(errno));
            exit(EXIT_FAILURE);
        }
        fclose(outputFd);
    }
}

void InitParam(char* argv[], int argc)
{
    static struct option long_options[] = {{"dbname", REQUIRED_ARGUMENT, NULL, 'd'},
            {"compatibility", REQUIRED_ARGUMENT, NULL, 'c'},
            {"file", REQUIRED_ARGUMENT, NULL, 'f'},
            {"host", REQUIRED_ARGUMENT, NULL, 'h'},
            {"port", REQUIRED_ARGUMENT, NULL, 'p'},
            {"password", REQUIRED_ARGUMENT, NULL, 'W'},
            {"username", REQUIRED_ARGUMENT, NULL, 'U'},
            {"output", REQUIRED_ARGUMENT, NULL , 'o'},};
    int c;
    int longIndex;
    while ((c = getopt_long(argc, argv, "d:f:h:o:p:U:W:c:", long_options, &longIndex)) != -1) {
        switch (c) {
            case 'd':
                g_assessmentSettings.dbname = strdup(optarg);
                break;
            case 'f':
                g_assessmentSettings.inputFile = strdup(optarg);
                break;
            case 'c':
                for (int i = DB_CMPT_A; i <= DB_CMPT_PG; i++) {
                    if (pg_strcasecmp(optarg, g_dbCompatArray[i].name) == 0) {
                        g_assessmentSettings.database = i;
                        break;
                    }
                }
                if (g_assessmentSettings.database == -1) {
                    fprintf(stderr, _("%s: compatibility: only support A\\B\\C\\PG \n"), pset.progname);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                g_assessmentSettings.host = strdup(optarg);
                break;
            case 'o':
                g_assessmentSettings.outputFile = strdup(optarg);
                break;
            case 'p':
                g_assessmentSettings.port = strdup(optarg);
                break;
            case 'U':
                if (strlen(optarg) >= MAXPGPATH) {
                    fprintf(stderr, _("%s: invalid username, max username len:%d\n"), pset.progname, MAXPGPATH);
                    exit(EXIT_FAILURE);
                }
                g_assessmentSettings.username = strdup(optarg);
                break;
            case 'W': {
                g_assessmentSettings.password = strdup(optarg);
                auto rc = memset_s(optarg, strlen(optarg), 0, strlen(optarg));
                securec_check_c(rc, "", "");
                break;
            }
            default:
                fprintf(stderr, _("Try \"%s --help\" for more information.\n"), pset.progname);
                exit(EXIT_FAILURE);
                break;
        }
    }
}

FILE* CheckAndOpenFile(char* path, const char* mode)
{
    FILE* file = nullptr;
    if (g_assessmentSettings.inputFile == nullptr) {
        fprintf(stderr, _("%s: -f must be specified\n"), pset.progname);
        exit(EXIT_FAILURE);
    } else {
        file = fopen(g_assessmentSettings.inputFile, mode);
        if (file == nullptr) {
            fprintf(stderr, _("%s: can`t open %s : %s \n"), pset.progname, g_assessmentSettings.inputFile,
                    strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return file;
}