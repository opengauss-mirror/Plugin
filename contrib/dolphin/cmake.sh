#!/bin/bash
CMAKE_OPT="-DENABLE_MULTIPLE_NODES=OFF -DENABLE_PRIVATEGAUSS=OFF -DENABLE_THREAD_SAFETY=ON -DENABLE_LITE_MODE=ON"
cpus_num=$(grep -w processor /proc/cpuinfo|wc -l)
rm -f dolphin--1.0.sql
touch dolphin--1.0.sql
for i in `ls sql_script`; do cat sql_script/$i >> dolphin--1.0.sql; done
for i in `ls sql_script_post`; do cat sql_script_post/$i >> dolphin--1.0.sql; done
rm -f dolphin--2.0.sql
touch dolphin--2.0.sql
cat dolphin--1.0.sql upgrade_script/dolphin--1.0--1.1.sql upgrade_script/dolphin--1.1--2.0.sql >> dolphin--2.0.sql
rm -f dolphin--1.0--2.0.sql
touch dolphin--1.0--2.0.sql
cat upgrade_script/dolphin--1.0--1.1.sql upgrade_script/dolphin--1.1--2.0.sql >> dolphin--1.0--2.0.sql
rm -f dolphin--2.0--1.0.sql
touch dolphin--2.0--1.0.sql
cat rollback_script/dolphin--2.0--1.1.sql rollback_script/dolphin--1.1--1.0.sql >> dolphin--2.0--1.0.sql
cp llvmir/openGauss_expr_dolphin_${BUILD_TUPLE}.ir openGauss_expr_dolphin.ir
DOLPHIN_CMAKE_BUILD_DIR=`pwd`/tmp_build
[ -d "${DOLPHIN_CMAKE_BUILD_DIR}" ] && rm -rf ${DOLPHIN_CMAKE_BUILD_DIR}
mkdir -p ${DOLPHIN_CMAKE_BUILD_DIR}
cd ${DOLPHIN_CMAKE_BUILD_DIR}
cmake .. ${CMAKE_OPT}
make VERBOSE=1 -sj ${cpus_num}
make install -sj ${cpus_num}
