AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_plugin_SRC)

SET(TGT_spq_plugin_INC ../include
        ./spq_optimizer/libnaucrates/include)

set(spq_plugin_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_plugin_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_plugin_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_src TGT_spq_plugin_SRC TGT_spq_plugin_INC "${spq_plugin_DEF_OPTIONS}" "${spq_plugin_COMPILE_OPTIONS}" "${spq_plugin_LINK_OPTIONS}")

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/spq
    ${CMAKE_CURRENT_SOURCE_DIR}/spq_optimizer
    ${CMAKE_CURRENT_SOURCE_DIR}/spq_optimizer_util
    ${CMAKE_CURRENT_SOURCE_DIR}/executor
)

add_subdirectory(spq)
add_subdirectory(spq_optimizer)
add_subdirectory(spq_optimizer_util)
add_subdirectory(executor)
