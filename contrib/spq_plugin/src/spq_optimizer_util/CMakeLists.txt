AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_optimizer_util_SRC)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RULE_MESSAGES OFF)
set(CMAKE_SKIP_RPATH TRUE)

SET(TGT_spq_optimizer_util_INC ${PROJECT_SRC_DIR}/include ../../include
                               ../spq_optimizer/libspqdbcost/include
                               ../spq_optimizer/libspqopt/include
                               ../spq_optimizer/libspqos/include
                               ../spq_optimizer/libnaucrates/include)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/relcache
    ${CMAKE_CURRENT_SOURCE_DIR}/translate
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

add_subdirectory(config)
add_subdirectory(relcache)
add_subdirectory(translate)
add_subdirectory(utils)

set(spq_optimizer_util_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_optimizer_util_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_optimizer_util_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_optimizer_util TGT_spq_optimizer_util_SRC TGT_spq_optimizer_util_INC "${spq_optimizer_util_DEF_OPTIONS}" "${spq_optimizer_util_COMPILE_OPTIONS}" "${spq_optimizer_util_LINK_OPTIONS}")
