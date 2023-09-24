AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_libspqos_SRC)

SET(TGT_spq_libspqos_INC ../../libspqdbcost/include
                        ../../libspqopt/include
                        ../../libspqos/include
                        ../../libnaucrates/include)

set(spq_libspqos_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_libspqos_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_libspqos_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_libspqos TGT_spq_libspqos_SRC TGT_spq_libspqos_INC "${spq_libspqos_DEF_OPTIONS}" "${spq_libspqos_COMPILE_OPTIONS}" "${spq_libspqos_LINK_OPTIONS}")

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/error
    ${CMAKE_CURRENT_SOURCE_DIR}/io
    ${CMAKE_CURRENT_SOURCE_DIR}/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/string
    ${CMAKE_CURRENT_SOURCE_DIR}/task
)

add_subdirectory(common)
add_subdirectory(error)
add_subdirectory(io)
add_subdirectory(memory)
add_subdirectory(string)
add_subdirectory(task)

