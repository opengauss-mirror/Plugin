AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_libnaucrates_SRC)

SET(TGT_spq_libnaucrates_INC ../../libspqdbcost/include
                             ../../libspqopt/include
                             ../../libspqos/include
                             ../../libnaucrates/include)

set(spq_libnaucrates_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_libnaucrates_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_libnaucrates_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_libnaucrates TGT_spq_libnaucrates_SRC TGT_spq_libnaucrates_INC "${spq_libnaucrates_DEF_OPTIONS}" "${spq_libnaucrates_COMPILE_OPTIONS}" "${spq_libnaucrates_LINK_OPTIONS}")

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/md
    ${CMAKE_CURRENT_SOURCE_DIR}/operators
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/statistics
    ${CMAKE_CURRENT_SOURCE_DIR}/xml
)

add_subdirectory(base)
add_subdirectory(md)
add_subdirectory(operators)
add_subdirectory(parser)
add_subdirectory(statistics)
add_subdirectory(xml)

