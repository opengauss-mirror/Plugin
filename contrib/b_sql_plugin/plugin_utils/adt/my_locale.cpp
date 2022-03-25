/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include <cstring>
#include "plugin_utils/my_locale.h"

/* please ensure that locale_name is in order */
static MyLocale my_locale_array[] = {
    {"ar_AE", '.', ','},
    {"ar_BH", '.', ','},
    {"ar_DZ", '.', ','},
    {"ar_EG", '.', ','},
    {"ar_IN", '.', ','},
    {"ar_IQ", '.', ','},
    {"ar_JO", '.', ','},
    {"ar_KW", '.', ','},
    {"ar_LB", '.', ','},
    {"ar_LY", '.', ','},
    {"ar_MA", '.', ','},
    {"ar_OM", '.', ','},
    {"ar_QA", '.', ','},
    {"ar_SA", '.', '\0'},
    {"ar_SD", '.', ','},
    {"ar_SY", '.', ','},
    {"ar_TN", '.', ','},
    {"ar_YE", '.', ','},
    {"be_BY", ',', '.'},
    {"bg_BG", ',', '\0'},
    {"ca_ES", ',', '\0'},
    {"cs_CZ", ',', ' '},
    {"da_DK", ',', '.'},
    {"de_AT", ',', '\0'},
    {"de_BE", ',', '.'},
    {"de_CH", '.', '\''},
    {"de_DE", ',', '.'},
    {"de_LU", ',', '.'},
    {"en_AU", '.', ','},
    {"en_CA", '.', ','},
    {"en_GB", '.', ','},
    {"en_IN", '.', ','},
    {"en_NZ", '.', ','},
    {"en_PH", '.', ','},
    {"en_US", '.', ','},
    {"en_ZA", '.', ','},
    {"en_ZW", '.', ','},
    {"es_AR", ',', '.'},
    {"es_BO", ',', '\0'},
    {"es_CL", ',', '\0'},
    {"es_CO", ',', '\0'},
    {"es_CR", '.', '\0'},
    {"es_DO", '.', '\0'},
    {"es_EC", ',', '\0'},
    {"es_ES", ',', '\0'},
    {"es_GT", '.', '\0'},
    {"es_HN", '.', '\0'},
    {"es_MX", '.', '\0'},
    {"es_NI", '.', '\0'},
    {"es_PA", '.', '\0'},
    {"es_PE", '.', '\0'},
    {"es_PR", '.', '\0'},
    {"es_PY", ',', '\0'},
    {"es_SV", '.', '\0'},
    {"es_US", '.', ','},
    {"es_UY", ',', '\0'},
    {"es_VE", ',', '\0'},
    {"el_GR", ',', '.'},
    {"et_EE", ',', ' '},
    {"eu_ES", ',', '\0'},
    {"fi_FI", ',', ' '},
    {"fo_FO", ',', '.'},
    {"fr_BE", ',', '.'},
    {"fr_CA", ',', ' '},
    {"fr_CH", ',', '\0'},
    {"fr_FR", ',', '\0'},
    {"fr_LU", ',', '\0'},
    {"gl_ES", ',', '\0'},
    {"gu_IN", '.', ','},
    {"he_IL", '.', ','},
    {"hi_IN", '.', ','},
    {"hr_HR", ',', '\0'},
    {"hu_HU", ',', '.'},
    {"id_ID", ',', '.'},
    {"is_IS", ',', '.'},
    {"it_CH", ',', '\''},
    {"it_IT", ',', '\0'},
    {"ja_JP", '.', ','},
    {"ko_KR", '.', ','},
    {"lt_LT", ',', '.'},
    {"lv_LV", ',', ' '},
    {"mk_MK", ',', ' '},
    {"mn_MN", ',', '.'},
    {"ms_MY", '.', ','},
    {"nb_NO", ',', '.'},
    {"nl_BE", ',', '.'},
    {"nl_NL", ',', '\0'},
    {"no_NO", ',', '.'},
    {"pl_PL", ',', '\0'},
    {"pt_BR", ',', '\0'},
    {"pt_PT", ',', '\0'},
    {"rm_CH", ',', '\''},
    {"ro_RO", ',', '.'},
    {"ru_RU", ',', ' '},
    {"ru_UA", ',', '.'},
    {"sk_SK", ',', ' '},
    {"sl_SI", ',', ' '},
    {"sq_AL", ',', '.'},
    {"sr_RS", '.', '\0'},
    {"sv_FI", ',', ' '},
    {"sv_SE", ',', ' '},
    {"ta_IN", '.', ','},
    {"te_IN", '.', ','},
    {"th_TH", '.', ','},
    {"tr_TR", ',', '.'},
    {"uk_UA", ',', '.'},
    {"ur_PK", '.', ','},
    {"vi_VN", ',', '.'},
    {"zh_CN", '.', ','},
    {"zh_HK", '.', ','},
    {"zh_TW", '.', ','},
};

constexpr int LOCALE_NUM = sizeof(my_locale_array) / sizeof(MyLocale);

MyLocale* MyLocaleSearch(char* target)
{
    int start = 0;
    int end = LOCALE_NUM - 1;

    while (start <= end) {
        int mid = (start + end) / 2;
        int cmp_res = strcmp(target, my_locale_array[mid].locale_name);
        if (cmp_res > 0) {
            start = mid + 1;
        } else if (cmp_res < 0) {
            end = mid - 1;
        } else {
            return &my_locale_array[mid];
        }
    }
    return NULL;
}