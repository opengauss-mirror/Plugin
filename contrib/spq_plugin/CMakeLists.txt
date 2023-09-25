#This is the main CMAKE for build all spqplugin.
# spqplugin.so

file(GLOB_RECURSE TGT_spqplugin_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_opt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/guc_spq.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spqplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_optimizer_util/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_optimizer/libspqdbcost/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_optimizer/libspqopt/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_optimizer/libspqos/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_optimizer/libnaucrates/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/executor/*.cpp
)

SET(TGT_spqplugin_INC
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_OPENGS_DIR}/contrib/ndpplugin/component/thread
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_optimizer/libspqos/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_optimizer/libspqopt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_optimizer/libspqdbcost/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spq_optimizer/libnaucrates/include
)

add_subdirectory(src)

set(spqplugin_DEF_OPTIONS ${MACRO_OPTIONS})
set(spqplugin_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spqplugin_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_shared_libtarget(spqplugin TGT_spqplugin_SRC TGT_spqplugin_INC "${spqplugin_DEF_OPTIONS}" "${spqplugin_COMPILE_OPTIONS}" "${spqplugin_LINK_OPTIONS}")

set_target_properties(spqplugin PROPERTIES PREFIX "")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/spqplugin.control
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/spqplugin--1.0.sql
    DESTINATION share/postgresql/extension/
)

install(TARGETS spqplugin DESTINATION lib/postgresql)

