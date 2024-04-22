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
 * show_charset.cpp
 * SHOW {CHARACTER SET | CHARSET} [LIKE 'pattern' | WHERE expr] 
 *
 * IDENTIFICATION
 *    contrib/dolphin/plugin_utils/adt/show/show_charset.cpp
 *
 * -------------------------------------------------------------------------
 */
#include "plugin_utils/show_common.h"
#include "funcapi.h"

PG_FUNCTION_INFO_V1_PUBLIC(ShowCharset);
extern "C" DLL_PUBLIC Datum ShowCharset(PG_FUNCTION_ARGS);

struct CharacterSet {
    char *name;
    char *description;
    char *default_collation;
    unsigned int maxLen;
    bool server = true;
};

CharacterSet g_characterSets[] = {
    {"abc", "alias for WIN1258", "", 1},
    {"alt", "alias for WIN866", "", 1},
    {"big5", "Big Five", "", 2, false},
    {"euc_cn", "Extended UNIX Code-CN", "", 3},
    {"eucjis2004", "Extended UNIX Code-JP, JIS X 0213", "", 3},
    {"eucjp", "Extended UNIX Code-JP, JIS X 0213", "", 3},
    {"euckr", "Extended UNIX Code-KR", "", 3},
    {"euctw", "Extended UNIX Code-Taiwan, China", "", 3},
    {"gb18030", "National Standard", "gb18030_chinese_ci", 4, false},
    {"gbk", "Extended National Standard", "gbk_chinese_ci", 2},
    {"iso88591", "ISO 8859-1, ECMA 94", "", 1},
    {"iso885910", "ISO 8859-10, ECMA 144", "", 1},
    {"iso885913", "ISO 8859-13", "", 1},
    {"iso885914", "ISO 8859-14", "", 1},
    {"iso885915", "ISO 8859-15", "", 1},
    {"iso885916", "ISO 8859-16, ASRO SR 14111", "", 1},
    {"iso88592", "ISO 8859-2, ECMA 94", "", 1},
    {"iso88593", "ISO 8859-3, ECMA 94", "", 1},
    {"iso88594", "ISO 8859-4, ECMA 94", "", 1},
    {"iso88595", "ISO 8859-5, ECMA 113", "", 1},
    {"iso88596", "ISO 8859-6, ECMA 114", "", 1},
    {"iso88597", "ISO 8859-7, ECMA 118", "", 1},
    {"iso88598", "ISO 8859-8, ECMA 121", "", 1},
    {"iso88599", "ISO 8859-9, ECMA 128", "", 1},
    {"johab", "JOHAB; Extended Unix Code for simplified Chinese", "", 3, false},
    {"koi8", "_dirty_ alias for KOI8-R (backward compatibility)", "", 1},
    {"koi8r", "KOI8-R; RFC1489", "", 1},
    {"koi8u", "KOI8-U; RFC2319", "", 1},
    {"latin1", "alias for ISO-8859-1", "", 1},
    {"latin10", "alias for ISO-8859-16", "", 1},
    {"latin2", "alias for ISO-8859-2 ", "", 1},
    {"latin3", "alias for ISO-8859-3 ", "", 1},
    {"latin4", "alias for ISO-8859-4 ", "", 1},
    {"latin5", "alias for ISO-8859-9 ", "", 1},
    {"latin6", "alias for ISO-8859-10", "", 1},
    {"latin7", "alias for ISO-8859-13", "", 1},
    {"latin8", "alias for ISO-8859-14", "", 1},
    {"latin9", "alias for ISO-8859-15", "", 1},
    {"mskanji", "alias for Shift_JIS", "", 2, false},
    {"muleinternal", "Mule internal code", "", 4},
    {"shiftjis", "Shift JIS", "", 2, false},
    {"shiftjis2004", "SHIFT-JIS-2004; Shift JIS for Japanese, standard JIS X 0213", "", 2, false},
    {"sjis", "alias for Shift_JIS", "", 2, false},
    {"sqlascii", "Unspecified (see the text)", "", 1},
    {"tcvn", "alias for WIN1258", "", 1},
    {"tcvn5712", "alias for WIN1258", "", 1},
    {"uhc", "Unified Hangul Code", "", 2, false},
    {"unicode", "alias for UTF8", "", 4},
    {"utf8", "Unicode, 8-bit", "utf8_general_ci", 4},
    {"utf8mb4", "Unicode, 8-bit, aliases for utf8", "utf8mb4_general_ci", 4},
    {"vscii", "alias for WIN1258", "", 4},
    {"win", "_dirty_ alias for windows-1251 (backward compatibility)", "", 1},
    {"win1250", "alias for Windows-1250", "", 1},
    {"win1251", "alias for Windows-1251", "", 1},
    {"win1252", "alias for Windows-1252", "", 1},
    {"win1253", "alias for Windows-1253", "", 1},
    {"win1254", "alias for Windows-1254", "", 1},
    {"win1255", "alias for Windows-1255", "", 1},
    {"win1256", "alias for Windows-1256", "", 1},
    {"win1257", "alias for Windows-1257", "", 1},
    {"win1258", "alias for Windows-1258", "", 1},
    {"win866", "Windows CP866", "", 1},
    {"win874", "alias for Windows-874", "", 1},
    {"win932", "alias for Shift_JIS", "", 2, false},
    {"win936", "alias for GBK", "", 2},
    {"win949", "alias for UHC", "", 2, false},
    {"win950", "alias for BIG5", "", 2, false},
    {"windows1250", "Windows CP1250", "", 1},
    {"windows1251", "Windows CP1251", "", 1},
    {"windows1252", "Windows CP1252", "", 1},
    {"windows1253", "Windows CP1253", "", 1},
    {"windows1254", "Windows CP1254", "", 1},
    {"windows1255", "Windows CP1255", "", 1},
    {"windows1256", "Windows CP1256", "", 1},
    {"windows1257", "Windows CP1257", "", 1},
    {"windows1258", "Windows CP1258", "", 1},
    {"windows866", "Windows CP866", "", 1},
    {"windows874", "Windows CP874", "", 1},
    {"windows932", "alias for Shift_JIS", "", 2, false},
    {"windows936", "alias for GBK", "", 2},
    {"windows949", "alias for UHC", "", 2, false},
    {"windows950", "alias for BIG5", "", 2, false},
};

