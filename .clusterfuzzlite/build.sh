#!/bin/bash
# SPDX-License-Identifier: MulanPSL-2.0
set -eu

cd "$SRC/Plugin/contrib/age/drivers/golang"
compile_native_go_fuzzer_v2 \
    github.com/apache/incubator-age/drivers/golang/parser \
    FuzzAgeParser age_parser_fuzzer
test -x "$OUT/age_parser_fuzzer"
