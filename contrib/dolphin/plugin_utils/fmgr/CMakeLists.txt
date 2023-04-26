AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_fmgr_SRC)
set(dolphin_fmgr_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${PROJECT_OPENGS_DIR}/contrib/log_fdw
    ${PROJECT_OPENGS_DIR}/contrib/gc_fdw
    ${PROJECT_SRC_DIR}/include/libcomm
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LZ4_INCLUDE_PATH}
    ${LIBCGROUP_INCLUDE_PATH}
    ${EVENT_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

set(dolphin_fmgr_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_fmgr_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_fmgr_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_fmgr dolphin_fmgr_SRC dolphin_fmgr_INC "${dolphin_fmgr_DEF_OPTIONS}" "${dolphin_fmgr_COMPILE_OPTIONS}" "${dolphin_fmgr_LINK_OPTIONS}")
