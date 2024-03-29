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
 * Report.cpp
 *    Assessment Report Generator
 *
 * IDENTIFICATION
 *    contrib/assessment/Report.cpp
 *
 * -------------------------------------------------------------------------
 */

#include "Report.h"
#include "string"
#include <regex>

using namespace std;

void CompatibilityTable::AppendOneSQL(int line, std::string sql, AssessmentType assessmentType,
        CompatibilityType compatibilityType, std::string errDetail)
{
    this->sqlCompatibilities.emplace_back(line, sql, assessmentType, compatibilityType, errDetail);
}

static string GetCompatibilityString(CompatibilityType compatibilityType)
{
    switch (compatibilityType) {
        case COMPATIBLE:
            return "完全兼容";
        case AST_COMPATIBLE:
            return "语法兼容";
        case INCOMPATIBLE:
            return "不兼容";
        case UNSUPPORTED_COMPATIBLE:
            return "暂不支持评估";
        case SKIP_COMMAND:
            return "忽略语句";
        default:
            return "unreached branch";
    }
}

bool CompatibilityTable::GenerateSQLCompatibilityStatistic()
{
    stringstream ss;
    ss << "<a class=\"wdr\" name=\"top\"></a>"
          "<h2 class=\"wdr\">SQL 兼容总览</h2>"
          "<table class=\"tdiff\" summary=\"This table displays SQL Assessment Data\"><tr>\n"
          "<th class=\"wdrbg\" scope=\"col\">总数</th>"
          "<th class=\"wdrbg\" scope=\"col\">"
       << GetCompatibilityString(COMPATIBLE)
       << "</th>"
          "<th class=\"wdrbg\" scope=\"col\">"
       << GetCompatibilityString(AST_COMPATIBLE)
       << "</th>"
          "<th class=\"wdrbg\" scope=\"col\">" <<
       GetCompatibilityString(INCOMPATIBLE)
       << "</th>"
          "<th class=\"wdrbg\" scope=\"col\">"
       << GetCompatibilityString(UNSUPPORTED_COMPATIBLE)
       << "</th>"
          "<th class=\"wdrbg\" scope=\"col\">"
       << GetCompatibilityString(SKIP_COMMAND)
       << "</th>\n</tr>\n";
    auto str = ss.str();
    if (fwrite(str.c_str(), 1, str.length(), fd) != str.length()) {
        return false;
    }

    auto total = this->sqlCompatibilities.size();
    auto compatible = 0;
    auto astCompatible = 0;
    auto incompatible = 0;
    auto unsupportedCompatible = 0;
    auto skipCommand = 0;
    for (auto &compatibility: this->sqlCompatibilities) {
        switch (compatibility.compatibility) {
            case COMPATIBLE:
                compatible++;
                break;
            case AST_COMPATIBLE:
                astCompatible++;
                break;
            case INCOMPATIBLE:
                incompatible++;
                break;
            case UNSUPPORTED_COMPATIBLE:
                unsupportedCompatible++;
                break;
            case SKIP_COMMAND:
                skipCommand++;
                break;
            default:
                break;
        }
    }
    stringstream data;
    data << "<tr>"
        << "<td class=\"wdrnc\" style=\"text-align: center;font-size: 16px;\">"
         << to_string(total)
         << "</td>\n"
         << "<td class=\"wdrnc\" style=\"text-align: center;font-size: 16px;\">"
         << to_string(compatible)
         << "</td>\n"
         << "<td class=\"wdrnc\" style=\"text-align: center;font-size: 16px;\">"
         << to_string(astCompatible)
         << "</td>\n"
         << "<td class=\"wdrnc\" style=\"text-align: center;font-size: 16px;\">"
         << to_string(incompatible)
         << "</td>\n"
         << "<td class=\"wdrnc\" style=\"text-align: center;font-size: 16px;\">"
         << to_string(unsupportedCompatible)
         << "</td>\n"
         << "<td class=\"wdrnc\" style=\"text-align: center;font-size: 16px;\">"
         << to_string(skipCommand)
         << "</td>\n"
         << "</tr>\n"
         << "</table>\n";
    auto dataStr = data.str();
    if (fwrite(dataStr.c_str(), 1, dataStr.length(), fd) != dataStr.length()) {
        return false;
    }
    return true;
}

bool CompatibilityTable::GenerateReport()
{
    string str = "<a class=\"wdr\" name=\"top\"></a><h2 class=\"wdr\">SQL 兼容详情</h2>"
                 "<table class=\"tdiff\" summary=\"This table displays SQL Assessment Data\" width=100%><tr>\n"
                 "<th class=\"wdrbg\" scope=\"col\">行号</th>"
                 "<th class=\"wdrbg\" scope=\"col\">SQL语句</th>"
                 "<th class=\"wdrbg\" scope=\"col\">兼容性</th>"
                 "<th class=\"wdrbg\" scope=\"col\">兼容性详情</th>"
                 "</tr>\n";
    if (fwrite(str.c_str(), 1, str.length(), fd) != str.length()) {
        return false;
    }
    
    for (size_t index = 0; index < this->sqlCompatibilities.size(); index++) {
        string type;
        if ((index & 1) == 0) {
            type = "wdrnc";
        } else {
            type = "wdrc";
        }
        auto &sqlCompatibility = this->sqlCompatibilities[index];
        string sqlDetail =  "<tr style=\"border-top-width: 2px;\">"
                "<td style=\"width:3%\" class=\"" + type + "\">" + to_string(sqlCompatibility.line) + "\n</td>\n"
                "<td style=\"width:76%\" class=\"" + type + "\">" + sqlCompatibility.sql + "\n</td>\n"
                "<td class=\"" + type + "\" align=\"left\" >" + GetCompatibilityString(sqlCompatibility.compatibility) + "</td>\n"
                "<td class=\"" + type + "_err\" align=\"left\" >" + sqlCompatibility.errDetail + "</td>\n"
                "</tr>\n";

        if (fwrite(sqlDetail.c_str(), 1, sqlDetail.length(), fd) != sqlDetail.length()) {
            return false;
        }
    }
    string tableSuffix = "</table>\n";
    if (fwrite(tableSuffix.c_str(), 1, tableSuffix.length(), fd) != tableSuffix.length()) {
        return false;
    }
    return true;
}

