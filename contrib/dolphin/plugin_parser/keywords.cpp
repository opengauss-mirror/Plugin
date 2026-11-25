/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * Plugin is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/* -------------------------------------------------------------------------
 *
 * The preceding notice applies at the Plugin project level.
 * The upstream attribution below and the applicable license terms in
 * Third_Party_Open_Source_Software_Notice apply to the inherited code.
 *
 *
 * keywords.cpp
 *	  lexical token lookup for key words in openGauss
 *
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/common/backend/parser/keywords.cpp
 *
 * -------------------------------------------------------------------------
 *
 * From openGauss-server, with include paths adapted for Dolphin.
 * See Third_Party_Open_Source_Software_Notice for the upstream notices and terms.
 */
#include "c.h"
#include "plugin_parser/keywords.h"

/*ScanKeywordList lookup.data.for.SQL.keywords.*/
#include "plugin_parser/kwlist_d.h"

#define PG_KEYWORD(kwname,value,category) category,

const uint8 ScanKeywordCategories[SCANKEYWORDS_NUM_KEYWORDS] = {
    #include "plugin_parser/kwlist.h"
};

#undef PG_KEYWORD

