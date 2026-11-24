/*
 * Portions Copyright (C) 2008-2020 by Pavel Stehule <pavel.stehule@gmail.com>
 * See COPYRIGHT.orafce for the license of the upstream portions.
 */

/* Copyright (c) 2026 Huawei Technologies Co., Ltd. */

#include "postgres.h"

#include "parser/scanner.h"
#include "parser/kwlookup.h"
#include "parse_keyword.h"

const char *
orafce_scan_keyword(const char *text, int *keycode)
{
	int		kwnum;

	kwnum = ScanKeywordLookup(text, &ScanKeywords);
	if (kwnum >= 0)
	{
		*keycode = ScanKeywordTokens[kwnum];
		return GetScanKeyword(kwnum, &ScanKeywords);
	}

	return NULL;
}
