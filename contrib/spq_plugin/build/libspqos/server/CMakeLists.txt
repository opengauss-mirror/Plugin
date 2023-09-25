# Copyright (c) 2015, Pivotal Software, Inc. All Rights Reserved.

# Include path for unit test headers.
include_directories(include)

# CExceptionTest has a function that intentionally calls itself with infinite
# recursion to test exception handling when exhausting the stack. Some C++
# compilers (notably Clang) detect this and generate a warning. We suppress
# such warnings ONLY for CExceptionTest, which we compile as a static library
# separate from the rest of the tests.
check_cxx_compiler_flag("-Wno-infinite-recursion"
                        COMPILER_HAS_WNO_INFINITE_RECURSION)
if (COMPILER_HAS_WNO_INFINITE_RECURSION)
  set_source_files_properties(src/unittest/spqos/error/CExceptionTest.cpp
    PROPERTIES COMPILE_FLAGS "-Wno-infinite-recursion")
endif()

# CCacheTest sets some variables that are only read in DEBUG builds, causing
# warnings about unused variables for RELEASE builds. We suppress such warnings
# ONLY for CCacheTest, which we compile as a static library separate from the
# rest of the tests.
check_cxx_compiler_flag("-Wno-unused-variable"
                        COMPILER_HAS_WNO_UNUSED_VARIABLE)
if (COMPILER_HAS_WNO_UNUSED_VARIABLE)
  set_source_files_properties(src/unittest/spqos/memory/CCacheTest.cpp
    PROPERTIES COMPILE_FLAGS "-Wno-unused-variable")
endif()

file(GLOB_RECURSE hdrs ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add headers to make them visible in some IDEs (Clion, VS, Xcode)
list(APPEND srcs ${hdrs})

# Monolithic spqos_test executable.
add_executable(spqos_test ${srcs})

target_link_libraries(spqos_test
                      spqos)

# Convenience function to add the test specified by 'TEST_NAME' to the set of
# tests to be run by CTest.
function(add_spqos_test TEST_NAME)
  add_test(NAME spqos_test_${TEST_NAME}
           COMMAND spqos_test -U ${TEST_NAME})
endfunction()
# Adaptation of the add_spqos_test for setting up a custom allocator instead of
# using CMemoryPoolMalloc in spqos_init().
function(add_spqos_custom_alloc_test TEST_NAME)
  add_test(NAME spqos_custom_alloc_test_${TEST_NAME}
           COMMAND spqos_test -U ${TEST_NAME} "-c")
endfunction()

option(ENABLE_EXTENDED_TESTS
       "Enable extended tests for fault-injection and timing that may take a long time to run."
       OFF)

# Individual unit tests are added here in the same order they appear in
# src/startup/main.cpp

# common
add_spqos_test(CAutoPTest)
add_spqos_test(CAutoRefTest)
add_spqos_test(CAutoRgTest)
add_spqos_test(CBitSetIterTest)
add_spqos_test(CBitSetTest)
add_spqos_test(CBitVectorTest)
add_spqos_test(CDynamicPtrArrayTest)
add_spqos_test(CEnumSetTest)
add_spqos_test(CDoubleTest)
add_spqos_test(CHashMapTest)
add_spqos_test(CHashMapIterTest)
add_spqos_test(CHashSetTest)
add_spqos_test(CHashSetIterTest)
add_spqos_test(CRefCountTest)
add_spqos_test(CListTest)
add_spqos_test(CStackTest)
add_spqos_test(CSyncHashtableTest)
add_spqos_test(CSyncListTest)

# error
add_spqos_test(CErrorHandlerTest)
add_spqos_test(CExceptionTest)
add_spqos_test(CLoggerTest)
add_spqos_test(CMessageTest)
add_spqos_test(CMessageTableTest)
add_spqos_test(CMessageRepositoryTest)
add_spqos_test(CMiniDumperTest)

# io
add_spqos_test(COstreamBasicTest)
add_spqos_test(COstreamStringTest)
add_spqos_test(COstreamFileTest)
add_spqos_test(CFileTest)

# memory
add_spqos_test(CMemoryPoolBasicTest)
add_spqos_test(CCacheTest)

# custom allocator
add_spqos_custom_alloc_test(CMemoryPoolBasicTest)
add_spqos_custom_alloc_test(CCacheTest)

# string
add_spqos_test(CWStringTest)
add_spqos_test(CStringTest)

# task
add_spqos_test(CTaskLocalStorageTest)

# test
add_spqos_test(CUnittestTest_1)
add_spqos_test(CUnittestTest_2)
add_spqos_test(CUnittestTest_3)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
  # fault-simulation
  add_spqos_test(CFSimulatorTest)
  if (ENABLE_EXTENDED_TESTS)
    # fault-simulation while running other tests
    add_spqos_test(CFSimulatorTestExt)

    # time-slicing
    add_spqos_test(CTimeSliceTest)
  endif()
endif()
