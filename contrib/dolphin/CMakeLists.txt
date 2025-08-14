#This is the main CMAKE for build bin.
# Copyright (c) Huawei Technologies Co., Ltd. 2020-2030. All rights reserved.
# description: CMake build main entrance
# date: 2021-02-08
# version: 1.0.0
cmake_minimum_required(VERSION 3.16.5)

#project name
project(openGauss)

if(POLICY CMP0068)
  cmake_policy(SET CMP0068 NEW)
endif()

if(POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

SET(CMAKE_VERBOSE_MAKEFILE OFF CACHE INTERNAL "")
SET(CMAKE_RULE_MESSAGES OFF CACHE INTERNAL "")
SET(CMAKE_SKIP_RPATH TRUE CACHE INTERNAL "")
#set CMAKE_AR parameters,set 'qc' to 'crs'
set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> crs <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> crs <TARGET> <LINK_FLAGS> <OBJECTS>")
#forbidden the default fPIC option in cmake mechanism
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
#set LIBRARY_OUTPUT
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#Set the options
option (OPT_BUILD_3RD "Build the with --3rd." OFF)
if("${ENABLE_MULTIPLE_NODES}" STREQUAL "ON")
    MESSAGE(FATAL_ERROR " The option ENABLE_MULTIPLE_NODE is not supported thus should be turned off.")
endif()
if("${ENABLE_PRIVATEGAUSS}" STREQUAL "ON")
    MESSAGE(FATAL_ERROR " The option ENABLE_PRIVATEGAUSS is not supported thus should be turned off.")
endif()

#FIXME: make it an argument
set(openGauss "../..")
set(ENV{openGauss} "../..")

#------------------------------------------------------------------------------------
set(PROJECT_TRUNK_DIR ${CMAKE_SOURCE_DIR}/../..)
set(PROJECT_OPENGS_DIR ${PROJECT_TRUNK_DIR} CACHE INTERNAL "")

# PROJECT_TRUNK_DIR: ./
# PROJECT_OPENGS_DIR: ${PROJECT_TRUNK_DIR}/${openGauss}/

#set src dir
set(PROJECT_SRC_DIR ${PROJECT_OPENGS_DIR}/src CACHE INTERNAL "")

set(3RD_PATH $ENV{THIRD_BIN_PATH})
set(prefix_home $ENV{PREFIX_HOME})
set(CMAKE_INSTALL_PREFIX ${prefix_home} CACHE INTERNAL "")
if("$ENV{GCC_VERSION}" STREQUAL "")
    execute_process(COMMAND gcc --version OUTPUT_VARIABLE GCC_VERSION_INFO OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE "\n" " " GCC_VERSION_INFO_TMP ${GCC_VERSION_INFO})
    string(REPLACE " " ";" GCC_VERSION_INFO_LIST ${GCC_VERSION_INFO_TMP})
    list(GET GCC_VERSION_INFO_LIST 2 GCC_VERSION)
else()
    set(GCC_VERSION $ENV{GCC_VERSION})
endif()
if("${GCC_VERSION}" STREQUAL "7.3.0")
    set(GCC_VERSION_LIT 7.3)
endif()

if("$ENV{BUILD_TUPLE}" STREQUAL "")
    execute_process(COMMAND uname -p OUTPUT_VARIABLE CURRENT_TUPLE OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(BUILD_TUPLE ${CURRENT_TUPLE})
else()
    set(BUILD_TUPLE $ENV{BUILD_TUPLE})
endif()

message(WARNING "dolphin cmake begin")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../../cmake/src)

include(build_function)
include(set_thirdparty_path)
include(build_options)

include_directories(${PROJECT_SRC_DIR}/include ${PROJECT_INCLUDE_DIR})
include_directories(${KERBEROS_INCLUDE_PATH} ${LIBOBS_INCLUDE_PATH} ${LIBCURL_INCLUDE_PATH} ${LIBOPENSSL_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CJSON_INCLUDE_PATH} ${BOOST_INCLUDE_PATH})


#add_compile_options(-fPIC)
set(LIB_DOLPHIN_OPTIONS -fvisibility=hidden -fPIC -DDOLPHIN)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RULE_MESSAGES OFF)
set(CMAKE_SKIP_RPATH TRUE)

add_definitions(-DDOLPHIN)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_parser
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_utils
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_executor
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_optimizer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_pl/plpgsql/src
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_vector
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_storage
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_catalog
)

add_subdirectory(plugin_protocol)
add_subdirectory(plugin_parser)
add_subdirectory(plugin_utils)
add_subdirectory(plugin_executor)
add_subdirectory(plugin_optimizer)
add_subdirectory(plugin_pl/plpgsql/src)
add_subdirectory(plugin_vector)
add_subdirectory(plugin_storage)
add_subdirectory(plugin_catalog)

set(dolphin_outer_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_outer_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_outer_LINK_OPTIONS ${LIB_LINK_OPTIONS})
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_outer_SRC)
list(APPEND dolphin_outer_SRC ${CMAKE_CURRENT_SOURCE_DIR}/plugin_utils/fmgrtab.cpp)
set(dolphin_outer_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)
add_static_objtarget(dolphin_outer_obj dolphin_outer_SRC dolphin_outer_INC "${dolphin_outer_DEF_OPTIONS}" "${dolphin_outer_COMPILE_OPTIONS}" "${dolphin_outer_LINK_OPTIONS}")

set(dolphin_objects
    $<TARGET_OBJECTS:dolphin_parser>
    $<TARGET_OBJECTS:dolphin_outer_obj>
    $<TARGET_OBJECTS:dolphin_catalog>
    $<TARGET_OBJECTS:dolphin_opt_commands>
    $<TARGET_OBJECTS:dolphin_opt_plan>
    $<TARGET_OBJECTS:dolphin_opt_prep>
    $<TARGET_OBJECTS:dolphin_opt_util>
    $<TARGET_OBJECTS:dolphin_executor>
    $<TARGET_OBJECTS:dolphin_plpgsql>
    $<TARGET_OBJECTS:dolphin_proto>
    $<TARGET_OBJECTS:dolphin_storage>
    $<TARGET_OBJECTS:dolphin_utils_adt>
    $<TARGET_OBJECTS:dolphin_adt_show>
    $<TARGET_OBJECTS:dolphin_fmgr>
    $<TARGET_OBJECTS:dolphin_mb>
    $<TARGET_OBJECTS:dolphin_vector>
)

set(dolphin_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_shared_libtarget(dolphin dolphin_objects dolphin_outer_INC "${dolphin_DEF_OPTIONS}" "${dolphin_COMPILE_OPTIONS}" "${dolphin_LINK_OPTIONS}")
set_target_properties(dolphin PROPERTIES PREFIX "")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin.control
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--5.1.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--1.0--1.1.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--1.1--1.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--1.1--2.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--2.0--1.1.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--2.0--3.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--3.0--2.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--3.0--4.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--4.0--3.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--4.0--4.1.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--4.1--4.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--4.1--4.2.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--4.2--4.1.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--4.2--5.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--5.0--4.2.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--5.0--5.1.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--5.1--5.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--2.0--2.0.1.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--2.0.1--2.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--4.0--4.0.1.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dolphin--4.0.1--4.0.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/openGauss_expr_dolphin.ir
    DESTINATION share/postgresql/extension/
)
install(TARGETS dolphin DESTINATION lib/postgresql)
