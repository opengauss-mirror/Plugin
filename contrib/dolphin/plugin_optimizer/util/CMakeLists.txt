AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_opt_util_SRC)
set(dolphin_opt_util_INC 
    ${PROJECT_SRC_DIR}/gausskernel/cbb/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LIBCGROUP_INCLUDE_PATH}
    ${PROJECT_SRC_DIR}/include/libcomm
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

set(dolphin_opt_util_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_opt_util_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_opt_util_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_opt_util dolphin_opt_util_SRC dolphin_opt_util_INC "${dolphin_opt_util_DEF_OPTIONS}" "${dolphin_opt_util_COMPILE_OPTIONS}" "${dolphin_opt_util_LINK_OPTIONS}")
