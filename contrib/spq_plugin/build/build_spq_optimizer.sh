#!/bin/sh

spq_code_path=$BUILD_ROOT/contrib/spq

/usr/bin/cp -f $spq_code_path/build/CMakeLists.txt $spq_code_path/src/spq_optimizer/
/usr/bin/cp -f $spq_code_path/build/libspqdbcost/CMakeLists.txt $spq_code_path/src/spq_optimizer/libspqdbcost/
/usr/bin/cp -f $spq_code_path/build/libspqos/server/CMakeLists.txt $spq_code_path/src/spq_optimizer/libspqos/server/
/usr/bin/cp -f $spq_code_path/build/libspqos/CMakeLists.txt $spq_code_path/src/spq_optimizer/libspqos/
/usr/bin/cp -f $spq_code_path/build/server/CMakeLists.txt $spq_code_path/src/spq_optimizer/server/
/usr/bin/cp -f $spq_code_path/build/libnaucrates/CMakeLists.txt $spq_code_path/src/spq_optimizer/libnaucrates/
/usr/bin/cp -f $spq_code_path/build/libspqopt/CMakeLists.txt $spq_code_path/src/spq_optimizer/libspqopt/

cd $spq_code_path/src/spq_optimizer/

cmake -GNinja -H. -Bbuild
ninja -C build

cd build

ctest -j8 --output-on-failure
