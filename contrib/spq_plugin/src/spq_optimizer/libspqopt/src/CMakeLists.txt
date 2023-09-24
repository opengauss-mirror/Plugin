AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_libspqopt_SRC)

SET(TGT_spq_libspqopt_INC ../../libspqdbcost/include
                         ../../libspqopt/include
                         ../../libspqos/include
                         ../../libnaucrates/include)

set(spq_libspqopt_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_libspqopt_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_libspqopt_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_libspqopt TGT_spq_libspqopt_SRC TGT_spq_libspqopt_INC "${spq_libspqopt_DEF_OPTIONS}" "${spq_libspqopt_COMPILE_OPTIONS}" "${spq_libspqopt_LINK_OPTIONS}")

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/eval
    ${CMAKE_CURRENT_SOURCE_DIR}/mdcache
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata
    ${CMAKE_CURRENT_SOURCE_DIR}/minidump
    ${CMAKE_CURRENT_SOURCE_DIR}/operators
    ${CMAKE_CURRENT_SOURCE_DIR}/optimizer
    ${CMAKE_CURRENT_SOURCE_DIR}/search
    ${CMAKE_CURRENT_SOURCE_DIR}/translate
    ${CMAKE_CURRENT_SOURCE_DIR}/xforms
)

add_subdirectory(base)
add_subdirectory(engine)
add_subdirectory(eval)
add_subdirectory(mdcache)
add_subdirectory(metadata)
add_subdirectory(minidump)
add_subdirectory(operators)
add_subdirectory(optimizer)
add_subdirectory(search)
add_subdirectory(translate)
add_subdirectory(xforms)

