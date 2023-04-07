set(SOURCES
  test_auto_policy.cpp
  test_chunk_stats.cpp
  test_compression.cpp
  test_ddl_hook.cpp
  test_continuous_aggs.cpp
)
include(${PROJECT_SOURCE_DIR}/tsl/src/build-defs.cmake)

add_library(${TSL_TESTS_LIB_NAME} OBJECT ${SOURCES})

# Since the test library will be linked into the loadable extension
# module, it needs to be compiled as position-independent code
# (e.g., the -fPIC compiler flag for GCC)
set_target_properties(${TSL_TESTS_LIB_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(${TSL_TESTS_LIB_NAME} PUBLIC TS_SUBMODULE)
