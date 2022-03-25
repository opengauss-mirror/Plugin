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

struct MyLocale{
    char locale_name[LOCALE_LEN];
    char decimal_point;
    char thousand_sep;
};

MyLocale* MyLocaleSearch(char* target);

#endif  // OPENGAUSS_MYLOCALE_H
