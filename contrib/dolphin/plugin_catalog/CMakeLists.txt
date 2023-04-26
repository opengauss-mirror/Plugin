AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_catalog_SRC)
set(dolphin_catalog_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${PROJECT_OPENGS_DIR}/contrib/log_fdw
    ${PROJECT_TRUNK_DIR}/distribute/bin/gds
    ${PROJECT_SRC_DIR}/include/libcomm
    ${LZ4_INCLUDE_PATH}
    ${LIBCGROUP_INCLUDE_PATH}
    ${EVENT_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

set(dolphin_catalog_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_catalog_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_catalog_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_catalog dolphin_catalog_SRC dolphin_catalog_INC "${dolphin_catalog_DEF_OPTIONS}" "${dolphin_catalog_COMPILE_OPTIONS}" "${dolphin_catalog_LINK_OPTIONS}")