bool CompatibilityTable::GenerateReportHeader(char* fileName, char* output, const char* dbName)
{
    this->fd = fopen(output, "wb+");
    if (fd == nullptr) {
        return false;
    }
    stringstream ss;
    ss << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
          "<head><title>openGauss 兼容性评估报告</title>\n"
          "<style type=\"text/css\">\n"
          "a.wdr {font:bold 8pt Arial,Helvetica,sans-serif;color:#663300;vertical-align:top;"
          "margin-top:0pt; margin-bottom:0pt;}\n"
          "table.tdiff {  border_collapse: collapse; } \n"
          "body.wdr {font:bold 10pt Arial,Helvetica,Geneva,sans-serif;color:black; background:White;}\n"
          "h1.wdr {font:bold 20pt Arial,Helvetica,Geneva,sans-serif;color:#8B0000;background-color:White;"
          "border-bottom:1px solid #cccc99;margin-top:0pt; margin-bottom:0pt;padding:0px 0px 0px 0px;}\n"
          "h2.wdr {font:bold 18pt Arial,Helvetica,Geneva,sans-serif;color:#8B0000;"
          "background-color:White;margin-top:4pt; margin-bottom:0pt;}\n"
          "h3.wdr {font:bold 16pt Arial,Helvetica,Geneva,sans-serif;color:#8B0000;"
          "background-color:White;margin-top:4pt; margin-bottom:0pt;}\n"
          "li.wdr {font: 8pt Arial,Helvetica,Geneva,sans-serif; color:black; background:White;}\n"
          "th.wdrnobg {font:bold 8pt Arial,Helvetica,Geneva,sans-serif; color:black; "
          "background:White;padding-left:4px; padding-right:4px;padding-bottom:2px}\n"
          "th.wdrbg {font:bold 8pt Arial,Helvetica,Geneva,sans-serif; color:White; "
          "background:#8F170B;padding-left:4px; padding-right:4px;padding-bottom:2px}\n"
          "td.wdrnc {font:8pt Arial,Helvetica,Geneva,sans-serif;color:black; white-space:pre-wrap;word-break;;"
          "background:#F4F6F6; vertical-align:top;word-break: break-word; width: 4%;}\n"
          "td.wdrc {font:8pt Arial,Helvetica,Geneva,sans-serif;color:black; white-space:pre-wrap;word-break;"
          "background:White; vertical-align:top;word-break: break-word; width: 4%;}\n"
          "td.wdrnc_err {font:8pt fangsong;color:black; white-space:pre-wrap;word-break;;"
          "background:#F4F6F6; vertical-align:top;word-break: break-word; width: 20%;}\n"
          "td.wdrc_err {font:8pt fangsong;color:black; white-space:pre-wrap;word-break;"
          "background:White; vertical-align:top;word-break: break-word; width: 20%;}\n"
          "td.wdrtext {font:8pt Arial,Helvetica,Geneva,sans-serif;color:black;background:White;vertical-align:top;"
          "white-space:pre-wrap;word-break: break-word;}"
          "</style>";

    ss << "<script type=\"text/javascript\">function msg(titlename, inputname, objname) {\n"
          "if (document.getElementById(inputname).value == \"1\") {\n"
          "    document.getElementById(objname).style.display=\"block\";\n"
          "    document.getElementById(titlename).innerHTML = \"-\" + titlename;\n"
          "    document.getElementById(inputname).value = \"0\";\n"
          "} else {\n"
          "    document.getElementById(objname).style.display=\"none\";\n"
          "    document.getElementById(titlename).innerHTML = \"+\" + titlename;\n"
          "    document.getElementById(inputname).value = \"1\";\n"
          "}}</script>\n"
          "</head><body class=\"wdr\"\n>";

    ss << "<h1 class=\"wdr\">" << dbName << " 兼容性评估报告</h1>\n";
    auto str = ss.str();
    if (fwrite(str.c_str(), 1, str.length(), fd) != str.length()) {
        return false;
    }
    return true;
}

bool CompatibilityTable::GenerateReportEnd()
{
    string str = "\n</body></html>";
    if (fwrite(str.c_str(), 1, str.length(), fd) != str.length()) {
        return false;
    }
    return true;
}

CompatibilityTable::~CompatibilityTable()
{
    if (fd != nullptr) {
        fclose(fd);
    }
}