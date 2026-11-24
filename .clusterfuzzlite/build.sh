#!/bin/bash
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# Plugin is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

# SPDX-License-Identifier: MulanPSL-2.0
set -eu

cd "$SRC/Plugin/contrib/age/drivers/golang"
compile_native_go_fuzzer_v2 \
    github.com/apache/incubator-age/drivers/golang/parser \
    FuzzAgeParser age_parser_fuzzer
test -x "$OUT/age_parser_fuzzer"
