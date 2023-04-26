AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_storage_SRC)
set(dolphin_storage_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_SRC_DIR}/gausskernel/cbb/communication
    ${PROJECT_SRC_DIR}/include/iprange
    ${PROJECT_SRC_DIR}/include/libcomm
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LIBCGROUP_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
    ${LIBCURL_INCLUDE_PATH}
)

set(dolphin_storage_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_storage_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_storage_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_storage dolphin_storage_SRC dolphin_storage_INC "${dolphin_storage_DEF_OPTIONS}" "${dolphin_storage_COMPILE_OPTIONS}" "${dolphin_storage_LINK_OPTIONS}")
