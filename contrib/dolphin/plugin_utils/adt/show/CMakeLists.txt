# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# Plugin is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_adt_show_SRC)
set(dolphin_adt_show_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

set(dolphin_adt_show_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_adt_show_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_adt_show_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_adt_show dolphin_adt_show_SRC dolphin_adt_show_INC "${dolphin_adt_show_DEF_OPTIONS}" "${dolphin_adt_show_COMPILE_OPTIONS}" "${dolphin_adt_show_LINK_OPTIONS}")
