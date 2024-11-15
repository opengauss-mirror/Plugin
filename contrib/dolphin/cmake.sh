#!/bin/bash
CMAKE_OPT="-DENABLE_MULTIPLE_NODES=OFF -DENABLE_PRIVATEGAUSS=OFF -DENABLE_THREAD_SAFETY=ON -DENABLE_LITE_MODE=ON"
cpus_num=$(grep -w processor /proc/cpuinfo|wc -l)
rm -f dolphin--4.1.sql
touch dolphin--4.1.sql
for i in `ls sql_script`; do cat sql_script/$i >> dolphin--4.1.sql; done
for i in `ls sql_script_post`; do cat sql_script_post/$i >> dolphin--4.1.sql; done
cat upgrade_script/dolphin--1.0--1.1.sql upgrade_script/dolphin--1.1--2.0.sql upgrade_script/dolphin--2.0--3.0.sql upgrade_script/dolphin--3.0--4.0.sql upgrade_script/dolphin--4.0--4.1.sql >> dolphin--4.1.sql
rm -f dolphin--1.0--1.1.sql
cp upgrade_script/dolphin--1.0--1.1.sql dolphin--1.0--1.1.sql
rm -f dolphin--1.1--1.0.sql
cp rollback_script/dolphin--1.1--1.0.sql dolphin--1.1--1.0.sql
rm -f dolphin--1.1--2.0.sql
cp upgrade_script/dolphin--1.1--2.0.sql dolphin--1.1--2.0.sql
rm -f dolphin--2.0--1.1.sql
cp rollback_script/dolphin--2.0--1.1.sql dolphin--2.0--1.1.sql
rm -f dolphin--2.0--3.0.sql
cp upgrade_script/dolphin--2.0--3.0.sql dolphin--2.0--3.0.sql
rm -f dolphin--3.0--2.0.sql
cp rollback_script/dolphin--3.0--2.0.sql dolphin--3.0--2.0.sql
rm -f dolphin--3.0--4.0.sql
cp upgrade_script/dolphin--3.0--4.0.sql dolphin--3.0--4.0.sql
rm -f dolphin--4.0--3.0.sql
cp rollback_script/dolphin--4.0--3.0.sql dolphin--4.0--3.0.sql
rm -f dolphin--2.0--2.0.1.sql
cp upgrade_script/dolphin--2.0--2.0.1.sql dolphin--2.0--2.0.1.sql
rm -f dolphin--2.0.1--2.0.sql
cp rollback_script/dolphin--2.0.1--2.0.sql dolphin--2.0.1--2.0.sql
rm -f dolphin--4.0.1--4.0.sql dolphin--4.0--4.0.1.sql
cp dolphin--2.0--2.0.1.sql dolphin--4.0--4.0.1.sql
cp dolphin--2.0.1--2.0.sql dolphin--4.0.1--4.0.sql
rm -f dolphin--4.0--4.1.sql
cp upgrade_script/dolphin--4.0--4.1.sql dolphin--4.0--4.1.sql
rm -f dolphin--4.1--4.0.sql
cp rollback_script/dolphin--4.1--4.0.sql dolphin--4.1--4.0.sql
cp llvmir/openGauss_expr_dolphin_${BUILD_TUPLE}.ir openGauss_expr_dolphin.ir
DOLPHIN_CMAKE_BUILD_DIR=`pwd`/tmp_build
[ -d "${DOLPHIN_CMAKE_BUILD_DIR}" ] && rm -rf ${DOLPHIN_CMAKE_BUILD_DIR}
mkdir -p ${DOLPHIN_CMAKE_BUILD_DIR}
cd ${DOLPHIN_CMAKE_BUILD_DIR}
cmake .. ${CMAKE_OPT}
make VERBOSE=1 -sj ${cpus_num}
make install -sj ${cpus_num}