/**
 * tuple desc of show character set
 * @return tuple desc
 */
TupleDesc ShowCharsetTupleDesc()
{
    /* isServer used by show collation. show collation returns collations supported by the server */
    TupleDescEntry tupleDescEntries[] = {
        {"charset", NAMEOID},
        {"description", TEXTOID},
        {"default collation", TEXTOID},
        {"maxlen", INT4OID},
        {"server", BOOLOID},
    };
    return ConstructTupleDesc(tupleDescEntries, sizeof(tupleDescEntries) / sizeof(TupleDescEntry));
}

/**
 * first call of show charset
 */
void ShowCharsetFirstCall(PG_FUNCTION_ARGS)
{
    FuncCallContext *fctx = SRF_FIRSTCALL_INIT();
    MemoryContext oldContext = MemoryContextSwitchTo(fctx->multi_call_memory_ctx);
    fctx->tuple_desc = ShowCharsetTupleDesc();
    fctx->max_calls = sizeof(g_characterSets) / sizeof(CharacterSet);
    fctx->call_cntr = 0;
    MemoryContextSwitchTo(oldContext);
}

Datum ShowCharset(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL()) {
        ShowCharsetFirstCall(fcinfo);
    }
    FuncCallContext *fctx = SRF_PERCALL_SETUP();
    if (fctx->call_cntr < fctx->max_calls) {
        Datum values[fctx->tuple_desc->natts];
        bool nulls[fctx->tuple_desc->natts] = {false, false, false, false, false};
        CharacterSet *characterSet = &g_characterSets[fctx->call_cntr];
        values[0] = DirectFunctionCall1(namein, CStringGetDatum(characterSet->name));
        values[1] = CStringGetDatum(cstring_to_text(characterSet->description));
        values[2] = CStringGetDatum(cstring_to_text(characterSet->default_collation));
        values[3] = Int32GetDatum(characterSet->maxLen);
        values[4] = BoolGetDatum(characterSet->server);
        HeapTuple returnTuple = heap_form_tuple(fctx->tuple_desc, values, nulls);
        SRF_RETURN_NEXT(fctx, HeapTupleGetDatum(returnTuple));
    } else {
        SRF_RETURN_DONE(fctx);
    }
}

ResTarget *MakeResTargetByName(char *name)
{
    ColumnRef *columnRef = makeNode(ColumnRef);
    columnRef->fields = list_make1(makeString(name));
    ResTarget *resTarget = makeNode(ResTarget);
    resTarget->val = (Node *)columnRef;
    return resTarget;
}


SelectStmt *MakeShowCharacterQuery(List *args, Node *likeWhereOpt, bool isLikeExpr)
{
    SelectStmt *selectStmt = MakeCommonQuery("show_character_set", "charset", args, likeWhereOpt, isLikeExpr);
    list_free_ext(selectStmt->targetList);
    selectStmt->targetList = lappend(selectStmt->targetList, MakeResTargetByName("charset"));
    selectStmt->targetList = lappend(selectStmt->targetList, MakeResTargetByName("description"));
    selectStmt->targetList = lappend(selectStmt->targetList, MakeResTargetByName("default collation"));
    selectStmt->targetList = lappend(selectStmt->targetList, MakeResTargetByName("maxlen"));
    return selectStmt;
}