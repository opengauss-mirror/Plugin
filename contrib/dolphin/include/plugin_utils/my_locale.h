/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#ifndef OPENGAUSS_MYLOCALE_H
#define OPENGAUSS_MYLOCALE_H

#define LOCALE_LEN 6        /* length of locale name with '\0' ended. e.g. "en_US" */

#include "postgres.h"
#include "utils/dynahash.h"
#include "utils/hsearch.h"
#include "plugin_postgres.h"
#include "utils/numeric.h"
extern double use_convert_timevalue_to_scalar(Datum value, Oid typid);
extern int conv_n(char *result, int128 data, int from_base_s, int to_base_s);

extern Numeric int64_to_numeric(int64 v);
struct MyLocale{
    char locale_name[LOCALE_LEN];
    char decimal_point;
    char thousand_sep;
#ifdef DOLPHIN
    char **month_names;
    char **ab_month_names;
    char **day_names;
    char **ab_day_names;
#endif
};

MyLocale* MyLocaleSearch(char* target);

#endif  // OPENGAUSS_MYLOCALE_H
